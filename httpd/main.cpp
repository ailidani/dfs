

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <resolv.h>

#include <string>
#include <iostream>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/zeroconf.h>
#include <libdfs/version.h>
#include <libdfs/repostore.h>
#include <libdfs/localrepo.h>
#include <libdfs/httpserver.h>

using namespace std;

LocalRepo repository;

void usage()
{
    printf("Ori Distributed Personal File System (%s) - HTTP Server\n",
            ORI_VERSION_STR);
    cout << "Usage: ori_httpd [OPTIONS] FSNAME" << endl << endl;
    cout << "Options:" << endl;
    cout << "    -p port    Set the HTTP port number (default: 8080)" << endl;
#if !defined(WITHOUT_MDNS)
    cout << "    -m         Enable mDNS (default)" << endl;
    cout << "    -n         Disable mDNS" << endl;
#endif
    cout << "    -h         Show this message" << endl;
}

int main(int argc, char *argv[])
{
    int ch;
    bool mDNS_flag = true;
    unsigned long port = 8080;
    string rootPath;

    while ((ch = getopt(argc, argv, "p:mnh")) != -1) {
        switch (ch) {
            case 'p':
            {
                char *p;
                port = strtoul(optarg, &p, 10);
                if (*p != '\0' || port > 65535) {
                    cout << "Invalid port number '" << optarg << "'" << endl;
                    usage();
                    return 1;
                }
                break;
            }
            case 'm':
                mDNS_flag = true;
                break;
            case 'n':
                mDNS_flag = false;
                break;
            case 'h':
                usage();
                return 0;
            case '?':
            default:
                usage();
                return 1;
        }
    }
    argc -= optind;
    argv += optind;

    if (argc != 1) {
        cout << "File system name is missing!" << endl;
        usage();
        return 1;
    }

    rootPath = RepoStore_FindRepo(argv[0]);

    try {
        repository.open(rootPath);
    } catch (std::exception &e) {
        cout << e.what() << endl;
        cout << "Could not open the local repository!" << endl;
        usage();
        return 1;
    }

    dfs_open_log(repository.getLogPath());
    LOG("libevent %s", event_get_version());

    HTTPServer server = HTTPServer(repository, port);
    server.start(mDNS_flag);

    return 0;
}

