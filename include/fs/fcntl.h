#ifndef _H_FCNTL_
#define _H_FCNTL_

#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002 // 可读可写
#define O_CREATE 0x40
#define O_DIRECTORY 0x0200000

// mmap
#define PROT_NONE       0x0
#define PROT_READ       0x1
#define PROT_WRITE      0x2
#define PROT_EXEC       0x4

#define MAP_FILE 0
#define MAP_SHARED      0x01
#define MAP_PRIVATE     0x02
#define MAP_FAILED ((void *) -1)


#define AT_FDCWD -100

#endif