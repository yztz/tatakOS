#ifndef _H_TIME_
#define _H_TIME_

#include "common.h"

struct timespec {
	uint64_t tv_sec;        /* 秒 */
	uint64_t tv_nsec;       /* 微秒 */
};

typedef struct timespec timespec_t;

#define SEC2TICK(sec) ((sec) * 10)
#define TICK2SEC(ticks) (ticks/10)
#define TICK2TIMESPEC(tick) \
	(timespec_t){.tv_sec=(tick)/10, .tv_nsec=(tick%10)*100*1000*1000}

static inline void time_print(timespec_t *tm) {
	printf("time: %lds %ldns\n", tm->tv_sec, tm->tv_nsec);
}

#endif