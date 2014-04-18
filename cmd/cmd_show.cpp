
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include <libdfs/udsclient.h>
#include <libdfs/udsrepo.h>

#include "fuse_cmd.h"

using namespace std;

extern UDSRepo repository;

int
cmd_show(int argc, char * const argv[])
{
    string rootPath = OF_RepoPath();

    if (rootPath == "") {
        cout << "No repository found!" << endl;
        return 1;
    }

    cout << "--- Repository ---" << endl;
    cout << "Root: " << rootPath << endl;
    cout << "UUID: " << repository.getUUID() << endl;
    cout << "Version: " << repository.getVersion() << endl;
    cout << "HEAD: " << repository.getHead().hex() << endl;
    //printf("Peers:\n");
    // for
    // printf("    %s\n", hostname);
    return 0;
}

