
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>

#include <string>
#include <iostream>

#include <libdfs/udsclient.h>
#include <libdfs/udsrepo.h>

#include "fuse_cmd.h"

using namespace std;

extern UDSRepo repository;

void
usage_snapshot(void)
{
    cout << "cmd snapshot [OPTIONS] [SNAPSHOT NAME]" << endl;
    cout << endl;
    cout << "Create a snapshot of the file system." << endl;
    cout << endl;
    cout << "An additional snapshot name may be set, but it may only" << endl;
    cout << "contain letters, numbers, underscores, and periods." << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "    -m message     Add a message to the snapshot" << endl;
}

int
cmd_snapshot(int argc, char * const argv[])
{
    int ch;
    bool hasMsg = false;
    bool hasName = false;
    string msg;
    string name;

    struct option longopts[] = {
        { "message",    required_argument,  NULL,   'm' },
        { NULL,         0,                  NULL,   0   }
    };

    while ((ch = getopt_long(argc, argv, "m:", longopts, NULL)) != -1) {
        switch (ch) {
            case 'm':
                hasMsg = true;
                msg = optarg;
                break;
            default:
                printf("Usage: cmd snapshot [OPTIONS] [SNAPSHOT NAME]\n");
                return 1;
        }
    }
    argc -= optind;
    argv += optind;

    if (argc != 0 && argc != 1) {
        cout << "Wrong number of arguments." << endl;
        cout << "Usage: cmd snapshot [OPTIONS] [SNAPSHOT NAME]" << endl;
        return 1;
    }

    if (argc == 1) {
        hasName = true;
        name = argv[0];
    }

    if (hasName && !hasMsg) {
        msg = "Created snapshot '" + name + "'";
        hasMsg = true;
    }

    strwstream req;

    req.writePStr("snapshot");
    req.writeUInt8(hasMsg ? 1 : 0);
    req.writeUInt8(hasName ? 1 : 0);
    if (hasMsg)
        req.writeLPStr(msg);
    if (hasName)
        req.writeLPStr(name);

    strstream resp = repository.callExt("FUSE", req.str());
    if (resp.ended()) {
        cout << "status failed with an unknown error!" << endl;
        return 1;
    }

    switch (resp.readUInt8())
    {
        case 0:
            cout << "No changes" << endl;
            return 0;
        case 1:
        {
            ObjectHash hash;
            resp.readHash(hash);
            cout << "Committed " << hash.hex() << endl;
            return 0;
        }
        default:
            NOT_IMPLEMENTED(false);
    }

    return 0;
}

