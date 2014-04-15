
#include <stdint.h>

#include <string>
#include <iostream>
#include <iomanip>

#include <libdfs/udsrepo.h>

using namespace std;

extern UDSRepo repository;

int
cmd_filelog(int argc, char * const argv[])
{
    ObjectHash commit = repository.getHead();
    list<pair<Commit, ObjectHash> > revs;
    Commit lastCommit;
    ObjectHash lastCommitHash;
    ObjectHash lastHash;

    if (argc != 2) {
	cout << "Wrong number of arguments!" << endl;
	return 1;
    }

    while (commit != EMPTY_COMMIT) {
	Commit c = repository.getCommit(commit);
	ObjectHash objId;

	objId = repository.lookup(c, argv[1]);

	if (lastHash != objId && !lastHash.isEmpty()) {
	    revs.push_back(make_pair(lastCommit, lastCommitHash));
	}
	lastCommit = c;
	lastCommitHash = commit;
	lastHash = objId;

	commit = c.getParents().first;
	// XXX: Handle merge cases
    }

    if (!lastHash.isEmpty()) {
	revs.push_back(make_pair(lastCommit, lastCommitHash));
    }

    for (list<pair<Commit, ObjectHash> >::iterator it = revs.begin();
            it != revs.end();
            it++) {
	Commit c = (*it).first;
	time_t timeVal = c.getTime();
	char timeStr[26];

	ctime_r(&timeVal, timeStr);

	cout << "Commit:  " << (*it).second.hex() << endl;
	cout << "Parents: " << c.getParents().first.hex() << endl;
	cout << "Author:  " << c.getUser() << endl;
	// XXX: print file id?
	cout << "Date:    " << timeStr << endl;
	cout << c.getMessage() << endl << endl;
    }

    return 0;
}

