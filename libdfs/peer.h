
#ifndef libdfs_peer_h
#define libdfs_peer_h

#include <stdint.h>
#include <time.h>

#include <utility>
#include <string>
//#include <boost/tr1/memory.hpp>
#include <memory>

class Repo;
class HttpClient;
class SshClient;

/*
 * Peer class manages connections to other hosts and persists information to
 * disk. Eventually the last synced commitId should be saved as an
 * optimization.
 */

class Peer
{
public:
    Peer();
    explicit Peer(const std::string &peerFile);
    ~Peer();
    std::string getBlob() const;
    void fromBlob(const std::string &blob);
    void setUrl(const std::string &url);
    std::string getUrl() const;
    void setRepoId(const std::string &uuid);
    std::string getRepoId() const;
    void setInstaClone(bool ic);
    bool isInstaCloning() const;
    Repo *getRepo();
private:
    void save() const;
    // Persistent state
    bool instaCloning;
    std::string url;
    std::string repoId;
    // Volatile State
    std::string peerFile;
    std::shared_ptr<Repo> cachedRepo;
    std::shared_ptr<HttpClient> hc;
    std::shared_ptr<SshClient> sc;
};

#endif
