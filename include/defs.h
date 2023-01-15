// 未来这个文件将被计划性去除

#ifndef _H_DEFS_
#define _H_DEFS_

#include "sbi.h"
struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct spinlock;
struct sleeplock;
struct stat;
struct superblock;

// console.c
void            consoleinit(void);
void            devnull_init(void);
void            devzero_init(void);
int             consoleready(void);


#include "printf.h"
#include "str.h"

// swtch.S
void            swtch(struct context*, struct context*);


// syscall.c
int             argint(int, int*);
int             argstr(int, char*, int);
int             argaddr(int, uint64 *);
// int             arglong(int, uint64 *);
int             fetchstr(uint64, char*, int);
int             fetchaddr(uint64, uint64*);
void            syscall();

char            *syscall_name(int num);

// trap.c
extern uint64   ticks;
void            trapinit(void);
void            trapinithart(void);
extern struct spinlock tickslock;

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))


#endif