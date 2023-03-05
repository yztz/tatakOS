#ifndef _H_BLK_DEVICE_
#define _H_BLK_DEVICE_

#include "types.h"

/// @brief Logical Block Address
typedef uint32_t LBA_t;

struct blk_device {
    void (*blk_read)(LBA_t blkno, int blknum, char *buf);
    void (*blk_write)(LBA_t blkno, int blknum, const char *buf);
    // int (*blk_size)();
};

typedef struct blk_device blk_dev_t;


#endif