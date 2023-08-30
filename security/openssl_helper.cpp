#include "openssl_helper.h"
#include <openssl/bn.h>
#include <openssl/conf.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <QDebug>

namespace {
constexpr int g_aes_buff_len = 512;
constexpr int g_ecdsa_pubkey_len = 65;

EC_KEY *getECKey(const unsigned char *pub, int len)
{
    EC_KEY *eckey =
        EC_KEY_new_by_curve_name(NID_X9_62_prime256v1); /*secp256r1*/

    const unsigned char *ptr = pub;

    o2i_ECPublicKey(&eckey, &ptr, len);
    return eckey;
}

QByteArray toDER(const QByteArray &ba)
{
    //  DER format example
    //  30 44
    //  02 20
    //  2b 2b 52 9b db dc 93 e7
    //  8a f7 e0 02 28 b1 79 91
    //  8b 03 2d 76 90 2f 74 ef
    //  45 44 26 f7 d0 6c d0 f9
    //  02 20
    //  62 dd c7 64 51 cd 04 cb
    //  56 7c a5 c5 e0 47 e8 ac
    //  41 d3 d4 cf 7c b9 24 34
    //  d5 5c b4 86 cc cf 6a f2

    unsigned char cnt_r = 0x20;
    unsigned char cnt_s = 0x20;
    if (ba[0] & 0x80) {
        cnt_r += 0x01;
    }

    if (ba[32] & 0x80) {
        cnt_s += 0x01;
    }

    int der_len = 0x06 + cnt_r + cnt_s;
    QByteArray der(der_len, 0x00);

    int offset = 0;
    der[offset++] = 0x30;
    der[offset++] = 0x04 + cnt_r + cnt_s;
    der[offset++] = 0x02;
    der[offset++] = cnt_r;
    if (cnt_r == 0x21) {
        der[offset++] = (char)0x00;
    }

    memcpy(der.data() + offset, ba.data(), 32);
    offset += 32;

    der[offset++] = 0x02;
    der[offset++] = cnt_s;
    if (cnt_s == 0x21) {
        der[offset++] = (char)0x00;
    }

    memcpy(der.data() + offset, ba.data() + 32, 32);

    return der;
}
}  // namespace

QByteArray OpenSSLHelper::AES::GenerateIV()
{
    const EVP_CIPHER *cipher_type = EVP_aes_128_cbc();
    int len = EVP_CIPHER_iv_length(cipher_type);
    QByteArray iv(len, '0');
    RAND_bytes((unsigned char *)iv.data(), iv.size());

    return iv;
}

bool OpenSSLHelper::AES::CryptoStream(QDataStream &in,
                                      QDataStream &out,
                                      QByteArray key,
                                      QByteArray iv,
                                      bool encrypt)
{
    const EVP_CIPHER *cipher_type = EVP_aes_128_cbc();

    /* Make sure that the length of key and iv is enough.*/
    const int key_len = EVP_CIPHER_key_length(cipher_type);
    const int iv_len = EVP_CIPHER_iv_length(cipher_type);

    if (key.length() < key_len)
        key.append(QByteArray(key_len - key.length(), 'F'));

    if (iv.length() < iv_len) {
        iv.append(QByteArray(iv_len - iv.length(), 'F'));
    }

    /* Allow enough space in output buffer for additional block */
    int cipher_block_size = EVP_CIPHER_block_size(cipher_type);
    unsigned char in_buf[g_aes_buff_len];
    unsigned char out_buf[g_aes_buff_len + cipher_block_size];

    int num_bytes_read;
    int out_len;
    EVP_CIPHER_CTX *ctx;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        qDebug() << "[AES] ERROR: EVP_CIPHER_CTX_new failed."
                 << ERR_error_string(ERR_get_error(), NULL);
        return false;
    }

    /* Initialise the encryption operation. */
    if (!EVP_CipherInit_ex(ctx,
                           cipher_type,
                           NULL,
                           (unsigned char *)key.data(),
                           (unsigned char *)iv.data(),
                           encrypt)) {
        qDebug() << "[AES] ERROR: EVP_CipherInit_ex failed."
                 << ERR_error_string(ERR_get_error(), NULL);
        EVP_CIPHER_CTX_cleanup(ctx);
        return false;
    }

    /* Read data until EOF. Update the ciphering with each read.*/
    while (1) {
        num_bytes_read = in.readRawData((char *)in_buf, sizeof(in_buf));

        /* Provide the message to be encrypted, and obtain the encrypted output.
         * EVP_EncryptUpdate can be called multiple times if necessary.*/
        if (!EVP_CipherUpdate(ctx, out_buf, &out_len, in_buf, num_bytes_read)) {
            qDebug() << "[AES] ERROR: EVP_CipherUpdate failed."
                     << ERR_error_string(ERR_get_error(), NULL);
            EVP_CIPHER_CTX_cleanup(ctx);
            return false;
        }

        out.writeRawData((char *)out_buf, out_len);

        if (num_bytes_read < g_aes_buff_len) {
            /* Reached End of file */
            break;
        }
    }

    /* Finalise the encryption. Further ciphertext bytes may be written at this
     * stage.*/
    if (!EVP_CipherFinal_ex(ctx, out_buf, &out_len)) {
        qDebug() << "[AES] ERROR: EVP_CipherFinal_ex failed."
                 << ERR_error_string(ERR_get_error(), NULL);
        EVP_CIPHER_CTX_cleanup(ctx);
        return false;
    }

    out.writeRawData((char *)out_buf, out_len);

    /*Clean up*/
    EVP_CIPHER_CTX_cleanup(ctx);
    return true;
}

bool OpenSSLHelper::AES::CryptoByteArray(const QByteArray &in_ba,
                                         QByteArray &out_ba,
                                         QByteArray key,
                                         QByteArray iv,
                                         bool encrypt)
{
    QDataStream in(in_ba);
    QDataStream out(&out_ba, QIODevice::WriteOnly);
    bool res = CryptoStream(in, out, key, iv, encrypt);

    return res;
}

bool OpenSSLHelper::ECDSA::GenerateSignature(const QByteArray &msg,
                                             QByteArray &key,
                                             QByteArray &signature,
                                             bool der_format)
{
    EC_KEY *eckey =
        EC_KEY_new_by_curve_name(NID_X9_62_prime256v1); /*secp256r1*/

    if (!EC_KEY_generate_key(eckey)) {
        EC_KEY_free(eckey);
        return false;
    }

    int pbkey_size = i2o_ECPublicKey(eckey, NULL);
    if (!pbkey_size) {
        EC_KEY_free(eckey);
        return false;
    }

    unsigned char *pbkey = new unsigned char[pbkey_size];
    memset(pbkey, 0, pbkey_size);
    unsigned char *pbkey_ptr = pbkey;

    if (i2o_ECPublicKey(eckey, &pbkey_ptr) != pbkey_size) {
        EC_KEY_free(eckey);
        delete[] pbkey;
        return false;
    }

    if (pbkey[0] == 0x04 && pbkey_size == g_ecdsa_pubkey_len) {
        key = QByteArray((char *)pbkey + 1, pbkey_size - 1);
    } else if (key.length() == (g_ecdsa_pubkey_len - 1)) {
        key = QByteArray((char *)pbkey, pbkey_size);
    } else {
        qDebug() << "[ECDSA] SignMessage: Pubkey Error";
        delete[] pbkey;
        return false;
    }

    delete[] pbkey;

    QByteArray hash = CalculateSha256(msg);
    ECDSA_SIG *sig =
        ECDSA_do_sign((const unsigned char *)hash.data(), hash.length(), eckey);

    if (sig == NULL) {
        EC_KEY_free(eckey);
        ECDSA_SIG_free(sig);
        return false;
    }

    int sig_size = i2d_ECDSA_SIG(sig, NULL);
    if (!sig_size) {
        EC_KEY_free(eckey);
        ECDSA_SIG_free(sig);
        return false;
    }

    if (der_format) {
        unsigned char *sig_data = new unsigned char[sig_size];
        memset(sig_data, 0, sig_size);
        unsigned char *sig_ptr = sig_data;
        if (i2d_ECDSA_SIG(sig, &sig_ptr) != sig_size) {
            EC_KEY_free(eckey);
            ECDSA_SIG_free(sig);
            delete[] sig_data;
            return false;
        }

        signature = QByteArray((char *)sig_data, sig_size);
        delete[] sig_data;

    } else {
        const BIGNUM *r;
        const BIGNUM *s;
        ECDSA_SIG_get0(sig, &r, &s);
        int rbytes = BN_num_bytes(r);  // Number of bytes needed for R
        int sbytes = BN_num_bytes(s);  // Number of bytes needed for S

        unsigned char *rdata = new unsigned char[rbytes];
        memset(rdata, 0, rbytes);

        unsigned char *sdata = new unsigned char[sbytes];
        memset(sdata, 0, sbytes);

        BN_bn2bin(r, rdata);  // Place R first in the buffer
        BN_bn2bin(s, sdata);  // Place S last in the buffer

        signature.clear();
        signature.append((char *)rdata, rbytes);
        signature.append((char *)sdata, sbytes);

        delete[] rdata;
        delete[] sdata;
    }

    EC_KEY_free(eckey);
    ECDSA_SIG_free(sig);
    return true;
}

bool OpenSSLHelper::ECDSA::VerifySignature(const QByteArray &msg,
                                           const QByteArray &key,
                                           const QByteArray &signature,
                                           bool der_format)
{
    QByteArray hash = CalculateSha256(msg);
    QByteArray der_sig;
    if (der_format) {
        der_sig = signature;
    } else {
        der_sig = toDER(signature);
    }

    unsigned char pbkey[g_ecdsa_pubkey_len];
    if (key.at(0) == 0x04 && key.length() == g_ecdsa_pubkey_len) {
        memcpy(pbkey, key.data(), g_ecdsa_pubkey_len);
    } else if (key.length() == (g_ecdsa_pubkey_len - 1)) {
        pbkey[0] = 0x04;
        memcpy(pbkey + 1, key.data(), key.length());
    } else {
        qDebug() << "[ECDSA] VerifySignature: Pubkey Error";
        return false;
    }

    EC_KEY *eckey = getECKey((const unsigned char *)pbkey, sizeof(pbkey));
    int result = ECDSA_verify(0,
                              (const unsigned char *)hash.data(),
                              hash.length(),
                              (const unsigned char *)der_sig.data(),
                              der_sig.length(),
                              eckey);

    EC_KEY_free(eckey);

    if (result == 1) {
        // qDebug() << "[ECDSATool] VerifySignature: Valid";
        return true;
    } else if (result == 0) {
        qDebug() << "[ECDSA] VerifySignature: Invalid";
    } else {
        qDebug() << "[ECDSA] VerifySignature: Error";
    }

    return false;
}

QByteArray OpenSSLHelper::CalculateSha256(const QByteArray &msg)
{
    QByteArray ba(SHA256_DIGEST_LENGTH, 0x00);
    unsigned char *hash = (unsigned char *)ba.data();

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, msg.data(), msg.length());
    SHA256_Final(hash, &sha256);

    return ba;
}
