
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include "mdsconf.h"

using namespace std;

int cmd_hostremove(int argc, const char *argv[])
{
    MDSConf rc = MDSConf();

    if (argc != 2)
    {
        cout << "Sepcify a static host to remove" << endl;
        cout << "usage: mds hostremove <HOSTNAME>" << endl;
    }

    rc.removeHost(argv[1]);

    return 0;
}

