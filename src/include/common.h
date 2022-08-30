#ifndef _H_COMMON_
#define _H_COMMON_

#include "param.h"
#include "sys/error.h"
#include "types.h"
#include "utils.h"

#define __STR(name) #name
#define STR(name) __STR(name)

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

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