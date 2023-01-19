#ifndef _H_USYS_
#define _H_USYS_

#include "types.h"

int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int*);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int lseek(int, off_t, int);
int close(int);
int kill(int);
int exec(char*, char**);
int openat(int fd, const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
// int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdirat(int fd, const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int ktest(void);
uint64 timetag(void);
int memuse(void);
int halt(void);
int philosophy(void);

#endif