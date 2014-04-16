
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>

#include <libdfsutil/dfsutil.h>

#include "orisyncconf.h"

using namespace std;

string
generateKey()
{
    int i;
    string val = "";
    const char alphanumeric[37] = "abcdefghijklmnopqrstuvwxyz0123456789";

    for (i = 0; i < 8; i++) {
        val += alphanumeric[rand() % 36];
    }

    return val;
}

int
cmd_init(int argc, const char *argv[])
{
    bool isFirst = false;
    string val;
    string clusterName;
    string clusterKey;
    OriSyncConf rc = OriSyncConf();

    do {
        cout << "Is this the first machine in the cluster (y/n)? ";
        cin >> val;
    } while (val != "y" && val != "n");

    if (val == "y") {
        isFirst = true;
    }

    cout << "Enter the cluster name: ";
    cin >> clusterName;

    if (isFirst) {
        clusterKey = generateKey();
    } else {
        cout << "Enter the cluster key: ";
        cin >> clusterKey;
    }

    rc.setCluster(clusterName, clusterKey);
    rc.setUUID(Util_NewUUID());

    cout << endl;
    cout << "Use the following configuration for all other machines:" << endl;
    cout << "Cluster Name: " << clusterName << endl;
    cout << "Cluster Key:  " << clusterKey << endl;
    cout << endl;
    cout << "Now use 'orisync add' to register repositories." << endl;

    return 0;
}

