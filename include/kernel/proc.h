#ifndef _H_PROC_
#define _H_PROC_

#include "types.h"
#include "atomic/spinlock.h"
#include "kernel/fdtable.h"
#include "list.h"
#include "kernel/cpu.h"

/// @brief process state
enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE, MAXPSTATE };

/// @brief process struct
struct proc {
    /// @brief protect struct field below
    struct spinlock lock;
    
    /* p->lock must be held when using these: */

    /// @brief process state
    union {
        const enum procstate state;        // Process state
        enum procstate __state;
    };

    /// @brief sleep chan, if non-zero, sleeping on chan
    void *chan;
    /// @brief futex chan
    void *futex_chan;
    /// @brief which waitqueue the proc is sleeping in
    wq_t *wait_channel;
    /// @brief process kill state, if non-zero, have been killed
    int killed;
    /// @brief exit status to be returned to parent's wait. Ref: WEXITSTATUS
    int xstate;
    /// @brief process ID (tid in fact)
    int pid;

    /// @brief parent process, wait_lock must be held when using this
    struct proc *parent;

    /* these are private to the process, so p->lock need not be held. */

    /// @brief memory map
    mm_t *mm;
    /// @brief kernel stack address
    uint64_t kstack;
    /// @brief U-mode before-trap state
    utf_t *trapframe;
    /// @brief S-mode before-trap state, which is stored in the kernel stack
    ktf_t *k_trapframe;
    /// @brief swtch() here to run process
    struct context context;
    /// @brief file open table
    fdtable_t *fdtable;
    /// @brief thread group info, we get 'pid' from here
    tg_t *tg;
    /// @brief for state queue linklist
    list_head_t state_head;
    /// @brief for thread group linklist
    list_head_t thrd_head;
    /// @brief the addr to copy tid
    uint64_t set_tid_addr;
    /// @brief the addr to clear
    uint64_t clear_tid_addr;
    /// @brief current working directory
    struct fat_entry *cwd;
    /// @brief excutable file
    struct fat_entry *exe;
    /// @brief used to help do timing
    uint64_t stub_time;
    /// @brief time cost in U-mode 
    uint64_t u_time;
    /// @brief time cost in S-mode 
    uint64_t s_time;
    /// @brief process name
    char name[64];
};

void       exit(int);

uint64_t   growproc(uint64_t n);

int        wakeup(void *);

void       proc_setmm(proc_t *p, mm_t *mm);
void       proc_settg(proc_t *p, tg_t *tg);
void       proc_setfdtbl(proc_t *p, fdtable_t *fdtbl);
void       proc_settf(proc_t *p, utf_t *tf);

proc_t *   proc_new(kthread_callback_t);
void       proc_switchmm(proc_t *p, mm_t *newmm);
utf_t *    proc_get_tf(proc_t *p);
proc_t*    kthread_create(char *name, kthread_callback_t);
void       freeproc(struct proc *p);
int        get_proc_cnt();
void       wake_up_process(proc_t *p);
void       wake_up_process_locked(proc_t *p);
void       sig_send(proc_t *p, int signum);
int        freechild();


/**
 * @brief sleep
 * @deprecated Use waitqueue instead.
 *             It's just like shit and MUST be refactored!!!
 * 
 * @param chan the chan the proc is sleeping on
 * @param lock lock to release before schedule
 */
void sleep(void *chan, struct spinlock *lock);

/**
 * @brief Wait for children 
 * 
 * @param cid child pid. -1 means any children.
 * @param addr buffer to save exit code
 * @param options some other options, not used now
 * @return int child pid. -1 means no children.
 */
int waitpid(int cid, uint64_t addr, int options);

/**
 * @brief Get current process context
 * @deprecated use current instead
 * @return the current struct proc *, or zero if none.
*/
struct proc *myproc();

/**
 * @brief  Migrate state of process. 
 * @pre p->lock must be held.
 * 
 * @param p
 * @param newstate 
 */
void pstate_migrate(proc_t *p, int newstate);

/**
 * @brief print all processes info
 * 
 */
void procdump(void);


extern void usertrapret(void);


#endif
