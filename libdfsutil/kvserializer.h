

#ifndef libdfsutil_kvserializer_h
#define libdfsutil_kvserializer_h

#include <stdint.h>
#include <iostream>
#include <string>
#include <map>

#include "serializationexception.h"

class KVSerializer
{
public:
    enum KVType {
        KVTypeNull, KVTypeString, KVTypeBool,
        KVTypeU8, KVTypeU16, KVTypeU32, KVTypeU64,
    };
    KVSerializer();
    ~KVSerializer();
    void putStr(const std::string &key, const std::string &value);
    void putU8(const std::string &key, uint8_t value);
    void putU16(const std::string &key, uint16_t value);
    void putU32(const std::string &key, uint32_t value);
    void putU64(const std::string &key, uint64_t value);
    std::string getStr(const std::string &key) const;
    uint8_t getU8(const std::string &key) const;
    uint16_t getU16(const std::string &key) const;
    uint32_t getU32(const std::string &key) const;
    uint64_t getU64(const std::string &key) const;
    KVType getType(const std::string &key) const;
    bool hasKey(const std::string &key) const;
    void remove(const std::string &key);
    void removeAll();
    void fromBlob(const std::string &blob);
    std::string getBlob() const;
    void dump() const;
private:
    std::map<std::string, std::string> table;
};

#endif

