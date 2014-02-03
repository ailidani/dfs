
#ifndef libdfsutil_dfscrypt_h
#define libdfsutil_dfscrypt_h

#include "objecthash.h"

std::string OriCrypt_MD5String(const std::string &str);
ObjectHash OriCrypt_HashString(const std::string &str);
ObjectHash OriCrypt_HashBlob(const uint8_t *data, size_t len);
ObjectHash OriCrypt_HashFile(const std::string &path);
std::string OriCrypt_Encrypt(const std::string &plaintext, const std::string &key);
std::string OriCrypt_Decrypt(const std::string &ciphertext, const std::string &key);

#endif
