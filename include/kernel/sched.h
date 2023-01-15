#ifndef _H_SCHED_
#define _H_SCHED_

#include "kernel/proc.h"
#include "kernel/taskqueue.h"

extern tq_t *pstatelist[MAXPSTATE];

#define pstate_list_lock(state) tq_lock(pstatelist[state])
#define pstate_list_unlock(state) tq_unlock(pstatelist[state])
#define pstate_list_poll(state) tq_poll(pstatelist[state])
#define pstate_list_offer(state, proc) tq_offer(pstatelist[state], proc)
#define pstate_list_delete(state, proc) tq_remove(pstatelist[state], proc)


#endif