#ifndef SECURITY_CENTER_PRIVATE_H
#define SECURITY_CENTER_PRIVATE_H

#include <QMap>

class SecurityCenter;

class SecurityCenterPrivate
{
public:
    explicit SecurityCenterPrivate(SecurityCenter *q);
    ~SecurityCenterPrivate();

    QByteArray RandomString(int len);

    enum AESDataType
    {
        E_AES_KEY,
        E_AES_IV,
        E_AES_VERF,
        E_AES_PSW
    };

    QByteArray DefaultAESData(AESDataType type);
    bool SetDefaultAESData(AESDataType type, const QByteArray &ba);

    bool TestAES();
    bool TestECDSA();

private:
    SecurityCenter *q_ptr_;
    QMap<AESDataType, QByteArray> aes_data_map;
};

#endif  // SECURITY_CENTER_PRIVATE_H
