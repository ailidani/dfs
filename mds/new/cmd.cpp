
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include <libdfs/localrepo.h>
#include <libdfsutil/dfsutil.h>

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

int cmd_hosts(int argc, const char *argv[])
{
    MDSConf rc = MDSConf();
    list<string> hosts = rc.getHosts();
    list<string>::iterator it;

    cout << "Registered Static Hosts:" << endl;
    for (it = hosts.begin(); it != hosts.end(); it++) {
        cout << *it << endl;
    }

    return 0;
}

string generateKey()
{
    int i;
    string val = "";
    const char alphanumeric[37] = "abcdefghijklmnopqrstuvwxyz0123456789";

    for (i = 0; i < 8; i++) {
        val += alphanumeric[rand() % 36];
    }

    return val;
}

int cmd_init(int argc, const char *argv[])
{
    bool isFirst = false;
    string val;
    string clusterName;
    string clusterKey;
    MDSConf rc = MDSConf();

    do {
        cout << "Is this the first MDS machine in the cluster (y/n)? ";
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
    cout << "Now use 'mds add' to register repositories." << endl;

    return 0;
}

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
