

#include <stdint.h>

#include <set>
#include <string>
#include <iostream>

#include <libdfs/udsrepo.h>

using namespace std;

extern UDSRepo repository;

int
cmd_branch(int argc, char * const argv[])
{
    strwstream req;
    string status;

    if (argc == 1) {
	string branch;

        req.writePStr("branch");
        req.writePStr("get");
        strstream resp = repository.callExt("FUSE", req.str());
        if (resp.ended()) {
            cout << "branch failed with an unknown error!" << endl;
            return 1;
        }

        resp.readLPStr(branch);

	cout << branch << endl;

	return 0;
    }

    if (argc == 2) {
        req.writePStr("branch");
        req.writePStr("set");
        req.writeLPStr(argv[1]);
        strstream resp = repository.callExt("FUSE", req.str());
        if (resp.ended()) {
            cout << "branch failed with an unknown error!" << endl;
            return 1;
        }

        resp.readLPStr(status);
        if (status != "") {
            cout << status << endl;
            return 1;
        }

        return 0;
    }

    cout << "Wrong number of arguments!" << endl;

    return 1;
}

