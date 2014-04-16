
#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include "orisyncconf.h"

using namespace std;

int
cmd_hosts(int argc, const char *argv[])
{
    OriSyncConf rc = OriSyncConf();
    list<string> hosts = rc.getHosts();
    list<string>::iterator it;

    cout << "Registered Static Hosts:" << endl;
    for (it = hosts.begin(); it != hosts.end(); it++) {
        cout << *it << endl;
    }

    return 0;
}

