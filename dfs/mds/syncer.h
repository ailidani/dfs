
#ifndef mds_syncer_h
#define mds_syncer_h

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

#include "hostinfo.h"

class Syncer : public Thread
{
public:
    Syncer() : Thread()
    {
    }
    void pullRepo(HostInfo &localHost,
                  HostInfo &remoteHost,
                  const std::string &uuid);
    void checkRepo(HostInfo &infoSnapshot, const std::string &uuid);
    void run();
};

#endif
