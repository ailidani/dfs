

#include <stdint.h>

#include <string>
#include <set>
#include <iostream>

#include "tuneables.h"

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfsstr.h>
#include <libdfsutil/dfsfile.h>
#include <libdfsutil/scan.h>

#include <libdfs/repostore.h>

using namespace std;

string RepoStore_GetRepoPath(const string &fsName)
{
    if (!OriFile_Exists(Util_GetHome() + REPOSTORE_ROOTDIR)) {
        OriFile_MkDir(Util_GetHome() + REPOSTORE_ROOTDIR);
    }

    return Util_GetHome() + REPOSTORE_ROOTDIR"/" + fsName + ".ori";
}

string RepoStore_FindRepo(const string &fsNameOrPath)
{
    if (Util_IsValidName(fsNameOrPath)) {
        string val = RepoStore_GetRepoPath(fsNameOrPath);
        if (OriFile_Exists(val))
            return val;
    }

    return fsNameOrPath;
}

int getRepoHelper(set<string> *repos, const std::string &path)
{
    string fsName = OriFile_Basename(path);

    if (OriStr_EndsWith(fsName , ".ori"))
    {
        fsName = fsName.substr(0, fsName.size() - 4);
        repos->insert(fsName);
    }

    return 0;
}

set<string> RepoStore_GetRepos()
{
    set<string> repos;

    DirIterate(Util_GetHome() + REPOSTORE_ROOTDIR, &repos, getRepoHelper);

    return repos;
}

