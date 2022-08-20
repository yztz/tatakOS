#ifndef _H_TIME_
#define _H_TIME_

#include "common.h"

struct timespec {
	uint64_t ts_sec;        /* 秒 */
	uint64_t ts_nsec;       /* 纳秒 */
};

struct timeval {
	uint64_t		tv_sec;		/* seconds */
	uint64_t	tv_usec;	/* and microseconds */
};

typedef struct timespec timespec_t;
typedef struct timeval timeval_t;

#define SEC2TICK(sec) ((sec) * 1000 / TICK_GRANULARITY)
#define MS2TICK(ms) ((ms) / TICK_GRANULARITY)
#define US2TICK(us) ((us) * TICK_GRANULARITY / 1000)
#define NS2TICK(ns) ((ns) / 1000 / 1000 / TICK_GRANULARITY)

#define TICK2SEC(tick) (tick * TICK_GRANULARITY / 1000)
#define TICK2MS(tick) (tick * TICK_GRANULARITY)
#define TICK2US(tick) (tick * TICK_GRANULARITY * 1000)
#define TICK2NS(tick) (tick * TICK_GRANULARITY * 1000 * 1000)

#define TICK2TIMESPEC(tick) \
	(timespec_t){.ts_sec=TICK2SEC(tick), .ts_nsec=TICK2NS(tick) % (1000 * 1000 * 1000)}

#define TICK2TIMEVAL(tick) \
	(timeval_t){.tv_sec=TICK2SEC(tick), .tv_usec=TICK2US(tick) % (1000 * 1000)}

#define RT2MS(rt, freq) (rt * 1000/freq)
#define RT2US(rt, freq) (rt * 1000 * 1000/freq)

#define RT2TIMESPEC(rt, freq) {\
	.ts_sec=rt/freq, .ts_nsec=(rt * (1000 * 1000 * 1000)/freq) % (1000 * 1000 * 1000)\
}

static inline uint64_t ts2ticks(timespec_t *ts) {
	return ts ? SEC2TICK(ts->ts_sec) + NS2TICK(ts->ts_nsec) : 0;
}

static inline void time_print(timespec_t *ts) {
	printf("time: %lds %ldns\n", ts->ts_sec, ts->ts_nsec);
}

#endif