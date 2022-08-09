#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "utils.h"
#include "defs.h"
#include "mm/vm.h"
#include "mm/mm.h"


#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "fs/file.h"

#define __MODULE_NAME__ PROC
#include "debug.h"

struct cpu cpus[NUM_CORES];

struct proc proc[NPROC];

struct proc *initproc;

atomic_t proc_cnt = INIT_ATOMIC;

int nextpid = 0;
int nexttid = 1;
struct spinlock pid_lock;
struct spinlock tid_lock;

extern void forkret(void);


#define IS_MAIN_THREAD(p) ((p)->tg->master_pid==p->pid)

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
struct spinlock wait_lock;


int get_proc_cnt() {
  return atomic_get(&proc_cnt);
}


// initialize the proc table at boot time.
void
procinit(void)
{
  struct proc *p;
  nextpid = 0;
  initlock(&pid_lock, "nextpid");
  initlock(&tid_lock, "tid");
  initlock(&wait_lock, "wait_lock");
  for(p = proc; p < &proc[NPROC]; p++) {
      initlock(&p->lock, "proc");
      p->state = UNUSED;
      p->mm = NULL;
      p->kstack = 0;
      p->trapframe = NULL;
  }
  for(int i = 0; i < NUM_CORES; i++) {
    cpus[i].intena = 0;
    cpus[i].noff = 0;
    cpus[i].proc = NULL;
  }
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
struct cpu*
mycpu(void) {
  int id = cpuid();
  struct cpu *c = &cpus[id];
  return c;
}

// Return the current struct proc *, or zero if none.
struct proc*
myproc(void) {
  push_off();
  struct cpu *c = mycpu();
  struct proc *p = c->proc;
  pop_off();
  return p;
}

int allocpid() {
  int pid;
  
  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  return pid;
}

// int alloctid() {
//   int tid;
//   acquire(&tid_lock);
//   tid = nexttid;
//   nexttid = nexttid + 1;
//   release(&tid_lock);

//   return tid;
// }

proc_t *proc_new(int is_kthread) {
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->state == UNUSED) {
      goto found;
    } else {
      release(&p->lock);
    }
  }
  return 0;

found:
  // printf("kill is %d and kill addr is %#lx\n", p->killed, &p->killed);
  p->pid = allocpid();
  // p->tid = alloctid();
  p->killed = 0;
  p->sig_pending = 0;
  p->sig_mask = 0;
  p->signaling = 0;
  p->set_tid_addr = 0;
  p->clear_tid_addr = 0;
  p->state = USED;
  INIT_LIST_HEAD(&p->head);

  if((p->kstack = (uint64)kmalloc(KSTACK_SZ)) == 0) {
    debug("kstack alloc failure");
    goto bad;
  }

  if(!is_kthread) {
    if((p->trapframe = tf_new()) == 0) {
      debug("trapframe alloc failure");
      goto bad;
    }
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + KSTACK_SZ;

  atomic_inc(&proc_cnt);
  
  return p;


bad:
  freeproc(p);
  release(&p->lock);
  return NULL;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
void freeproc(struct proc *p) {
  fdtbl_free(&p->fdtable);
  mmap_free(&p->mm);
  sig_free(&p->signal);
  tg_free(&p->tg);
  tf_free(&p->trapframe);

  if(p->kstack) kfree_safe(&p->kstack);
  if(p->exe) eput(p->exe);


  // memset(p->ofile, 0, NOFILE * sizeof(struct file *));
  p->exe = NULL;
  p->pid = 0;
  p->parent = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->futex_chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = UNUSED;

  atomic_dec(&proc_cnt);
}

// a user program that calls exec("/init")
// od -t xC initcode
uchar initcode[] = {
  #include "generated/initcode_bin.h"
};

#include "fs/file.h"
static void init_std(proc_t *p) {
  struct file *stdin = filealloc();
  struct file *stdout = filealloc();

  fdtbl_setfile(p->fdtable, 0, stdin, 0);
  fdtbl_setfile(p->fdtable, 1, stdout, 0);
  fdtbl_setfile(p->fdtable, 2, stdout, 0);

  stdin->type = FD_DEVICE;
  stdout->type = FD_DEVICE;
  stdin->readable = 1;
  stdin->writable = 0;
  stdout->readable = 0;
  stdout->writable = 1;
  stdin->dev = &devs[CONSOLE];
  stdout->dev = &devs[CONSOLE];
}

// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  mm_t *mm;
  fdtable_t *fdtable;
  signal_t *sig;
  tg_t *tg;

  p = proc_new(0);
  initproc = p;

  if(p == NULL) {
    panic("alloc");
  }

  if((mm = mmap_new()) == NULL || (fdtable = fdtbl_new()) == NULL || 
    (sig = sig_new()) == NULL || (tg = tg_new(p)) == NULL) 
    panic("alloc");
  
  proc_switchmm(p, mm);
  proc_setfdtbl(p, fdtable);
  proc_setsig(p, sig);
  proc_settg(p, tg);

  const int USER_SIZE = 2 * PGSIZE;
  // debug("initcode size: %d", sizeof(initcode));

  if(sizeof(initcode) >= USER_SIZE)
    panic("inituvm: more than a page");
  
  // debug("initcode size: %d", sizeof(initcode));

  if(do_mmap_alloc(p->mm, PGSIZE, PGSIZE + USER_SIZE, 0, PROT_WRITE|PROT_READ|PROT_EXEC|PROT_USER) == -1) {
    panic("mmap1 failure");
  }

  if(mmap_map_stack(p->mm, USTACKSIZE) == -1) {
    panic("mmap2 failure");
  }

  enable_sum();
  memmove((void *)PGSIZE, initcode, sizeof(initcode));
  disable_sum();

  // prepare for the very first "return" from kernel to user.
  proc_get_tf(p)->epc = PGSIZE;      // user program counter
  proc_get_tf(p)->sp = USERSPACE_END;  // user stack pointer

  safestrcpy(p->name, "initcode", sizeof(p->name));

  init_std(p);

  p->state = RUNNABLE;

  release(&p->lock);
}

uint64 growproc(uint64_t newbreak) {
  struct proc *p = myproc();
  if(newbreak == 0) {
    return PROGRAM_BREAK(p->mm);
  }
  if(mmap_ext_heap(p->mm, newbreak) == -1) {
    return -1;
  }
  
  return PROGRAM_BREAK(p->mm);
}


int kthread_create(char *name, void (*entry)()) {
  if(initproc == NULL)
    panic("initproc required");

  struct proc *np = proc_new(1);

  if(np == NULL)
    return -1;

  proc_setmm(np, initproc->mm);
  np->context.ra = (uint64_t)entry;

  strncpy(np->name, name, 20);

  np->state = RUNNABLE;

  release(&np->lock);

  return 0;
}

int do_clone(proc_t *p, uint64_t stack, int flags, uint64_t ptid, uint64_t tls, uint64_t ctid) {
  int pid;
  struct proc *np;
  mm_t *newmm;
  fdtable_t *newfdtbl;
  signal_t *newsig;
  tg_t *newtg;

  // Allocate process.
  if((np = proc_new(0)) == NULL){
    return -1;
  }

  if((flags & CLONE_VM)) {
    newmm = p->mm;
  } else {
    if((newmm = mmap_clone(p->mm)) == NULL)
      goto bad;
  }

  if((flags & CLONE_FILES)) {
    newfdtbl = p->fdtable;
  } else {
    if((newfdtbl = fdtbl_clone(p->fdtable)) == NULL)
      goto bad;
  }

  if((flags & CLONE_THREAD)) {
    // debug("new thread statk is %#lx", stack);
    newtg = p->tg;
    tg_join(newtg, np);
  } else {
    if((newtg = tg_new(np)) == NULL)
      goto bad;
  }

  if((flags & CLONE_SIGHAND)) {
    newsig = p->signal;
  } else {
    if((newsig = sig_clone(p->signal)) == NULL)
      goto bad;
  }

  if((flags & CLONE_CHILD_CLEARTID)) {
    np->clear_tid_addr = ctid;
  }

  if((flags & CLONE_CHILD_SETTID)) {
    panic("not support now");
  }

  if((flags & CLONE_PARENT_SETTID)) {
    copy_to_user(ptid, &np->pid, sizeof(int));
  }

  proc_setmm(np, newmm);
  proc_setfdtbl(np, newfdtbl);
  proc_setsig(np, newsig);
  proc_settg(np, newtg);

  // 拷贝trapframe
  memcpy(proc_get_tf(np), proc_get_tf(p), sizeof(tf_t));

  if((flags & CLONE_SETTLS)) {
    proc_get_tf(np)->tp = tls;
  }

  // 将返回值置为0
  proc_get_tf(np)->a0 = 0;

  // 如果指定了栈，那么重设sp
  if(stack) {
    proc_get_tf(np)->sp = stack;
  }

  
  np->cwd = edup(p->cwd);
  if(np->exe)
    np->exe = edup(p->exe);

  safestrcpy(np->name, p->name, sizeof(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);


  // np->cur_mmap_sz = p->cur_mmap_sz;
  return pid;


 bad:
  freeproc(np);
  release(&np->lock);
  return -1;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
void
reparent(struct proc *p)
{
  struct proc *pp;

  for(pp = proc; pp < &proc[NPROC]; pp++){
    if(pp->parent == p){
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

extern void buddy_print_free();
#include "kernel/futex.h"

void exit(int status) {
  proc_t *p = myproc();
  // tg_t *thrdgrp = p->tg;
  // proc_t *mp = tg_main_thrd(thrdgrp);
  int thrdcnt = tg_quit(p->tg);

  if(p == initproc) {
    panic("init exiting");
  }

  debug("PID %d EXIT", p->pid);

  if(thrdcnt == 0) {
    fdtbl_closeall(p->fdtable);
  }

  eput(p->cwd);
  p->cwd = 0;

  /* free the mm field of the process 
  释放页表和结构体，在waitpid中释放*/
  // exit_mm(p);

  acquire(&wait_lock);

  // Give any children to init.
  reparent(p);

  // Parent might be sleeping in wait().
  if(thrdcnt == 0) {
    wakeup(p->parent);
  }
  
  acquire(&p->lock);

  p->xstate = status;
  p->state = ZOMBIE;

  release(&wait_lock);

  if(p->clear_tid_addr) {
    memset_user(p->clear_tid_addr, 0, sizeof(int));
    acquire(&p->tg->lock);
    futex_wake((void *)p->clear_tid_addr, 1);
    release(&p->tg->lock);
  }

  // Jump into the scheduler, never to return.
  sched();
  panic("zombie exit");
}

    
extern void lru_add_drain();
// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
waitpid(int cid, uint64 addr, int options)
{
  struct proc *np;
  int havekids, pid, haveckid;
  struct proc *p = myproc();

  acquire(&wait_lock);

  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    haveckid = 0;
    for(np = proc; np < &proc[NPROC]; np++){
      if(np->parent == p){
        // make sure the child isn't still in exit() or swtch().
        acquire(&np->lock);

        havekids = 1;

        pid = np->pid;
        if(cid != -1 && cid != pid) continue;

        haveckid = 1;
        if(np->state == ZOMBIE){
          // Found one.
         
          /* 进程退出的时候，有些页还在pagevec中，没有释放，看上去好像内存泄露了，所以这里加上这句。 */
          lru_add_drain();
          if(addr != 0 && copyout(addr, (char *)&np->xstate,
                                  sizeof(np->xstate)) < 0) {
            release(&np->lock);
            release(&wait_lock);
            return -1;
          }
          freeproc(np);
          release(&np->lock);
          release(&wait_lock);
          return pid;
        }
        release(&np->lock);
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || !haveckid || p->killed){
      release(&wait_lock);
      return -1;
    }
    
    // Wait for a child to exit.
    sleep(p, &wait_lock);  //DOC: wait-sleep
  }
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run.
//  - swtch to start running that process.
//  - eventually that process transfers control
//    via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  
  c->proc = 0;
  for(;;){
    // Avoid deadlock by ensuring that devices can interrupt.
    intr_on();
    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state == RUNNABLE) {
        // Switch to chosen process.  It is the process's job
        // to release its lock and then reacquire it
        // before jumping back to us.
        p->state = RUNNING;
        c->proc = p;
        switchuvm(p->mm);
        swtch(&c->context, &p->context);
        switchkvm();
        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;
      }
      release(&p->lock);
    }
  }
}

void proc_setmm(proc_t *p, mm_t *newmm) {
  mmap_ref(newmm);
  p->mm = newmm;
}

void proc_setsig(proc_t *p, signal_t *newsig) {
  sig_ref(newsig);
  p->signal = newsig;
}

void proc_settg(proc_t *p, tg_t *tg) {
  tg_ref(tg);
  p->tg = tg;
}

void proc_setfdtbl(proc_t *p, fdtable_t *fdtbl) {
  fdtbl_ref(fdtbl);
  p->fdtable = fdtbl;
}

tf_t *proc_get_tf(proc_t *p) {
  return p->trapframe;
}


// Switch to scheduler.  Must hold only p->lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->noff, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&p->lock))
    panic("sched p->lock");
  if(mycpu()->noff != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;

  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  struct proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

#include "fs/fat.h"
// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
void
forkret(void)
{
  static int first = 1;
  proc_t *p = myproc();
  // Still holding p->lock from scheduler.
  release(&p->lock);

  if (first) {
    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    first = 0;

    extern fat32_t *fat;
    fat_mount(ROOTDEV, &fat);
    p->cwd = namee(NULL, "/");
    // init dir...
    entry_t *tmp = create(fat->root, "/tmp", T_DIR);
    if(tmp)
      eunlockput(tmp);
  }

  usertrapret();
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
/**
 * 可能有时候不需要再sleep前释放掉某个spinlock，如lock_page，
 * 所以新增了:
 * if(lk != NULL)
 */
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  sig_handle(p->signal);
  // Must acquire p->lock in order to
  // change p->state and then call sched.
  // Once we hold p->lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup locks p->lock),
  // so it's okay to release lk.
  if(lk != &p->lock){
    acquire(&p->lock);

    if(lk != NULL)
      release(lk);
    else{
      page_t *page = (page_t *)chan;
      page_spin_unlock(page);
    }
  }

  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  release(&p->lock);

  if(lk != NULL)
    acquire(lk);
  else{
    page_t *page = (page_t *)chan;
    page_spin_lock(page);
  }
}

// Wake up all processes sleeping on chan.
// Must be called without any p->lock.
void
wakeup(void *chan)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    if(p != myproc()){ 
      acquire(&p->lock);
      if(p->state == SLEEPING && p->chan == chan) {
        p->state = RUNNABLE;
      }
      release(&p->lock);
    }
  }
}

void wake_up_process(proc_t *p) {
  if(p->state != SLEEPING)
    ER();
  acquire(&p->lock);
  p->state = RUNNABLE;
  release(&p->lock);
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
int
kill(int pid)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->pid == pid){
      p->killed = 1;
      if(p->state == SLEEPING){
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      return 0;
    }
    release(&p->lock);
  }
  return -1;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
int
either_copyout(int user_dst, uint64 dst, void *src, uint64 len)
{
  if(user_dst){
    return copyout(dst, src, len);
  } else {
    memmove((char *)dst, src, len);
    return 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
int
either_copyin(void *dst, int user_src, uint64 src, uint64 len)
{
  if(user_src){
    return copyin(dst, src, len);
  } else {
    memmove(dst, (char*)src, len);
    return 0;
  }
}


void proc_switchmm(proc_t *p, mm_t *newmm) {
  proc_setmm(p, newmm);
  switchuvm(p->mm);
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  struct proc *p;
  char *state;

  printf("\n");
  for(p = proc; p < &proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    printf("%d %s %s chan %#lx futex %#lx\n", p->pid, state, p->name, p->chan, p->futex_chan);
  }
}
