
#ifndef libdfsutil_dfsutil_h
#define libdfsutil_dfsutil_h


#include <stdint.h>

#include <string>
#include <vector>

// GCC Only
#define UNUSED __attribute__((unused))

bool Util_IsValidName(const std::string &path);

std::vector<std::string> Util_PathToVector(const std::string &path);
std::string Util_GetFullname();
std::string Util_GetHome();
std::string Util_GetOSType();
std::string Util_GetMachType();
int Util_SetBlocking(int fd, bool block);

std::string Util_NewUUID();
bool Util_IsPathRemote(const std::string &path);

std::string Util_SystemError(int status);

#endif
