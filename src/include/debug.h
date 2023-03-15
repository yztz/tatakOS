#ifndef _H_DEBUG_
#define _H_DEBUG_

#include "str.h"
#include "common.h"


#define assert_s(conditon, fmt, ...) if(!(conditon)) {kprintf(fmt"\n", ##__VA_ARGS__);panicked=1;LOOP()}
#define assert(condition) assert_s(condition, "assert fail: "#condition)

#if defined DEBUG && !defined QUIET

#ifndef __MODULE_NAME__
#define __MODULE_NAME__ OTHER
#endif // __MODULE_NAME__

#define debug(fmt, ...) kprintf("[DEBUG]["STR(__MODULE_NAME__)": %s] "fmt"\n",__FUNCTION__ ,##__VA_ARGS__);
#define debug_if(condition, fmt, ...) {if(condition) debug(fmt, ##__VA_ARGS__)}
#define debug_statement(statement) statement

extern int first_user_pid;

#else // DEBUG

#define debug(fmt, ...)
#define debug_if(condition, fmt, ...)
#define debug_statement(statement)

#endif // DEBUG

#define grn(str) 	"\e[32;1m"str"\e[0m"
#define ylw(str) 	"\e[33;1m"str"\e[0m"
#define rd(str) 	"\e[31;1m"str"\e[0m"

#define info(fmt, ...) kprintf("[INFO] "fmt"\n", ##__VA_ARGS__);
#define warn(fmt, ...) kprintf("[WARN] "ylw(fmt)"\n", ##__VA_ARGS__);
#define ERROR(str)    error(str, __FILE__, __func__, __LINE__)
#define ER()    ERROR("")

#endif // _H_DEBUG_