
#ifndef libdfsutil_dfsfile_h
#define libdfsutil_dfsfile_h

#include <stdint.h>

#include <string>

bool OriFile_Exists(const std::string &path);
bool OriFile_IsDirectory(const std::string &path);
int OriFile_MkDir(const std::string &path);
int OriFile_RmDir(const std::string &path);
size_t OriFile_GetSize(const std::string &path);
std::string OriFile_RealPath(const std::string &path);
std::string OriFile_ReadFile(const std::string &path);
bool OriFile_WriteFile(const std::string &blob, const std::string &path);
bool OriFile_WriteFile(const char *blob, size_t len, const std::string &path);
bool OriFile_Append(const std::string &blob, const std::string &path);
bool OriFile_Append(const char *blob, size_t len, const std::string &path);
int OriFile_Copy(const std::string &origPath, const std::string &newPath);
int OriFile_Move(const std::string &origPath, const std::string &newPath);
int OriFile_Delete(const std::string &path);
int OriFile_Rename(const std::string &from, const std::string &to);

std::string OriFile_Basename(const std::string &path);
std::string OriFile_Dirname(const std::string &path);

#endif
