
#ifndef libdfs_httpclient_h
#define libdfs_httpclient_h


#include <string>

void HttpClient_requestDoneCB(struct evhttp_request *, void *);

class HttpClient
{
public:
    HttpClient(const std::string &remotePath);
    ~HttpClient();

    int connect();
    void disconnect();
    bool connected();

    // At the moment the protocol is synchronous
    int getRequest(const std::string &command,
                   std::string &response);
    int postRequest(const std::string &url,
                    const std::string &payload,
                    std::string &response);
    int putRequest(const std::string &command,
                   const std::string &payload,
                   std::string &response);

private:
    struct event_base *base;
    struct evdns_base *dnsBase;
    struct evhttp_connection *con;
    std::string remoteHost;
    std::string remotePort;
    std::string remoteRepo;
    friend void HttpClient_requestDoneCB(struct evhttp_request *,
                                         void *);
};

#endif
