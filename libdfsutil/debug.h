

#ifndef libdfsutil_debug_h
#define libdfsutil_debug_h

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
void dfs_log(int level, const char *fmt, ...);
#else
void dfs_log(int level, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#endif
int dfs_open_log(const std::string &logPath);

#define LEVEL_SYS       0 /* Assert/Panic/Abort/NotImplemented */
#define LEVEL_ERR       1 /* Error */
#define LEVEL_WRN       2 /* Warning */
#define LEVEL_MSG       3 /* Stdout */
#define LEVEL_LOG       4 /* Log */
#define LEVEL_DBG       5 /* Debug */
#define LEVEL_VRB       6 /* Verbose */

/*
 * Remove all logging in PERF builds
 */
#ifdef DFS_PERF
#define SYSERROR(fmt, ...) dfs_log(LEVEL_ERR, fmt "\n", ##__VA_ARGS__)
#define WARNING(fmt, ...)
#define MSG(fmt, ...)
#define LOG(fmt, ...)
#else
#define SYSERROR(fmt, ...) dfs_log(LEVEL_ERR, fmt "\n", ##__VA_ARGS__)
#define WARNING(fmt, ...) dfs_log(LEVEL_WRN, fmt "\n", ##__VA_ARGS__)
#define MSG(fmt, ...) dfs_log(LEVEL_MSG, fmt "\n", ##__VA_ARGS__)
#define LOG(fmt, ...) dfs_log(LEVEL_LOG, fmt "\n", ##__VA_ARGS__)
#endif

/*
 * Only DEBUG builds compile in DLOG messages
 */
#ifdef DEBUG
#define DLOG(fmt, ...) dfs_log(LEVEL_DBG, fmt "\n", ##__VA_ARGS__)
#else
#define DLOG(fmt, ...)
#endif

#ifdef DEBUG
#define ASSERT(_x) \
    if (!(_x)) { \
        dfs_log(LEVEL_SYS, "ASSERT("#_x"): %s %s:%d\n", \
                __FUNCTION__, __FILE__, __LINE__); \
        assert(_x); \
    }
#else
#define ASSERT(_x)
#endif

#ifdef _WIN32
#define PANIC() { dfs_log(LEVEL_SYS, "PANIC: " \
                         "function %s, file %s, line %d\n", \
                         __FUNCTION__, __FILE__, __LINE__); abort(); }
#define NOT_IMPLEMENTED(_x) if (!(_x)) { dfs_log(LEVEL_SYS, \
                                "NOT_IMPLEMENTED(" #_x "): %s %s:%d\n", \
                                __FUNCTION__, __FILE__, __LINE__); abort(); }
#else /* _WIN32 */
#define PANIC() { dfs_log(LEVEL_SYS, "PANIC: " \
                         "function %s, file %s, line %d\n", \
                         __func__, __FILE__, __LINE__); abort(); }
#define NOT_IMPLEMENTED(_x) if (!(_x)) { dfs_log(LEVEL_SYS, \
                                "NOT_IMPLEMENTED(" #_x "): %s %s:%d\n", \
                                __func__, __FILE__, __LINE__); abort(); }
#endif /* _WIN32 */

void OriDebug_PrintHex(const std::string &data,
                       off_t off = 0,
                       size_t limit = 0);
void OriDebug_PrintBacktrace();
void OriDebug_LogBacktrace();

#endif

