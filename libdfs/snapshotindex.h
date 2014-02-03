

#ifndef libdfs_snapshotindex_h
#define libdfs_snapshotindex_h

#include <assert.h>

#include <string>
#include <map>

class SnapshotIndex
{
public:
    SnapshotIndex();
    ~SnapshotIndex();
    void open(const std::string &indexFile);
    void close();
    void rewrite();
    void addSnapshot(const std::string &name, const ObjectHash &commitId);
    void delSnapshot(const std::string &name);
    const ObjectHash &getSnapshot(const std::string &name) const;
    std::map<std::string, ObjectHash> getList();
private:
    int fd;
    std::string fileName;
    std::map<std::string, ObjectHash> snapshots;
};

#endif

