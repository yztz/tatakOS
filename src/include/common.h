#ifndef _H_COMMON_
#define _H_COMMON_

#include "types.h"
#include "param.h"
#include "utils.h"

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define POW2(n) (1 << n)
#define IS_POW2(n) ((n & (n - 1)) == 0)


#endif