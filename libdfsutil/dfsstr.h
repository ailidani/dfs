

#ifndef libdfsutil_dfsstr_h
#define libdfsutil_dfsstr_h

#include <stdint.h>

#include <string>
#include <vector>

std::vector<std::string> OriStr_Split(const std::string &str, char sep);
std::string OriStr_Join(const std::vector<std::string> &str, char sep);

bool OriStr_StartsWith(const std::string &str, const std::string &part);
bool OriStr_EndsWith(const std::string &str, const std::string &part);

#endif
