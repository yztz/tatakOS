#ifndef _H_DEBUG_
#define _H_DEBUG_


#define assert(conditon) if(!(conditon)) panic("assert fail");


#if defined DEBUG && !defined QUIET
#include "printf.h"
#include "str.h"
#include "common.h"

#ifndef __MODULE_NAME__
#define __MODULE_NAME__ OTHER
#endif

#define debug(fmt, ...) printf("["STR(__MODULE_NAME__)": %s] "fmt"\n",__FUNCTION__ ,##__VA_ARGS__);
#define debug_if(condition, fmt, ...) {if(condition) debug(fmt, ##__VA_ARGS__)}
#else 

#define debug(fmt, ...)
#define debug_if(condition, fmt, ...)

#endif // DEBUG

#define grn(str) 	"\e[32;1m"str"\e[0m"
#define ylw(str) 	"\e[33;1m"str"\e[0m"
#define rd(str) 	"\e[31;1m"str"\e[0m"
#define bl(str) 	"\e[34;1m"str"\e[0m"


#endif // _H_DEBUG_