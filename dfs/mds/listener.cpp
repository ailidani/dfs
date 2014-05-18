
#include "listener.h"
#include "mds.h"
#include "server.h"

Listener::Listener() : Thread()
{
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

void Listener::updateHost(KVSerializer &kv, const std::string &srcIp)
{
    RWKey::sp key = MDS::instance().hostsLock.writeLock();
    std::map<std::string, HostInfo *>::iterator it;
    std::string hostId = kv.getStr("hostId");

    // Update
    it = MDS::instance().hosts.find(hostId);
    if (it == MDS::instance().hosts.end()) {
    	MDS::instance().hosts[hostId] = new HostInfo(hostId, kv.getStr("cluster"));
    }
    MDS::instance().hosts[hostId]->update(kv);
    MDS::instance().hosts[hostId]->setPreferredIp(srcIp);

    server::instance().add_peer(hostId);
}

void Listener::parse(const char *buf, int len, sockaddr_in *source)
{
    std::string ctxt;
    std::string ptxt;
    KVSerializer kv;
    char srcip[INET_ADDRSTRLEN];

    // Message too small
    if (len < 32)
        return;
    std::cout<<"message not too small"<<std::endl;

    // Cluster ID
    ctxt.assign(buf, 32);
    ctxt[31] = '\0';
    if (strncmp(ctxt.c_str(), MDS::instance().rc.getCluster().c_str(), 31) != 0)
        return;

    ctxt.assign(buf+32, len-32);
    ptxt = OriCrypt_Decrypt(ctxt, MDS::instance().rc.getKey());
    try {
        kv.fromBlob(ptxt);
        //kv.dump();

        // Prevent replay attacks from leaking information
        uint64_t now = time(NULL);
        uint64_t ts = kv.getU64("time");
        if (ts > now + MDS_ADVSKEW || ts < now - MDS_ADVSKEW)
            return;
        std::cout<<"time is fine"<<std::endl;
        // Ignore requests from self
        if (kv.getStr("hostId") == MDS::instance().rc.getUUID())
            return;
        std::cout<<"not self"<<std::endl;

        // Ignore messages from other clusters
        if (kv.getStr("cluster") != MDS::instance().rc.getCluster())
            return;
        std::cout<<"same cluster"<<std::endl;

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

void Listener::dumpHosts()
{
    RWKey::sp key = MDS::instance().hostsLock.readLock();
    std::map<std::string, HostInfo *>::iterator it;

    std::cout << "=== Begin Hosts ===" << std::endl;
    for (it = MDS::instance().hosts.begin(); it != MDS::instance().hosts.end(); it++) {
        std::cout << it->second->getHost() << std::endl;
    }
    std::cout << "==== End Hosts ====" << std::endl << std::endl;
}

void Listener::run()
{
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

        dumpHosts();
    }

    DLOG("Listener exited!");
}
