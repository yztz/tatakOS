#ifndef _H_FCNTL_
#define _H_FCNTL_

#define O_RDONLY    00000000
#define O_WRONLY    00000001
#define O_RDWR      00000002 // 可读可写
#define O_EXCL		00000200
#define O_CREATE    00000100
#define O_TRUNC     00001000
#define O_LARGEFILE 00100000
#define O_DIRECTORY 00200000
#define O_NOCTTY       0400
#define O_APPEND      02000
#define O_NONBLOCK    04000
#define O_DSYNC      010000
#define O_SYNC     04010000
#define O_RSYNC    04010000
#define O_NOFOLLOW  0400000
#define O_CLOEXEC  02000000

#define SEEK_SET	0	/* seek relative to beginning of file */
#define SEEK_CUR	1	/* seek relative to current file position */
#define SEEK_END	2	/* seek relative to end of file */
#define SEEK_DATA	3	/* seek to the next data */
#define SEEK_HOLE	4	/* seek to the next hole */
#define SEEK_MAX	SEEK_HOLE

#define F_DUPFD  0
#define F_GETFD  1
#define F_SETFD  2
#define F_GETFL  3
#define F_SETFL  4
#define F_DUPFD_CLOEXEC 1030

#define FD_CLOEXEC	1

// mmap
// #define PROT_NONE       0x0
// #define PROT_READ       0x1
// #define PROT_WRITE      0x2
// #define PROT_EXEC       0x4

// #define MAP_SHARED      (1UL << 0)
// #define MAP_PRIVATE     (1UL << 1)

/* These are linux-specific */
// #define MAP_GROWSDOWN	0x01000		/* stack-like segment */
// #define MAP_DENYWRITE	0x02000		/* ETXTBSY */
/* 这个标识符被我用来从0地址开始映射可执行文件，否则地址为0被视为自动分配空间 */
// #define MAP_EXECUTABLE	0x04000		/* mark it as an executable */
// #define MAP_LOCKED	0x08000		/* lock the mapping */


// #define MAP_FAILED ((void *) -1)
#define AT_FDCWD -100

#endif