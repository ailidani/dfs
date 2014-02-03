

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <string>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/systemexception.h>
#include <libdfs/localrepo.h> // ORI_PATH_UDSSOCK
#include <libdfs/udsclient.h>
#include <libdfs/udsrepo.h>

using namespace std;

#define D_READ 0
#define D_WRITE 1

/*
 * UDSClient
 */

UDSClient::UDSClient()
{
}

UDSClient::UDSClient(const string &repoPath)
    : fd(-1)
{
    udsPath = repoPath + ORI_PATH_UDSSOCK;
}

UDSClient::~UDSClient()
{
    disconnect();
}

int UDSClient::connect()
{
    int status, sock;
    socklen_t len;
    struct sockaddr_un remote;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0)
        throw SystemException();

    memset(&remote, 0, sizeof(remote));
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, udsPath.c_str());
    len = SUN_LEN(&remote);
    status = ::connect(sock, (struct sockaddr *)&remote, len);
    if (status < 0)
        throw SystemException();

    fd = sock;
    streamToChild.reset(new fdwstream(fd));

    // Sync by waiting for message from server
    if (!respIsOK()) {
        WARNING("Couldn't connect to UDS server!");
        return -1;
    }

    return 0;
}

void UDSClient::disconnect()
{
    close(fd);

    fd = -1;
}

bool UDSClient::connected() {
    return fd != -1;
}

void UDSClient::sendCommand(const string &command) {
    ASSERT(connected());
    streamToChild->writePStr(command);
}

void UDSClient::sendData(const string &data) {
    ASSERT(connected());
    size_t len = data.size();
    size_t off = 0;
    while (len > 0) {
        ssize_t written = write(fd, data.data()+off, len);
        if (written < 0) {
            perror("UDSClient::sendData write");
            exit(1);
        }
        len -= written;
        off += written;
    }
}

bytestream *UDSClient::getStream() {
    return new fdstream(fd, -1);
}

bool UDSClient::respIsOK() {
    uint8_t resp = 0;
    int status = read(fd, &resp, 1);
    if (status == 1 && resp == 0) return true;
    else {
        string errStr;
        fdstream fs(fd, -1);
        fs.readPStr(errStr);
        WARNING("UDS error (%d): %s", (int)resp, errStr.c_str());
        return false;
    }
}


/*
 * cmd_udsclient
 */
int cmd_udsclient(int argc, char * const argv[]) {
    if (argc < 2) {
        printf("Usage: ori udsclient REPOPATH\n");
        exit(1);
    }

    string remotePath = argv[1];
    UDSClient client(remotePath);
    if (client.connect() < 0) {
        printf("Error connecting to %s\n", argv[1]);
        exit(1);
    }

    printf("Connected\n");

    UDSRepo repo(&client);
    set<ObjectInfo> objs = repo.listObjects();
    vector<ObjectHash> hashes;
    for (set<ObjectInfo>::iterator it = objs.begin();
            it != objs.end();
            it++) {
        hashes.push_back((*it).hash);
        printf("Object: %s\n", (*it).hash.hex().c_str());
    }

    //bytestream *bs = repo.getObjects(hashes);

    // Test getObject
    Object::sp obj(repo.getObject(hashes[0]));
    if (!obj.get()) {
        fprintf(stderr, "Couldn't get object\n");
        return 1;
    }
    bytestream *bs = obj->getPayloadStream();
    string payload = bs->readAll();
    printf("Got payload (%lu)\n%s\n", payload.size(), payload.c_str());
    if (bs->error()) {
        fprintf(stderr, "Stream error: %s\n", bs->error());
    }
    printf("Object info:\n");
    obj->getInfo().print();

    set<string> exts = repo.listExt();
    set<string>::iterator extIt;
    printf("\nExtensions:\n");
    for (extIt = exts.begin(); extIt != exts.end(); extIt++)
    {
        printf("%s\n", (*extIt).c_str());
    }

    sleep(1);

    return 0;
}
