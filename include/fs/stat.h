#ifndef _H_STAT_
#define _H_STAT_

#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device

typedef unsigned int mode_t;
typedef long int off_t;

struct kstat {
        uint64 st_dev;
        uint64 st_ino;
        mode_t st_mode;
        uint32 st_nlink;
        uint32 st_uid;
        uint32 st_gid;
        uint64 st_rdev;
        unsigned long __pad;
        off_t st_size;
        uint32 st_blksize;
        int __pad2;
        uint64 st_blocks;
        long st_atime_sec;
        long st_atime_nsec;
        long st_mtime_sec;
        long st_mtime_nsec;
        long st_ctime_sec;
        long st_ctime_nsec;
        unsigned __unused[2];
};

#endif