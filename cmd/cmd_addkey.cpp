

#include <stdint.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include <libdfsutil/dfsfile.h>
#include <libdfsutil/key.h>
#include <libdfs/localrepo.h>

using namespace std;

extern LocalRepo repository;

int
cmd_addkey(int argc, char * const argv[])
{
    int status;
    string rootPath = LocalRepo::findRootPath();

    if (rootPath == "") {
        cout << "No repository found!" << endl;
        return 1;
    }

    if (argc != 2)
    {
	cout << "Sepcify someone's public key to add." << endl;
	cout << "usage: ori addkey <public_key>" << endl;
    }

    switch (Key_GetType(argv[1]))
    {
	case KeyType::Invalid:
	    cout << "File not found or invalid key." << endl;
	    return 1;
	case KeyType::Private:
	    cout << "This appears to be a private key please specify a private key."
		 << endl;
	    return 1;
	case KeyType::Public:
	default:
	    break;
    }

    PublicKey pub = PublicKey();
    try {
        pub.open(argv[1]);
    } catch (exception e) {
	cout << "It appears that the key is invalid." << endl;
	return 1;
    }

    cout << "E-mail:      " << pub.getEmail() << endl;
    cout << "Fingerprint: " << pub.computeDigest() << endl;

    status = OriFile_Copy(argv[1],
                          rootPath + ORI_PATH_TRUSTED + pub.computeDigest());
    if (status < 0)
    {
        cout << "Failed to copy the public key into the repository." << endl;
    }

    return 0;
}

