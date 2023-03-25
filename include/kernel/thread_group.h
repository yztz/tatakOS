#ifndef _H_THREAD_GROUP_
#define _H_THREAD_GROUP_

#include "types.h"
#include "atomic/spinlock.h"
#include "list.h"
#include "kernel/waitqueue.h"

struct thread_group {
    int tg_id;
    int ref;
    int thrdcnt;
    wq_t futex_wq;
    spinlock_t lock;
    list_head_t member;
};


/**
 * @brief create a new thread group
 * 
 * @param p main process whose pid will be used as PID intsead of TID
 * @return tg_t* 
 */
tg_t *tg_new(proc_t *p);

/**
 * @brief free a thread group
 * 
 * @param pself 
 */
void tg_free(tg_t **pself);

void tg_ref(tg_t *self);
void tg_join(tg_t *self, proc_t *p);
int tg_thrd_cnt(tg_t *self);
int tg_quit(tg_t *self);

/**
 * @brief get PID
 * 
 * @param self 
 * @return int PID of this thread group
 */
int tg_pid(tg_t *self);

#endif