#ifndef COMM_CONNECTION_H
#define COMM_CONNECTION_H

#include <QString>

namespace Communication {
namespace Parameter {
class Serial;
class Socket;
}  // namespace Parameter

class ConnectionData;

enum ConnectionType
{
    E_UNKNOW,
    E_SERIAL,
    E_SOCKET,
    E_SIMULATION = 0xFF
};

class Connection
{
public:
    Connection(ConnectionType type = E_UNKNOW,
               const QString &name = QString(),
               const QString &lib_name = QString());

    ~Connection();

    // copy constructor and assignment operator
    Connection(const Connection &source);
    Connection &operator=(const Connection &source);

    // get and set the display text of parameter
    QString ParameterText(const QString &name) const;
    bool SetParameterText(const QString &name, const QString &value);

    // get parameter
    ConnectionType Type() const;
    QString Name() const;
    QString LibName() const;
    int Station() const;
    int Delay() const;
    int Timeout() const;
    int Retry() const;

    // set parameter
    void SetName(const QString &name);
    void SetLibName(const QString &lib_name);
    void SetStation(int station);
    void SetDelay(int delay);
    void SetTimeout(int timeout);
    void SetRetry(int retry);

    bool IsValid() const;
    bool IsSimulation() const;

    const Parameter::Serial *SerialParameter() const;
    const Parameter::Socket *SocketParameter() const;

private:
    ConnectionData *d_;
};
}  // namespace Communication

#endif  // COMM_CONNECTION_H
