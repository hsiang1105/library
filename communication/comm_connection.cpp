#include "comm_connection.h"
#include "comm_parameter.h"

namespace Communication {
class ConnectionData
{
public:
    ConnectionData()
      : type(E_UNKNOW)
      , name("N/A")
      , lib_name("N/A")
      , station(1)
      , delay(0)
      , timeout(5000)
      , retry(2)
      , serial_param(NULL)
      , socket_param(NULL)
    {
    }

    ConnectionType type;
    QString name;
    QString lib_name;
    int station;
    int delay;
    int timeout;
    int retry;
    Parameter::Serial *serial_param;
    Parameter::Socket *socket_param;
};

Connection::Connection(ConnectionType type,
                       const QString &name,
                       const QString &lib_name)
  : d_(new ConnectionData)
{
    d_->type = type;
    d_->name = name;
    d_->lib_name = lib_name;

    switch (d_->type) {
        case E_SERIAL:
            d_->serial_param = new Parameter::Serial;
            break;

        case E_SOCKET:
            d_->socket_param = new Parameter::Socket;
            break;

        default:
            break;
    }
}

Connection::~Connection()
{
    if (d_) {
        delete d_;
        d_ = NULL;
    }
}

Connection::Connection(const Connection &source)
  : d_(new ConnectionData)
{
    d_->type = source.Type();
    d_->name = source.Name();
    d_->lib_name = source.LibName();
    d_->station = source.Station();
    d_->delay = source.Delay();
    d_->timeout = source.Timeout();
    d_->retry = source.Retry();

    switch (d_->type) {
        case E_SERIAL:
            d_->serial_param = new Parameter::Serial;
            *d_->serial_param = *source.SerialParameter();
            break;

        case E_SOCKET:
            d_->socket_param = new Parameter::Socket;
            *d_->socket_param = *source.SocketParameter();
            break;

        default:
            break;
    }
}

Connection &Connection::operator=(const Connection &source)
{
    if (this != &source) {
        // copy properties
        d_->type = source.Type();
        d_->name = source.Name();
        d_->lib_name = source.LibName();
        d_->station = source.Station();
        d_->delay = source.Delay();
        d_->timeout = source.Timeout();
        d_->retry = source.Retry();

        // delete old data
        if (d_->serial_param) {
            delete d_->serial_param;
            d_->serial_param = NULL;
        }

        if (d_->socket_param) {
            delete d_->socket_param;
            d_->socket_param = NULL;
        }

        // new data
        switch (d_->type) {
            case E_SERIAL:
                d_->serial_param = new Parameter::Serial;
                *d_->serial_param = *source.SerialParameter();
                break;

            case E_SOCKET:
                d_->socket_param = new Parameter::Socket;
                *d_->socket_param = *source.SocketParameter();
                break;

            default:
                break;
        }
    }

    return *this;
}

QString Connection::ParameterText(const QString &name) const
{
    QString value = "N/A";

    if (name == "Type") {
        switch (d_->type) {
            case E_SERIAL:
                value = QObject::tr("Serial");
                break;

            case E_SOCKET:
                if (d_->socket_param->Type() == Parameter::E_UDP) {
                    value = QObject::tr("UDP");
                } else {
                    value = QObject::tr("TCP");
                }
                break;

            case E_SIMULATION:
                value = QObject::tr("Simulation");
                break;

            default:
                break;
        }

    } else if (name == "Name") {
        value = d_->name;

    } else if (name == "LibName") {
        value = d_->lib_name;

    } else if (name == "Station") {
        value = QString::number(d_->station);

    } else if (name == "Delay") {
        value = QString::number(d_->delay);

    } else if (name == "Timeout") {
        value = QString::number(d_->timeout);

    } else if (name == "Retry") {
        value = QString::number(d_->retry);

    } else {
        if (d_->serial_param) {
            value = d_->serial_param->ParameterText(name);
        } else if (d_->socket_param) {
            value = d_->socket_param->ParameterText(name);
        }
    }

    return value;
}

bool Connection::SetParameterText(const QString &name, const QString &value)
{
    if (name == "Name") {
        SetName(value);
        return true;

    } else if (name == "LibName") {
        SetLibName(value);
        return true;

    } else if (name == "Station") {
        int num = value.toInt();
        SetStation(num);
        return true;

    } else if (name == "Delay") {
        int num = value.toInt();
        SetDelay(num);
        return true;

    } else if (name == "Timeout") {
        int num = value.toInt();
        SetTimeout(num);
        return true;

    } else if (name == "Retry") {
        int num = value.toInt();
        SetRetry(num);
        return true;

    } else {
        if (d_->serial_param) {
            return d_->serial_param->SetParameterText(name, value);
        } else if (d_->socket_param) {
            return d_->socket_param->SetParameterText(name, value);
        }
    }

    return false;
}

ConnectionType Connection::Type() const
{
    return d_->type;
}

QString Connection::Name() const
{
    return d_->name;
}

QString Connection::LibName() const
{
    return d_->lib_name;
}

int Connection::Station() const
{
    return d_->station;
}

int Connection::Delay() const
{
    return d_->delay;
}

int Connection::Timeout() const
{
    return d_->timeout;
}

int Connection::Retry() const
{
    return d_->retry;
}

void Connection::SetName(const QString &name)
{
    d_->name = name;
}

void Connection::SetLibName(const QString &lib_name)
{
    d_->lib_name = lib_name;
}

void Connection::SetStation(int station)
{
    d_->station = station;
}

void Connection::SetDelay(int delay)
{
    d_->delay = delay;
}

void Connection::SetTimeout(int timeout)
{
    d_->timeout = timeout;
}

void Connection::SetRetry(int retry)
{
    d_->retry = retry;
}

bool Connection::IsValid() const
{
    return (d_->type != E_UNKNOW);
}

bool Connection::IsSimulation() const
{
    return (d_->type == E_SIMULATION);
}

const Parameter::Serial *Connection::SerialParameter() const
{
    return d_->serial_param;
}

const Parameter::Socket *Connection::SocketParameter() const
{
    return d_->socket_param;
}
}  // namespace Communication
