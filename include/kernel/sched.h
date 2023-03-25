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

/**
 * @brief Per-CPU process scheduler.
 *        Each CPU calls scheduler() after setting itself up.
 *        Scheduler never returns.  It loops, doing:
 *         - choose a process to run.
 *         - swtch to start running that process.
 *         - eventually that process transfers control
 *           via swtch back to the scheduler.
 */
extern void scheduler(void) __attribute__((noreturn));


/**
 * @brief Switch to scheduler.
 * @pre Must hold only p->lock and have changed proc->state
 * 
 */
void sched(void);
int sched_timeout(int timeout);

/**
 * @brief Give up the CPU for one scheduling round.
 * 
 */
void yield(void);


#endif