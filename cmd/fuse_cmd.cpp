

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>

#include <libdfsutil/dfsfile.h>

#include "fuse_cmd.h"

using namespace std;

bool OF_HasFuse()
{
    return OF_ControlPath() != "";
}

string OF_RootPath()
{
    char *cwdbuf = getcwd(NULL, 0);
    string path = cwdbuf;
    free(cwdbuf);

    while (path.size() > 0) {
        string control_path = path + "/" + ORI_CONTROL_FILENAME;
        if (OriFile_Exists(control_path)) {
            return path;
        }
        path = OriFile_Dirname(path);
    }

    return "";
}

string OF_ControlPath()
{
    char *cwdbuf = getcwd(NULL, 0);
    string path = cwdbuf;
    free(cwdbuf);

    while (path.size() > 0) {
        string control_path = path + "/" + ORI_CONTROL_FILENAME;
        if (OriFile_Exists(control_path)) {
            return control_path;
        }
        path = OriFile_Dirname(path);
    }

    return "";
}

string OF_RepoPath()
{
    int status;
    string controlPath = OF_ControlPath();
    int fd;
    struct stat sb;
    char buf[1024];

    if (controlPath.size() == 0)
        return "";

    fd = open(controlPath.c_str(), O_RDONLY);
    if (fd < 0) {
        perror("OF_RepoPath: open");
        return "";
    }

    status = fstat(fd, &sb);
    if (status < 0) {
        perror("OF_RepoPath: fstat");
        return "";
    }

    status = read(fd, buf, sb.st_size);
    if (status < 0) {
        perror("OF_RepoPath: read");
        return "";
    }

    buf[status] = '\0';

    return string(buf);
}

