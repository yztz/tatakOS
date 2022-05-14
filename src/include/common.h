#ifndef _H_COMMON_
#define _H_COMMON_

#include "types.h"
#include "param.h"
#include "utils.h"

#define __STR(name) #name
#define STR(name) __STR(name)

#define UNUSED(x) x __attribute__((__unused__))

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define POW2(n) (1 << n)
#define IS_POW2(n) ((n & (n - 1)) == 0)

/* 缓存空间描述符 */
typedef struct _buf_desc_t {
    char *buf;
    int size;
} buf_desc_t;


#endif