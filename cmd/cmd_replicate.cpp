/*
 * Copyright (c) 2012-2013 Stanford University
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
#include <stdlib.h>
#include <string.h>

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
#include <libdfs/repostore.h>
#include <libdfs/localrepo.h>
#include <libdfs/remoterepo.h>
#include <libdfs/treediff.h>

using namespace std;

extern LocalRepo repository;

void
usage_replicate()
{
    cout << "ori replicate [OPTIONS] SOURCE [DESTINATION]" << endl;
    cout << endl;
    cout << "Create a local clone of a repository." << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "    --full         Full clone (default)" << endl;
    cout << "    --non-bare     Non-bare repository" << endl;
    cout << "    --shallow      Shallow clone" << endl;
}

int
cmd_replicate(int argc, char * const argv[])
{
    int ch;
    int clone_mode = 0;
    int status;
    string srcRoot;
    string newRoot;
    bool bareRepo = true;

    struct option longopts[] = {
        { "full",       no_argument,    NULL,   'f' },
        { "shallow",    no_argument,    NULL,   's' },
        { "non-bare",   no_argument,    NULL,   'n' },
        { NULL,         0,              NULL,   0   }
    };

    while ((ch = getopt_long(argc, argv, "fs", longopts, NULL)) != -1) {
        switch (ch) {
            case 'f':
                if (clone_mode != 0) {
                    printf("Cannot set multiple clone modes!\n");
                    return 1;
                }
                clone_mode = 1;
                break;
            case 'n':
                bareRepo = false;
                break;
            case 's':
                if (clone_mode != 0) {
                    printf("Cannot set multiple clone modes!\n");
                    return 1;
                }
                clone_mode = 2;
                break;
            default:
                printf("Usage: ori clone [OPTIONS] <REPO SOURCE> [<DIR>]\n");
                return 1;
        }
    }
    argc -= optind;
    argv += optind;

    if (argc != 1 && argc != 2) {
        printf("Specify a repository to clone.\n");
        printf("usage: ori clone [options] <repo> [<dir>]\n");
        return 1;
    }

    srcRoot = argv[0];

    // If repository is local find the full path
    if (Util_IsValidName(srcRoot))
        srcRoot = RepoStore_FindRepo(srcRoot);

    if (argc == 2) {
        newRoot = argv[1];
    } else {
        newRoot = srcRoot.substr(srcRoot.rfind("/")+1);
        // If there are no slashes in the path then look for a colon
        if (newRoot == srcRoot)
            newRoot = srcRoot.substr(srcRoot.rfind(":")+1);
    }

    if (Util_IsValidName(newRoot) && bareRepo) {
        newRoot = RepoStore_GetRepoPath(newRoot);
    }

    printf("Cloning from %s to %s\n", srcRoot.c_str(), newRoot.c_str());

    RemoteRepo srcRepo;
    srcRepo.connect(srcRoot);

    if (!OriFile_Exists(newRoot)) {
        mkdir(newRoot.c_str(), 0755);
    }
    status = LocalRepo_Init(newRoot, bareRepo, srcRepo->getUUID());
    if (status != 0) {
        printf("Failed to construct an empty repository!\n");
        return 1;
    }

    LocalRepo dstRepo;
    dstRepo.open(newRoot);

    // Setup remote pointer
    string originPath = srcRoot;
    if (!Util_IsPathRemote(srcRoot)) {
        originPath = OriFile_RealPath(srcRoot);
    }
    dstRepo.addPeer("origin", originPath);
    if (clone_mode == 2) {
        dstRepo.setInstaClone("origin", true);
    }

    ObjectHash head = srcRepo->getHead();

    if (clone_mode != 2) {
        dstRepo.pull(srcRepo.get());
    }

    if (!head.isEmpty())
        dstRepo.updateHead(head);

    RefcountMap refs = dstRepo.recomputeRefCounts();
    if (!dstRepo.rewriteRefCounts(refs))
        return 1;

    return 0;
}

