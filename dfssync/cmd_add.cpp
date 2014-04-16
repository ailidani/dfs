
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include <libdfs/localrepo.h>

#include "orisyncconf.h"

using namespace std;

int
cmd_add(int argc, const char *argv[])
{
    OriSyncConf rc = OriSyncConf();

    if (argc != 2)
    {
        cout << "Sepcify a repository to add" << endl;
        cout << "usage: orisync add <repository>" << endl;
    }

    // XXX: verify repo

    rc.addRepo(argv[1]);

    return 0;
}

