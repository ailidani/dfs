
#include <stdint.h>

#include <string>
#include <iostream>
#include <iomanip>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfs/localrepo.h>

#include "fuse_cmd.h"

using namespace std;

extern LocalRepo repository;

extern "C" {
#include <libdiffmerge/blob.h>
int *text_diff(Blob *pA_Blob, Blob *pB_Blob, Blob *pOut, uint64_t diffFlags);
};

int
cmd_diff(int argc, char * const argv[])
{
    return 0;
    //if (OF_RunCommand("diff"))
    //    return 0;

    Commit c;
    ObjectHash tip = repository.getHead();
    if (tip != EMPTY_COMMIT) {
        c = repository.getCommit(tip);
    }

    TreeDiff td;
    td.diffToDir(c, repository.getRootPath(), &repository);

    Blob a, b, out;

    for (size_t i = 0; i < td.entries.size(); i++) {
        if (td.entries[i].type == TreeDiffEntry::Modified) {
            printf("Index: %s\n", td.entries[i].filepath.c_str());
            printf("==================================================================\n");
            // XXX: Handle file move
            printf("--- %s\n+++ %s\n",
                   td.entries[i].filepath.c_str(),
                   td.entries[i].filepath.c_str());

            // print diff
            string path = repository.getRootPath() + td.entries[i].filepath;
            string buf = repository.getPayload(td.entries[i].hashes.first);

            blob_init(&a, buf.data(), buf.size());
            blob_read_from_file(&b, path.c_str());
            blob_zero(&out);
            text_diff(&a, &b, &out, 0);
            blob_write_to_file(&out, "-");
        }
    }

    return 0;
}

