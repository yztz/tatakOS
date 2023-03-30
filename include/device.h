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
    int (*read)(int, uint64_t, int);
    /// @brief write dev
    int (*write)(int, uint64_t, int);
};

#define CONSOLE 1

extern device_t devs[];


#endif
