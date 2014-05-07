
#include "announcer.h"
#include "mds.h"

Announcer::Announcer() : Thread()
{
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
    dstAddr.sin_port = htons(MDS_UDPPORT);
}

std::string Announcer::generate()
{
    RWKey::sp key = MDS::get()->infoLock.readLock();
    char buf[32];
    std::string msg;

    // First 31 bytes of cluster with null
    memset(buf, 0, 32);
    strncpy(buf, MDS::get()->rc.getCluster().c_str(), 31);
    msg.assign(buf, 32);
    msg.append(OriCrypt_Encrypt(MDS::get()->myInfo.getBlob(), MDS::get()->rc.getKey()));

    return msg;
}

void Announcer::run()
{
    while (!interruptionRequested()) {
        int status;
        std::string msg;
        size_t len;

        msg = generate();
        len = msg.size();

        status = sendto(fd, msg.c_str(), len, 0,
                        (struct sockaddr *)&dstAddr,
                        sizeof(dstAddr));
        if (status < 0) {
            perror("sendto");
        }

        sleep(MDS_ADVINTERVAL);
    }

    DLOG("Announcer exited!");
}

