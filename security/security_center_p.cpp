#include "security_center_p.h"
#include <QDebug>
#include <QTime>
#include "openssl_helper.h"
#include "security_center.h"

using namespace OpenSSLHelper;

namespace {
constexpr int g_aes128_len = 16;
const QByteArrayList g_randoms =
    {"QuGH78G3UwYBNsqsegKFxhUK8uV2kzF4", "TgTVES5maWMn9N7Kt7Nz977VPtkp92nh",
     "c4XgyEQK2DsBXh8TVe6UGN29QQF48Y3W", "8nB3xHFUzDarqn89qFnPK4UMtTQvKhkX",
     "tUEdr98bgRyqKcknPEyHcKBZqarBX93p", "HquKse42WBhqb7Zax4eK594bB5SCkqHB",
     "a7wHwFN4rBDGvtQbuS8yp5CysgTNfYnu", "WevepAPx5W9YR93NR2WYrQhkvqPN24xu",
     "cpcXEWWpWGq2DGvcZqBbs2DKCA6U9G9x", "HyHvs3trhPS5ExFMyuVytQCTxazDZFxs",
     "yEv2E94xpQukXG6P62rhtYGA5ZWmr9Z5", "Qa4WpsUXYAwnaE6ChyFVydnkHDh2WAs3",
     "wCNbhkz5vzsQRfUU2fq3n7H4pfk2w82X", "ScaCsFxubQsVRPcEe4M4rTZpWevqVNM5",
     "nSW5gWgNe9E9Z9q8A3YV5dHSqfFpQNwE", "sxvyggpGhYgs9EByvWCTvVuBY582CtRZ",
     "uTcexzaxmFPwButBaUyH6bEpYyWaFvNE", "vShxFHMqURFSpFnP6twsQQTbrDqzvMpR",
     "BCTkWQmA4rwKZvpXdzCS2dzateKWX64P", "PEMrxxegYZxQuqmyPgDDBzmQFmPWZ25E",
     "EBXBEsxWCrMNKu8R5RkVXh99DT9kxCtM", "XarC7NyRGVgYnwZgMEm6uDqxBtX9m8WY",
     "7dzZtA7TuBc4AZERUWmxqrM6vBXe7C3y", "9HVdfdZaQaARKPpdnSm7qtkvwMxZargg",
     "pyBRkGrxV53UWER7NKtqRBP6kasRgCFu", "6BZcFqM8G5Qa3c2snpDB55nZVVPUpFhC",
     "gwRPDMRP3wb5S6cQgCR4sbdFHX3TDvAc", "DAWrNwmPEDtFS38Vg2gR6F3P2euwyMzF",
     "5x49Dx7Ncu5uadcvgnbTcAQ3GyGqHZQ6", "xsPp2XuGwV32yn5MCKgdEVmT2WNwHUnZ",
     "Frp4nQpQfyBzvTHysmAq6WXGDqZUsmQC", "3sQns4pwd9kFWhH95pcZ5bVZr3ecVh5V"};
}  // namespace

SecurityCenterPrivate::SecurityCenterPrivate(SecurityCenter *q)
  : q_ptr_(q)
{
}

SecurityCenterPrivate::~SecurityCenterPrivate() {}

QByteArray SecurityCenterPrivate::RandomString(int len)
{
    const std::string str =
        "1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";

    const int str_size = str.size();

    QByteArray ba;
    QTime t(0, 0, 0);
    int seed = t.msecsTo(QTime::currentTime());
    qsrand(seed);
    for (int i = 0; i < len; ++i) {
        int index = qrand() % str_size;
        ba.append(str.at(index));
    }

    return ba;
}

QByteArray SecurityCenterPrivate::DefaultAESData(
    SecurityCenterPrivate::AESDataType type)
{
    QByteArray ba = aes_data_map.value(type);
    if (ba.isEmpty()) {
        switch (type) {
            case E_AES_KEY: {
                int spec_num = 0x4B + 0x45 + 0x59;  // 'K'+'E'+'Y'
                int index = spec_num % (g_randoms.size());
                const QByteArray &random = g_randoms.at(index);
                ba = random.left(g_aes128_len);
            } break;
            case E_AES_IV: {
                int spec_num = 0x49 + 0x56;  // 'I'+'V'
                int index = spec_num % (g_randoms.size());
                const QByteArray &random = g_randoms.at(index);
                ba = random.left(g_aes128_len);
            } break;
            case E_AES_VERF: {
                int spec_num = 0x56 + 0x45 + 0x52 + 0x46;  // 'V'+'E'+'R'+'F'
                int index = spec_num % (g_randoms.size());
                const QByteArray &random = g_randoms.at(index);
                ba = random.left(g_aes128_len);
            } break;
            case E_AES_PSW:
                ba = QByteArray(g_aes128_len, 0xFF);
                break;
            default:
                break;
        }
    }

    return ba;
}

bool SecurityCenterPrivate::SetDefaultAESData(
    SecurityCenterPrivate::AESDataType type,
    const QByteArray &ba)
{
    if (ba.size() == g_aes128_len) {
        aes_data_map.insert(type, ba);
        return true;
    }

    qDebug() << QString("[SecurityCenterPrivate] wrong AES length = %1(%2)")
                    .arg(ba.size())
                    .arg(g_aes128_len);

    return false;
}

bool SecurityCenterPrivate::TestAES()
{
    QByteArray in_ba = "QWERasdfZXCV";
    QByteArray enc_in_ba;
    QByteArray out_ba;
    QByteArray key = "1234567890";
    QByteArray iv = AES::GenerateIV();
    if (!AES::CryptoByteArray(in_ba, enc_in_ba, key, iv, true)) {
        qDebug() << "[TestAES] CryptoByteArray encrypt failed";
        return false;
    }

    if (!AES::CryptoByteArray(enc_in_ba, out_ba, key, iv, false)) {
        qDebug() << "[TestAES] CryptoByteArray decrypt failed";
        return false;
    }

    QStringList dumps;
    dumps << QString("[in](%1) %2 | HEX=%3")
                 .arg(in_ba.size())
                 .arg(QString(in_ba))
                 .arg(QString(in_ba.toHex()));

    dumps << QString("[enc in](%1) %2 | HEX=%3")
                 .arg(enc_in_ba.size())
                 .arg(QString(enc_in_ba))
                 .arg(QString(enc_in_ba.toHex()));

    dumps << QString("[key](%1) %2 | HEX=%3")
                 .arg(key.size())
                 .arg(QString(key))
                 .arg(QString(key.toHex()));

    dumps << QString("[iv](%1) %2 | HEX=%3")
                 .arg(iv.size())
                 .arg(QString(iv))
                 .arg(QString(iv.toHex()));

    dumps << QString("[out](%1) %2 | HEX=%3")
                 .arg(out_ba.size())
                 .arg(QString(out_ba))
                 .arg(QString(out_ba.toHex()));

    for (const QString &dump : dumps) {
        qDebug() << dump;
    }

    return true;
}

bool SecurityCenterPrivate::TestECDSA()
{
    QByteArray msg = RandomString(64);
    QByteArray key;
    QByteArray sig;

    if (!ECDSA::GenerateSignature(msg, key, sig)) {
        qDebug() << "[TestECDSA] GenerateSignature failed";
        return false;
    }

    if (!ECDSA::VerifySignature(msg, key, sig)) {
        qDebug() << "[TestECDSA] VerifySignature failed";
        return false;
    }

    QStringList dumps;
    dumps << QString("[msg](%1) %2").arg(msg.size()).arg(QString(msg));

    dumps << QString("[key](%1) HEX=%2")
                 .arg(key.size())
                 .arg(QString(key.toHex()));

    dumps << QString("[sig](%1) HEX=%2")
                 .arg(sig.size())
                 .arg(QString(sig.toHex()));

    for (const QString &dump : dumps) {
        qDebug() << dump;
    }

    return true;
}
