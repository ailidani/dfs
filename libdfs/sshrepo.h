

#ifndef libdfs_sshrepo_h
#define libdfs_sshrepo_h

#include <string>
#include <vector>
#include <deque>

#include "repo.h"
#include "sshclient.h"

class SshObject;
class SshRepo : public Repo
{
public:
    friend class SshObject;
    SshRepo(SshClient *client);
    ~SshRepo();

    std::string getUUID();
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

private:
    SshClient *client;

    std::string &_payload(const ObjectHash &id);
    void _addPayload(const ObjectHash &id, const std::string &payload);
    void _clearPayload(const ObjectHash &id);

    std::map<ObjectHash, std::string> payloads;

    std::unordered_set<ObjectHash> *containedObjs;
};

class SshObject : public Object
{
public:
    SshObject(SshRepo *repo, ObjectInfo info);
    ~SshObject();

    bytestream *getPayloadStream();

private:
    SshRepo *repo;
};

#endif
