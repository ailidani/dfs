

#ifndef libdfsutil_tuneables_h
#define libdfsutil_tuneables_h


#define COPYFILE_BUFSZ	(256 * 1024)
#define HASHFILE_BUFSZ	(256 * 1024)
#define COMPFILE_BUFSZ  (16 * 1024)

#define LARGEFILE_MINIMUM (1024 * 1024)


// Minimum compressable object (FastLZ requires 66 bytes)
#define ZIP_MINIMUM_SIZE 512
// How much of a payload to check for compressibility
#define COMPCHECK_BYTES 1024
// Maximum compression ratio (0.8 means compressed file is 80% size of original)
#define COMPCHECK_RATIO 0.95

// These are soft maximums ("heuristics")
// 64 MB
#define PACKFILE_MAXSIZE (1024*1024*64)
#define PACKFILE_MAXOBJS (2048)

// Choose the hash algorithm (choose one)
#define ORI_USE_SHA256
//#define ORI_USE_SKEIN
#if !defined(ORI_USE_SHA256) && !defined(ORI_USE_SKEIN)
#error "Please select one hash algorithm."
#endif

// Choose the compression algorithm (choose one)
#define ORI_USE_LZMA
//#define ORI_USE_FASTLZ
#if defined(ORI_USE_COMPRESISON) && !defined(ORI_USE_LZMA) && !defined(ORI_USE_FASTLZ)
#error "Please select one compression algorithm."
#endif

// Choose large blob chunk size
#define ORI_USE_FIXED
//#define ORI_USE_RK
#if !defined(ORI_USE_FIXED) && !defined(ORI_USE_RK)
#error "Please select one large blob chunk size algorithm."
#endif

#endif
