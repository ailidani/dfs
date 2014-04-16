
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include "orisyncconf.h"

using namespace std;

int
cmd_remove(int argc, const char *argv[])
{
    OriSyncConf rc = OriSyncConf();

    if (argc != 2)
    {
        cout << "Sepcify a repository to remove" << endl;
        cout << "usage: orisync remove <repository>" << endl;
    }

    rc.removeRepo(argv[1]);

    return 0;
}

