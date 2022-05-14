#ifndef _H_TIME_
#define _H_TIME_

#include "types.h"

struct timespec {
	uint64_t tv_sec;        /* 秒 */
	uint64_t tv_usec;       /* 微秒 */
};

typedef struct timespec timespec_t;

#define SEC2TICK(sec) ((sec) * 10)
#define TICK2TIMESPEC(tick) \
	(timespec_t){.tv_sec=(tick + 10)/10, .tv_usec=(tick%10)*100}

#endif