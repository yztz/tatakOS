#include "kernel/sched.h"

#define __MODULE_NAME__ SCHED
#include "debug.h"

// We set queues for the process' states respectively
TASK_QUEUE_INIT(unused_queue);
TASK_QUEUE_INIT(used_queue);
TASK_QUEUE_INIT(runnable_queue);
TASK_QUEUE_INIT(sleep_queue);
TASK_QUEUE_INIT(zombie_queue);

#define pstate_list_lock(state) tq_lock(pstatelist[state])
#define pstate_list_unlock(state) tq_unlock(pstatelist[state])
#define pstate_list_poll(state) tq_poll(pstatelist[state])
#define pstate_list_offer(state, proc) tq_offer(pstatelist[state], proc)
#define pstate_list_delete(state, proc) tq_remove(pstatelist[state], proc)

tq_t *pstatelist[MAXPSTATE] = {
    [UNUSED] &unused_queue,
    [USED] &used_queue,
    [SLEEPING] &sleep_queue,
    [RUNNABLE] &runnable_queue,
    // [RUNNING] &running_queue,
    [ZOMBIE] &zombie_queue,
};


// p->lock hold
static inline void pstate_set(proc_t *p, int newstate) {
    pstate_list_lock(newstate);
    pstate_list_offer(newstate, p);
    pstate_list_unlock(newstate);

    p->__state = newstate;
}


// p->lock held
// This general API is used to migrate state of process
void pstate_migrate(proc_t *p, int newstate) {
    int oldstate = p->state;
    if (oldstate == newstate) return;
    assert(pstatelist[newstate]);

    if (p->state != RUNNING) {
        pstate_list_lock(oldstate);
        pstate_list_delete(oldstate, p);
        pstate_list_unlock(oldstate);
    }

    pstate_set(p, newstate);
}


static void inline set_current(proc_t *p) {
    write_csr(sscratch, p);
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run.
//  - swtch to start running that process.
//  - eventually that process transfers control
//    via swtch back to the scheduler.
extern void swtch(struct context *old, struct context *new);
void scheduler(void) {
    struct proc *p;
    struct cpu *c = mycpu();

    tq_t *rq = &runnable_queue;

    c->proc = 0;
    for (;;) {
        // Avoid deadlock by ensuring that devices can interrupt.
        intr_on();

        // Poll a runnable process to run
        tq_lock(rq);
        if ((p = tq_poll(rq)) == NULL) {
            tq_unlock(rq);
            continue;
        }
        acquire(&p->lock);
        tq_unlock(rq);

        p->__state = RUNNING;
        c->proc = p;
        set_current(p);
        switchuvm(p->mm);
        swtch(&c->context, &p->context);
        switchkvm();
        set_current(NULL);
        c->proc = 0;
        release(&p->lock);
    }
}


// Switch to scheduler.  Must hold only p->lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->noff, but that would
// break in the few places where a lock is held but
// there's no process.
void sched(void) {
    int intena;
    struct proc *p = myproc();
    struct cpu *cpu = mycpu();

    if (!holding(&p->lock))
        panic("sched p->lock");
    if (mycpu()->noff != 1)
        panic("sched locks");
    if (p->state == RUNNING)
        panic("sched running");
    if (intr_get())
        panic("sched interruptible");

    intena = cpu->intena;

    swtch(&p->context, &mycpu()->context);
    mycpu()->intena = intena;
}


// Give up the CPU for one scheduling round.
void yield(void) {
    struct proc *p = myproc();
    acquire(&p->lock);
    pstate_migrate(p, RUNNABLE);
    sched();
    release(&p->lock);
}


// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
/**
 * 可能有时候不需要再sleep前释放掉某个spinlock，如lock_page，
 * 所以新增了:
 * if(lk != NULL)
 */
void sleep(void *chan, struct spinlock *lk) {
    struct proc *p = myproc();

    // sig_handle(p->signal);
    // Must acquire p->lock in order to
    // change p->state and then call sched.
    // Once we hold p->lock, we can be
    // guaranteed that we won't miss any wakeup
    // (wakeup locks p->lock),
    // so it's okay to release lk.
    if (lk != &p->lock) {
        acquire(&p->lock);

        if (lk != NULL)
            release(lk);
        else {
            page_t *page = (page_t *)chan;
            page_spin_unlock(page);
        }
    }

    p->chan = chan;
    pstate_migrate(p, SLEEPING);

    sched();

    // Tidy up.
    p->chan = 0;

    // Reacquire original lock.
    release(&p->lock);

    // sig_handle(p->signal);

    if (lk != NULL)
        acquire(lk);
    else {
        page_t *page = (page_t *)chan;
        page_spin_lock(page);
    }
}


// Wake up all processes sleeping on chan.
// Must be called without any p->lock.
int wakeup(void *chan) {
    struct proc *me = myproc();
    struct proc *p, *tmp;
    int ans = 0;

    pstate_list_lock(SLEEPING);
    list_for_each_entry_safe(p, tmp, &sleep_queue.head, state_head) {
        if (p != me) {
            int hold = holding(&p->lock);

            if (!hold) acquire(&p->lock);
            if (p->chan == chan) {
                pstate_list_delete(SLEEPING, p);
                pstate_set(p, RUNNABLE);
                ans++;
            }
            if (!hold) release(&p->lock);
        }
    }
    pstate_list_unlock(SLEEPING);

    return ans;
}


void wake_up_process(proc_t *p) {
    // if(p->state != SLEEPING)
    //   ER();
    acquire(&p->lock);
    pstate_migrate(p, RUNNABLE);
    release(&p->lock);
}
