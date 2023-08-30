#include "security_center.h"
#include "security_center_p.h"

SecurityCenter::SecurityCenter(QObject *parent)
  : QObject(parent)
  , d_ptr_(new SecurityCenterPrivate(this))
{
}

SecurityCenter::~SecurityCenter()
{
    if (d_ptr_) {
        delete d_ptr_;
        d_ptr_ = NULL;
    }
}

SecurityCenter *SecurityCenter::Instance()
{
    static SecurityCenter instance;
    return &instance;
}

void SecurityCenter::Test()
{
    d_ptr_->TestAES();
    d_ptr_->TestECDSA();
}
