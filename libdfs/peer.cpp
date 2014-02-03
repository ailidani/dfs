
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include <iostream>
//#include <boost/tr1/memory.hpp>
#include <memory>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfsfile.h>
#include <libdfs/repo.h>
#include <libdfs/localrepo.h>
#include <libdfs/httpclient.h>
#include <libdfs/httprepo.h>
#include <libdfs/sshclient.h>
#include <libdfs/sshrepo.h>
#include <libdfs/peer.h>

using namespace std;
//using namespace std::tr1;

/********************************************************************
 *
 *
 * Peer
 *
 *
 ********************************************************************/

Peer::Peer()
    : instaCloning(false), url(""), repoId(""), peerFile("")
{
    instaCloning = false;
    url = "";
    repoId = "";
    peerFile = "";
}

Peer::Peer(const std::string &path)
{
    instaCloning = false;
    url = "";
    repoId = "";
    peerFile = path;

    if (OriFile_Exists(path)) {
        string blob = OriFile_ReadFile(path);
        fromBlob(blob);
    }
}

Peer::~Peer()
{
}

string Peer::getBlob() const
{
    stringstream ss;
    string blob;

    blob = "url " + url + "\n";
    blob += "repoId " + repoId + "\n";
    if (instaCloning)
        blob += "instaCloning\n";

    return blob;
}

void Peer::fromBlob(const string &blob)
{
    string line;
    stringstream ss(blob);

    while (getline(ss, line, '\n')) {
        if (line.substr(0, 4) == "url ") {
            url = line.substr(4);
        } else if (line.substr(0, 7) == "repoId ") {
            repoId = line.substr(7);
        } else if (line.substr(0, 12) == "instaCloning") {
            instaCloning = true;
        } else{
            printf("Unsupported peer feature!\n");
            PANIC();
        }
    }
}

void Peer::setUrl(const std::string &path)
{
    // XXX: Verify URL or Path

    url = path;

    save();
}

std::string Peer::getUrl() const
{
    return url;
}

void Peer::setRepoId(const std::string &uuid)
{
    repoId = uuid;

    save();
}

std::string Peer::getRepoId() const
{
    return repoId;
}

void Peer::setInstaClone(bool ic)
{
    instaCloning = ic;

    save();
}

bool Peer::isInstaCloning() const
{
    return instaCloning;
}

Repo* Peer::getRepo()
{
    if (cachedRepo) {
        return cachedRepo.get();
    }

    if (Util_IsPathRemote(url)) {
        if (strncmp(url.c_str(), "http://", 7) == 0) {
            hc.reset(new HttpClient(url));
            cachedRepo.reset(new HttpRepo(hc.get()));
            hc->connect();
        } else {
            sc.reset(new SshClient(url));
            cachedRepo.reset(new SshRepo(sc.get()));
            sc->connect();
        }
    } else {
        char *path = realpath(url.c_str(), NULL);
        LocalRepo *local = new LocalRepo(path);
        local->open(path);
        cachedRepo.reset(local);
        free(path);
    }

    if (repoId == "") {
        setRepoId(cachedRepo->getUUID());
    }

    return cachedRepo.get();
}

void Peer::save() const
{
    if (peerFile != "") {
        string blob = getBlob();

        OriFile_WriteFile(blob.data(), blob.size(), peerFile);
    }
}

