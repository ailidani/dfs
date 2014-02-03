

#ifndef libdfs_localobject_h
#define libdfs_localobject_h

#include <stdint.h>

#include <utility>
#include <string>
#include <map>
//#include <boost/tr1/memory.hpp>
#include <memory>

#include <libdfsutil/stream.h>
#include "object.h"
#include "packfile.h"

class LocalObject : public Object
{
public:
    typedef std::shared_ptr<LocalObject> sp;

    LocalObject(PfTransaction::sp transaction, size_t ix);
    LocalObject(Packfile::sp packfile, const IndexEntry &entry);
    ~LocalObject();

    // BaseObject implementation
    bytestream *getPayloadStream();

private:
    PfTransaction::sp transaction;
    size_t ix_tr;

    Packfile::sp packfile;
    IndexEntry entry;
    //void setupLzma(lzma_stream *strm, bool encode);
    //bool appendLzma(int dstFd, lzma_stream *strm, lzma_action action);
};



#endif

