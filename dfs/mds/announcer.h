
#ifndef mds_announcer_h
#define mds_announcer_h

#include <libdfsutil/thread.h>
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

class Announcer : public Thread
{
public:
    Announcer();
    ~Announcer() {
        close(fd);
    }
    std::string generate();
    void run();
private:
    int fd;
    struct sockaddr_in dstAddr;
};

#endif
