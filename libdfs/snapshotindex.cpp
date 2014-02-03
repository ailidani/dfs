

#include <stdint.h>

#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <set>

#include <libdfsutil/debug.h>
#include <libdfsutil/objecthash.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfsfile.h>
#include <libdfsutil/systemexception.h>
#include <libdfs/snapshotindex.h>

using namespace std;

SnapshotIndex::SnapshotIndex()
{
    fd = -1;
}

SnapshotIndex::~SnapshotIndex()
{
    close();
}

void SnapshotIndex::open(const string &indexFile)
{
    struct stat sb;

    fileName = indexFile;

    // Read index
    fd = ::open(indexFile.c_str(), O_RDWR | O_CREAT,
              S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        WARNING("Could not open the snapshot index file!");
        throw SystemException();
    };


    if (::fstat(fd, &sb) < 0) {
        WARNING("Could not fstat the snapshot index file!");
        throw SystemException();
    }

    int len = sb.st_size;
    string blob(len, '\0');
    int status UNUSED = read(fd, &blob[0], len);
    ASSERT(status == len);

    string line;
    stringstream ss(blob);

    while (getline(ss, line, '\n')) {
	ObjectHash hash;
        string name;

        hash = ObjectHash::fromHex(line.substr(0, 64));
        name = line.substr(65);

        snapshots[name] = hash;
    }
    ::close(fd);

    // Reopen append only
    fd = ::open(indexFile.c_str(), O_WRONLY | O_APPEND);
    ASSERT(fd >= 0); // Assume that the repository lock protects the index

    // Delete temporary index if present
    if (OriFile_Exists(indexFile + ".tmp")) {
        OriFile_Delete(indexFile + ".tmp");
    }
}

void SnapshotIndex::close()
{
    if (fd != -1) {
        ::fsync(fd);
        ::close(fd);
        fd = -1;
    }
}

void SnapshotIndex::rewrite()
{
    int fdNew, tmpFd;
    string newIndex = fileName + ".tmp";
    map<string, ObjectHash>::iterator it;

    fdNew = ::open(newIndex.c_str(), O_RDWR | O_CREAT,
                   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fdNew < 0) {
        perror("open");
        cout << "Could not open a temporary index file!" << endl;
        return;
    };

    // Write new index
    for (it = snapshots.begin(); it != snapshots.end(); it++)
    {
        int status;
        string indexLine;

        indexLine = (*it).second.hex() + " " + (*it).first + "\n";

        status = write(fdNew, indexLine.data(), indexLine.size());
        ASSERT(status == (int)indexLine.size());
    }

    OriFile_Rename(newIndex, fileName);

    tmpFd = fd;
    fd = fdNew;
    ::close(tmpFd);
}

void SnapshotIndex::addSnapshot(const string &name, const ObjectHash &commitId)
{
    string indexLine;

    ASSERT(!commitId.isEmpty());

    indexLine = commitId.hex() + " " + name + "\n";

    write(fd, indexLine.data(), indexLine.size());

    snapshots[name] = commitId;
}

void SnapshotIndex::delSnapshot(const std::string &name)
{
    // delete in map

    rewrite();
}

const ObjectHash& SnapshotIndex::getSnapshot(const string &name) const
{
    map<string, ObjectHash>::const_iterator it = snapshots.find(name);
    ASSERT(it != snapshots.end());

    return (*it).second;
}

map<string, ObjectHash> SnapshotIndex::getList()
{
    return snapshots;
}

