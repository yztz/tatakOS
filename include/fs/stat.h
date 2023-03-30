#ifndef _H_STAT_
#define _H_STAT_

#define T_DIR       1 // Directory
#define T_FILE      2 // File
#define T_DEVICE    3 // Device

#define S_IFMT     0170000  // bit mask for the file type bit field
#define S_IFSOCK   0140000  // socket
#define S_IFLNK    0120000  // symbolic link
#define S_IFREG    0100000  // regular file
#define S_IFBLK    0060000  // block device
#define S_IFDIR    0040000  // directory
#define S_IFCHR    0020000  // character device
#define S_IFIFO    0010000  // FIFO

typedef unsigned int mode_t;
typedef long int off_t;

#define UTIME_NOW	((1l << 30) - 1l)
#define UTIME_OMIT	((1l << 30) - 2l)

struct kstat {
    uint64_t st_dev;
    uint64_t st_ino;
    mode_t st_mode;
    uint32_t st_nlink;
    uint32_t st_uid;
    uint32_t st_gid;
    uint64_t st_rdev;
    unsigned long __pad;
    off_t st_size;
    uint32_t st_blksize;
    int __pad2;
    uint64_t st_blocks;
    long st_atime_sec;
    long st_atime_nsec;
    long st_mtime_sec;
    long st_mtime_nsec;
    long st_ctime_sec;
    long st_ctime_nsec;
    unsigned __unused[2];
};

#endif