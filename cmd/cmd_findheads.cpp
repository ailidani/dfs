
#include <stdint.h>

#include <string>
#include <iostream>

#include <libdfs/localrepo.h>

using namespace std;

extern LocalRepo repository;

/*
 * Find lost Heads
 */
int
cmd_findheads(int argc, char * const argv[])
{
    RefcountMap refs = repository.recomputeRefCounts();

    for (RefcountMap::iterator it = refs.begin();
            it != refs.end();
            it++) {
        if ((*it).first == EMPTY_COMMIT)
            continue;

        if ((*it).second == 0
            && repository.getObjectType((*it).first)) {
            Commit c = repository.getCommit((*it).first);

            // XXX: Check for existing branch names

            cout << "commit:  " << (*it).first.hex() << endl;
            cout << "parents: " << c.getParents().first.hex() << endl;
            cout << c.getMessage() << endl;
        }
    }

    return 0;
}

