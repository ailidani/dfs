
#ifndef libdfs_backup_h
#define libdfs_backup_h

#include <stdexcept>
//#include <boost/tr1/unordered_map.hpp>
#include <unordered_map>
//#include <boost/tr1/memory.hpp>
#include <memory>

#include "object.h"

class BackupError : std::exception
{
public:
    explicit BackupError(const std::string &msg)
        : msg(msg) {}
    ~BackupError() throw() {}
    const char *what() const throw() { return msg.c_str(); }
private:
    std::string msg;
};

class MemoryObject : public Object
{
public:
    MemoryObject(const ObjectInfo &info, const std::string &payload)
        : Object(info), payload(payload)
    {
    }

    bytestream *getPayloadStream() {
        return new strstream(payload);
    }

    std::string getPayload() {
        return payload;
    }

private:
    std::string payload;
};

class BackupService
{
public:
    typedef std::shared_ptr<BackupService> sp;

    // Is cached for the lifetime of a BackupService
    bool hasKey(const std::string &key) {
        HasKeyCache::iterator it = hasKeyCache.find(key);
        if (it != hasKeyCache.end()) {
            return (*it).second;
        }

        bool hk = realHasKey(key);
        hasKeyCache[key] = hk;
        return hk;
    }

    virtual bool realHasKey(const std::string &key) = 0;

    virtual bool getData(const std::string &key, std::string &out) = 0;
    virtual bool putData(const std::string &key, const std::string &data) = 0;
    virtual bool putFile(const std::string &key, const std::string &filename) = 0;

    virtual Object::sp getObj(const std::string &key) {
        std::string data;
        bool success = getData(key, data);
        if (!success) return Object::sp();

        ObjectInfo info;
        info.fromString(data.substr(0, ObjectInfo::SIZE));
        return Object::sp(new MemoryObject(info,
                    data.substr(ObjectInfo::SIZE)));
    }

    virtual bool putObj(const std::string &key, Object::sp obj) {
        std::string data = obj->getInfo().toString() + obj->getPayload();
        return putData(key, data);
    }

protected:
    typedef std::unordered_map<std::string, bool> HasKeyCache;
    HasKeyCache hasKeyCache;
};

#endif

