

#ifndef libdfsutil_dfsnet_h
#define libdfsutil_dfsnet_h

std::string OriNet_ResolveHost(const std::string &hostname);
std::vector<std::string> OriNet_GetAddrs();
std::string OriNet_Hostname();
std::string OriNet_Domainname();

#endif
