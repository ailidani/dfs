
#include <stdint.h>

#include <string>
#include <vector>
#include <set>
#include <queue>
#include <iostream>

#include "tuneables.h"

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfsfile.h>
#include <libdfsutil/dfscrypt.h>
#include <libdfsutil/dag.h>

#include <libdfs/object.h>
#include <libdfs/largeblob.h>
#include <libdfs/repo.h>

using namespace std;


ObjectHash EMPTY_COMMIT =
ObjectHash::fromHex("0000000000000000000000000000000000000000000000000000000000000000");

#ifdef ORI_USE_SHA256
ObjectHash EMPTYFILE_HASH =
ObjectHash::fromHex("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
#endif
#ifdef ORI_USE_SKEIN
ObjectHash EMPTYFILE_HASH =
ObjectHash::fromHex("c8877087da56e072870daa843f176e9453115929094c3a40c463a196c29bf7ba");
#endif

/*
 * Repo
 */

Repo::Repo() {
}

Repo::~Repo() {
}

/*
 * Default implementations
 */

vector<bool> Repo::hasObjects(const ObjectHashVec &ids)
{
    vector<bool> rval = vector<bool>(ids.size());

    for (int i = 0; i < ids.size(); i++) {
        rval[i] = hasObject(ids[i]);
    }

    return rval;
}

/*
 * High-level operations
 */

void Repo::copyFrom(Object *other)
{
    addObject(other->getInfo().type, other->getInfo().hash, other->getPayload());
}

/*
 * Add a blob to the repository. This is a low-level interface.
 */
ObjectHash Repo::addBlob(ObjectType type, const string &blob)
{
    ObjectHash hash = OriCrypt_HashString(blob);
    addObject(type, hash, blob);
    return hash;
}


bytestream * Repo::getObjects(const std::deque<ObjectHash> &objs)
{
    ObjectHashVec vec;
    for (size_t i = 0; i < objs.size(); i++) {
        vec.push_back(objs[i]);
    }
    return getObjects(vec);
}


/*
 * Add a file to the repository. This is a low-level interface.
 */
ObjectHash Repo::addSmallFile(const string &path)
{
    diskstream ds(path);
    return addBlob(ObjectInfo::Blob, ds.readAll());
}

/*
 * Add a file to the repository. This is a low-level interface.
 */
pair<ObjectHash, ObjectHash> Repo::addLargeFile(const string &path)
{
    string blob;
    string hash;
    LargeBlob lb = LargeBlob(this);

    lb.chunkFile(path);
    blob = lb.getBlob();

    // TODO: this should only be called when committing,
    // we'll take care of backrefs then
    /*if (!hasObject(hash)) {
        map<uint64_t, LBlobEntry>::iterator it;

        for (it = lb.parts.begin(); it != lb.parts.end(); it++) {
            addBackref((*it).second.hash);
        }
    }*/

    return make_pair(addBlob(ObjectInfo::LargeBlob, blob), lb.totalHash);
}

/*
 * Add a file to the repository. This is an internal interface that pusheds the
 * work to addLargeFile or addSmallFile based on our size threshold.
 */
pair<ObjectHash, ObjectHash> Repo::addFile(const string &path)
{
    size_t sz = OriFile_GetSize(path);

    if (sz > LARGEFILE_MINIMUM)
        return addLargeFile(path);
    else
        return make_pair(addSmallFile(path), ObjectHash());
}




Tree Repo::getTree(const ObjectHash &treeId)
{
    Object::sp o(getObject(treeId));
    if (!o.get()) {
        throw std::runtime_error("Object not found");
    }
    string blob = o->getPayload();

    ASSERT(treeId == EMPTYFILE_HASH || o->getInfo().type == ObjectInfo::Tree);

    Tree t;
    t.fromBlob(blob);

    return t;
}

Commit Repo::getCommit(const ObjectHash &commitId)
{
    Object::sp o(getObject(commitId));
    string blob = o->getPayload();

    ASSERT(commitId == EMPTYFILE_HASH || o->getInfo().type == ObjectInfo::Commit);

    Commit c;
    if (blob.size() == 0) {
        printf("Error getting commit blob\n");
        PANIC();
        return c;
    }
    c.fromBlob(blob);

    return c;
}

LargeBlob
Repo::getLargeBlob(const ObjectHash &objId)
{
    Object::sp o(getObject(objId));
    string blob = o->getPayload();

    ASSERT(objId == EMPTYFILE_HASH || o->getInfo().type == ObjectInfo::LargeBlob);

    LargeBlob lb(this);
    if (blob.size() == 0) {
        printf("Error getting commit blob\n");
        PANIC();
        return lb;
    }
    lb.fromBlob(blob);

    return lb;
}

DAG<ObjectHash, Commit>
Repo::getCommitDag()
{
    vector<Commit> commits = listCommits();
    vector<Commit>::iterator it;
    DAG<ObjectHash, Commit> cDag = DAG<ObjectHash, Commit>();

    cDag.addNode(ObjectHash(), Commit());
    for (it = commits.begin(); it != commits.end(); it++) {
	cDag.addNode((*it).hash(), (*it));
    }

    for (it = commits.begin(); it != commits.end(); it++) {
	pair<ObjectHash, ObjectHash> p = (*it).getParents();
	cDag.addEdge(p.first, (*it).hash());
	if (!p.second.isEmpty())
	    cDag.addEdge(p.first, it->hash());
    }

    return cDag;
}

/*
 * Lookup a path given a Commit and return the object ID.
 */
ObjectHash
Repo::lookup(const Commit &c, const string &path)
{
    vector<string> pv = Util_PathToVector(path);
    ObjectHash objId = c.getTree();

    if (path == "/")
        return objId;

    if (pv.size() == 0)
	return ObjectHash();

    for (size_t i = 0; i < pv.size(); i++) {
	map<string, TreeEntry>::iterator e;
        Tree t = getTree(objId);
	e = t.tree.find(pv[i]);
	if (e == t.tree.end()) {
	    return ObjectHash();
	}
        objId = t.tree[pv[i]].hash;
    }

    return objId;
}

void Repo::transmit(bytewstream *bs, const ObjectHashVec &objs)
{
    NOT_IMPLEMENTED(false);
}

void Repo::receive(bytestream *bs)
{
    NOT_IMPLEMENTED(false);
}

set<string> Repo::listExt()
{
    set<string> val;

    return val;
}

bool Repo::hasExt(const string &ext)
{
    set<string> exts = listExt();

    return exts.count(ext) != 0;
}

string Repo::callExt(const string &ext, const string &data)
{
    NOT_IMPLEMENTED(false);
}

