
#ifndef mds_listener_h
#define mds_listener_h

#include "mds.h"

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
        addr.sin_port = htons(MDS_UDPPORT);

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
            if (ts > now + MDS_ADVSKEW || ts < now - MDS_ADVSKEW)
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

#endif