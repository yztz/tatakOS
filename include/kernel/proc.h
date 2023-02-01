#ifndef _H_PROC_
#define _H_PROC_

#include "atomic/spinlock.h"
#include "platform.h"
#include "mm/mmap.h"
#include "mm/trapframe.h"
#include "fdtable.h"
#include "signal.h"
#include "kernel/thread_group.h"

/**
 * @brief Saved registers for kernel context switches.
 * 
 */
struct context {
    uint64 ra;
    uint64 sp;

    // callee-saved
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

struct pagevec;

/**
 * @brief Per-CPU state.
 * 
 */
struct cpu {
    /// @brief The process running on this cpu, or null.
    struct proc *proc;
    /// @brief swtch() here to enter scheduler().
    struct context context;
    /// @brief 
    struct pagevec *inactive_pvec;
    /// @brief 
    struct pagevec *active_pvec;
    /// @brief Depth of push_off() nesting.
    int noff;
    /// @brief Were interrupts enabled before push_off()?
    int intena;
};


extern struct cpu cpus[NUM_CORES];


enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE, MAXPSTATE };

struct fat_entry;
// Per-process state
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
    uint64 kstack;
    /// @brief U-mode before-trap state
    tf_t *trapframe;
    /// @brief S-mode before-trap state, which is stored in the kernel stack
    ktf_t *k_trapframe;
    /// @brief swtch() here to run process
    struct context context;
    /// @brief file open table
    fdtable_t *fdtable;
    /// @brief signal info
    signal_t *signal;
    /// @brief thread group info, we get 'pid' from here
    tg_t *tg;
    /// @brief pending signal
    sigset_t sig_pending;
    /// @brief ignored signal
    sigset_t sig_mask;
    /// @brief signal handling state
    int signaling;
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

typedef struct proc proc_t;
typedef void (*kthread_callback)(proc_t *);

void            exit(int);
int             do_clone(proc_t *p, uint64_t stack, int flags, uint64_t ptid, uint64_t tls, uint64_t ctid);
void            forkret(proc_t *p);
uint64          growproc(uint64_t n);
void            proc_mapstacks();
int             kill(int);
struct cpu *mycpu(void);
struct cpu *getmycpu(void);
/**
 * @deprecated use current instead
 * @return the current struct proc *, or zero if none.
*/
struct proc *myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
int             sched_timeout(int timeout);
void            sleep(void *, struct spinlock *);
void            sleep_deep(void *chan, struct spinlock *lk);
void            userinit(void);
int             waitpid(int cid, uint64 addr, int options);
int             wakeup(void *);
void            yield(void);
void            procdump(void);

void            proc_setmm(proc_t *p, mm_t *mm);
void            proc_setsig(proc_t *p, signal_t *sig);
void            proc_settg(proc_t *p, tg_t *tg);
void            proc_setfdtbl(proc_t *p, fdtable_t *fdtbl);
void            proc_settf(proc_t *p, tf_t *tf);

void            proc_switchmm(proc_t *p, mm_t *newmm);
tf_t *          proc_get_tf(proc_t *p);
int             kthread_create(char *name, void (*entry)());
void            freeproc(struct proc *p);
int             get_proc_cnt();
void            wake_up_process(proc_t *p);
void            sig_send(proc_t *p, int signum);
int             freechild();
void            pstate_migrate(proc_t *p, int newstate);

struct pagevec *my_inactive_pvec();
struct pagevec *my_active_pvec();


extern void usertrapret(void);


#endif
