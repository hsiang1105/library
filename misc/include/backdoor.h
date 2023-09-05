#ifndef BACKDOOR_H
#define BACKDOOR_H

#include <QObject>

class QtMessageForwarder : public QObject
{
    Q_OBJECT
public:
    explicit QtMessageForwarder(QObject *parent = 0);
    void ForwardMessage(const QString &text, int type = QtDebugMsg);

signals:
    void MessageForwarded(const QString &text, int type = QtDebugMsg);
};

namespace MiscHelper {
namespace Backdoor {

enum Mode
{
    E_FILE_LOG_MODE,
    E_FORWARDER_MODE,
    E_PREVIEW_MODE
};

bool HasBackdoor(Mode mode);
QString BackdoorName(Mode mode);
bool IsFileLogMode();
bool IsForwarderMode();
bool IsPreviewMode();
void RedirectQtMessage();
QtMessageForwarder *CurrentForwarder();

}  // namespace Backdoor
}  // namespace MiscHelper

#endif  // BACKDOOR_H
