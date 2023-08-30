#ifndef OPENSSL_HELPER_H
#define OPENSSL_HELPER_H

#include <QDataStream>

namespace OpenSSLHelper {
namespace AES {
QByteArray GenerateIV();
bool CryptoStream(QDataStream &in,
                  QDataStream &out,
                  QByteArray key,
                  QByteArray iv,
                  bool encrypt);

bool CryptoByteArray(const QByteArray &in_ba,
                     QByteArray &out_ba,
                     QByteArray key,
                     QByteArray iv,
                     bool encrypt);

}  // namespace AES

namespace ECDSA {
bool GenerateSignature(const QByteArray &msg,
                       QByteArray &key,
                       QByteArray &signature,
                       bool der_format = false);

bool VerifySignature(const QByteArray &msg,
                     const QByteArray &key,
                     const QByteArray &signature,
                     bool der_format = false);

}  // namespace ECDSA

QByteArray CalculateSha256(const QByteArray &msg);

}  // namespace OpenSSLHelper

#endif  // OPENSSL_HELPER_H
