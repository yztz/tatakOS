#ifndef _H_PARAM_
#define _H_PARAM_

#define NPROC        50  // maximum number of processes
#define NOFILE       5  // open files per process
#define NFILE        30  // open files per system
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXARGLEN    64
#define MAXENV       32  // max exec arguments

#define NBUF         (30)  // size of disk block cache
#define MAXPATH      128   // maximum file path name
#define MAX_MAP      15    // maximum io map

#define MAX_FD       256

#define BSIZE 512
#define NENTRY 10
#define MAX_FILE_NAME 64

#define USTACKSIZE (4096 * 40)

#define TICK_GRANULARITY 10L // ms

/* 最大readahead页数占当前空余内存的页数 */
#define READ_AHEAD_RATE 10
/* Buddy最大Order */
#define BUDDY_ORDER_NUM 10

/* Which page allocator */
#define BUDDY_ALLOCATOR 0
#define FREELIST_ALLOCATOR 1

#define PAGE_ALLOCATOR BUDDY_ALLOCATOR




#endif