
#ifndef mds_listener_h
#define mds_listener_h

#include <libdfsutil/thread.h>
#include <libdfsutil/kvserializer.h>
#include <stdint.h>
#include <stdio.h>
#include <cstdlib>

#include <unistd.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>

class Listener : public Thread
{
public:
    Listener();
    ~Listener() {
        close(fd);
    }
    void updateHost(KVSerializer &kv, const std::string &srcIp);
    void parse(const char *buf, int len, sockaddr_in *source);
    void dumpHosts();
    void run();
private:
    int fd;
};

#endif
