

#ifndef libdfs_packfile_h
#define libdfs_packfile_h

#include <set>
#include <deque>
//#include <boost/tr1/memory.hpp>
#include <memory>
//#include <boost/tr1/unordered_map.hpp>
#include <unordered_map>

#include <libdfsutil/objecthash.h>
#include <libdfsutil/stream.h>
#include <libdfsutil/lrucache.h>
#include "object.h"

typedef uint32_t offset_t;
typedef uint32_t packid_t;
typedef uint32_t numobjs_t;

struct IndexEntry
{
    ObjectInfo info;
    offset_t offset;
    uint32_t packed_size;
    packid_t packfile;

    const static size_t SIZE = ObjectInfo::SIZE + sizeof(offset_t) +
        sizeof(uint32_t) + sizeof(packid_t);
};

class Packfile;
class Index;
class PfTransaction
{
public:
    typedef std::shared_ptr<PfTransaction> sp;

    PfTransaction(Packfile *pf, Index *idx);
    ~PfTransaction();

    bool full() const;
    void addPayload(ObjectInfo info, const std::string &payload);
    bool has(const ObjectHash &hash) const;
    void commit();

    std::vector<ObjectInfo> infos;
    std::vector<std::string> payloads;
    size_t totalSize;
    bool committed;

    std::unordered_map<ObjectHash, size_t> hashToIx;

private:
    Packfile *pf;
    Index *idx;
    float _checkCompressionRatio(const std::string &payload);
};

class Packfile
{
public:
    typedef std::shared_ptr<Packfile> sp;

    Packfile(const std::string &filename, packid_t id);
    ~Packfile();

    packid_t getPackfileID() const;

    bool full() const;
    PfTransaction::sp begin(Index *idx);
    void commit(PfTransaction *t, Index *idx);
    //void addPayload(ObjectInfo info, const std::string &payload, Index *idx);
    bytestream *getPayload(const IndexEntry &entry);
    /// @returns true when the packfile is empty
    bool purge(const std::set<ObjectHash> &hset, Index *idx);

    typedef void (*ReadEntryCb)(const ObjectInfo &info, offset_t off,
                                void *arg);
    void readEntries(ReadEntryCb cb, void *arg);

    void transmit(bytewstream *bs, std::vector<IndexEntry> objects);
    /// @returns false if nothing to receive
    bool receive(bytestream *bs, Index *idx);

private:
    int fd;
    std::string filename;
    packid_t packid;
    size_t numObjects;
    size_t fileSize;
};


#define PFMGR_FREELIST ".freelist"

class PackfileManager
{
public:
    typedef std::shared_ptr<PackfileManager> sp;

    PackfileManager(const std::string &rootPath);
    ~PackfileManager();

    Packfile::sp getPackfile(packid_t id);
    Packfile::sp newPackfile();
    bool hasPackfile(packid_t id);
    std::vector<packid_t> getPackfileList();

private:
    std::string rootPath;

    std::deque<packid_t> freeList;
    void _recomputeFreeList();
    bool _loadFreeList();
    void _writeFreeList();

    LRUCache<uint32_t, Packfile::sp, 96> _packfileCache;

    std::string _getPackfileName(packid_t id);
};

#endif
