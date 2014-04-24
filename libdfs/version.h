

#ifndef libdfs_version_h
#define libdfs_version_h

#define STR_HELPER(x)  #x
#define STR(x) STR_HELPER(x)

#define ORI_MAJOR_VERSION       0
#define ORI_MINOR_VERSION       8
#define ORI_PATCH_VERSION       1
#define ORI_VERSION_STR \
    "Version " STR(ORI_MAJOR_VERSION) "." STR(ORI_MINOR_VERSION) "." STR(ORI_PATCH_VERSION)

#define ORI_FS_MAJOR_VERSION    1
#define ORI_FS_MINOR_VERSION    1

#define ORI_FS_VERSION_STR \
    "ORI" STR(ORI_FS_MAJOR_VERSION) "." STR(ORI_FS_MINOR_VERSION)

#endif

