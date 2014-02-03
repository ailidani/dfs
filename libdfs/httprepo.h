

#ifndef libdfs_httprepo_h
#define libdfs_httprepo_h

#include <string>
#include <vector>
//#include <boost/tr1/unordered_set.hpp>

#include "repo.h"

class HttpObject;
class HttpRepo : public Repo
{
public:
    friend class HttpObject;
    HttpRepo(HttpClient *client);
    ~HttpRepo();

    void preload(const std::vector<std::string> &objs);

    std::string getUUID();
    ObjectHash getHead();
    int distance();

    Object::sp getObject(const ObjectHash &id);
    ObjectInfo getObjectInfo(const ObjectHash &id);
    bool hasObject(const ObjectHash &id);
    std::vector<bool> hasObjects(const ObjectHashVec &objs);
    bytestream *getObjects(const ObjectHashVec &objs);
    std::set<ObjectInfo> listObjects();
    int addObject(ObjectType type, const ObjectHash &hash,
            const std::string &payload);
    std::vector<Commit> listCommits();

private:
    HttpClient *client;

    std::string &_payload(const ObjectHash &id);
    void _addPayload(const ObjectHash &id, const std::string &payload);
    void _clearPayload(const ObjectHash &id);

    std::map<ObjectHash, std::string> payloads;

    std::unordered_set<ObjectHash> *containedObjs;
};

class HttpObject : public Object
{
public:
    HttpObject(HttpRepo *repo, ObjectInfo info);
    ~HttpObject();

    bytestream *getPayloadStream();

private:
    HttpRepo *repo;
};

#endif
