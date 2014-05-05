
#ifndef mds_h
#define mds_h

#include <stdint.h>
#include <stdio.h>
#include <cstdlib>

#include <unistd.h>
#include <errno.h>

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

class MDS
{
public:
	static MDS* get()
	{
		if(mds == 0)
			mds = new MDS;
		return mds;
	}
	int start_server();

	std::string process(const std::string &data);

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

	HostInfo * get_current_master(ObjectHash objhash)
	{
		MasterMap::iterator it;
		it = masters.find(objhash);
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

	typedef std::unordered_map<ObjectHash, HostInfo *> MasterMap;
	//MetadataLog metadata_log;

	std::unordered_set<ObjectHash> myobjs;
	MasterMap masters;

};

#endif
