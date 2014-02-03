

#ifndef libdfs_udsclient_h
#define libdfs_udsclient_h

#include <string>

#include <libdfsutil/stream.h>
#include "object.h"

class UDSClient
{
public:
    UDSClient();
    UDSClient(const std::string &remotePath);
    ~UDSClient();

    int connect();
    void disconnect();
    bool connected();

    // At the moment the protocol is synchronous
    void sendCommand(const std::string &command);
    void sendData(const std::string &data);
    bytestream *getStream();

    bool respIsOK();

private:
    std::string udsPath, remoteRepo;

    int fd;
    bytewstream::ap streamToChild;
};


#endif
