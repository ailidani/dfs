
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include "mdsconf.h"

using namespace std;

int cmd_remove(int argc, const char *argv[])
{
    MDSConf rc = MDSConf();

    if (argc != 2)
    {
        cout << "Sepcify a repository to remove" << endl;
        cout << "usage: mds remove <repository>" << endl;
    }

    rc.removeRepo(argv[1]);

    return 0;
}

