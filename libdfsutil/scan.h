

#ifndef libdfsutil_scan_h
#define libdfsutil_scan_h

#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

/*
 * Scan a directory.
 */
template <typename T>
int
DirIterate(const std::string &path,
           T arg,
           int (*f)(T, const std::string&))
{
    int status;
    DIR *dir;
    struct dirent *entry;
    std::string fullpath;

    if (path == "") {
        dir = opendir(".");
    } else {
        dir = opendir(path.c_str());
    }

    if (dir == NULL)
        return 1;

    while ((entry = readdir(dir)) != NULL) {
        if (path != "") {
            fullpath = path + "/";
        } else {
            fullpath = "";
        }
        fullpath += entry->d_name;

        if (strcmp(entry->d_name, ".") == 0)
            continue;
        if (strcmp(entry->d_name, "..") == 0)
            continue;

        // '.ori' should never be scanned
        if (strcmp(entry->d_name, ".ori") == 0)
            continue;

        if (f != NULL) {
            status = f(arg, fullpath);
            if (status != 0) {
                closedir(dir);
                return status;
            }
        }
    }

    closedir(dir);
    return 0;
}

/*
 * Recursive traverse of a directory tree.
 */
template <typename T>
int
DirTraverse(const std::string &path,
           T arg,
           int (*f)(T, const std::string&))
{
    int status;
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    std::string fullpath;

    if (path == "") {
        dir = opendir(".");
    } else {
        dir = opendir(path.c_str());
    }

    if (dir == NULL) {
        perror("Scan_RTraverse opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (path != "") {
            fullpath = path + "/";
        } else {
            fullpath = "";
        }
        fullpath += entry->d_name;

        if (strcmp(entry->d_name, ".") == 0)
            continue;
        if (strcmp(entry->d_name, "..") == 0)
            continue;

        // '.ori' should never be scanned
        if (strcmp(entry->d_name, ".ori") == 0)
            continue;

        if (f != NULL) {
            status = f(arg, fullpath);
            if (status != 0) {
                closedir(dir);
                return status;
            }
        }

        // This check avoids symbol links to directories.
        if (entry->d_type == DT_DIR) {
            status = DirTraverse(fullpath, arg, f);
            if (status != 0) {
                //closedir(dir);
                //return status;
                fprintf(stderr, "Couldn't scan directory %s\n", fullpath.c_str());
                continue;
            }
        }
    }

    closedir(dir);
    return 0;
}

/*
 * Recursive traverse of a directory tree calling the callback function for
 * directories after traversing inside.
 */
template <typename T>
int
DirRTraverse(const std::string &path,
           T arg,
           int (*f)(T, const std::string&))
{
    int status;
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    std::string fullpath;

    if (path == "") {
        dir = opendir(".");
    } else {
        dir = opendir(path.c_str());
    }

    if (dir == NULL) {
        perror("Scan_RTraverse opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (path != "") {
            fullpath = path + "/";
        } else {
            fullpath = "";
        }
        fullpath += entry->d_name;

        if (strcmp(entry->d_name, ".") == 0)
            continue;
        if (strcmp(entry->d_name, "..") == 0)
            continue;

        // '.ori' should never be scanned
        if (strcmp(entry->d_name, ".ori") == 0)
            continue;

        // This check avoids symbol links to directories.
        if (entry->d_type == DT_DIR) {
            status = DirRTraverse(fullpath, arg, f);
            if (status != 0) {
                //closedir(dir);
                //return status;
                fprintf(stderr, "Couldn't scan directory %s\n", fullpath.c_str());
                continue;
            }
        }

        if (f != NULL) {
            status = f(arg, fullpath);
            if (status != 0) {
                closedir(dir);
                return status;
            }
        }
    }

    closedir(dir);
    return 0;
}


#endif

