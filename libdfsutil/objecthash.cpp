

#include <stdint.h>
#include <string.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/objecthash.h>

ObjectHash::ObjectHash()
{
    clear();
}

// Private constructor
ObjectHash::ObjectHash(const char *source)
{
    memcpy(hash, source, SIZE);
}

static uint8_t hexdigit(char c)
{
    ASSERT(isdigit(c) || (c >= 'a' && c <= 'f'));
    if (isdigit(c))
        return c - '0';
    return c - 'a' + 10;
}

ObjectHash ObjectHash::fromHex(std::string hex)
{
    ASSERT(hex.size() == STR_SIZE);

    std::transform(hex.begin(), hex.end(), hex.begin(), ::tolower);

    char hash[SIZE];
    for (size_t i = 0; i < SIZE; i++) {
        hash[i] = hexdigit(hex[i*2]) * 16 + hexdigit(hex[i*2+1]);
    }

    ObjectHash rval(hash);
    ASSERT(rval.hex() == hex);
    return rval;
}

/*bool ObjectHash::operator <(const ObjectHash &other) const
{
    return memcmp(hash, other.hash, SIZE) < 0;
}

bool ObjectHash::operator ==(const ObjectHash &other) const
{
    return memcmp(hash, other.hash, SIZE) == 0;
}*/

void ObjectHash::clear()
{
    memset(hash, 0, SIZE);
}

bool ObjectHash::isEmpty() const
{
    for (size_t i = 0; i < SIZE; i++) {
        if (hash[i] != 0)
            return false;
    }
    return true;
}

std::string ObjectHash::hex() const
{
    std::stringstream rval;

    // Convert into string.
    for (size_t i = 0; i < SIZE; i++)
    {
	rval << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return rval.str();
}

std::string ObjectHash::bin() const
{
    return std::string((const char *)hash, SIZE);
}

std::size_t hash_value(ObjectHash const& key) {
    return *((std::size_t*)key.hash);
}

