#ifndef _H_PARAM_
#define _H_PARAM_

/* maximum nr of processes */
#define NPROC        50  
/* static open files per process */
#define NOFILE       5
/* maximum device nr */
#define NDEV         10
/* device nr of file system root disk */
#define ROOTDEV       1
/* maximum exec arg nr */
#define MAXARG       32 
/* maximum exec arg length */
#define MAXARGLEN    64
/* maximum exec env nr */
#define MAXENV       32  
/* size of disk block cache */
#define NBUF         30   
/* maximum file descriptor */
#define MAX_FD       256
/* block size */
#define BSIZE        512
/* maximum file entry in memory */
#define NENTRY       10
/* maximum file path name */
#define MAXPATH      128 
/* maximum file name length */
#define MAX_FILE_NAME 64
/* user stack size */
#define USTACKSIZE (4096 * 20)
/* kernel stack size */
#define KSTACK_SZ (PGSIZE * 4)
/* clock IRQ interval (ms) */
#define TICK_GRANULARITY 10L
/* percentage of the maximum readahead pages to the free */
#define READ_AHEAD_RATE 10
/* maximum order for buddy */
#define BUDDY_ORDER_NUM 10
/* serialize kernel ouput (kprintf), output maybe out of order if zero */
#define KERNEL_OUTPUT_LOCK 1
/* first user program name */
#define USER0 "init0"

#endif
