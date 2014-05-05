
#include "mds.h"

MDS* MDS::mds = 0;

MDS::MDS() : Thread()
{
    rc = MDSConf();
    announcer = new Announcer();
    listener = new Listener();
    repoMonitor = new RepoMonitor();
    syncer = new Syncer();

    myInfo = HostInfo(rc.getUUID(), rc.getCluster());
    // XXX: Update addresses periodically
    myInfo.setHost(OriStr_Join(OriNet_GetAddrs(), ','));

    OriPriv * priv = GetOriPriv();

}

int MDS::start_server()
{
	MSG("Starting MDS");

    announcer->start();
    listener->start();
    repoMonitor->start();
    syncer->start();

    struct event_base *base = event_base_new();
    struct evhttp *httpd = evhttp_new(base);
    evhttp_bind_socket(httpd, "0.0.0.0", 8051);

    evhttp_set_cb(httpd, "/", Httpd_getRoot, NULL);

    // Event loop
    event_base_dispatch(base);

    evhttp_free(httpd);
    event_base_free(base);

    // XXX: Wait for worker threads

    return 0;
}

int MDS::unlink(const std::string &path)
{
	OriPriv *priv = GetOriPriv();

#ifdef FSCK_A_LOT
	priv->fsck();
#endif /* FSCK_A_LOT */

	RWKey::sp lock = priv->nsLock.writeLock();
	try {
		OriFileInfo *info = priv->getFileInfo(path);

		if (info->isDir())
			return -EPERM;

		// Remove temporary file
		if (info->path != "")
			unlink(info->path.c_str());

		if (info->isReg() || info->isSymlink()) {
			priv->unlink(path);
		} else {
			// XXX: Support files
			ASSERT(false);
		}
	} catch (SystemException e) {
		return -e.getErrno();
	}

	priv->journal("unlink", path);

	return 0;
}

int MDS::symlink(const std::string &target_path, const std::string &link_path)
{
	OriPriv *priv = GetOriPriv();
	OriDir *parentDir;
	std::string parentPath;

#ifdef FSCK_A_LOT
	priv->fsck();
#endif /* FSCK_A_LOT */

	parentPath = OriFile_Dirname(link_path);
	if (parentPath == "")
		parentPath = "/";

	RWKey::sp lock = priv->nsLock.writeLock();
	try {
		parentDir = priv->getDir(parentPath);
	} catch (SystemException e) {
		return -e.getErrno();
	}

	OriFileInfo *info = priv->addSymlink(link_path);
	info->statInfo.st_mode |= 0755;
	info->link = target_path;
	info->statInfo.st_size = info->path.length();
	info->type = FILETYPE_DIRTY;

	parentDir->add(OriFile_Basename(link_path), info->id);

	return 0;
}

int MDS::readlink(const std::string &path, char* buf, size_t size)
{
	OriPriv *priv = GetOriPriv();
	OriFileInfo *info;

#ifdef FSCK_A_LOT
	priv->fsck();
#endif /* FSCK_A_LOT */

	RWKey::sp lock = priv->nsLock.readLock();
	try {
		info = priv->getFileInfo(path);
	} catch (SystemException e) {
		return -e.getErrno();
	}

	std::memcpy(buf, info->link.c_str(), MIN(info->link.length() + 1, size));

	return 0;
}

int MDS::rename(const std::string &from_path, const std::string &to_path)
{
    OriPriv *priv = GetOriPriv();

#ifdef FSCK_A_LOT
    priv->fsck();
#endif /* FSCK_A_LOT */

    RWKey::sp lock = priv->nsLock.writeLock();
    try {
        OriFileInfo *info = priv->getFileInfo(from_path);
        OriFileInfo *toFile = NULL;
        OriDir *toFileDir = NULL;

        try {
            toFile = priv->getFileInfo(to_path);
        } catch (SystemException e) {
            // Fall through
        }

        // Not sure if FUSE checks for these two error cases
        if (toFile != NULL && toFile->isDir()) {
            toFileDir = priv->getDir(to_path);

            if (!toFileDir->isEmpty())
                return -ENOTEMPTY;
        }
        if (toFile != NULL && info->isDir() && !toFile->isDir()) {
            return -EISDIR;
        }

        // XXX: Need to support renaming directories (nlink, OriPriv::Rename)
        if (info->isDir()) {
            FUSE_LOG("ori_rename: Directory rename attempted %s to %s",
                     from_path, to_path);
            return -EINVAL;
        }

        priv->rename(from_path, to_path);
    } catch (SystemException &e) {
        return -e.getErrno();
    }

    std::string journalArg = from_path;
    journalArg += ":";
    journalArg += to_path;
    priv->journal("rename", journalArg);

    return 0;
}

int MDS::mkdir(const std::string &path, mode_t mode)
{
    OriPriv *priv = GetOriPriv();

#ifdef FSCK_A_LOT
    priv->fsck();
#endif /* FSCK_A_LOT */

    RWKey::sp lock = priv->nsLock.writeLock();
    try {
        OriFileInfo *info = priv->addDir(path);
        info->statInfo.st_mode |= mode;
    } catch (SystemException e) {
        return -e.getErrno();
    }

    priv->journal("mkdir", path);

    return 0;
}

int MDS::rmdir(const std::string &path)
{
    OriPriv *priv = GetOriPriv();

#ifdef FSCK_A_LOT
    priv->fsck();
#endif /* FSCK_A_LOT */

    RWKey::sp lock = priv->nsLock.writeLock();
    try {
        OriDir *dir = priv->getDir(path);

        if (!dir->isEmpty()) {
            OriDir::iterator it;

            FUSE_LOG("Directory not empty!");
            for (it = dir->begin(); it != dir->end(); it++) {
                FUSE_LOG("DIR: %s\n", it->first.c_str());
            }

            return -ENOTEMPTY;
        }

        priv->rmDir(path);

    } catch (SystemException &e) {
        FUSE_LOG("ori_rmdir: Caught exception %s", e.what());
        return -e.getErrno();
    }

    priv->journal("rmdir", path);

    return 0;
}

/*
 * Main control entry point that dispatches to the various MDS request.
 */
std::string MDS::process(const std::string &data)
{
    std::string cmd;
    strstream str = strstream(data);

    str.readPStr(cmd);

    if (cmd == "is_master")
        return req_is_master(str);


    // Makes debugging easier when a bad request comes in
    return "UNSUPPORTED REQUEST";
}
