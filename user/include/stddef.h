#ifndef __STDDEF_H__
#define __STDDEF_H__

/* Represents true-or-false values */
typedef int bool;

/* Explicitly-sized versions of integer types */
typedef char int8;
typedef unsigned char uint8_t;
typedef short int16;
typedef unsigned short uint16_t;
typedef int int32;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned int uint;

#define ULONG_MAX (0xffffffffffffffffULL)
#define LONG_MAX (0x7fffffffffffffffLL)
#define INTMAX_MAX LONG_MAX
#define UINT_MAX (0xffffffffU)
#define INT_MAX (0x7fffffff)
#define UCHAR_MAX (0xffU)
#define CHAR_MAX (0x7f)

/* *
 * Pointers and addresses are 32 bits long.
 * We use pointer types to represent addresses,
 * uintptr_t to represent the numerical values of addresses.
 * */
#if __riscv_xlen == 64
typedef int64_t intptr_t;
typedef uint64_t uintptr_t;
#elif __riscv_xlen == 32
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
#endif

/* size_t is used for memory object sizes */
typedef uintptr_t size_t;
typedef intptr_t ssize_t;

typedef int pid_t;

#define NULL ((void *)0)

#define SIGCHLD   17

#define va_start(ap, last) (__builtin_va_start(ap, last))
#define va_arg(ap, type) (__builtin_va_arg(ap, type))
#define va_end(ap) (__builtin_va_end(ap))
#define va_copy(d, s) (__builtin_va_copy(d, s))
typedef __builtin_va_list va_list;

#define O_RDONLY 000
#define O_WRONLY 001
#define O_RDWR 002 // 可读可写
//#define O_CREATE 0x200
#define O_CREATE 0100
#define O_DIRECTORY 0200000

#define DIR 0x040000
#define FILE 0x100000

#define AT_FDCWD -100

typedef struct {
    uint64_t sec;  // 自 Unix 纪元起的秒数
    uint64_t usec; // 微秒数
} TimeVal;

typedef struct {
    uint64_t dev;    // 文件所在磁盘驱动器号，不考虑
    uint64_t ino;    // inode 文件所在 inode 编号
    uint32_t mode;   // 文件类型
    uint32_t nlink;  // 硬链接数量，初始为1
    uint64_t pad[7]; // 无需考虑，为了兼容性设计
} Stat;

typedef unsigned int mode_t;
typedef long int off_t;

struct kstat {
    uint64_t st_dev;
    uint64_t st_ino;
    mode_t st_mode;
    uint32_t st_nlink;
    uint32_t st_uid;
    uint32_t st_gid;
    uint64_t st_rdev;
    unsigned long __pad;
    off_t st_size;
    uint32_t st_blksize;
    int __pad2;
    uint64_t st_blocks;
    long st_atime_sec;
    long st_atime_nsec;
    long st_mtime_sec;
    long st_mtime_nsec;
    long st_ctime_sec;
    long st_ctime_nsec;
    unsigned __unused[2];
};



struct linux_dirent64 {
    uint64_t        d_ino;
    int64_t         d_off;
    unsigned short  d_reclen;
    unsigned char   d_type;
    char            d_name[];
};



// for mmap
#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define PROT_GROWSDOWN 0X01000000
#define PROT_GROWSUP 0X02000000

#define MAP_FILE 0
#define MAP_SHARED 0x01
#define MAP_PRIVATE 0X02
#define MAP_FAILED ((void *) -1)

#endif // __STDDEF_H__
