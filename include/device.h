#ifndef _H_DEVICE_
#define _H_DEVICE_

#include "types.h"
#include "param.h"

/**
 * @brief coarse implementation for **char** device
 * @note need refactor
 */
struct device {
    /// @brief read dev
    int (*read)(int, uint64, int);
    /// @brief write dev
    int (*write)(int, uint64, int);
};

#define CONSOLE 1
#define DEVNULL 2
#define DEVZERO 3
#define DEVRTC  4

extern device_t devs[];

void dev_stat(device_t *dev, struct kstat *stat);

#endif
