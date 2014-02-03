

#ifndef libdfs_index_h
#define libdfs_index_h

#include <assert.h>

#include <string>
#include <set>
//#include <boost/tr1/unordered_map.hpp>
#include <unordered_map>

#include "object.h"
#include "packfile.h"

class Index
{
public:
    Index();
    ~Index();
    void open(const std::string &indexFile);
    void close();
    void sync();
    void rewrite();
    void dump();
    void updateEntry(const ObjectHash &objId, const IndexEntry &entry);
    const IndexEntry &getEntry(const ObjectHash &objId) const;
    const ObjectInfo &getInfo(const ObjectHash &objId) const;
    bool hasObject(const ObjectHash &objId) const;
    std::set<ObjectInfo> getList();
private:
    int fd;
    std::string fileName;
    std::unordered_map<ObjectHash, IndexEntry> index;

    void _writeEntry(const IndexEntry &e);
};

#endif

