#include "comm_parameter.h"
#include <QRegExp>

#if defined(VIRTUAL_SERIAL_SUPPORT)
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

#ifdef Q_OS_WIN
#include "windows.h"

namespace {
typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

    LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
    fnIsWow64Process =
        (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")),
                                            "IsWow64Process");

    if (NULL != fnIsWow64Process) {
        if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
            // handle error
            qDebug() << "!!!ERROR";
        }
    }
    return bIsWow64;
}

bool SearchVirtualSerial(QString &port_name)
{
    QSerialPort *client = NULL;
    for (QSerialPortInfo info : QSerialPortInfo::availablePorts()) {
        // check vendor id
        if (info.vendorIdentifier() == 0x30E3) {
            // check product id
            if (info.productIdentifier() == 0x1001) {
                // create client
                client = new QSerialPort(info);

                int retry_count = 0;
                while (retry_count++ < 2) {
                    if (client->open(QSerialPort::ReadWrite)) {
                        port_name = info.portName();

                        // release client
                        client->close();
                        delete client;
                        client = Q_NULLPTR;

                        return true;
                    }

                    // remove reset usb
                    // qDebug()<<"Error: reset USB";
                    // ResetVirtualSerial();
                }

                qDebug() << "Error: USB open fail!";

                // release client
                if (client) {
                    client->close();
                    delete client;
                    client = Q_NULLPTR;
                }
            }
        }
    }

    return false;
}

void ResetVirtualSerial()
{
    QString operate("runas");
    QString parameter("restart *VID_30E3*PID_1001*");
    QString path("/utility");

    path.prepend(QApplication::applicationDirPath());
    if (QSysInfo::windowsVersion() < QSysInfo::WV_VISTA) {
        if (IsWow64())
            path.append("/restrUSBToolXP64.exe");
        else
            path.append("/restrUSBToolXP32.exe");

    } else {
        if (IsWow64())
            path.append("/restrUSBTool64.exe");
        else
            path.append("/restrUSBTool32.exe");
    }

    SHELLEXECUTEINFO shexecinfo;
    memset(&shexecinfo, 0, sizeof(shexecinfo));
    shexecinfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shexecinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shexecinfo.hwnd = NULL;
    shexecinfo.lpVerb = operate.toStdWString().c_str();
    shexecinfo.lpFile = path.toStdWString().c_str();
    shexecinfo.lpParameters = parameter.toStdWString().c_str();
    shexecinfo.lpDirectory = NULL;
    shexecinfo.nShow = SW_HIDE;
    shexecinfo.hInstApp = NULL;
    int ret = ShellExecuteEx(&shexecinfo);
    qDebug() << "[SerialParameter] resetUsb return code:" << ret;

    long timeout = WaitForSingleObject(shexecinfo.hProcess, 2000);
    qDebug() << "[SerialParameter] resetUsb timeout:" << timeout;
}
}  // namespace
#endif
#endif

namespace Communication {
namespace Parameter {
class SerialData
{
public:
    SerialData()
      : port(-1)
      , baudrate(QSerialPort::UnknownBaud)
      , parity(QSerialPort::UnknownParity)
      , databits(QSerialPort::UnknownDataBits)
      , stopbits(QSerialPort::UnknownStopBits)
      , flow_control(QSerialPort::UnknownFlowControl)
    {
    }

    int port;
    BaudRateType baudrate;
    ParityType parity;
    DataBitsType databits;
    StopBitsType stopbits;
    FlowControlType flow_control;
};

class SocketData
{
public:
    SocketData()
      : type(E_TCP)
      , addr(QHostAddress("192.168.0.1"))
      , local_addr(QHostAddress())
      , port(502)
      , local_port(0)
    {
    }

    SocketType type;
    QHostAddress addr;
    QHostAddress local_addr;
    uint16_t port;
    uint16_t local_port;
};

Serial::Serial()
  : d_(new SerialData)
{
}

Serial::~Serial()
{
    if (d_) {
        delete d_;
        d_ = NULL;
    }
}

Serial::Serial(const Serial &source)
  : d_(new SerialData)
{
    *d_ = *source.d_;
}

Serial &Serial::operator=(const Serial &source)
{
    if (this != &source) {
        *d_ = *source.d_;
    }
    return *this;
}

QString Serial::ParameterText(const QString &name) const
{
    QString value = "N/A";

    if (name == "Port") {
        value = QString("COM%1").arg(d_->port);

    } else if (name == "Baudrate") {
        value = QString::number(d_->baudrate);

    } else if (name == "Parity") {
        if (d_->parity == QSerialPort::NoParity) {
            value = "None";
        } else if (d_->parity == QSerialPort::OddParity) {
            value = "Odd";
        } else if (d_->parity == QSerialPort::EvenParity) {
            value = "Even";
        }

    } else if (name == "DataBits") {
        value = QString::number((int)d_->databits);

    } else if (name == "StopBits") {
        if (d_->stopbits == QSerialPort::OneStop) {
            value = "1";
        } else if (d_->stopbits == QSerialPort::OneAndHalfStop) {
            value = "1.5";
        } else if (d_->stopbits == QSerialPort::TwoStop) {
            value = "2";
        }

    } else if (name == "FlowControl") {
        if (d_->flow_control == QSerialPort::NoFlowControl) {
            value = "None";
        } else if (d_->flow_control == QSerialPort::HardwareControl) {
            value = "Hardware";
        } else if (d_->flow_control == QSerialPort::SoftwareControl) {
            value = "Software";
        }
    }

    return value;
}

bool Serial::SetParameterText(const QString &name, const QString &value)
{
    bool ok = false;
    int num = 0;
    if (name == "Port") {
        if (value.startsWith("COM", Qt::CaseInsensitive)) {
            num = value.mid(3).toInt(&ok);
            if (ok) {
                SetPort(num);
                return true;
            }
        }

    } else if (name == "Baudrate") {
        num = value.toInt(&ok);
        if (ok) {
            SetBaudrate(num);
            return true;
        }

    } else if (name == "Parity") {
        if (value.toUpper() == "NONE") {
            SetParity(QSerialPort::NoParity);
            return true;
        } else if (value.toUpper() == "ODD") {
            SetParity(QSerialPort::OddParity);
            return true;
        } else if (value.toUpper() == "EVEN") {
            SetParity(QSerialPort::EvenParity);
            return true;
        }

    } else if (name == "DataBits") {
        num = value.toInt(&ok);
        if (ok) {
            SetDataBits((DataBitsType)num);
            return true;
        }

    } else if (name == "StopBits") {
        if (value == "1") {
            SetStopBits(QSerialPort::OneStop);
            return true;
        } else if (value == "1.5") {
            SetStopBits(QSerialPort::OneAndHalfStop);
            return true;
        } else if (value == "2") {
            SetStopBits(QSerialPort::TwoStop);
            return true;
        }

    } else if (name == "FlowControl") {
        if (value.toUpper() == "NONE") {
            SetFlowControl(QSerialPort::NoFlowControl);
            return true;
        } else if (value.toUpper() == "HARDWARE") {
            SetFlowControl(QSerialPort::HardwareControl);
            return true;
        } else if (value.toUpper() == "SOFTWARE") {
            SetFlowControl(QSerialPort::SoftwareControl);
            return true;
        }
    }

    return false;
}

int Serial::Port() const
{
    return d_->port;
}

BaudRateType Serial::Baudrate() const
{
    return d_->baudrate;
}

ParityType Serial::Parity() const
{
    return d_->parity;
}

DataBitsType Serial::DataBits() const
{
    return d_->databits;
}

StopBitsType Serial::StopBits() const
{
    return d_->stopbits;
}

FlowControlType Serial::FlowControl() const
{
    return d_->flow_control;
}

void Serial::SetPort(int port)
{
    d_->port = port;
}

void Serial::SetBaudrate(BaudRateType baudrate)
{
    d_->baudrate = baudrate;
}

void Serial::SetParity(ParityType parity)
{
    d_->parity = parity;
}

void Serial::SetDataBits(DataBitsType databits)
{
    d_->databits = databits;
}

void Serial::SetStopBits(StopBitsType stopbits)
{
    d_->stopbits = stopbits;
}

void Serial::SetFlowControl(FlowControlType flow_control)
{
    d_->flow_control = flow_control;
}

Socket::Socket()
  : d_(new SocketData)
{
}

Socket::~Socket()
{
    if (d_) {
        delete d_;
        d_ = NULL;
    }
}

Socket::Socket(const Socket &source)
  : d_(new SocketData)
{
    *d_ = *source.d_;
}

Socket &Socket::operator=(const Socket &source)
{
    if (this != &source) {
        *d_ = *source.d_;
    }
    return *this;
}

QString Socket::ParameterText(const QString &name) const
{
    QString value = "N/A";

    if (name == "Type") {
        if (d_->type == E_TCP) {
            value = "TCP";
        } else if (d_->type == E_UDP) {
            value = "UDP";
        }

    } else if (name == "Address") {
        value = d_->addr.toString();

    } else if (name == "IPv4") {
        value = IPv4();

    } else if (name == "Port") {
        value = QString::number(d_->port);

    } else if (name == "LocalAddress") {
        value = d_->local_addr.toString();

    } else if (name == "LocalIPv4") {
        value = LocalIPv4();

    } else if (name == "LocalPort") {
        value = QString::number(d_->local_port);
    }

    return value;
}

bool Socket::SetParameterText(const QString &name, const QString &value)
{
    if (name == "Type") {
        if (value.toUpper() == "TCP") {
            SetType(E_TCP);
            return true;
        } else if (value.toUpper() == "UDP") {
            SetType(E_UDP);
            return true;
        }

    } else if (name == "LocalAddress") {
        auto addr = QHostAddress(value);
        if (!addr.isNull()) {
            d_->addr = addr;
            return true;
        }

    } else if (name == "IPv4") {
        SetIPv4(value);
        if (!d_->addr.isNull()) {
            return true;
        }

    } else if (name == "Port") {
        bool ok = false;
        int num = value.toInt(&ok);
        if (ok) {
            SetPort(num);
            return true;
        }

    } else if (name == "LocalAddress") {
        auto addr = QHostAddress(value);
        if (!addr.isNull()) {
            d_->addr = addr;
            return true;
        }

    } else if (name == "LocalIPv4") {
        SetLocalIPv4(value);
        if (!d_->addr.isNull()) {
            return true;
        }

    } else if (name == "LocalPort") {
        bool ok = false;
        int num = value.toInt(&ok);
        if (ok) {
            SetLocalPort(num);
            return true;
        }
    }

    return false;
}

SocketType Socket::Type() const
{
    return d_->type;
}

QHostAddress Socket::Address() const
{
    return d_->addr;
}

QHostAddress Socket::LocalAddress() const
{
    return d_->local_addr;
}

QString Socket::IPv4() const
{
    QString ipv4;
    if (d_->addr.protocol() == QAbstractSocket ::IPv4Protocol) {
        ipv4 = d_->addr.toString();
    }

    return ipv4;
}

QString Socket::LocalIPv4() const
{
    QString ipv4;
    if (d_->local_addr.protocol() == QAbstractSocket ::IPv4Protocol) {
        ipv4 = d_->local_addr.toString();
    }

    return ipv4;
}

uint16_t Socket::Port() const
{
    return d_->port;
}

uint16_t Socket::LocalPort() const
{
    return d_->local_port;
}

void Socket::SetType(SocketType type)
{
    d_->type = type;
}

void Socket::SetHostAddress(const QHostAddress &addr)
{
    d_->addr = addr;
}

void Socket::SetLocalAddress(const QHostAddress &addr)
{
    d_->local_addr = addr;
}

void Socket::SetIPv4(const QString &ipv4)
{
    QRegExp exp(
        QString("%1\\.%1\\.%1\\.%1")
            .arg("(1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})"));

    if (exp.exactMatch(ipv4)) {
        QHostAddress addr(ipv4);
        if (addr.isNull() == false) {
            d_->addr = addr;
        }
    }
}

void Socket::SetLocalIPv4(const QString &ipv4)
{
    QRegExp exp(
        QString("%1\\.%1\\.%1\\.%1")
            .arg("(1{0,1}[0-9]{0,2}|2[0-4]{1,1}[0-9]{1,1}|25[0-5]{1,1})"));

    if (exp.exactMatch(ipv4)) {
        QHostAddress addr(ipv4);
        if (addr.isNull() == false) {
            d_->local_addr = addr;
        }
    }
}

void Socket::SetPort(uint16_t port)
{
    d_->port = port;
}

void Socket::SetLocalPort(uint16_t port)
{
    d_->local_port = port;
}

}  // namespace Parameter
}  // namespace Communication
