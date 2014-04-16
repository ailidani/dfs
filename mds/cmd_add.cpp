
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include <libdfs/localrepo.h>

#include "mdsconf.h"

using namespace std;

int cmd_add(int argc, const char *argv[])
{
    MDSConf rc = MDSConf();

    if (argc != 2)
    {
        cout << "Sepcify a repository to add" << endl;
        cout << "usage: mds add <repository>" << endl;
    }

    // XXX: verify repo

    rc.addRepo(argv[1]);

    return 0;
}

