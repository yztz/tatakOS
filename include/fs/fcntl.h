#ifndef _H_FCNTL_
#define _H_FCNTL_

#define O_RDONLY    00000000
#define O_WRONLY    00000001
#define O_RDWR      00000002 // 可读可写
#define O_EXCL		00000200
#define O_CREATE    00000100
#define O_TRUNC     00001000
#define O_DIRECTORY 00200000

#define SEEK_SET	0	/* seek relative to beginning of file */
#define SEEK_CUR	1	/* seek relative to current file position */
#define SEEK_END	2	/* seek relative to end of file */
#define SEEK_DATA	3	/* seek to the next data */
#define SEEK_HOLE	4	/* seek to the next hole */
#define SEEK_MAX	SEEK_HOLE

// mmap
// #define PROT_NONE       0x0
// #define PROT_READ       0x1
// #define PROT_WRITE      0x2
// #define PROT_EXEC       0x4

// #define MAP_SHARED      (1UL << 0)
// #define MAP_PRIVATE     (1UL << 1)

#define AT_FDCWD -100

#endif