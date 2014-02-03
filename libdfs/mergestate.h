
#ifndef libdfs_mergestate_h
#define libdfs_mergestate_h

#include <stdint.h>
#include <time.h>

#include <utility>
#include <string>

#include <libdfsutil/objecthash.h>

class MergeState
{
public:
    MergeState();
    ~MergeState();
    void setParents(ObjectHash p1, ObjectHash p2 = ObjectHash());
    std::pair<ObjectHash, ObjectHash> getParents() const;
    std::string getBlob() const;
    void fromBlob(const std::string &blob);

    ObjectHash hash() const; // TODO: cache this
private:
    std::pair<ObjectHash, ObjectHash> parents;
};

#endif

