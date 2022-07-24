#ifndef _H_SYS_
#define _H_SYS_
#include "common.h"

uint64 sys_timetag(void);
uint64 sys_ktest(void);


struct utsname {
	char sysname[65];
	char nodename[65];
	char release[65];
	char version[65];
	char machine[65];
	char domainname[65];
};

typedef struct utsname utsname_t;


typedef struct __fsid_t {
	int __val[2];
} fsid_t;

typedef uint64_t fsblkcnt_t;
typedef uint64_t fsfilcnt_t;

struct statfs {
	unsigned long f_type, f_bsize;
	fsblkcnt_t f_blocks, f_bfree, f_bavail;
	fsfilcnt_t f_files, f_ffree;
	fsid_t f_fsid;
	unsigned long f_namelen, f_frsize, f_flags, f_spare[4];
};


#endif