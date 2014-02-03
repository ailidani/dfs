

#define __STDC_LIMIT_MACROS

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <openssl/sha.h>

#ifdef OPENSSL_NO_SHA256
#error "SHA256 not supported!"
#endif

#include "tuneables.h"

#include <libdfsutil/debug.h>
#include <libdfs/object.h>
#include <libdfs/localobject.h>

using namespace std;

/*
 * Object
 */
LocalObject::LocalObject(PfTransaction::sp transaction, size_t ix)
    : Object(transaction->infos[ix]), transaction(transaction), ix_tr(ix),
      packfile()
{
}

LocalObject::LocalObject(Packfile::sp packfile, const IndexEntry &entry)
    : Object(entry.info), packfile(packfile), entry(entry)
{
}

LocalObject::~LocalObject()
{
}

// XXX: Eliminate duplicate compression code!
bytestream* LocalObject::getPayloadStream()
{
    if (packfile.get()) {
        return packfile->getPayload(entry);
    }
    if (transaction.get()) {
        switch(info.getAlgo()) {
            case ObjectInfo::ZIPALGO_NONE:
                return new strstream(transaction->payloads[ix_tr]);
            case ObjectInfo::ZIPALGO_FASTLZ:
                return new zipstream(new strstream(transaction->payloads[ix_tr]),
                                     DECOMPRESS, info.payload_size);
            case ObjectInfo::ZIPALGO_LZMA:
            case ObjectInfo::ZIPALGO_UNKNOWN:
                NOT_IMPLEMENTED(false);
        }
    }
    return NULL;
}

/*
 * Static methods
 */



/*
 * Private methods
 */

