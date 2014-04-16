
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include "orisyncconf.h"

using namespace std;

int
cmd_hostremove(int argc, const char *argv[])
{
    OriSyncConf rc = OriSyncConf();

    if (argc != 2)
    {
        cout << "Sepcify a static host to remove" << endl;
        cout << "usage: orisync hostremove <HOSTNAME>" << endl;
    }

    rc.removeHost(argv[1]);

    return 0;
}

