

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <string>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfscrypt.h>
#include <libdfsutil/key.h>
#include <libdfsutil/stream.h>
#include <libdfs/commit.h>

using namespace std;

/********************************************************************
 *
 *
 * Commit
 *
 *
 ********************************************************************/

Commit::Commit()
    : version(0), flags(0), message(), treeObjId(), user(),
      snapshotName(), date(0), signature(""), graftRepo(),
      graftPath(), graftCommitId()
{
    version = COMMIT_VERSION;
    parents.first.clear();
    parents.second.clear();
}

Commit::~Commit()
{
}

void Commit::setParents(ObjectHash p1, ObjectHash p2)
{
    parents.first = p1;
    parents.second = p2;
}

pair<ObjectHash, ObjectHash>
Commit::getParents() const
{
    return parents;
}

void Commit::setMessage(const string &msg)
{
    message = msg;
}

string Commit::getMessage() const
{
    return message;
}

void Commit::setTree(const ObjectHash &tree)
{
    treeObjId = tree;
}

ObjectHash Commit::getTree() const
{
    return treeObjId;
}

void Commit::setUser(const string &user)
{
    this->user = user;
}

string Commit::getUser() const
{
    return user;
}

void Commit::setSnapshot(const std::string &snapshot)
{
    snapshotName = snapshot;
}

std::string Commit::getSnapshot() const
{
    return snapshotName;
}

void Commit::setTime(time_t t)
{
    date = t;
}

time_t Commit::getTime() const
{
    return date;
}

void Commit::sign(const PrivateKey &key)
{
    string blob = getBlob(/* withSignature */false);

    flags |= COMMIT_FLAG_HAS_SIGNATURE;
    signature = key.sign(blob);
}

bool Commit::verify(const PublicKey &key)
{
    string blob = getBlob(/* withSignature */false);

    if (~flags & COMMIT_FLAG_HAS_SIGNATURE)
        return false;

    return key.verify(blob, signature);
}

bool Commit::hasSignature()
{
    return flags & COMMIT_FLAG_HAS_SIGNATURE;
}

void Commit::setGraft(const string &repo,
                 const string &path,
                 const ObjectHash &commitId)
{
    flags |= COMMIT_FLAG_IS_GRAFT;
    graftRepo = repo;
    graftPath = path;
    graftCommitId = commitId;
}

pair<string, string> Commit::getGraftRepo() const
{
    return make_pair(graftRepo, graftPath);
}

ObjectHash Commit::getGraftCommit() const
{
    return graftCommitId;
}

string Commit::getBlob(bool withSignature) const
{
    strwstream ss;

    ss.enableTypes();

    ss.writeUInt32(version);
    if (withSignature) {
        ss.writeUInt32(flags);
    } else {
        ss.writeUInt32(flags & ~COMMIT_FLAG_HAS_SIGNATURE);
    }

    ss.writeHash(treeObjId);
    if (!parents.second.isEmpty()) {
        ss.writeUInt8(2);
        ss.writeHash(parents.first);
        ss.writeHash(parents.second);
    } else if (!parents.first.isEmpty()) {
        ss.writeUInt8(1);
        ss.writeHash(parents.first);
    } else {
        ss.writeUInt8(0);
    }

    ss.writePStr(user);
    ss.writeUInt64(date);
    ss.writePStr(snapshotName);

    if (flags & COMMIT_FLAG_IS_GRAFT) {
        assert(graftPath != "");
        assert(!graftCommitId.isEmpty());

        ss.writePStr(graftRepo);
        ss.writePStr(graftPath);
        ss.writeHash(graftCommitId);
    }

    if (withSignature) {
        if (flags & COMMIT_FLAG_HAS_SIGNATURE) {
            // XXX: Write key fingerprint
            ss.writeLPStr(signature);
        }
    }

    ss.writePStr(message);

    return ss.str();
}

void Commit::fromBlob(const string &blob)
{
    strstream ss(blob);

    ss.enableTypes();

    version = ss.readUInt32();
    flags = ss.readUInt32();

    if (version > COMMIT_VERSION) {
        cout << "Unsupported Commit object version: " << hex << setw(8)
             << setfill('0') << version << endl;
        NOT_IMPLEMENTED(false);
    }

    ss.readHash(treeObjId);
    uint8_t numParents = ss.readUInt8();
    if (numParents == 2) {
        ss.readHash(parents.first);
        ss.readHash(parents.second);
    } else if (numParents == 1) {
        ss.readHash(parents.first);
    }

    ss.readPStr(user);
    date = ss.readUInt64();
    ss.readPStr(snapshotName);

    if (flags & COMMIT_FLAG_IS_GRAFT) {
        ss.readPStr(graftRepo);
        ss.readPStr(graftPath);
        ss.readHash(graftCommitId);

        assert(graftPath != "");
        assert(!graftCommitId.isEmpty());
    }

    if (flags & COMMIT_FLAG_HAS_SIGNATURE) {
        ss.readLPStr(signature);
    }

    ss.readPStr(message);
}

ObjectHash Commit::hash() const
{
    const std::string &blob = getBlob();
    ObjectHash h = OriCrypt_HashString(blob);
    /*fprintf(stderr, "Commit blob len %lu, hash %s\n", blob.size(),
            h.hex().c_str());
    OriDebug_PrintHex(blob);*/
    return h;
}

void Commit::print() const
{
    time_t timeVal = date;
    char timeStr[26];

    ctime_r(&timeVal, timeStr);

    cout << "Version: 0x" << hex << setw(8) << setfill('0') << version << endl;
    cout << "Flags: 0x" << hex << setw(8) << setfill('0') << flags << endl;
    cout << "Parents: "
         << (parents.first.isEmpty() ? "" : parents.first.hex())
         << " "
         << (parents.second.isEmpty() ? "" : parents.second.hex()) << endl;
    cout << "Tree:    " << treeObjId.hex() << endl;
    cout << "Author:  " << user << endl;
    cout << "Date:    " << timeStr;
    if (flags & COMMIT_FLAG_HAS_SIGNATURE) {
        cout << "Signature: ";
        OriDebug_PrintHex(signature.data(), 0, signature.length());
        cout << endl;
    }
    if (!graftCommitId.isEmpty()) {
        cout << "Graft Repo: " << graftRepo << endl;
        cout << "Graft Path: " << graftPath << endl;
        cout << "Graft Commit: " << graftCommitId.hex() << endl;
    }
    cout << "----- BEGIN MESSAGE -----" << endl;
    cout << message << endl;
    cout << "----- END MESSAGE -----" << endl;
}

