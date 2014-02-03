
#ifndef libdfsutil_objectinfo_h
#define libdfsutil_objectinfo_h

#include <string>

#include "objecthash.h"

#define ORI_OBJECT_TYPESIZE	    4
#define ORI_OBJECT_FLAGSSIZE        4
#define ORI_OBJECT_SIZE		    8
#define ORI_OBJECT_HDRSIZE	    24

#define ORI_FLAG_UNCOMPRESSED   0x0000
#define ORI_FLAG_FASTLZ         0x0001
#define ORI_FLAG_LZMA           0x0002
#define ORI_FLAG_ZIPMASK        0x000F

#define ORI_FLAG_DEFAULT        0x0000

struct ObjectInfo {
    enum Type { Null, Commit, Tree, Blob, LargeBlob, Purged };
    enum ZipAlgo { ZIPALGO_UNKNOWN, ZIPALGO_NONE, ZIPALGO_FASTLZ, ZIPALGO_LZMA };

    ObjectInfo();
    ObjectInfo(const ObjectHash &hash);

    std::string toString() const;
    void fromString(const std::string &info);
    //ssize_t writeTo(int fd, bool seekable = true);
    bool hasAllFields() const;

    // Flags operations
    bool isCompressed() const;
    ZipAlgo getAlgo() const;
    void setAlgo(ZipAlgo algo);
    bool operator <(const ObjectInfo &) const;

    // Object type
    static const char *getStrForType(Type t);
    static Type getTypeForStr(const char *str);

    // For debug use
    void print(std::ostream &outStream = std::cout) const;

    Type type;
    ObjectHash hash;
    uint32_t flags;
    uint32_t payload_size;

    static const size_t SIZE = 4+ObjectHash::SIZE+2*sizeof(uint32_t);
};

typedef ObjectInfo::Type ObjectType;

#endif
