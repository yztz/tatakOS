#ifndef __STRING_H__
#define __STRING_H__

#include "stddef.h"

int isspace(int c);
int isdigit(int c);
int atoi(const char *s);
void *memset(void *dest, int c, size_t n);
int strcmp(const char *l, const char *r);
size_t strlen(const char *);
size_t strnlen(const char *s, size_t n);
char *strncpy(char *restrict d, const char *restrict s, size_t n);
int strncmp(const char *_l, const char *_r, size_t n);

#define _STR(x) #x
#define STR(x) _STR(x)


#define	red(str)      "\e[1;31m"str"\e[0m"
#define	green(str)    "\e[1;32m"str"\e[0m"
#define	yellow(str)   "\e[1;33m"str"\e[0m"
#define	blue(str)     "\e[1;34m"str"\e[0m"
#define	magenta(str)  "\e[1;35m"str"\e[0m"
#define	cyan(str)     "\e[1;36m"str"\e[0m"
#define	white(str)    "\e[1;37m"str"\e[0m"

#endif // __STRING_H__
