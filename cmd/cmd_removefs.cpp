/*
 * Copyright (c) 2013 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

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

using namespace std;

void
usage_removefs()
{
    cout << "ori removefs FSNAME" << endl;
    cout << endl;
    cout << "Remove a local replica. This is a permanant operation," << endl;
    cout << "is local to this machine." << endl;
}

static int
removefsHelper(int unused, const string &path)
{
    if (OriFile_IsDirectory(path))
        OriFile_RmDir(path);
    else
        OriFile_Delete(path);

    return 0;
}

/*
 * Remove a local replica.
 */
int
cmd_removefs(int argc, char * const argv[])
{
    string fsName;
    string rootPath;

    if (argc != 2) {
        if (argc == 1)
            printf("Argument required!\n");
        if (argc > 2)
            printf("Too many arguments!\n"); 
        printf("Usage: ori removefs FSNAME\n");
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

    // XXX: unregister with autosync

    DirRTraverse(rootPath, 0, removefsHelper);
    OriFile_RmDir(rootPath);

    return 0;
}

