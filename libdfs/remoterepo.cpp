

#include <stdint.h>

#include <sys/types.h>

#include <string>
//#include <boost/tr1/memory.hpp>
#include <memory>

#include <libdfsutil/dfsutil.h>
#include <libdfsutil/systemexception.h>
#include <libdfs/repo.h>
#include <libdfs/localrepo.h>
#include <libdfs/sshrepo.h>
#include <libdfs/sshclient.h>
#include <libdfs/httprepo.h>
#include <libdfs/httpclient.h>
#include <libdfs/udsrepo.h>
#include <libdfs/udsclient.h>
#include <libdfs/remoterepo.h>

using namespace std;
//using namespace std::tr1;

RemoteRepo::RemoteRepo()
    : r(NULL)
{
}

RemoteRepo::~RemoteRepo()
{
    disconnect();
}

bool RemoteRepo::connect(const string &url)
{
    this->url = url;
    if (Util_IsPathRemote(url)) {
        if (strncmp(url.c_str(), "http://", 7) == 0)
        {
            hc.reset(new HttpClient(url));
            r = new HttpRepo(hc.get());
            return (hc->connect() == 0);
        }
        else
        {
            sc.reset(new SshClient(url));
            r = new SshRepo(sc.get());
            return (sc->connect() == 0);
        }
    }
    else
    {
        UDSClient *udsClient = NULL;
        try {
            udsClient = new UDSClient(url);
            int status = udsClient->connect();
            uc.reset(udsClient);
            r = new UDSRepo(uc.get());
            return (status == 0);
        } catch (SystemException e) {
            delete udsClient;
        }
        LocalRepo *lr = new LocalRepo(url);
        r = lr;
        lr->open(url);
        return true;
    }

    return false;
}

void RemoteRepo::disconnect()
{
    if (hc)
        hc->disconnect();
    if (sc)
        sc->disconnect();
    if (uc)
        uc->disconnect();
    if (r)
        delete r;
}

Repo* RemoteRepo::operator->()
{
    return r;
}

Repo* RemoteRepo::get()
{
    return r;
}

