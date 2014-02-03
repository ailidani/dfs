

#ifndef libdfs_tree_h
#define libdfs_tree_h

#include <cassert>
#include <stdint.h>
#include <string.h>

#include <string>
#include <map>
#include <vector>

#include <libdfsutil/objecthash.h>

#define ATTR_FILESIZE "Ssize"
#define ATTR_PERMS "Sperms"
#define ATTR_USERNAME "Suser"
#define ATTR_GROUPNAME "Sgroup"
#define ATTR_CTIME "Sctime"
#define ATTR_MTIME "Smtime"
#define ATTR_SYMLINK "Slink"

class AttrMap
{
    typedef std::map<std::string, std::string> _MapType;
public:
    AttrMap();

    template <typename T>
    T getAs(const std::string &attrName) const {
        _MapType::const_iterator it = attrs.find(attrName);
        ASSERT(it != attrs.end());
        ASSERT(it->second.size() >= sizeof(T));
        return *(T *)it->second.data();
    }

    const std::string &getAsStr(const std::string &attrName) const {
        _MapType::const_iterator it = attrs.find(attrName);
        ASSERT(it != attrs.end());
        return (*it).second;
    }

    template <typename T>
    void setAs(const std::string &attrName, const T &value) {
        attrs[attrName].resize(sizeof(T));
        memcpy(&attrs[attrName][0], &value, sizeof(T));
    }

    void setAsStr(const std::string &attrName, const std::string &value) {
        attrs[attrName] = value;
    }

    bool has(const std::string &attrName) const;

    void setFromFile(const std::string &filename);
    void setCreation(mode_t perms);
    void mergeFrom(const AttrMap &other);

    _MapType attrs;

    // Mirrorring std::map functions
    typedef _MapType::iterator iterator;
    typedef _MapType::const_iterator const_iterator;
    iterator begin() { return attrs.begin(); }
    const_iterator begin() const { return attrs.begin(); }
    iterator end() { return attrs.end(); }
    const_iterator end() const { return attrs.end(); }

    const_iterator find(const std::string &key) const { return attrs.find(key); }
    void insert(const _MapType::value_type &val) { attrs.insert(val); }
};

class Repo;
class Tree;
class TreeEntry
{
public:
    enum EntryType {
		Null,
		Blob,
		LargeBlob,
		Tree
    };

    TreeEntry();
    TreeEntry(const ObjectHash &hash, const ObjectHash &lhash);
    ~TreeEntry();
    bool isTree();

    EntryType type;
    AttrMap attrs;

    ObjectHash hash;
    ObjectHash largeHash;

    // TODO: not sure this is the best place
    void extractToFile(const std::string &filename, Repo *src) const;

    bool hasBasicAttrs();

    void print() const;
};

class Tree
{
public:
    Tree();
    ~Tree();
    const std::string getBlob() const;
    void fromBlob(const std::string &blob);
    ObjectHash hash() const; // TODO: cache this

    typedef std::map<std::string, TreeEntry> Flat;
    Flat flattened(Repo *r) const;
    static Tree unflatten(const Flat &flat, Repo *r);

    void print() const;

    typedef std::map<std::string, TreeEntry>::iterator iterator;
    iterator begin() { return tree.begin(); }
    iterator end() { return tree.end(); }
    iterator find(const std::string &name) { return tree.find(name); }

    std::map<std::string, TreeEntry> tree;
};

#endif
