#include "device.h"
#include "atomic/spinlock.h"
#include "fs/stat.h"

void dev_stat(device_t *dev, struct kstat *stat) {
    stat->st_ino = 0;
    stat->st_gid = 0;
    stat->st_uid = 0;
    stat->st_dev = 0;
    stat->st_rdev = 0;
    stat->st_mode = S_IFCHR;
    stat->st_blksize = 0;
    stat->st_blocks = 0;
    stat->st_size = 0;
    stat->st_atime_nsec = 0;
    stat->st_atime_sec = 0;
    stat->st_mtime_sec = 0;
    stat->st_mtime_nsec = 0;
    stat->st_ctime_sec = 0;
    stat->st_ctime_nsec = 0;
    stat->st_nlink = 1;
}