
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include <iostream>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfsfile.h>

#include "mdsconf.h"

using namespace std;

MDSConf::MDSConf()
    : name(""), key(""), repos(), rcFile("")
{
    string home = Util_GetHome();

    ASSERT(home != "");

    if (!OriFile_Exists(home + "/.ori"))
        OriFile_MkDir(home + "/.ori");

    rcFile = home + "/.ori/mdsconf";
    if (OriFile_Exists(rcFile)) {
        load();
    }
}

MDSConf::~MDSConf()
{
}

void MDSConf::setCluster(const string &clusterName,
                        const string &clusterKey)
{
    name = clusterName;
    key = clusterKey;

    save();
}

void MDSConf::setUUID(const string &uuid)
{
    machineid = uuid;

    save();
}

string MDSConf::getCluster() const
{
    return name;
}

string MDSConf::getKey() const
{
    return key;
}

string MDSConf::getUUID() const
{
    return machineid;
}

void MDSConf::addRepo(const string &repoPath)
{
    repos.push_back(repoPath);

    save();
}

void MDSConf::removeRepo(const string &repoPath)
{
    repos.remove(repoPath);

    save();
}

list<string> MDSConf::getRepos() const
{
    return repos;
}

void MDSConf::addHost(const string &host)
{
    hosts.push_back(host);

    save();
}

void MDSConf::removeHost(const string &host)
{
    hosts.remove(host);

    save();
}

list<string> MDSConf::getHosts() const
{
    return hosts;
}

void MDSConf::save() const
{
    if (rcFile != "") {
        string blob = getBlob();

        OriFile_WriteFile(blob.data(), blob.size(), rcFile);
    }
}

void MDSConf::load()
{
    string blob = OriFile_ReadFile(rcFile);

    fromBlob(blob);
}

string MDSConf::getBlob() const
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

void MDSConf::fromBlob(const string &blob)
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

