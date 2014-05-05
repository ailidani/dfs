
#include "mds.h"

MDS* MDS::mds = 0;

MDS::MDS()
{
    rc = MDSConf();
    announcer = new Announcer();
    listener = new Listener();
    repoMonitor = new RepoMonitor();
    syncer = new Syncer();

    myInfo = HostInfo(rc.getUUID(), rc.getCluster());
    // XXX: Update addresses periodically
    myInfo.setHost(OriStr_Join(OriNet_GetAddrs(), ','));
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
