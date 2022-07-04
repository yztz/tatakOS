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
// #define MAP_SHARED      0x01
// #define MAP_PRIVATE     0x02
#define MAP_SHARED	0x01		/* Share changes */
#define MAP_PRIVATE	0x02		/* Changes are private */
#define MAP_TYPE	0x0f		/* Mask for type of mapping */
#define MAP_FIXED	0x10		/* Interpret addr exactly */
#define MAP_ANONYMOUS	0x20		/* don't use a file */

/* These are linux-specific */
#define MAP_GROWSDOWN	0x01000		/* stack-like segment */
#define MAP_DENYWRITE	0x02000		/* ETXTBSY */
#define MAP_EXECUTABLE	0x04000		/* mark it as an executable */
#define MAP_LOCKED	0x08000		/* lock the mapping */


#define MAP_FAILED ((void *) -1)
#define AT_FDCWD -100

#endif