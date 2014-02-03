
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfscrypt.h>
#include <libdfsutil/stream.h>
#include <libdfs/mergestate.h>

using namespace std;

/********************************************************************
 *
 *
 * MergeState
 *
 *
 ********************************************************************/

MergeState::MergeState()
{
}

MergeState::~MergeState()
{
}

void
MergeState::setParents(ObjectHash p1, ObjectHash p2)
{
    parents.first = p1;
    parents.second = p2;
}

pair<ObjectHash, ObjectHash>
MergeState::getParents() const
{
    return parents;
}

string
MergeState::getBlob() const
{
    strwstream ss;

    if (!parents.second.isEmpty()) {
        ss.writeUInt8(2);
        ss.writeHash(parents.first);
        ss.writeHash(parents.second);
    } else {
        PANIC(); // Both parents must be set
    }

    return ss.str();
}

void
MergeState::fromBlob(const string &blob)
{
    strstream ss(blob);

    uint8_t numParents = ss.readUInt8();
    if (numParents == 2) {
        ss.readHash(parents.first);
        ss.readHash(parents.second);
    } else {
        PANIC(); // Both parents must be set
    }

    // Verify that everything is set!
}

ObjectHash
MergeState::hash() const
{
    return OriCrypt_HashString(getBlob());
}
