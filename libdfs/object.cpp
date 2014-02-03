
#define __STDC_LIMIT_MACROS

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

using namespace std;

std::string Object::getPayload() {
    bytestream::ap bs(getPayloadStream());
    return bs->readAll();
}

