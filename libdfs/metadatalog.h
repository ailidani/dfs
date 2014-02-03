

#ifndef libdfs_metadatalog_h
#define libdfs_metadatalog_h

#include <unordered_map>

#include <libdfsutil/objecthash.h>

typedef int32_t refcount_t;
typedef std::unordered_map<ObjectHash, refcount_t> RefcountMap;
typedef std::unordered_map<std::string, std::string> ObjMetadata;
typedef std::unordered_map<ObjectHash, ObjMetadata> MetadataMap;

class MetadataLog;
class MdTransaction
{
public:
    MdTransaction(MetadataLog *log);
    ~MdTransaction();
    typedef std::shared_ptr<MdTransaction> sp;

    void addRef(const ObjectHash &hash);
    void decRef(const ObjectHash &hash);
    void setMeta(const ObjectHash &hash, const std::string &key,
            const std::string &value);
private:
    friend class MetadataLog;
    MetadataLog *log;
    RefcountMap counts;
    MetadataMap metadata;
};

class MetadataLog
{
public:
    MetadataLog();
    ~MetadataLog();

    void open(const std::string &filename);
    void sync();
    /// rewrites the log file, optionally with new counts
    void rewrite(const RefcountMap *refs = NULL, const MetadataMap *data = NULL);

    void addRef(const ObjectHash &hash, MdTransaction::sp trs =
            MdTransaction::sp());
    refcount_t getRefCount(const ObjectHash &hash) const;
    std::string getMeta(const ObjectHash &hash, const std::string &key) const;

    MdTransaction::sp begin();
    void commit(MdTransaction *tr);

    void dumpRefs() const;
    void dumpMeta() const;

private:
    friend class MdTransaction;
    int fd;
    std::string filename;
    RefcountMap refcounts;
    MetadataMap metadata;
};

#endif
