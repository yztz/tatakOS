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


#endif