
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include "orisyncconf.h"

using namespace std;

int
cmd_hostadd(int argc, const char *argv[])
{
    OriSyncConf rc = OriSyncConf();

    if (argc != 2)
    {
        cout << "Sepcify a host to add" << endl;
        cout << "usage: orisync hostadd <HOSTNAME>" << endl;
    }

    // XXX: verify hostname

    rc.addHost(argv[1]);

    return 0;
}

