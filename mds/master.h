
#ifndef MDS_MASTER_H
#define MDS_MASTER_H

#include <libdfsutil/ObjectHash.h>
#include <libdfsutil/mutex.h>
#include <libdfs/metadatalog.h>

#include "repocontrol.h"
#include <unordered_set>


class Master
{
public:
	enum
	{
		HANDLE_NOT_DEL = 1,
    	HANDLE_MARKED_FOR_DEL = 2
    };

    Master(RepoControl rc);
    ~Master();
    bool is_master(ObjectHash objhash)
    {
    	std::unordered_set<ObjectHash>::iterator it;
    	it = myobjs.find(objhash);
    	return it != myobjs.end();
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
	typedef std::unordered_map<ObjectHash, HostInfo *> MasterMap;
	MetadataLog metadata_log;

	std::unordered_set<ObjectHash> myobjs;
	MasterMap masters;

};



#endif // MDS_MASTER_H
