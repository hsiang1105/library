#include "comm_client.h"
#include <QApplication>
#include <QDebug>
#include <QEventLoop>
#include <QLibrary>
#include <QSerialPort>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>
#include "abstract_protocol.h"
#include "comm_connection.h"
#include "comm_parameter.h"
#include "communication.h"

typedef Protocol::AbstractProtocol *(*InitProtocol)();

namespace {
constexpr int g_wait_timeout = 1;
constexpr int g_char_timeout = 50;
}  // namespace

namespace Communication {
class ClientData
{
public:
    ClientData()
      : conn(NULL)
      , protocol(NULL)
      , socket(NULL)
      , serial(NULL)
      , is_simulating(false)
      , is_connected(false)
      , is_reconn_enabled(true)
      , is_restore_enabled(false)
      , is_write_only(false)
      , error(E_NO_ERROR)
      , station(1)
      , timeout(5000)
      , retry(2)
      , delay(0)
    {
    }

    ~ClientData()
    {
        Close();

        if (protocol) {
            delete protocol;
            protocol = NULL;
        }

        if (conn) {
            delete conn;
            conn = NULL;
        }
    }

    void Close()
    {
        if (is_simulating) {
            is_simulating = false;
        }

        if (serial) {
            serial->close();
            delete serial;
            serial = NULL;
        }

        if (socket) {
            socket->disconnectFromHost();
            delete socket;
            socket = NULL;
        }
    }

    QByteArray Convert(const Protocol::ReturnData &ret)
    {
        QByteArray ba((char *)ret.data.data(), (int)ret.data.size());
        error = ret.error;
        return ba;
    }

    QMutex mutex;
    Connection *conn;
    Protocol::AbstractProtocol *protocol;
    QAbstractSocket *socket;
    QSerialPort *serial;
    PropertyList prop_list;
    bool is_simulating;
    bool is_connected;
    bool is_reconn_enabled;
    bool is_restore_enabled;
    bool is_write_only;
    int error;
    int station;
    int timeout;
    int retry;
    int delay;
};

Client::Client(const Connection &conn, QObject *parent)
  : QObject(parent)
  , d_(new ClientData)
{
    if (conn.IsValid()) {
        d_->conn = new Connection(conn);
        d_->station = conn.Station();
        d_->timeout = conn.Timeout();
        d_->retry = conn.Retry();
        d_->delay = conn.Delay();
    }

    connect(this,
            &Client::SigRequested,
            this,
            &Client::process,
            Qt::QueuedConnection);
}

Client::~Client()
{
    if (d_) {
        delete d_;
        d_ = NULL;
    }
}

bool Client::IsConnected() const
{
    QMutexLocker locker(&d_->mutex);
    return d_->is_connected;
}

bool Client::open()
{
    // close client
    close();

    // create protocol
    if (createProtocol()) {
        // open client
        if (createClient()) {
            return true;
        }
    }

    return false;
}

void Client::close()
{
    d_->Close();
}

bool Client::createProtocol()
{
    QString lib_path = QString("%1/%2")
                           .arg(qApp->applicationDirPath())
                           .arg(d_->conn->LibName());
#ifdef QT_DEBUG
    lib_path = lib_path.remove(lib_path.lastIndexOf('.'), 4);
    lib_path += "d.dll";
#endif

    QLibrary *lib = new QLibrary(lib_path);
    if (lib->load()) {
        // initialize protocol
        InitProtocol func = (InitProtocol)lib->resolve("InitializeProtocol");
        if (func) {
            if (d_->protocol) {
                delete d_->protocol;
                d_->protocol = NULL;
            }

            d_->protocol = func();

            // update interface to protocol
            Protocol::Interface interface = Protocol::E_NO_INTERFACE;
            switch (d_->conn->Type()) {
                case E_SERIAL:
                    interface = Protocol::E_SERIAL_PORT;
                    break;
                case E_SOCKET:
                    interface = Protocol::E_ETHERNET;
                    break;
                case E_SIMULATION:
                    interface = Protocol::E_SIMULATOR;
                    break;
                default:
                    break;
            }

            d_->protocol->SetParameter(Protocol::E_INTERFACE, interface);
            return true;
        }

        qDebug() << "[Client] failed to create protocol!";
    }

    QString messagge = tr("Failed to load library! (%1)").arg(lib_path);
    emit SigErrorOccurred(messagge);

    d_->error = E_INVALID_PROTOCOL;
    return false;
}

bool Client::createClient()
{
    QString message;
    switch (d_->conn->Type()) {
        case E_SERIAL: {
            auto serial_param = d_->conn->SerialParameter();
            d_->serial = new QSerialPort;
            d_->serial->setPortName(serial_param->ParameterText("Port"));
            d_->serial->setBaudRate(serial_param->Baudrate());
            d_->serial->setDataBits(serial_param->DataBits());
            d_->serial->setStopBits(serial_param->StopBits());
            d_->serial->setParity(serial_param->Parity());
            if (d_->serial->open(QSerialPort::ReadWrite)) {
                return true;
            }

            qDebug() << "[Client]" << d_->serial->errorString();
            message = tr("Failed to open serial port! (%1)")
                          .arg(d_->serial->portName());

            d_->error = E_CLIENT_OPEN_FAILED;

        } break;

        case E_SOCKET: {
            auto socket_param = d_->conn->SocketParameter();
            if (socket_param->Type() == Parameter::E_TCP) {
                d_->socket = new QTcpSocket;
            } else {
                d_->socket = new QUdpSocket;
            }

            QHostAddress local_addr(socket_param->LocalAddress());
            QHostAddress local_host(QHostAddress::LocalHost);
            if (!local_addr.isNull() && local_addr != local_host) {
                d_->socket->bind(local_addr, socket_param->LocalPort());
            }

            d_->socket->connectToHost(socket_param->Address(),
                                      socket_param->Port());

            if (waitConnected()) {
                return true;
            }

            qDebug() << "[Client]" << d_->socket->errorString();
            message = tr("Failed to connect to ethernet! (%1)")
                          .arg(socket_param->IPv4());

            d_->error = E_CLIENT_OPEN_FAILED;

        } break;

        case E_SIMULATION:
            d_->is_simulating = true;
            return true;
            break;

        default:
            message = tr("Interface not support! (%1)").arg(d_->conn->Type());

            d_->error = E_CLIENT_INTERFACE_ERROR;
            break;
    }

    emit SigErrorOccurred(message);
    return false;
}

bool Client::waitConnected()
{
    if (!d_->socket) {
        return false;
    }

    if (d_->socket->state() != QAbstractSocket::ConnectedState) {
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        connect(d_->socket,
                &QAbstractSocket::connected,
                &loop,
                &QEventLoop::quit);

        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start(d_->timeout);
        loop.exec();
    }

    if (d_->socket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }

    return false;
}

int Client::send(const QByteArray &ba)
{
    QIODevice *io = NULL;
    if (d_->serial) {
        io = d_->serial;
    } else if (d_->socket) {
        io = d_->socket;
    }

    if (io) {
        // clear buffer before sending
        int buff_len = io->bytesAvailable();
        if (buff_len) {
            io->readAll();
        }

        io->write(ba);
        if (!io->waitForBytesWritten(d_->timeout)) {
            qDebug() << "[Client] send timeout!";
            d_->error = E_CLIENT_TIMEOUT;
        }

    } else {
        d_->error = E_CLIENT_OPEN_FAILED;
    }

    return d_->error;
}

int Client::receive(QByteArray &ba, int expected_recv_len)
{
    QIODevice *io = NULL;
    if (d_->serial) {
        io = d_->serial;
    } else if (d_->socket) {
        io = d_->socket;
    }

    if (io) {
        int prev_len = 0;
        int char_timeout = 0;
        int timeout_count = 0;

        // check current length of available data
        io->waitForReadyRead(g_wait_timeout);
        int cur_len = prev_len = io->bytesAvailable();

        // if current length less than expected length, wait until timeout
        while (cur_len < expected_recv_len && timeout_count < d_->timeout) {
            // check current length of available data
            if (!io->waitForReadyRead(g_wait_timeout)) {
                timeout_count += g_wait_timeout;
            }

            cur_len = io->bytesAvailable();

            // if system receive no more data over specific timeout, it regards
            // as the end of data
            if (cur_len > 0) {
                if (prev_len != cur_len) {
                    char_timeout = 0;
                    prev_len = cur_len;
                } else {
                    char_timeout++;
                    // 50 * 3.5 character time
                    if (char_timeout > g_char_timeout) {
                        break;
                    }
                }
            }
        }

        // check timeout
        if (timeout_count >= d_->timeout) {
            qDebug() << "[Client] receive timeout!";
            d_->error = E_CLIENT_TIMEOUT;

        } else {
            // check receive
            if (cur_len) {
                ba.clear();
                ba.append(io->readAll());
            }

            if (expected_recv_len >= 0) {
                if (expected_recv_len != ba.length()) {
                    d_->error = E_CLIENT_RECV_FAILED;
                }
            }
        }

    } else {
        d_->error = E_CLIENT_OPEN_FAILED;
    }

    return d_->error;
}

QByteArray Client::transaction(const Protocol::Property &prop)
{
    QByteArray extract_ba;

    // simulation
    if (d_->conn->IsSimulation()) {
        extract_ba = d_->Convert(d_->protocol->Simulate(prop));
    }
    // online
    else {
        // create send message
        QByteArray send_ba = d_->Convert(d_->protocol->GenerateRequest(prop));
        if (E_NO_ERROR != d_->error) {
            return extract_ba;
        }

        // wait command delay
        QThread::msleep(d_->delay);

        // send data
        if (E_NO_ERROR != send(send_ba)) {
            return extract_ba;
        }

        // check if skip response or not
        if (d_->is_write_only) {
            return extract_ba;
        }

        // get expected response length
        int expected_recv_len = d_->protocol->ExpectedResponseLength(prop);

        // receive data
        QByteArray recv_ba;
        if (E_NO_ERROR != receive(recv_ba, expected_recv_len)) {
            return extract_ba;
        }

        // extract response message
        VecU8 response(recv_ba.constBegin(), recv_ba.constEnd());
        extract_ba = d_->Convert(d_->protocol->HandleResponse(prop, response));
    }

    return extract_ba;
}

void Client::preprocess(const Protocol::Property &prop)
{
    Q_UNUSED(prop)
}

void Client::postprocess(const Protocol::Property &prop)
{
    Q_UNUSED(prop)
}

void Client::handleError(int &retry)
{
    // check if reach retry limit
    if (retry < d_->retry) {
        return;
    }

    // query user to reconnect when error is timeout
    if (d_->error != E_CLIENT_TIMEOUT) {
        return;
    }

    if (!d_->is_reconn_enabled) {
        return;
    }

    // show query message
    bool ret = false;
    // Qt::ConnectionType type = Qt::AutoConnection;
    // if (this->thread() != qApp->thread()) {
    //     type = Qt::BlockingQueuedConnection;
    // }
    //
    // QMetaObject::invokeMethod(
    //     invoker_,
    //     "IsQueryAccepted",
    //     type,
    //     Q_RETURN_ARG(bool, ret),
    //     Q_ARG(QWidget *, NULL),
    //     Q_ARG(QString, tr("Connection")),
    //     Q_ARG(QString, tr("Connection failed!\nDo you want to reconnect?")));

    if (ret) {
        close();
        if (createClient()) {
            retry = -1;
        }
    }
}

void Client::Connect()
{
    if (open()) {
        d_->mutex.lock();
        d_->is_connected = true;
        d_->mutex.unlock();

        emit SigConnected();

    } else {
        Disconnect();
    }
}

void Client::Disconnect()
{
    d_->mutex.lock();
    d_->is_connected = false;
    d_->mutex.unlock();

    emit SigDisconnected();
}

void Client::Request(const Protocol::Property &prop)
{
    d_->mutex.lock();
    d_->prop_list.push_back(prop);
    d_->mutex.unlock();

    emit SigRequested();
}

void Client::process()
{
    if (d_->prop_list.empty()) {
        return;
    }

    if (!d_->is_connected) {
        return;
    }

    d_->mutex.lock();
    Protocol::Property prop = d_->prop_list.front();
    d_->prop_list.pop_front();
    d_->mutex.unlock();

    // preprocess request data
    preprocess(prop);

    // update station number
    prop.SetStation(d_->station);

    // fragment property to avoid that too long data transfer
    PropertyList split_list = d_->protocol->Fragment(prop);
    if (split_list.empty()) {
        d_->error = E_INVALID_REQUEST;
    }

    QByteArray proc_ba;
    for (const Protocol::Property &split_prop : split_list) {
        QByteArray ba;
        int retry = -1;
        while (retry++ < d_->retry) {
            // notify retry counts
            emit SigRetryOccurred(retry);

            // reset flag and data
            d_->error = E_NO_ERROR;

            // process command package
            ba = transaction(split_prop);

            // check result
            if (d_->error == E_NO_ERROR) {
                // collect processed data
                proc_ba.append(ba);
                break;
            }

            handleError(retry);
        }

        if (d_->error != E_NO_ERROR) {
            break;
        }
    }

    // postprocess request data
    postprocess(prop);

    // emit result
    emit SigResponse(prop, d_->error);
}
}  // namespace Communication
