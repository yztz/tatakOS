#ifndef _H_PROC_
#define _H_PROC_

#include "atomic/spinlock.h"
#include "platform.h"
#include "mm/mmap.h"

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



extern struct cpu cpus[NUM_CORES];


enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };


#define PROC_KSTACK(proc) ((proc)->memlayout.kstack->addr)
#define PROC_TRAPFRAME(proc) ((struct trapframe *)((proc)->memlayout.trapframe->addr))
#define PROC_VMA_HEAD(proc) ((proc)->memlayout.vma_head)

struct fat_entry;
// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID

  // wait_lock must be held when using this:
  struct proc *parent;         // Parent process

  // these are private to the process, so p->lock need not be held.
  // uint64 kstack;               // Virtual address of kernel stack
  // uint64 sz;                   // Size of process memory (bytes)
  mm_t *mm;

  // struct trapframe *trapframe; // data page for trampoline.S
  struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files
  struct file **ext_ofile;
  int nfd;
  struct fat_entry *cwd;           // Current directory
  // struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  uint64 ktrap_fp;
  // todo: mmap
  // struct vma vma[VMA_NUM];
  uint64 cur_mmap_sz;
};

typedef struct proc proc_t;


void            exit(int);
int             do_clone(uint64_t stack);
uint64          growproc(int);
void            proc_mapstacks();
pagetable_t     proc_pagetable(struct proc *);
void            proc_freepagetable(pagetable_t, uint64);
int             kill(int);
struct cpu*     mycpu(void);
struct cpu*     getmycpu(void);
struct proc*    myproc();
void            procinit(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            sleep(void*, struct spinlock*);
void            userinit(void);
int             waitpid(int cid, uint64 addr);
void            wakeup(void*);
void            yield(void);
void            procdump(void);
void            proc_setmm(proc_t *p, mm_t *newmm);



#endif