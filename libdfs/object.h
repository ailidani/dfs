

#ifndef libdfs_object_h
#define libdfs_object_h


#include <stdint.h>

#include <utility>
#include <string>
#include <map>
//#include <boost/tr1/memory.hpp>
#include <memory>

#include <libdfsutil/stream.h>
#include <libdfsutil/objectinfo.h>

class Object {
public:
    typedef std::shared_ptr<Object> sp;

    Object() {}
    Object(const ObjectInfo &info) : info(info) {}
    virtual ~Object() {}

    virtual const ObjectInfo &getInfo() const { return info; }
    virtual bytestream *getPayloadStream() = 0;

    virtual std::string getPayload();

protected:
    ObjectInfo info;
};

#endif
