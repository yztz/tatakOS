#include "kernel/sched.h"
#include "mm/vm.h"

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


// p->lock held
static inline void pstate_set(proc_t *p, int newstate) {
    pstate_list_lock(newstate);
    pstate_list_offer(newstate, p);
    pstate_list_unlock(newstate);

    p->__state = newstate;
}

// p->lock held
void pstate_migrate(proc_t *p, int newstate) {
    int oldstate = p->state;
    if (oldstate == newstate) return;
    assert(pstatelist[newstate]);

    // we have not set queue for running process
    if (p->state != RUNNING) {
        pstate_list_lock(oldstate);
        pstate_list_delete(oldstate, p);
        pstate_list_unlock(oldstate);
    }

    pstate_set(p, newstate);
}


/**
 * @brief Context switch
 *        Save current registers in old. Load from new.	
 */
extern void swtch(struct context *old, struct context *new);

void scheduler(void) {
    struct proc *p;
    struct cpu *c = mycpu();

    tq_t *rq = &runnable_queue;

    c->proc = NULL;
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
        swtch(&c->scheduler, &p->context);
        switchkvm();

        c->proc = NULL;
        set_current(NULL);

        release(&p->lock);
    }
}

void sched(void) {
    int intena;
    struct proc *p = current;

    if (!holding(&p->lock))
        panic("sched p->lock");

    struct cpu *cpu = mycpu();

    if (cpu->noff != 1)
        panic("sched locks");
    if (p->state == RUNNING)
        panic("sched running");
    if (intr_get())
        panic("sched interruptible");
        
    // Saves and restores intena 
    // because intena is a property of this kernel thread, not this CPU. 
    // It should be proc->intena and proc->noff, but that would
    // break in the few places where a lock is held but there's no process.
    intena = cpu->intena;

    swtch(&p->context, &cpu->scheduler);

    /* !IMPORTANT 
        call mycpu() again.
        CPU may have changed */
    mycpu()->intena = intena;
}

void yield() {
    struct proc *p = myproc();
    acquire(&p->lock);
    pstate_migrate(p, RUNNABLE);
    sched();
    release(&p->lock);
}


// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void sleep(void *chan, struct spinlock *lk) {
    proc_t *p = current;

    // sig_handle(p->signal);
    // Must acquire p->lock in order to
    // change p->state and then call sched.
    // Once we hold p->lock, we can be
    // guaranteed that we won't miss any wakeup
    // (wakeup locks p->lock),
    // so it's okay to release lk.
    if (lk != &p->lock) {
        acquire(&p->lock);
        // change state before release the lock
        // or else it may cause race condition
        pstate_migrate(p, SLEEPING);

        if (lk != NULL)
            release(lk);
        else {
            page_t *page = (page_t *)chan;
            page_spin_unlock(page);
        }
    } else {
        pstate_migrate(p, SLEEPING);
    }

    p->chan = chan;

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
        assert(p != me);
        int hold = holding(&p->lock);

        if (!hold) acquire(&p->lock);
        if (p->chan == chan) {
            pstate_list_delete(SLEEPING, p);
            pstate_set(p, RUNNABLE);
            ans++;
        }
        if (!hold) release(&p->lock);
    }
    pstate_list_unlock(SLEEPING);

    return ans;
}


void wake_up_process_locked(proc_t *p) {
    if (p->state == SLEEPING) {
        pstate_migrate(p, RUNNABLE);
    }
}

void wake_up_process(proc_t *p) {
    acquire(&p->lock);
    wake_up_process_locked(p);
    release(&p->lock);
}
