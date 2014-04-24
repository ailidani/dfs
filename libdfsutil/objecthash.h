
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

namespace std
{
	template <class T> std::size_t hash_value(T* const& v)
	{
		std::size_t x = static_cast<std::size_t>(
				reinterpret_cast<std::ptrdiff_t>(v));
		return x + (x>>3);
	}

	template <class T>
	inline void hash_combine(std::size_t & seed, const T & v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template <>
	struct hash<ObjectHash>
	{
	    inline std::size_t operator()(const ObjectHash & x) const
	    {
	    	//std::size_t seed = 0;
	    	//hash_combine(seed, x);
	    	//return seed;
	    	std::string value(x.hash, x.hash+x.SIZE);
	    	return hash<string>()(value);
	    }
	};
}

#endif
