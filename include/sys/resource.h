#ifndef _H_RESOURCE_
#define _H_RESOURCE_

#include "kernel/time.h"

#define RLIMIT_CPU     0
#define RLIMIT_FSIZE   1
#define RLIMIT_DATA    2
#define RLIMIT_STACK   3
#define RLIMIT_CORE    4
#define RLIMIT_RSS     5
#define RLIMIT_NPROC   6
#define RLIMIT_NOFILE  7
#define RLIMIT_MEMLOCK 8
#define RLIMIT_AS      9

typedef unsigned long long rlim_t;

struct rlimit {
    rlim_t rlim_cur;
    rlim_t rlim_max;
};

#define	RUSAGE_SELF	0		/* calling process */
#define	RUSAGE_CHILDREN	-1		/* terminated child processes */

struct rusage {
	timeval_t ru_utime; /* user CPU time used */
	timeval_t ru_stime; /* system CPU time used */
	// long   ru_maxrss;        /* maximum resident set size */
	// long   ru_ixrss;         /* integral shared memory size */
	// long   ru_idrss;         /* integral unshared data size */
	// long   ru_isrss;         /* integral unshared stack size */
	// long   ru_minflt;        /* page reclaims (soft page faults) */
	// long   ru_majflt;        /* page faults (hard page faults) */
	// long   ru_nswap;         /* swaps */
	// long   ru_inblock;       /* block input operations */
	// long   ru_oublock;       /* block output operations */
	// long   ru_msgsnd;        /* IPC messages sent */
	// long   ru_msgrcv;        /* IPC messages received */
	// long   ru_nsignals;      /* signals received */
	// long   ru_nvcsw;         /* voluntary context switches */
	// long   ru_nivcsw;        /* involuntary context switches */
};


#endif