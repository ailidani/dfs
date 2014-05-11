

#include "repomonitor.h"
#include "mds.h"

extern MDS* mds;

void RepoMonitor::updateRepo(const std::string &path)
{
    RepoControl repo = RepoControl(path);
    RepoInfo info;

    try {
        repo.open();
    } catch (SystemException &e) {
        WARNING("Failed to open repository %s: %s", path.c_str(), e.what());
        return;
    }

    RWKey::sp key = mds->infoLock.writeLock();
    if (mds->myInfo.hasRepo(repo.getUUID())) {
        info = mds->myInfo.getRepo(repo.getUUID());
    } else {
        info = RepoInfo(repo.getUUID(), repo.getPath());
    }
    info.updateHead(repo.getHead());
    mds->myInfo.updateRepo(repo.getUUID(), info);

    LOG("Checked %s: %s %s", path.c_str(), repo.getHead().c_str(), repo.getUUID().c_str());

    repo.close();

    return;
}

void RepoMonitor::run()
{
    std::list<std::string> repos = mds->rc.getRepos();
    std::list<std::string>::iterator it;

    while (!interruptionRequested()) {
        for (it = repos.begin(); it != repos.end(); it++) {
            updateRepo(*it);
        }

        sleep(MDS_MONINTERVAL);
    }

    DLOG("RepoMonitor exited!");
}
