#ifndef COMM_PARAMETER_H
#define COMM_PARAMETER_H

#include <QHostAddress>
#include <QSerialPort>
#include <QString>

namespace Communication {
namespace Parameter {
class SerialData;
class SocketData;

using BaudRateType = int;
using ParityType = QSerialPort::Parity;
using DataBitsType = QSerialPort::DataBits;
using StopBitsType = QSerialPort::StopBits;
using FlowControlType = QSerialPort::FlowControl;

//enum BaudRateType
//{
//    E_UNKNOWN_BAUD = -1,
//    E_BAUD_1200 = 1200,
//    E_BAUD_2400 = 2400,
//    E_BAUD_4800 = 4800,
//    E_BAUD_9600 = 9600,
//    E_BAUD_19200 = 19200,
//    E_BAUD_38400 = 38400,
//    E_BAUD_57600 = 57600,
//    E_BAUD_115200 = 115200,
//};

//enum ParityType
//{
//    E_UNKNOWN_PARITY = -1,
//    E_NO_PARITY = 0,
//    E_EVEN_PARITY = 2,
//    E_ODD_PARITY = 3
//};

//enum DataBitsType
//{
//    E_UNKNOWN_DATABITS = -1,
//    E_DATA5 = 5,
//    E_DATA6 = 6,
//    E_DATA7 = 7,
//    E_DATA8 = 8
//};

//enum StopBitsType
//{
//    E_UNKNOWN_STOP_BITS = -1,
//    E_STOP_BITS_1 = 1,
//    E_STOP_BITS_2 = 2,
//    E_STOP_BITS_1P5 = 3
//};

//enum FlowControlType
//{
//    E_UNKNOWN_FLOW_CONTROL = -1,
//    E_NO_FLOW_CONTROL = 0,
//    E_HW_CONTROL = 1,
//    E_SW_CONTROL = 2
//};

enum SocketType
{
    E_TCP,
    E_UDP
};

class Serial
{
public:
    Serial();
    ~Serial();

    // copy constructor and assignment operator
    Serial(const Serial &source);
    Serial &operator=(const Serial &source);

    // get and set the display text of parameter
    QString ParameterText(const QString &name) const;
    bool SetParameterText(const QString &name, const QString &value);

    // get parameter
    int Port() const;
    BaudRateType Baudrate() const;
    ParityType Parity() const;
    DataBitsType DataBits() const;
    StopBitsType StopBits() const;
    FlowControlType FlowControl() const;

    // set parameter
    void SetPort(int port);
    void SetBaudrate(BaudRateType baudrate);
    void SetParity(ParityType parity);
    void SetDataBits(DataBitsType databits);
    void SetStopBits(StopBitsType stopbits);
    void SetFlowControl(FlowControlType flow_control);

private:
    SerialData *d_;
};

class Socket
{
public:
    Socket();
    ~Socket();

    // copy constructor and assignment operator
    Socket(const Socket &source);
    Socket &operator=(const Socket &source);

    // get and set the display text of parameter
    QString ParameterText(const QString &name) const;
    bool SetParameterText(const QString &name, const QString &value);

    // get parameter
    SocketType Type() const;
    QHostAddress Address() const;
    QHostAddress LocalAddress() const;
    QString IPv4() const;
    QString LocalIPv4() const;
    uint16_t Port() const;
    uint16_t LocalPort() const;

    // set parameter
    void SetType(SocketType type);
    void SetHostAddress(const QHostAddress &addr);
    void SetLocalAddress(const QHostAddress &addr);
    void SetIPv4(const QString &ipv4);
    void SetLocalIPv4(const QString &ipv4);
    void SetPort(uint16_t port);
    void SetLocalPort(uint16_t port);

private:
    SocketData *d_;
};

}  // namespace Parameter
}  // namespace Communication

#endif  // COMM_PARAMETER_H
