#include "backdoor.h"
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

using namespace MiscHelper;

namespace {
QtMessageForwarder g_forwarder;

QString preprocessedMessage(QtMsgType type, const QString &text)
{
    QString msg = QString("%1:  %2");
    switch (type) {
        case QtDebugMsg:
            msg = msg.arg("DEBUG").arg(text);
            break;
        case QtWarningMsg:
            msg = msg.arg("WARNING").arg(text);
            break;
        case QtCriticalMsg:
            msg = msg.arg("CRITICAL").arg(text);
            break;
        case QtFatalMsg:
            msg = msg.arg("FATAL").arg(text);
            break;
        case QtInfoMsg:
            msg = msg.arg("INFO").arg(text);
            break;
        default:
            msg = msg.arg("UNKNKOWN").arg(text);
            break;
    }

    return msg;
}

void redirectFileHandler(QtMsgType type,
                         const QMessageLogContext &,
                         const QString &msg)
{
    QString format("yyyy-MM-dd THH:mm:ss.zzz");
    QString date = QDateTime::currentDateTime().toString(format);
    QString proc_msg =
        QString("(%1) %2\r\n").arg(date).arg(preprocessedMessage(type, msg));

    QFile file("app.log");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&file);
    ts << proc_msg;
}

void redirectForwarderHandler(QtMsgType type,
                              const QMessageLogContext &,
                              const QString &msg)
{
    QString proc_msg = preprocessedMessage(type, msg);
    g_forwarder.ForwardMessage(proc_msg, type);
}
}  // namespace

bool Backdoor::HasBackdoor(Backdoor::Mode mode)
{
    QString name = BackdoorName(mode);
    QString path =
        QString("%1/_backdoor/%2").arg(qApp->applicationDirPath()).arg(name);

    QFile file(path);
    if (file.exists()) {
        return true;
    }

    return false;
}

QString Backdoor::BackdoorName(Backdoor::Mode mode)
{
    QString name;

    switch (mode) {
        case E_FILE_LOG_MODE:
            name = "_file_log_mode_";
            break;
        case E_FORWARDER_MODE:
            name = "_forwarder_mode_";
            break;
        case E_PREVIEW_MODE:
            name = "_preview_mode_";
            break;
        default:
            break;
    }

    return name;
}

bool Backdoor::IsFileLogMode()
{
    bool res = HasBackdoor(E_FILE_LOG_MODE);
    return res;
}

bool Backdoor::IsForwarderMode()
{
    bool res = HasBackdoor(E_FORWARDER_MODE);
    return res;
}

bool Backdoor::IsPreviewMode()
{
    bool res = HasBackdoor(E_PREVIEW_MODE);
    return res;
}

void Backdoor::RedirectQtMessage()
{
    if (IsFileLogMode()) {
        qInstallMessageHandler(redirectFileHandler);
    } else if (IsForwarderMode()) {
        qInstallMessageHandler(redirectForwarderHandler);
    }
}

QtMessageForwarder *Backdoor::CurrentForwarder()
{
    return &g_forwarder;
}

QtMessageForwarder::QtMessageForwarder(QObject *parent)
  : QObject(parent)
{
}

void QtMessageForwarder::ForwardMessage(const QString &text, int type)
{
    emit MessageForwarded(text, type);
}
