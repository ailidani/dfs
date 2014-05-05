
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
#define FSCK_A_LOT
#endif

#include "mdsconf.h"
#include "repoinfo.h"
#include "hostinfo.h"
#include "repocontrol.h"

// Announcement UDP port
#define MDS_UDPPORT         5001
// Advertisement interval
#define MDS_ADVINTERVAL     5
// Reject advertisements with large time skew
#define MDS_ADVSKEW         5
// Repository check interval
#define MDS_MONINTERVAL     10
// Sync interval
#define MDS_SYNCINTERVAL    5

#define MDS_LOGFILE         "/.ori/mds.log"

class MDS : public Thread
{
public:
	static MDS* get()
	{
		if(mds == 0)
			mds = new MDS;
		return mds;
	}
	void run()
	{
		std::string oriHome = Util_GetHome() + "/.ori";
		if (!OriFile_Exists(oriHome))
			OriFile_MkDir(oriHome);
		// Chdir so that coredumps are placed in ~/.ori
		chdir(oriHome.c_str());
		dfs_open_log(Util_GetHome() + MDS_LOGFILE);

		start_server();
	}
	int start_server();

	int unlink(const std::string &path);
	int symlink(const std::string &target_path, const std::string &link_path);
	int readlink(const std::string &path, char* buf, size_t size);
	int rename(const std::string &from_path, const std::string &to_path);
	int mkdir(const std::string &path, mode_t mode);
	int rmdir(const std::string &path);


	//std::string process(const std::string &data);

	MDSConf rc;
	HostInfo myInfo;
	RWLock infoLock;
	std::map<std::string, HostInfo *> hosts;
	RWLock hostsLock;

protected:
	MDS();

	std::string req_is_master(strstream &str)
	{
		ObjectHash objhash;
		str.readHash(objhash);

		strwstream resp;

		std::unordered_set<ObjectHash>::iterator it;
		it = myobjs.find(objhash);
		if(it != myobjs.end())
			resp.writeUInt8(1);
		else resp.writeUInt8(0);

		return resp.str();
	}

	bool is_master(std::string path)
	{
		std::unordered_set<std::string>::iterator it;
		it = mypaths.find(path);
		return it != mypaths.end();
	}
	HostInfo * get_current_master(std::string path)
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
	static MDS* mds;

	Announcer *announcer;
	Listener *listener;
	RepoMonitor *repoMonitor;
	Syncer *syncer;

	std::unordered_set<std::string> mypaths;
	typedef std::unordered_map<std::string, HostInfo *> MasterMap;
	MasterMap masters;

	//typedef std::unordered_map<ObjectHash, HostInfo *> MasterMap;
	//MetadataLog metadata_log;

	std::unordered_set<ObjectHash> myobjs;


};

#endif
