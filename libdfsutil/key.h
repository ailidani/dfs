

#ifndef libdfsutil_key_h
#define libdfsutil_key_h

#include <string>

#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#ifdef OPENSSL_NO_SHA256
#error "SHA256 not supported!"
#endif

class PublicKey
{
public:
    PublicKey();
    ~PublicKey();
    void open(const std::string &keyfile);
    std::string getName();
    std::string getEmail();
    std::string computeDigest();
    bool verify(const std::string &blob, const std::string &digest) const;
private:
    X509 *x509;
    EVP_PKEY *key;
};

class PrivateKey
{
public:
    PrivateKey();
    ~PrivateKey();
    void open(const std::string &keyfile);
    std::string sign(const std::string &blob) const;
private:
    EVP_PKEY *key;
};

class KeyType
{
public:
    enum KeyType_t
    {
        Invalid,
        Public,
        Private,
    };
};

KeyType::KeyType_t Key_GetType(const std::string &keyfile);

#endif

