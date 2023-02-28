#ifndef _H_COMMON_
#define _H_COMMON_

#include <stdarg.h>
#include "param.h"
#include "sys/error.h"
#include "types.h"
#include "utils.h"
#include "str.h"

/* 缓存空间描述符 */
typedef struct _buf_desc_t {
    char *buf;
    int size;
} buf_desc_t;

extern volatile int panicked;

#endif