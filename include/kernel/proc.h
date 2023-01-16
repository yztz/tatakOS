#ifndef _H_PROC_
#define _H_PROC_

#include "atomic/spinlock.h"
#include "platform.h"
#include "mm/mmap.h"
#include "mm/trapframe.h"
#include "fdtable.h"
#include "signal.h"
#include "riscv.h"
#include "kernel/thread_group.h"

// Saved registers for kernel context switches.
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

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context context;     // swtch() here to enter scheduler().
  struct pagevec *inactive_pvec;
  struct pagevec *active_pvec;
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};


extern struct cpu cpus[NUM_CORES];


enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE, MAXPSTATE };

struct fat_entry;
// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:

  // readonly
  union {
    const enum procstate state;        // Process state
    enum procstate __state;
  };
  
  void *chan;                  // If non-zero, sleeping on chan
  void *futex_chan;             
  wq_t *wait_channel;
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID
  // int tid;

  // int fuext_waiting;

  // wait_lock must be held when using this:
  struct proc *parent;         // Parent process

  // these are private to the process, so p->lock need not be held.
  mm_t *mm;
  uint64 kstack;
  tf_t *trapframe;
  ktf_t *ktf;

  struct context context;      // swtch() here to run process
  
  fdtable_t *fdtable;
  signal_t *signal;
  tg_t *tg;

  sigset_t sig_pending;
  sigset_t sig_mask;
  int signaling;

  list_head_t state_head; // for state
  list_head_t thrd_head; // for thread group

  uint64_t set_tid_addr;
  uint64_t clear_tid_addr;

  struct fat_entry *cwd;           // Current directory
  struct fat_entry *exe;           // Excutable

  uint64_t stub_time;
  uint64_t u_time;
  uint64_t s_time;

  char name[20];               // Process name (debugging)

  uint64 ktrap_fp;
};

typedef struct proc proc_t;
typedef void (*kthread_callback)(proc_t *);

void            exit(int);
int             do_clone(proc_t *p, uint64_t stack, int flags, uint64_t ptid, uint64_t tls, uint64_t ctid);
void            forkret(proc_t *p);
uint64          growproc(uint64_t n);
void            proc_mapstacks();
int             kill(int);
struct cpu*     mycpu(void);
struct cpu*     getmycpu(void);
/**
 * @deprecated use current instead
 * @return the current struct proc *, or zero if none.
*/
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
int             sched_timeout(int timeout);
void            sleep(void*, struct spinlock*);
void            sleep_deep(void *chan, struct spinlock *lk);
void            userinit(void);
int             waitpid(int cid, uint64 addr, int options);
int             wakeup(void*);
void            yield(void);
void            procdump(void);

void            proc_setmm(proc_t *p, mm_t *mm);
void            proc_setsig(proc_t *p, signal_t *sig);
void            proc_settg(proc_t *p, tg_t *tg);
void            proc_setfdtbl(proc_t *p, fdtable_t *fdtbl);
void            proc_settf(proc_t *p, tf_t *tf);

void            proc_switchmm(proc_t *p, mm_t *newmm);
tf_t           *proc_get_tf(proc_t *p);
int             kthread_create(char *name, void (*entry)());
void            freeproc(struct proc *p);
int             get_proc_cnt();
void            wake_up_process(proc_t *p);
void            sig_send(proc_t *p, int signum);
int             freechild();
void            pstate_migrate(proc_t *p, int newstate);

struct pagevec* my_inactive_pvec();
struct pagevec* my_active_pvec();


extern void usertrapret(void);


static inline proc_t *get_current() {
    return (proc_t *)read_csr(sscratch);
}

#define current (get_current())

#endif