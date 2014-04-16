
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include <iostream>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfsfile.h>

#include "orisyncconf.h"

using namespace std;

/********************************************************************
 *
 *
 * OriSyncConf
 *
 *
 ********************************************************************/

OriSyncConf::OriSyncConf()
    : name(""), key(""), repos(), rcFile("")
{
    string home = Util_GetHome();

    ASSERT(home != "");

    if (!OriFile_Exists(home + "/.ori"))
        OriFile_MkDir(home + "/.ori");

    rcFile = home + "/.ori/orisyncrc";
    if (OriFile_Exists(rcFile)) {
        load();
    }
}

OriSyncConf::~OriSyncConf()
{
}

void
OriSyncConf::setCluster(const string &clusterName,
                        const string &clusterKey)
{
    name = clusterName;
    key = clusterKey;

    save();
}

void
OriSyncConf::setUUID(const string &uuid)
{
    machineid = uuid;

    save();
}

string
OriSyncConf::getCluster() const
{
    return name;
}

string
OriSyncConf::getKey() const
{
    return key;
}

string
OriSyncConf::getUUID() const
{
    return machineid;
}

void
OriSyncConf::addRepo(const string &repoPath)
{
    repos.push_back(repoPath);

    save();
}

void
OriSyncConf::removeRepo(const string &repoPath)
{
    repos.remove(repoPath);

    save();
}

list<string>
OriSyncConf::getRepos() const
{
    return repos;
}

void
OriSyncConf::addHost(const string &host)
{
    hosts.push_back(host);

    save();
}

void
OriSyncConf::removeHost(const string &host)
{
    hosts.remove(host);

    save();
}

list<string>
OriSyncConf::getHosts() const
{
    return hosts;
}

void
OriSyncConf::save() const
{
    if (rcFile != "") {
        string blob = getBlob();

        OriFile_WriteFile(blob.data(), blob.size(), rcFile);
    }
}

void
OriSyncConf::load()
{
    string blob = OriFile_ReadFile(rcFile);

    fromBlob(blob);
}

string
OriSyncConf::getBlob() const
{
    stringstream ss;
    string blob;
    
    blob = "# Cluster Configuration\n";
    blob += "cluster-name " + name + "\n";
    blob += "cluster-key " + key + "\n";
    blob += "machine-id " + machineid + "\n";

    blob += "\n# Hosts\n";
    for (list<string>::const_iterator it = hosts.begin(); it != hosts.end(); it++)
    {
        blob += "host " + *it + "\n";
    }

    blob += "\n# Repositories\n";
    for (list<string>::const_iterator it = repos.begin(); it != repos.end(); it++)
    {
        blob += "repo " + *it + "\n";
    }

    return blob;
}

void
OriSyncConf::fromBlob(const string &blob)
{
    string line;
    stringstream ss(blob);

    while (getline(ss, line, '\n')) {
        if (line.substr(0, 1) == "#" || line == "") {
            // comment or blank line
        } else if (line.substr(0, 13) == "cluster-name ") {
            name = line.substr(13);
        } else if (line.substr(0, 12) == "cluster-key ") {
            key = line.substr(12);
        } else if (line.substr(0, 11) == "machine-id ") {
            machineid = line.substr(11);
        } else if (line.substr(0, 5) == "host ") {
            hosts.push_back(line.substr(5));
        } else if (line.substr(0, 5) == "repo ") {
            repos.push_back(line.substr(5));
        } else {
            printf("Invalid config file: %s\n", line.c_str());
            PANIC();
        }
    }
}

