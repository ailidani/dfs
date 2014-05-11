
#ifndef mds_h
#define mds_h

#include <stdint.h>
#include <stdio.h>
#include <cstdlib>

#include <unistd.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>
#include <event2/util.h>

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsstr.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfscrypt.h>
#include <libdfsutil/dfsnet.h>
#include <libdfsutil/systemexception.h>
#include <libdfsutil/thread.h>
#include <libdfsutil/kvserializer.h>
#include <libdfs/localrepo.h>

#include <libdfsutil/dfsfile.h>
#include <libdfsutil/systemexception.h>
#include <libdfsutil/rwlock.h>
#include <libdfs/repostore.h>
#include <libdfs/remoterepo.h>

#include <dfs/logging.h>
#include <dfs/oripriv.h>

#ifdef DEBUG
//#define FSCK_A_LOT
#endif

#include "mdsconf.h"
#include "repoinfo.h"
#include "hostinfo.h"
#include "repocontrol.h"
#include "announcer.h"
#include "listener.h"
#include "repomonitor.h"
#include "syncer.h"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>

// Announcement UDP port
#define MDS_UDPPORT         5001
// Messaging UDP port
#define MDS_SERVER_PORT		5002
// Advertisement interval
#define MDS_ADVINTERVAL     10
// Reject advertisements with large time skew
#define MDS_ADVSKEW         50
// Repository check interval
#define MDS_MONINTERVAL     20
// Sync interval
#define MDS_SYNCINTERVAL    10

#define MDS_LOGFILE         "/.ori/mds.log"
#define MDS_CONFFILE		"/.ori/mds.conf"
#define MDS_METAFILE		"/.ori/mds.meta.xml"

class MDS
{
public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & mypaths;
		ar & masters;
	}
	/*
	static MDS* instance()
	{
		if(mds == 0)
			mds = new MDS;
		return mds;
	}
	*/
	MDS();
	~MDS();
	void init();
	int start_server();

	int mds_unlink(const std::string &path, bool fromFUSE);
	int mds_symlink(const std::string &target_path, const std::string &link_path, bool fromFUSE);
	int mds_readlink(const std::string &path, char* buf, size_t size);
	int mds_rename(const std::string &from_path, const std::string &to_path, bool fromFUSE);
	int mds_mkdir(const std::string &path, mode_t mode, bool fromFUSE);
	int mds_rmdir(const std::string &path, bool fromFUSE);

	MDSConf rc;
	HostInfo myInfo;
	RWLock infoLock;
	std::map<std::string, HostInfo *> hosts;
	RWLock hostsLock;

protected:
	inline bool is_master(std::string path)
	{
		std::set<std::string>::iterator it;
		it = mypaths.find(path);
		return it != mypaths.end();
	}
	inline HostInfo * get_current_master(std::string path)
	{
		MasterMap::iterator it;
		it = masters.find(path);
		if(it != masters.end())
		{
			return it->second;
		}
		else return NULL;
	}

private:
	//static MDS* mds;

	Announcer *announcer;
	Listener *listener;
	//RepoMonitor *repoMonitor;
	//Syncer *syncer;

	std::set<std::string> mypaths;
	typedef std::map<std::string, HostInfo *> MasterMap;
	MasterMap masters;

	void load();
	void save() const;

	std::string metafile;

};

#endif
