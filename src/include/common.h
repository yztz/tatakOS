#ifndef _H_COMMON_
#define _H_COMMON_

#include <stdarg.h>
#include "param.h"
#include "memlayout.h"
#include "sys/error.h"
#include "types.h"
#include "utils.h"
#include "str.h"

/// @brief Generic type to describe a buffer
typedef struct buf_desc {
    char *buf;
    int size;
} buf_desc_t;

extern volatile int panicked;

#endif