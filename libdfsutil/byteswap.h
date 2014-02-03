

#ifndef libdfsutil_byteswap_h
#define libdfsutil_byteswap_h

#include <cassert>
#include <string.h>
#include <stdlib.h>

#ifndef _WIN32
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#endif

#if defined(__FreeBSD__)
#include <sys/endian.h>
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#elif defined(__linux__)
#include <endian.h>
#endif

/* BSD byte swapping functions for Windows and Mac OS X */
#if defined(_WIN32)
#define htobe16(_x) _byteswap_ushort(_x)
#define be16toh(_x) _byteswap_ushort(_x)
#define htobe32(_x) _byteswap_ulong(_x)
#define be32toh(_x) _byteswap_ulong(_x)
#define htobe64(_x) _byteswap_uint64(_x)
#define be64toh(_x) _byteswap_uint64(_x)
#elif defined(__APPLE__)
#define ori_swap16(_x) \
    ((((_x) & 0xff00) >> 8) | \
    (((_x) & 0x00ff) << 8))
#define ori_swap32(_x) \
    ((((_x) & 0xff000000U) >> 24) | \
    (((_x) & 0x00ff0000U) >> 8) | \
    (((_x) & 0x0000ff00U) << 8) | \
    (((_x) & 0x000000ffU) << 24))
#define ori_swap64(_x) \
    ((((_x) & 0xff00000000000000ULL) >> 56) | \
    (((_x) & 0x00ff000000000000ULL) >> 40) | \
    (((_x) & 0x0000ff0000000000ULL) >> 24) | \
    (((_x) & 0x000000ff00000000ULL) >> 8) | \
    (((_x) & 0x00000000ff000000ULL) << 8) | \
    (((_x) & 0x0000000000ff0000ULL) << 24) | \
    (((_x) & 0x000000000000ff00ULL) << 40) | \
    (((_x) & 0x00000000000000ffULL) << 56))
#define htobe16(_x) ori_swap16(_x)
#define be16toh(_x) ori_swap16(_x)
#define htobe32(_x) ori_swap32(_x)
#define be32toh(_x) ori_swap32(_x)
#define htobe64(_x) ori_swap64(_x)
#define be64toh(_x) ori_swap64(_x)
#endif

#endif /* __BYTESWAP_H__ */

