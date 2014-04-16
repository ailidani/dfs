
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include "mdsconf.h"

using namespace std;

int cmd_list(int argc, const char *argv[])
{
    MDSConf rc = MDSConf();
    list<string> repos = rc.getRepos();
    list<string>::iterator it;

    cout << "Registered Repositories:" << endl;
    for (it = repos.begin(); it != repos.end(); it++) {
        cout << *it << endl;
    }

    return 0;
}

