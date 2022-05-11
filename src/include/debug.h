#ifndef _H_DEBUG_
#define _H_DEBUG_



#if defined DEBUG && !defined QUIET
#include "printf.h"
#include "str.h"
#include "common.h"

#ifndef __MODULE_NAME__
#define __MODULE_NAME__ __FILE__
#endif

#define debug(fmt, ...) printf("["STR(__MODULE_NAME__)"] "fmt"\n", ##__VA_ARGS__);



#define whenstr(a, b)  int __debug__flag = strncmp((a), (b), strlen((b))) == 0;

#define when(condition) \
    int __debug__flag = (condition);
#define endwhen \
    __debug__flag = 1;
#define ifprint(fmt, ...) {if(__debug__flag) printf(fmt"\n", ##__VA_ARGS__);}

#else 

#define when(condition)
#define whenstr(a, b)
#define endwhen
#define ifprint(fmt, ...)
#define debug(fmt, ...)

#endif // DEBUG


#endif // _H_DEBUG_