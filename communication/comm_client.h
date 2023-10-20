#ifndef COMM_CLIENT_H
#define COMM_CLIENT_H

#include <QMutex>
#include <QObject>

class QAbstractSocket;
class QIODevice;
class QSerialPort;

namespace Protocol {
class AbstractProtocol;
class Property;
}  // namespace Protocol

namespace Communication {
class Connection;
class ClientData;
}  // namespace Communication

namespace Communication {
class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(const Connection &conn, QObject *parent = 0);
    ~Client();

    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    bool IsConnected() const;

private:
    bool open();
    void close();
    bool createProtocol();
    bool createClient();
    bool waitConnected();
    int send(const QByteArray &ba);
    int receive(QByteArray &ba, int expected_recv_len);
    QByteArray transaction(const Protocol::Property &prop);
    void preprocess(const Protocol::Property &prop);
    void postprocess(const Protocol::Property &prop);
    void handleError(int &retry);

private:
    ClientData *d_;

signals:
    void SigConnected();
    void SigDisconnected();
    void SigRequested();
    void SigResponse(const Protocol::Property &prop, int status);
    void SigRetryOccurred(int retry_times);
    void SigErrorOccurred(const QString &message);

public slots:
    void Connect();
    void Disconnect();
    void Request(const Protocol::Property &prop);

private slots:
    void process();
};
}  // namespace Communication

#endif  // COMM_CLIENT_H
