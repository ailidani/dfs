
#include <stdint.h>

#include <set>
#include <string>
#include <iostream>

#include <libdfs/udsrepo.h>

using namespace std;

extern UDSRepo repository;

int
cmd_branches(int argc, char * const argv[])
{
    uint32_t len;
    strwstream req;
    string currentBranch;

    req.writePStr("branch");
    req.writePStr("get");
    strstream resp = repository.callExt("FUSE", req.str());
    if (resp.ended()) {
        cout << "branches failed with an unknown error!" << endl;
        return 1;
    }
    resp.readLPStr(currentBranch);

    req = strwstream();
    req.writePStr("branch");
    req.writePStr("list");
    resp = repository.callExt("FUSE", req.str());
    if (resp.ended()) {
        cout << "branches failed with an unknown error!" << endl;
        return 1;
    }

    len = resp.readUInt32();
    for (uint32_t i = 0; i < len; i++)
    {
        string branch;

        resp.readLPStr(branch);
	if (currentBranch == branch)
	    cout << branch << "*" << endl;
	else 
	    cout << branch << endl;
    }

    return 0;
}

