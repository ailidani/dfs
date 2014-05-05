

#include <cstring>
#include <stdint.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include <sys/un.h>

#include <string>
#include <map>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/systemexception.h>
#include <libdfs/repostore.h>
#include <libdfs/localrepo.h>
#include <libdfs/udsserver.h>


using namespace std;

static UDSServer *server = NULL;

string UDSExtensionCB(LocalRepo *repo, const string &data)
{
    return MDS::get()->process(data);
}

void UDSServerStart(LocalRepo *repo)
{
    LOG("Starting Unix domain socket server");

    server = new UDSServer(repo);
    if (server) {
        server->registerExt("MDS", UDSExtensionCB);
        server->start();
    }

    return;
}

void UDSServerStop()
{
    if (server != NULL) {
        server->shutdown();
    }
    delete server;
}

