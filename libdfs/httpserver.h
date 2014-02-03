

#ifndef libdfs_httpserver_h
#define libdfs_httpserver_h

#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

class HTTPServer
{
public:
    HTTPServer(LocalRepo &repository, uint16_t port);
    ~HTTPServer();
    void start(bool mDNSEnable);
protected:
    void entry(struct evhttp_request *req);
private:
    int authenticate(struct evhttp_request *req, struct evbuffer *buf);
    // Handlers
    void stop(struct evhttp_request *req);
    void getId(struct evhttp_request *req);
    void getVersion(struct evhttp_request *req);
    void head(struct evhttp_request *req);
    void getIndex(struct evhttp_request *req);
    void getCommits(struct evhttp_request *req);
    void contains(struct evhttp_request *req);
    void getObjs(struct evhttp_request *req);
    void getObjInfo(struct evhttp_request *req);
    LocalRepo &repo;
    uint16_t port;
    struct evhttp *httpd;
    /* set if a test needs to call loopexit on a base */
    struct event_base *base;
    friend void HTTPServerReqHandlerCB(struct evhttp_request *req, void *arg);
};

#endif

