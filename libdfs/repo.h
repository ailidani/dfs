// Repository

#ifndef libdfs_repo_h
#define libdfs_repo_h

#include <stdint.h>

#include <string>
#include <set>
#include <deque>

#include <libdfsutil/dag.h>
#include <libdfsutil/objecthash.h>
#include "tree.h"
#include "commit.h"
#include "object.h"

extern ObjectHash EMPTY_COMMIT;
extern ObjectHash EMPTYFILE_HASH;

typedef std::vector<ObjectHash> ObjectHashVec;

class LargeBlob;

class Repo
{
public:
    Repo();
    virtual ~Repo();

    // Repo information
    virtual std::string getUUID() = 0;
    virtual ObjectHash getHead() = 0;
    virtual int distance() = 0;

    // Objects
    virtual Object::sp getObject(
            const ObjectHash &id
            ) = 0;
    virtual ObjectInfo getObjectInfo(
            const ObjectHash &id
            ) = 0;
    virtual bool hasObject(const ObjectHash &id) = 0;
    virtual std::vector<bool> hasObjects(const ObjectHashVec &ids);
    virtual bytestream *getObjects(
            const ObjectHashVec &objs
            ) = 0;

    // Object queries
    virtual std::set<ObjectInfo> listObjects() = 0;
    virtual std::vector<Commit> listCommits() = 0;

    virtual int addObject(
            ObjectType type,
            const ObjectHash &hash,
            const std::string &payload
            ) = 0;

    // Wrappers
    virtual ObjectHash addBlob(ObjectType type, const std::string &blob);
    bytestream *getObjects(const std::deque<ObjectHash> &objs);

    ObjectHash addSmallFile(const std::string &path);
    std::pair<ObjectHash, ObjectHash>
        addLargeFile(const std::string &path);
    std::pair<ObjectHash, ObjectHash>
        addFile(const std::string &path);

    virtual Tree getTree(const ObjectHash &treeId);
    virtual Commit getCommit(const ObjectHash &commitId);
    virtual LargeBlob getLargeBlob(const ObjectHash &objId);

    // Lookup
    ObjectHash lookup(const Commit &c, const std::string &path);

    // Transport
    virtual void transmit(bytewstream *bs, const ObjectHashVec &objs);
    virtual void receive(bytestream *bs);

    // Extensions
    virtual std::set<std::string> listExt();
    virtual bool hasExt(const std::string &ext);
    virtual std::string callExt(const std::string &ext,
                                const std::string &data);

    // High level operations
    virtual void copyFrom(
            Object *other
            );
    virtual DAG<ObjectHash, Commit> getCommitDag();
};

#endif

