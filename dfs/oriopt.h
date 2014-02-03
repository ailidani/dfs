

#ifndef dfs_oriopt_h
#define dfs_oriopt_h

#include <string>

struct mount_ori_config {
    int shallow;
    int nocache;
    int journal;
    int single;
    int debug;
    std::string repoPath;
    std::string clonePath;
    std::string mountPoint;
};

#endif

