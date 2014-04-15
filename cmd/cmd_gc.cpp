
#include <stdint.h>

#include <string>
#include <iostream>

#include <libdfs/localrepo.h>

using namespace std;

extern LocalRepo repository;

/*
 * Reclaim unused space.
 */
int
cmd_gc(int argc, char * const argv[])
{
    repository.gc();

    return 0;
}

