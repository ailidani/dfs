
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
	MDS() {}
	~MDS() {}
	int start_server();

private:
	MDSConf rc;

	HostInfo myInfo;
	RWLock infoLock;
	map<string, HostInfo *> hosts;
	RWLock hostsLock;

	Announcer *announcer;
	Listener *listener;
	RepoMonitor *repoMonitor;
	Syncer *syncer;

};

#endif