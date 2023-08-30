#ifndef SECURITY_CENTER_H
#define SECURITY_CENTER_H

#include <QObject>

class SecurityCenterPrivate;
class SecurityCenter : public QObject
{
    Q_OBJECT

    explicit SecurityCenter(QObject *parent = 0);
    ~SecurityCenter();

    SecurityCenterPrivate *d_ptr_;

public:
    static SecurityCenter *Instance();

    void Test();
};

#define SEC_CTR SecurityCenter::Instance()

#endif  // SECURITY_CENTER_H
