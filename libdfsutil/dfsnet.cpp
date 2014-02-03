
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

#include <string>
#include <list>
#include <exception>
#include <stdexcept>

#include <libdfsutil/debug.h>
#include <libdfsutil/systemexception.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfsnet.h>

using namespace std;

std::string OriNet_ResolveHost(const string &hostname) {
    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;

    int status = getaddrinfo(hostname.c_str(), "80", &hints, &result);
    if (status < 0 || result == NULL) {
        perror("getaddrinfo");
        return "";
    }

    struct sockaddr_in *sa = (struct sockaddr_in *)result->ai_addr;
    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &sa->sin_addr, buf, INET_ADDRSTRLEN);

    freeaddrinfo(result);

    DLOG("Resolved IP addr: %s", buf);
    return buf;
}

vector<string> OriNet_GetAddrs()
{
    struct ifaddrs *ifaddr, *ifa;
    vector<string> rval;

    if (getifaddrs(&ifaddr) == -1) {
        throw SystemException();
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        int family;
        int status;
        char hostAddr[NI_MAXHOST];

        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;
        if (family != AF_INET)
            continue;

        struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
        if (addr->sin_addr.s_addr == htonl(INADDR_LOOPBACK))
            continue;

        status = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                             hostAddr, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (status != 0) {
            freeifaddrs(ifaddr);
            throw SystemException(EINVAL);
        }

        rval.push_back(hostAddr);
    }

    freeifaddrs(ifaddr);

    return rval;
}

string OriNet_Hostname()
{
    int status;
    char hostname[128];

    status = gethostname(hostname, sizeof(hostname));
    if (status < -1) {
        return "unknown";
    }

    return hostname;
}

string OriNet_Domainname()
{
    int status;
    char domainname[128];

    status = getdomainname(domainname, sizeof(domainname));
    if (status < -1) {
        return "unknown";
    }

    return domainname;
}

