

#ifndef libdfs_tempdir_h
#define libdfs_tempdir_h

#include <set>
#include <vector>
#include <string>
//#include <boost/tr1/memory.hpp>
#include <memory>
//#include <boost/tr1/unordered_map.hpp>
#include <unordered_map>

#include "repo.h"
#include "commit.h"
#include "index.h"

/** Class for managing a temporary directory of objects.
  * Uses an append-only log for keeping track of objects.
  */
class TempDir : public Repo
{
public:
    TempDir(const std::string &dirpath);
    ~TempDir();

    typedef std::shared_ptr<TempDir> sp;

    std::string pathTo(const std::string &file);
    /// Temp files are deleted along with the directory
    std::string newTempFile();

    // Repo implementation
    int distance() { return 0; }
    std::string getUUID() { NOT_IMPLEMENTED(false); }
    ObjectHash getHead() { NOT_IMPLEMENTED(false); }
    Object::sp getObject(const ObjectHash &objId);
    ObjectInfo getObjectInfo(const ObjectHash &objId);
    bool hasObject(const ObjectHash &objId);
    bytestream *getObjects(const ObjectHashVec &objs);
    std::set<ObjectInfo> listObjects();
    std::vector<Commit> listCommits() { NOT_IMPLEMENTED(false); }
    int addObject(ObjectType type, const ObjectHash &hash,
            const std::string &payload);

    std::string dirpath;

private:
    Index index;
    int objects_fd;
    std::unordered_map<ObjectHash, off_t> offsets;
};

class TempObject : public Object
{
public:
    TempObject(int fd, off_t off, size_t len, const ObjectInfo &info);

    bytestream *getPayloadStream();

private:
    int fd;
    off_t off;
    size_t len;
};

#endif
