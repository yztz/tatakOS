#ifndef _H_DEBUG_
#define _H_DEBUG_

#include "printf.h"
#include "str.h"
#include "common.h"

#define assert(conditon) if(!(conditon)) {printf("assert fail: "#conditon"\n");LOOP()}


#if defined DEBUG && !defined QUIET

#ifndef __MODULE_NAME__
#define __MODULE_NAME__ OTHER
#endif

#define debug(fmt, ...) printf("[DEBUG]["STR(__MODULE_NAME__)": %s] "fmt"\n",__FUNCTION__ ,##__VA_ARGS__);
#define debug_if(condition, fmt, ...) {if(condition) debug(fmt, ##__VA_ARGS__)}
#else 

#define debug(fmt, ...)
#define debug_if(condition, fmt, ...)

#endif // DEBUG

#define info(fmt, ...) printf("[INFO]["STR(__MODULE_NAME__)": %s] "fmt"\n",__FUNCTION__ ,##__VA_ARGS__);
#define ERROR(str)    error(str, __FILE__, __func__, __LINE__)
#define ER()    ERROR("")

#define grn(str) 	"\e[32;1m"str"\e[0m"
#define ylw(str) 	"\e[33;1m"str"\e[0m"
#define rd(str) 	"\e[31;1m"str"\e[0m"

/* 调试选项 */
// #define DEBUG_MM_RB

#endif // _H_DEBUG_