

#ifndef libdfs_sshclient_h
#define libdfs_sshclient_h

#include <string>

#include <libdfsutil/stream.h>
#include "object.h"

class SshClient
{
public:
    SshClient(const std::string &remotePath);
    ~SshClient();

    int connect();
    void disconnect();
    bool connected();

    // At the moment the protocol is synchronous
    void sendCommand(const std::string &command);
    void sendData(const std::string &data);
    bytestream *getStream();

    bool respIsOK();

private:
    std::string remoteHost, remoteRepo;

    int fdFromChild, fdToChild;
    bytewstream::ap streamToChild;
    int childPid;
};

#endif
