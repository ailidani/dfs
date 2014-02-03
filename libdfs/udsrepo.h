

#ifndef libdfs_udsrepo_h
#define libdfs_udsrepo_h

#include <string>
#include <vector>
#include <deque>

#include "repo.h"
#include "udsclient.h"

class UDSObject;
class UDSRepo : public Repo
{
public:
    friend class UDSObject;
    UDSRepo();
    UDSRepo(UDSClient *client);
    ~UDSRepo();

    std::string getUUID();
    std::string getVersion();
    ObjectHash getHead();
    int distance();

    Object::sp getObject(const ObjectHash &id);
    ObjectInfo getObjectInfo(const ObjectHash &id);
    bool hasObject(const ObjectHash &id);
    bytestream *getObjects(const ObjectHashVec &objs);
    std::set<ObjectInfo> listObjects();
    int addObject(ObjectType type, const ObjectHash &hash,
            const std::string &payload);
    std::vector<Commit> listCommits();

    // Transport
    virtual void transmit(bytewstream *out, const ObjectHashVec &objs);

    // Extensions
    virtual std::set<std::string> listExt();
    virtual std::string callExt(const std::string &ext,
                                const std::string &data);
private:
    UDSClient *client;

    std::string &_payload(const ObjectHash &id);
    void _addPayload(const ObjectHash &id, const std::string &payload);
    void _clearPayload(const ObjectHash &id);

    std::map<ObjectHash, std::string> payloads;

    std::unordered_set<ObjectHash> *containedObjs;
};

class UDSObject : public Object
{
public:
    UDSObject(UDSRepo *repo, ObjectInfo info);
    ~UDSObject();

    bytestream *getPayloadStream();

private:
    UDSRepo *repo;
};

#endif
