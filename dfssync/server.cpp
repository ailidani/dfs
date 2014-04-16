
#include <stdint.h>
#include <stdio.h>
#include <cstdlib>

#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>
#include <event2/util.h>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsstr.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfscrypt.h>
#include <libdfsutil/dfsnet.h>
#include <libdfsutil/systemexception.h>
#include <libdfsutil/thread.h>
#include <libdfsutil/kvserializer.h>
#include <libdfs/localrepo.h>

#include "orisyncconf.h"
#include "repoinfo.h"
#include "hostinfo.h"
#include "repocontrol.h"

using namespace std;

// Announcement UDP port
#define ORISYNC_UDPPORT         8051
// Advertisement interval
#define ORISYNC_ADVINTERVAL     5
// Reject advertisements with large time skew
#define ORISYNC_ADVSKEW         5
// Repository check interval
#define ORISYNC_MONINTERVAL     10
// Sync interval
#define ORISYNC_SYNCINTERVAL    5

OriSyncConf rc;

HostInfo myInfo;
RWLock infoLock;
map<string, HostInfo *> hosts;
RWLock hostsLock;

class Announcer : public Thread
{
public:
    Announcer() : Thread() {
        int status;
        int broadcast = 1;

        fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (fd < 0) {
            throw SystemException();
        }

        status = setsockopt(fd, SOL_SOCKET, SO_BROADCAST,
                            &broadcast, sizeof(broadcast));
        if (status < 0) {
            close(fd);
            throw SystemException();
        }

        memset(&dstAddr, 0, sizeof(dstAddr));
        dstAddr.sin_family = AF_INET;
        dstAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        dstAddr.sin_port = htons(ORISYNC_UDPPORT);
    }
    ~Announcer() {
        close(fd);
    }
    string generate() {
        RWKey::sp key = infoLock.readLock();
        char buf[32];
        string msg;

        // First 31 bytes of cluster with null
        memset(buf, 0, 32);
        strncpy(buf, rc.getCluster().c_str(), 31);
        msg.assign(buf, 32);
        msg.append(OriCrypt_Encrypt(myInfo.getBlob(), rc.getKey()));

        return msg;
    }
    void run() {
        while (!interruptionRequested()) {
            int status;
            string msg;
            size_t len;

            msg = generate();
            len = msg.size();

            status = sendto(fd, msg.c_str(), len, 0,
                            (struct sockaddr *)&dstAddr,
                            sizeof(dstAddr));
            if (status < 0) {
                perror("sendto");
            }

            sleep(ORISYNC_ADVINTERVAL);
        }

        DLOG("Announcer exited!");
    }
private:
    int fd;
    struct sockaddr_in dstAddr;
};

class Listener : public Thread
{
public:
    Listener() : Thread() {
        int status;
        struct sockaddr_in addr;
        int reuseaddr = 1;

        fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (fd < 0) {
            throw SystemException();
        }

        status = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                            &reuseaddr, sizeof(reuseaddr));
        if (status < 0) {
            perror("setsockopt");
            close(fd);
            throw SystemException();
        }
#ifdef __APPLE__
        status = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT,
                            &reuseaddr, sizeof(reuseaddr));
        if (status < 0) {
            perror("setsockopt");
            close(fd);
            throw SystemException();
        }
#endif /* __APPLE__ */

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(ORISYNC_UDPPORT);

        status = ::bind(fd, (struct sockaddr *)&addr, sizeof(addr));
        if (status < 0) {
            perror("bind");
            close(fd);
            throw SystemException();
        }
    }
    ~Listener() {
        close(fd);
    }
    void updateHost(KVSerializer &kv, const string &srcIp) {
        RWKey::sp key = hostsLock.writeLock();
        map<string, HostInfo *>::iterator it;
        string hostId = kv.getStr("hostId");
 
        // Update
        it = hosts.find(hostId);
        if (it == hosts.end()) {
            hosts[hostId] = new HostInfo(hostId, kv.getStr("cluster"));
        }
        hosts[hostId]->update(kv);
        hosts[hostId]->setPreferredIp(srcIp);
    }
    void parse(const char *buf, int len, sockaddr_in *source) {
        string ctxt;
        string ptxt;
        KVSerializer kv;
        char srcip[INET_ADDRSTRLEN];

        // Message too small
        if (len < 32)
            return;

        // Cluster ID
        ctxt.assign(buf, 32);
        ctxt[31] = '\0';
        if (strncmp(ctxt.c_str(), rc.getCluster().c_str(), 31) != 0)
            return;

        ctxt.assign(buf+32, len-32);
        ptxt = OriCrypt_Decrypt(ctxt, rc.getKey());
        try {
            kv.fromBlob(ptxt);
            //kv.dump();

            // Prevent replay attacks from leaking information
            uint64_t now = time(NULL);
            uint64_t ts = kv.getU64("time");
            if (ts > now + ORISYNC_ADVSKEW || ts < now - ORISYNC_ADVSKEW)
                return;

            // Ignore requests from self
            if (kv.getStr("hostId") == rc.getUUID())
                return;

            // Ignore messages from other clusters
            if (kv.getStr("cluster") != rc.getCluster())
                return;

            if (!inet_ntop(AF_INET, &(source->sin_addr),
                           srcip, INET_ADDRSTRLEN)) {
                LOG("Error parsing source ip");
                return;
            }

            // Add or update hostinfo
            updateHost(kv, srcip);
        } catch(SerializationException e) {
            LOG("Error encountered parsing announcement: %s", e.what());
            return;
        }
    }
    void dumpHosts() {
        RWKey::sp key = hostsLock.readLock();
        map<string, HostInfo *>::iterator it;

        cout << "=== Begin Hosts ===" << endl;
        for (it = hosts.begin(); it != hosts.end(); it++) {
            cout << it->second->getHost() << endl;
        }
        cout << "==== End Hosts ====" << endl << endl;
    }
    void run() {
        while (!interruptionRequested()) {
            char buf[1500];
            int len = 1500;
            struct sockaddr_in srcAddr;
            socklen_t srcAddrLen = sizeof(srcAddr);

            len = recvfrom(fd, buf, len, 0,
                           (struct sockaddr *)&srcAddr,
                           &srcAddrLen);
            if (len < 0) {
                perror("recvfrom");
                continue;
            }
            parse(buf, len, &srcAddr);

            //dumpHosts();
        }

        DLOG("Listener exited!");
    }
private:
    int fd;
};

class RepoMonitor : public Thread
{
public:
    RepoMonitor() : Thread() {
    }
    /*
     * Update repository information and return thre repoId, otherwise empty 
     * string.
     */
    void updateRepo(const string &path) {
        RepoControl repo = RepoControl(path);
        RepoInfo info;

        try {
            repo.open();
        } catch (SystemException &e) {
            WARNING("Failed to open repository %s: %s", path.c_str(), e.what());
            return;
        }

        RWKey::sp key = infoLock.writeLock();
        if (myInfo.hasRepo(repo.getUUID())) {
            info = myInfo.getRepo(repo.getUUID());
        } else {
            info = RepoInfo(repo.getUUID(), repo.getPath());
        }
        info.updateHead(repo.getHead());
        myInfo.updateRepo(repo.getUUID(), info);

        LOG("Checked %s: %s %s", path.c_str(), repo.getHead().c_str(), repo.getUUID().c_str());
        
        repo.close();

        return;
    }
    void run() {
        list<string> repos = rc.getRepos();
        list<string>::iterator it;

        while (!interruptionRequested()) {
            for (it = repos.begin(); it != repos.end(); it++) {
                updateRepo(*it);
            }

            sleep(ORISYNC_MONINTERVAL);
        }

        DLOG("RepoMonitor exited!");
    }
};

class Syncer : public Thread
{
public:
    Syncer() : Thread()
    {
    }
    void pullRepo(HostInfo &localHost,
                  HostInfo &remoteHost,
                  const std::string &uuid)
    {
        RepoInfo local = localHost.getRepo(uuid);
        RepoInfo remote = remoteHost.getRepo(uuid);
        RepoControl repo = RepoControl(local.getPath());

        DLOG("Local and Remote heads mismatch on repo %s", uuid.c_str());

        repo.open();
        if (!repo.hasCommit(remote.getHead())) {
            LOG("Pulling from %s:%s",
                remoteHost.getPreferredIp().c_str(),
                remote.getPath().c_str());
            repo.pull(remoteHost.getPreferredIp(), remote.getPath());
        }
        repo.close();
    }
    void checkRepo(HostInfo &infoSnapshot, const std::string &uuid)
    {
        map<string, HostInfo *> hostSnapshot;
        map<string, HostInfo *>::iterator it;
        RepoInfo localInfo = infoSnapshot.getRepo(uuid);

        hostSnapshot = hosts;

        for (it = hostSnapshot.begin(); it != hostSnapshot.end(); it++) {
            list<string> repos = it->second->listRepos();
            list<string>::iterator rIt;

            for (rIt = repos.begin(); rIt != repos.end(); rIt++) {
                RepoInfo info = it->second->getRepo(uuid);

                if (info.getHead() != localInfo.getHead()) {
                    pullRepo(infoSnapshot, *(it->second), uuid);
                }
            }
        }
    }
    void run() {
        while (!interruptionRequested()) {
            HostInfo infoSnapshot;
            list<string> repos;
            list<string>::iterator it;

            infoSnapshot = myInfo;
            repos = infoSnapshot.listRepos();

            for (it = repos.begin(); it != repos.end(); it++) {
                LOG("Syncer checking %s", (*it).c_str());
                checkRepo(infoSnapshot, *it);
            }

            sleep(ORISYNC_SYNCINTERVAL);
        }

        DLOG("Syncer exited!");
    }
};

Announcer *announcer;
Listener *listener;
RepoMonitor *repoMonitor;
Syncer *syncer;

void
Httpd_getRoot(struct evhttp_request *req, void *arg)
{
    struct evbuffer *buf;

    buf = evbuffer_new();
    if (buf == NULL) {
        evhttp_send_error(req, HTTP_INTERNAL, "Internal Error");
        return;
    }

    evbuffer_add_printf(buf, "Temporary String");
    evhttp_add_header(req->output_headers, "Content-Type", "text/html");
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
}

int
start_server()
{
    MSG("Starting OriSync");
    rc = OriSyncConf();
    announcer = new Announcer();
    listener = new Listener();
    repoMonitor = new RepoMonitor();
    syncer = new Syncer();

    myInfo = HostInfo(rc.getUUID(), rc.getCluster());
    // XXX: Update addresses periodically
    myInfo.setHost(OriStr_Join(OriNet_GetAddrs(), ','));

    announcer->start();
    listener->start();
    repoMonitor->start();
    syncer->start();

    struct event_base *base = event_base_new();
    struct evhttp *httpd = evhttp_new(base);
    evhttp_bind_socket(httpd, "0.0.0.0", 8051);

    evhttp_set_cb(httpd, "/", Httpd_getRoot, NULL);

    // Event loop
    event_base_dispatch(base);

    evhttp_free(httpd);
    event_base_free(base);

    // XXX: Wait for worker threads

    return 0;
}

