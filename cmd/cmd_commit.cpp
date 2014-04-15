
#include <stdint.h>

#include <string>
#include <iostream>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfs/localrepo.h>

#include "fuse_cmd.h"

using namespace std;

extern LocalRepo repository;

void
usage_commit(void)
{
    cout << "ori commit [MESSAGE]" << endl;
    cout << endl;
    cout << "Commit any outstanding changes into the repository." << endl;
    cout << endl;
    cout << "An optional message can be added to the commit." << endl;
}

int
cmd_commit(int argc, char * const argv[])
{
    //if (OF_RunCommand("snapshot"))
    //    return 0;

    Commit c;
    Tree tip_tree;
    ObjectHash tip = repository.getHead();
    if (tip != EMPTY_COMMIT) {
        c = repository.getCommit(tip);
        tip_tree = repository.getTree(c.getTree());
    }

    TreeDiff diff;
    diff.diffToDir(c, repository.getRootPath(), &repository);
    if (diff.entries.size() == 0) {
        cout << "Nothing to commit!" << endl;
        return 0;
    }

    Tree new_tree = diff.applyTo(tip_tree.flattened(&repository),
            &repository);

    Commit newCommit;
    if (argc == 2) {
        newCommit.setMessage(argv[1]);
    }
    repository.commitFromTree(new_tree.hash(), newCommit);

    return 0;
}

