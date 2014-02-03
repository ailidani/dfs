

#ifndef dfs_logging_h
#define dfs_logging_h

void ori_fuse_log_enable();
void ori_fuse_log(const char *what, ...)
    __attribute__((format(printf, 1, 2)));

#if defined(DEBUG)
#define FUSE_LOG(fmt, ...) ori_fuse_log(fmt "\n", ##__VA_ARGS__)
#define FUSE_PLOG(fmt, ...) ori_fuse_log(fmt "\n", ##__VA_ARGS__)
#elif defined(ORI_PERF)
#define FUSE_LOG(fmt, ...)
#define FUSE_PLOG(fmt, ...) ori_fuse_log(fmt "\n", ##__VA_ARGS__)
#else
#define FUSE_LOG(fmt, ...)
#define FUSE_PLOG(fmt, ...)
#endif

#endif

