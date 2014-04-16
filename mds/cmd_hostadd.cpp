
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include "mdsconf.h"

using namespace std;

int cmd_hostadd(int argc, const char *argv[])
{
    MDSConf rc = MDSConf();

    if (argc != 2)
    {
        cout << "Sepcify a host to add" << endl;
        cout << "usage: mds hostadd <HOSTNAME>" << endl;
    }

    // XXX: verify hostname

    rc.addHost(argv[1]);

    return 0;
}

