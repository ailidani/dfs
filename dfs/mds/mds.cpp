
#include "mds.h"
#include "message.hpp"
#include "server.h"


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

    // XXX get mypaths from file
    std::string home = Util_GetHome();
	ASSERT(home != "");
	if (!OriFile_Exists(home + "/.ori"))
		OriFile_MkDir(home + "/.ori");
	metafile = home + MDS_METAFILE;
	if (OriFile_Exists(metafile))
	{
		load();
	}

}

MDS::~MDS()
{
	save();
    delete server::instance();
}

void Httpd_getRoot(struct evhttp_request *req, void *arg)
{
    struct evbuffer *buf;

    buf = evbuffer_new();
    if (buf == NULL) {
        evhttp_send_error(req, HTTP_INTERNAL, "Internal Error");
        return;
    }

    evbuffer_add_printf(buf, "Temporary String");
    evhttp_add_header(req->output_headers, "Content-Type", "text/html");
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
}

int MDS::start_server()
{
	MSG("Starting MDS");

    announcer->start();
    listener->start();
    //repoMonitor->start();
    //syncer->start();

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

int MDS::mds_unlink(const std::string &path, bool fromFUSE)
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

	if(!fromFUSE) return 0;

	MessagePtr msg = boost::shared_ptr<CMessage>(new CMessage());
	msg->cmd_ = "unlink";
	msg->path_from_ = path;
	if(is_master(path))
	{
		mypaths.erase(path);
		// XXX send update
		msg->m_type = Update;
	}
	else
	{
		// XXX send request
		msg->m_type = Request;
	}
	server::instance()->send(msg);

	return 0;
}

int MDS::mds_symlink(const std::string &target_path, const std::string &link_path, bool fromFUSE)
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

	if(!fromFUSE) return 0;

	MessagePtr msg = boost::shared_ptr<CMessage>(new CMessage());
	msg->cmd_ = "symlink";
	msg->path_from_ = target_path;
	msg->path_to_ = link_path;
	if(is_master(link_path))
	{
		mypaths.insert(target_path);
		// XXX send update
		msg->m_type = Update;
	}
	else
	{
		// XXX send request
		msg->m_type = Request;
	}
	server::instance()->send(msg);

	return 0;
}

int MDS::mds_readlink(const std::string &path, char* buf, size_t size)
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

	memcpy(buf, info->link.c_str(), MIN(info->link.length() + 1, size));

	return 0;
}

int MDS::mds_rename(const std::string &from_path, const std::string &to_path, bool fromFUSE)
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
                     from_path.c_str(), to_path.c_str());
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

    if(!fromFUSE) return 0;

	MessagePtr msg = boost::shared_ptr<CMessage>(new CMessage());
	msg->cmd_ = "rename";
	msg->path_from_ = from_path;
	msg->path_to_ = to_path;
    if(is_master(from_path))
    {
    	mypaths.insert(to_path);
    	// XXX send update
    	msg->m_type = Update;
    }
    else
    {
    	// XXX send request
    	msg->m_type = Request;
    }
    server::instance()->send(msg);

    return 0;
}

int MDS::mds_mkdir(const std::string &path, mode_t mode, bool fromFUSE)
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

    if(!fromFUSE) return 0;

    mypaths.insert(path);

	MessagePtr msg = boost::shared_ptr<CMessage>(new CMessage());
	msg->cmd_ = "mkdir";
	msg->path_from_ = path;
	msg->mode_ = mode;
	msg->m_type = Update;
	server::instance()->send(msg);

    return 0;
}

int MDS::mds_rmdir(const std::string &path, bool fromFUSE)
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

    if(!fromFUSE) return 0;

	MessagePtr msg = boost::shared_ptr<CMessage>(new CMessage());
	msg->cmd_ = "rmdir";
	msg->path_from_ = path;
    if(is_master(path))
    {
    	mypaths.erase(path);
    	// XXX send change
    	msg->m_type = Update;
    }
    else
    {
    	// XXX send request
    	msg->m_type = Request;
    }
    server::instance()->send(msg);

    return 0;
}



void MDS::save() const
{
	if(metafile != "")
	{
		std::ofstream ofs(metafile);
		assert(ofs.good());
		boost::archive::xml_oarchive oa(ofs);
		oa << BOOST_SERIALIZATION_NVP(mypaths);
		oa << BOOST_SERIALIZATION_NVP(masters);
	}
}

void MDS::load()
{
	std::ifstream ifs(metafile);
	assert(ifs.good());
	boost::archive::xml_iarchive ia(ifs);
	ia >> BOOST_SERIALIZATION_NVP(mypaths);
	ia >> BOOST_SERIALIZATION_NVP(masters);
}

