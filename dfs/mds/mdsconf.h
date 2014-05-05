
#ifndef mds_mdsconf_h
#define mds_mdsconf_h

#include <stdint.h>

#include <string>
#include <list>

class MDSConf
{
public:
    MDSConf();
    ~MDSConf();
    void setCluster(const std::string &clusterName,
                    const std::string &clusterKey);
    void setUUID(const std::string &uuid);
    std::string getCluster() const;
    std::string getKey() const;
    std::string getUUID() const;
    void addRepo(const std::string &repoPath);
    void removeRepo(const std::string &repoPath);
    std::list<std::string> getRepos() const;
    void addHost(const std::string &host);
    void removeHost(const std::string &host);
    std::list<std::string> getHosts() const;
private:
    void load();
    void save() const;
    std::string getBlob() const;
    void fromBlob(const std::string &blob);
    // Persistent state
    std::string name;
    std::string key;
    std::string machineid;
    std::list<std::string> repos;
    std::list<std::string> hosts;
    // Volatile State
    std::string rcFile;
};

#endif

