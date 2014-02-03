

#ifndef libdfs_largeblob_h
#define libdfs_largeblob_h

#include <stdint.h>

#include <string>
#include <map>

#include "repo.h"

class LBlobEntry
{
public:
    LBlobEntry(const LBlobEntry &l);
    LBlobEntry(const ObjectHash &hash, uint16_t length);
    ~LBlobEntry();
    const ObjectHash hash;
    const uint16_t length;
};

class Repo;

class LargeBlob
{
public:
    LargeBlob(Repo *r);
    ~LargeBlob();
    void chunkFile(const std::string &path);
    void extractFile(const std::string &path);
    /// May read less than s bytes
    ssize_t read(uint8_t *buf, size_t s, off_t off) const;
    // XXX: Stream read/write operations
    const std::string getBlob();
    void fromBlob(const std::string &blob);
    size_t totalSize() const;

    /*
     * A map of the file parts contains the file offset as the key and the large
     * blob entry object as the value.  It allows O(log n) random access into
     * the file.
     */
    ObjectHash totalHash;
    std::map<uint64_t, LBlobEntry> parts;
    Repo *repo;
};

#endif

