
#ifndef libdfsutil_objecthash_h
#define libdfsutil_objecthash_h

#include <stdint.h>
#include <string.h>

struct ObjectHash {
    static const size_t SIZE = 32;
    static const size_t STR_SIZE = SIZE * 2;

    ObjectHash();
    static ObjectHash fromHex(std::string hex);

    bool operator <(const ObjectHash &other) const {
        return memcmp(hash, other.hash, SIZE) < 0;
    }
    bool operator ==(const ObjectHash &other) const {
        return memcmp(hash, other.hash, SIZE) == 0;
    }
    bool operator !=(const ObjectHash &other) const {
        return !(*this == other);
    }

    void clear();
    // TODO: empty hash "\0...\0" is valid?...
    bool isEmpty() const;
    /// Returns the hash as a hex string (size 2*SIZE)
    std::string hex() const;
    /// Copies the binary hash to a string (length SIZE)
    std::string bin() const;

    uint8_t hash[SIZE];

private:
    ObjectHash(const char *source);
};

std::size_t hash_value(ObjectHash const& key);

#endif
