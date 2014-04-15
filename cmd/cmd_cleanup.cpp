
#include <stdint.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <getopt.h>

#include <string>
#include <iostream>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfsfile.h>
#include <libdfsutil/scan.h>
#include <libdfs/repostore.h>
#include <libdfs/localrepo.h>

using namespace std;

void
usage_cleanup()
{
    cout << "ori cleanup FSNAME" << endl;
    cout << endl;
    cout << "Removes any remaining unclean data from a crashed FUSE" << endl;
    cout << "mount.  You may want to check the temporary directory" << endl;
    cout << "by hand to find any files you want to recover.  Be" << endl; 
    cout << "careful as this operation is permanent!" << endl;
}

static int
cleanupHelper(int unused, const string &path)
{
    int status;

    if (OriFile_IsDirectory(path))
        status = OriFile_RmDir(path);
    else
        status = OriFile_Delete(path);

    if (status < 0) {
        char buf[128];
        strerror_r(-status, buf, 128);
        fprintf(stderr, "Failed to delete '%s': %s", path.c_str(), buf);
    }

    return 0;
}

/*
 * Cleanup temporary state
 */
int
cmd_cleanup(int argc, char * const argv[])
{
    string fsName;
    string rootPath;

    if (argc != 2) {
        if (argc == 1)
            printf("Argument required!\n");
        if (argc > 2)
            printf("Too many arguments!\n"); 
        printf("Usage: ori cleanup FSNAME\n");
        return 1;
    }
    
    fsName = argv[1];
    if (!Util_IsValidName(fsName)) {
        printf("Name contains invalid charecters!\n");
        return 1;
    }

    rootPath = RepoStore_GetRepoPath(fsName);
    if (!OriFile_Exists(rootPath)) {
        printf("File system does not exist!\n");
        return 1;
    }

    // XXX: Add check to make sure it's not mounted

    OriFile_Delete(rootPath + ORI_PATH_UDSSOCK);
    DirRTraverse(rootPath + ORI_PATH_TMP + "fuse", 0, cleanupHelper);
    OriFile_RmDir(rootPath + ORI_PATH_TMP + "fuse");
    OriFile_Delete(rootPath + ORI_PATH_LOCK);

    // Common coredump names
    OriFile_Delete(rootPath + "/core");
    OriFile_Delete(rootPath + "/orifs.core");

    return 0;
}

