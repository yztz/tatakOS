#ifndef _H_PROC_
#define _H_PROC_

#include "atomic/spinlock.h"
#include "platform.h"
#include "mm/mmap.h"
#include "mm/trapframe.h"
#include "fdtable.h"
#include "signal.h"
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


// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context context;     // swtch() here to enter scheduler().
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};

#define CSIGNAL       0x000000ff /* Signal mask to be sent at exit.  */
#define CLONE_VM      0x00000100 /* Set if VM shared between processes.  */
#define CLONE_FS      0x00000200 /* Set if fs info shared between processes.  */
#define CLONE_FILES   0x00000400 /* Set if open files shared between processes.  */
#define CLONE_SIGHAND 0x00000800 /* Set if signal handlers shared.  */
#define CLONE_PIDFD   0x00001000 /* Set if a pidfd should be placed
				     in parent.  */
#define CLONE_PTRACE  0x00002000 /* Set if tracing continues on the child.  */
#define CLONE_VFORK   0x00004000 /* Set if the parent wants the child to
				     wake it up on mm_release.  */
#define CLONE_PARENT  0x00008000 /* Set if we want to have the same
				     parent as the cloner.  */
#define CLONE_THREAD  0x00010000 /* Set to add to same thread group.  */
#define CLONE_NEWNS   0x00020000 /* Set to create new namespace.  */
#define CLONE_SYSVSEM 0x00040000 /* Set to shared SVID SEM_UNDO semantics.  */
#define CLONE_SETTLS  0x00080000 /* Set TLS info.  */
#define CLONE_PARENT_SETTID 0x00100000 /* Store TID in userlevel buffer
					   before MM copy.  */
#define CLONE_CHILD_CLEARTID 0x00200000 /* Register exit futex and memory
					    location to clear.  */
#define CLONE_DETACHED 0x00400000 /* Create clone detached.  */
#define CLONE_UNTRACED 0x00800000 /* Set if the tracing process can't
				      force CLONE_PTRACE on this clone.  */
#define CLONE_CHILD_SETTID 0x01000000 /* Store TID in userlevel buffer in
					  the child.  */
#define CLONE_NEWCGROUP    0x02000000	/* New cgroup namespace.  */
#define CLONE_NEWUTS	0x04000000	/* New utsname group.  */
#define CLONE_NEWIPC	0x08000000	/* New ipcs.  */
#define CLONE_NEWUSER	0x10000000	/* New user namespace.  */
#define CLONE_NEWPID	0x20000000	/* New pid namespace.  */
#define CLONE_NEWNET	0x40000000	/* New network namespace.  */
#define CLONE_IO	0x80000000	/* Clone I/O context.  */


extern struct cpu cpus[NUM_CORES];


enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct fat_entry;
// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  void *chan;                  // If non-zero, sleeping on chan
  void *futex_chan;             
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

  struct context context;      // swtch() here to run process
  
  fdtable_t *fdtable;
  signal_t *signal;
  tg_t *tg;

  sigset_t sig_pending;
  sigset_t sig_mask;
  int signaling;

  list_head_t head; // for thread group

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

void            exit(int);
int             do_clone(proc_t *p, uint64_t stack, int flags, uint64_t ptid, uint64_t tls, uint64_t ctid);
uint64          growproc(uint64_t n);
void            proc_mapstacks();
int             kill(int);
struct cpu*     mycpu(void);
struct cpu*     getmycpu(void);
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep(void*, struct spinlock*);
void            userinit(void);
int             waitpid(int cid, uint64 addr, int options);
void            wakeup(void*);
void            yield(void);
void            procdump(void);
void            proc_setmm(proc_t *p, mm_t *mm);
void            proc_setsig(proc_t *p, signal_t *sig);
void            proc_settg(proc_t *p, tg_t *tg);
void            proc_setfdtbl(proc_t *p, fdtable_t *fdtbl);
void            proc_switchmm(proc_t *p, mm_t *newmm);
tf_t           *proc_get_tf(proc_t *p);
int             kthread_create(char *name, void (*entry)());
void            freeproc(struct proc *p);
int             get_proc_cnt();
void            wake_up_process(proc_t *p);
void            sig_send(proc_t *p, int signum);
int             freechild();

#endif