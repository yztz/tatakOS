#ifndef _H_IO_
#define _H_IO_

#include "types.h"

struct iovec {
    /// @brief Starting address
    void  *iov_base;
    /// @brief Number of bytes to transfer
    size_t iov_len;
};

extern uint64_t ioremap(uint64_t addr, size_t size);


#endif