

#ifndef libdfs_repostore_h
#define libdfs_repostore_h

#include <stdint.h>

#include <string>
#include <set>

#define REPOSTORE_ROOTDIR   "/.ori"

std::string RepoStore_GetRepoPath(const std::string &fsName);
std::string RepoStore_FindRepo(const std::string &fsNameOrPath);
std::set<std::string> RepoStore_GetRepos();

#endif

