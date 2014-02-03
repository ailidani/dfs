

#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <string>

#include <libdfsutil/dfsutil.h>
#include <libdfsutil/scan.h>
#include <libdfs/tempdir.h>
#include <libdfs/localobject.h>

//#ifndef DEBUG
#define REMOVE_TEMPDIRS
//#endif

TempDir::TempDir(const std::string &dirpath)
    : dirpath(dirpath), objects_fd(-1)
{
    index.open(pathTo("index"));

    objects_fd = open(pathTo("objects").c_str(),
            O_RDWR | O_CREAT, // | O_APPEND
            0644);
    if (objects_fd < 0) {
        perror("TempDir open");
    }
}

static int _rmtreeCb(int unused, const std::string &path)
{
    struct stat sb;
    if (lstat(path.c_str(), &sb) < 0) {
        perror("~TempDir lstat");
        return 0;
    }

    if (S_ISDIR(sb.st_mode)) {
        DirIterate(path, /* unused */0, _rmtreeCb);
        if (rmdir(path.c_str()) < 0) {
            perror("~TempDir rmdir");
            return 0;
        }
    }
    else {
        if (unlink(path.c_str()) < 0) {
            perror("~TempDir unlink");
            return 0;
        }
    }

    return 0;
}

TempDir::~TempDir()
{
    if (objects_fd > 0) {
	// XXX: printf("Closing temp dir\n");
        close(objects_fd);
        objects_fd = -1;
    }

    // Remove the temp dir
#ifdef REMOVE_TEMPDIRS
    _rmtreeCb(/* unused */0, dirpath.c_str());
#endif
}

std::string
TempDir::pathTo(const std::string &file)
{
    return dirpath + "/" + file;
}

std::string
TempDir::newTempFile()
{
    std::string templStr = pathTo("tmpfile.XXXXXX");
    char templ[PATH_MAX];
    strcpy(templ, templStr.c_str());
    int fd = mkstemp(templ);
    if (fd < 0) {
        perror("TempDir::newTempFile mkstemp");
        return "";
    }
    close(fd);
    return templ;
}

/*
 * Repo implementation
 */
Object::sp
TempDir::getObject(const ObjectHash &objId)
{
    assert(!objId.isEmpty());
    assert(objects_fd > 0);
    /*std::string path = pathTo(objId);
    if (Util_FileExists(path)) {
        LocalObject::sp o(new LocalObject());
        o->open(path, objId);
        return Object::sp(o);
    }
    return Object::sp();*/
    if (!hasObject(objId)) {
        printf("Temp objects do not contain %s\n", objId.hex().c_str());
        index.dump();
        return Object::sp();
    }

    off_t off = offsets[objId];
    int status = lseek(objects_fd, off, SEEK_SET);
    if (status < 0) {
        perror("TempDir::getObject lseek");
        return Object::sp();
    }

    std::string info_ser;
    info_ser.resize(ObjectInfo::SIZE);
    status = read(objects_fd, &info_ser[0], ObjectInfo::SIZE);
    if (status < 0) {
        perror("TempDir::getObject read");
        return Object::sp();
    }

    ObjectInfo info;
    info.fromString(info_ser);

    return Object::sp(new TempObject(objects_fd, off+ObjectInfo::SIZE,
                info.payload_size, info));
}

bytestream *
TempDir::getObjects(const ObjectHashVec &vec) {
    NOT_IMPLEMENTED(false);
    return NULL;
}

ObjectInfo
TempDir::getObjectInfo(const ObjectHash &objId)
{
    assert(!objId.isEmpty());
    return index.getInfo(objId);
}

bool
TempDir::hasObject(const ObjectHash &objId)
{
    assert(!objId.isEmpty());
    return index.hasObject(objId);
}

std::set<ObjectInfo>
TempDir::listObjects()
{
    return index.getList();
}

int
TempDir::addObject(ObjectType type, const ObjectHash &hash,
        const std::string &payload)
{
    assert(!hash.isEmpty());

    if (!index.hasObject(hash)) {
        ObjectInfo info(hash);
        info.type = type;
        info.payload_size = payload.size();

        assert(info.hasAllFields());

        off_t off = lseek(objects_fd, 0, SEEK_END);
        std::string info_ser = info.toString();

        write(objects_fd, info_ser.data(), info_ser.size());

        int written = write(objects_fd, payload.data(), payload.size());
        if (written < 0) return written;

        IndexEntry ie = {info, 0, 0, 0};
        index.updateEntry(info.hash, ie);
        offsets[info.hash] = off;
    }

    return 0;
}


/*
 * TempObject
 */
TempObject::TempObject(int fd, off_t off, size_t len, const ObjectInfo &info)
    : Object(info), fd(fd), off(off), len(len)
{
}

bytestream *
TempObject::getPayloadStream()
{
    return new fdstream(fd, off, len);
}
