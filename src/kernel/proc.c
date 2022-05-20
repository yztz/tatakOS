#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "utils.h"
#include "defs.h"
#include "mm/vm.h"

#include "debug.h"
#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "fs/file.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

struct cpu cpus[NUM_CORES];

struct proc proc[NPROC];

struct proc *initproc;

int nextpid = 1;
struct spinlock pid_lock;

extern void forkret(void);
static void freeproc(struct proc *p);

extern char trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
struct spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
// void
// proc_mapstacks() {
//   struct proc *p;
  
//   for(p = proc; p < &proc[NPROC]; p++) {
//     char *pa = kalloc();
//     if(pa == 0)
//       panic("kalloc");
//     uint64 va = KSTACK((int) (p - proc));
//     kvmmap(va, (uint64)pa, PGSIZE, PTE_R | PTE_W, PGSPEC_NORMAL);
//   }
// }

// initialize the proc table at boot time.
void
procinit(void)
{
  struct proc *p;
  
  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  for(p = proc; p < &proc[NPROC]; p++) {
      initlock(&p->lock, "proc");
      p->state = UNUSED;
      // p->kstack = KSTACK((int) (p - proc));
      for(int i = 0; i < NOFILE; i++) {
        p->ofile[i] = NULL;
      }
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

int
allocpid() {
  int pid;
  
  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  return pid;
}

void
init_mm(struct proc *p){

  p->mm = kmalloc(sizeof(struct mm_struct));
  p->mm->mmap = NULL;
  p->mm->mmap_cache = NULL;
  p->mm->map_count = 0;
  initlock(&p->mm->lock, "mm lock");
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
static struct proc*
allocproc(void)
{
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
  p->pid = allocpid();
  p->state = USED;

  // 申请Trapframe
  if((p->trapframe = (struct trapframe *)kalloc()) == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }
  // 申请内核栈
  if((p->kstack = (uint64_t)kalloc()) == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }

  // 获取空页表（已经映射了内核地址空间，trapfram以及内核栈）
  p->pagetable = proc_pagetable(p);
  if(p->pagetable == 0){
    freeproc(p);
    release(&p->lock);
    return 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  p->cur_mmap_sz = MMAP_BASE;
  init_mm(p);

  return p;
}

// void
// free_vma(struct proc *p){
//   pte_t *pte;
//   struct vma *v;
//   int i;
//   uint64 a;

//   // for(;;);
//   for(i=0; i < VMA_NUM; i++){
//     v = &(p->vma[i]);
//     // v->state is cleared in sys_munmap
//     if(v->state == VMA_USED){
//       for(a = PGROUNDDOWN(v->addr); a <= PGROUNDDOWN(v->end); a+=PGSIZE){
//       if ((pte = walk(p->pagetable, a, 0)) == 0)
//         continue;
//       if ((*pte & PTE_V) == 0)//this include the case *pte == 0; or *pte not 0, but *pte &  PTE_V == 0
//         continue;

//       // printf(ylw("a: %p\n"), a);
//       //may have bug here!
//       if (v->flags & MAP_SHARED)
//       {
//         // printf(ylw("PGMASK & : %p\n"), (v->end - a));
//         // writee(v->map_file->ep, 1, a, v->off + (a - v->addr), min(PGSIZE, (v->end - a)));
//       }
//       // printf(ylw("free_vam a: %p\n"), a); 
//       uvmunmap(p->pagetable, a, 1, 1);

//       }
//     }
//   }
// }

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
static void
freeproc(struct proc *p)
{
  if(p->trapframe)
    kfree((void*)p->trapframe);
  if(p->kstack)
    kfree((void*)p->kstack);
  p->trapframe = 0;
  p->kstack = 0;

  // free_vma(p);

  // for(;;);
  if(p->pagetable){
    proc_freepagetable(p->pagetable, p->sz);
    p->pagetable = 0;
  } 
  p->sz = 0;
  p->pid = 0;
  p->parent = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = UNUSED;
}

// Create a user page table for a given process,
// with no user memory, but with trampoline pages.
pagetable_t
proc_pagetable(struct proc *p)
{
  pagetable_t pagetable;

  // An empty page table with kernel page table copy.
  pagetable = uvmcreate();
  if(pagetable == 0)
    return 0;
  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  // if(mappages(pagetable, TRAMPOLINE, PGSIZE,
  //             (uint64)trampoline, PTE_R | PTE_X) < 0){
  //   uvmfree(pagetable, 0);
  //   return 0;
  // }

  // map the trapframe just below TRAMPOLINE, for trampoline.S.
  if(mappages(pagetable, TRAPFRAME, PGSIZE,
              (uint64)(p->trapframe), PTE_R | PTE_W) < 0){
    erasekvm(pagetable);
    uvmfree(pagetable, 0);
    return 0;
  }

  if(mappages(pagetable, KSTACK, PGSIZE,
              (uint64)(p->kstack), PTE_R | PTE_W) < 0){
    erasekvm(pagetable);
    uvmunmap(pagetable, TRAPFRAME, 1, 0);
    uvmfree(pagetable, 0);
    return 0;
  }

  return pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
void
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmunmap(pagetable, KSTACK, 1, 0);
  erasekvm(pagetable);
  uvmfree(pagetable, sz);
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
  struct file *stderr = filealloc();
  
  p->ofile[0] = stdin;
  p->ofile[1] = stdout;
  p->ofile[2] = stderr;

  stdin->type = FD_DEVICE;
  stdout->type = FD_DEVICE;
  stderr->type = FD_DEVICE;
  stdin->readable = 1;
  stdin->writable = 0;
  stdout->readable = 0;
  stdout->writable = 1;
  stderr->readable = 0;
  stderr->writable = 1;
  stdin->dev = &devs[CONSOLE];
  stdout->dev = &devs[CONSOLE];
  stderr->dev = &devs[CONSOLE];
}

// Set up first user process.
void
userinit(void)
{
  struct proc *p;

  p = allocproc();
  initproc = p;

  // allocate one user page and copy init's instructions
  // and data into it.
  uvminit(p->pagetable, initcode, sizeof(initcode));
  p->sz = PGSIZE;

  // prepare for the very first "return" from kernel to user.
  p->trapframe->epc = 0;      // user program counter
  p->trapframe->sp = PGSIZE;  // user stack pointer

  safestrcpy(p->name, "initcode", sizeof(p->name));
    // p->cwd = namei("/");

  init_std(p);

  p->state = RUNNABLE;

  release(&p->lock);
}


int
growproc(int n)
{
  uint sz;
  struct proc *p = myproc();

  sz = p->sz;
  if(n > 0){
    if((sz = uvmalloc(p->pagetable, sz, sz + n)) == 0) {
      return -1;
    }
  } else if(n < 0){
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  return 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
int
do_clone(uint64_t stack)
{
  int i, pid;
  struct proc *np;
  struct proc *p = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // 拷贝内存布局(如果开启了COW，那么仅仅是复制页表)
  if(uvmcopy(p->pagetable, np->pagetable, p->sz) < 0){
    freeproc(np);
    release(&np->lock);
    return -1;
  }
  np->sz = p->sz;

  // 复制trapframe
  *(np->trapframe) = *(p->trapframe);

  // 将返回地址重置为0
  np->trapframe->a0 = 0;

  // 如果指定了栈，那么重设sp
  if(stack)
    np->trapframe->sp = stack;

  // 增加文件描述符引用
  for(i = 0; i < NOFILE; i++)
    if(p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = edup(p->cwd);

  safestrcpy(np->name, p->name, sizeof(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);


  np->cur_mmap_sz = p->cur_mmap_sz;
  return pid;
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

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
void
exit(int status)
{
  struct proc *p = myproc();

  if(p == initproc) {
    panic("init exiting");
  }

  if(status == -1) {
    panic("exception quit");
  }
  // Close all open files.
  for(int fd = 0; fd < NOFILE; fd++){
    if(p->ofile[fd]){
      struct file *f = p->ofile[fd];
      fileclose(f);
      p->ofile[fd] = 0;
    }
  }

  eput(p->cwd);
  p->cwd = 0;

  acquire(&wait_lock);

  // Give any children to init.
  reparent(p);

  // Parent might be sleeping in wait().
  wakeup(p->parent);
  
  acquire(&p->lock);

  p->xstate = status;
  p->state = ZOMBIE;

  release(&wait_lock);

  // Jump into the scheduler, never to return.
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
waitpid(int cid, uint64 addr)
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
          
          if(addr != 0 && copyout(p->pagetable, addr, (char *)&np->xstate,
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
        switchuvm(p);
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

  // Still holding p->lock from scheduler.
  release(&myproc()->lock);

  if (first) {
    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    first = 0;

    extern fat32_t *fat;
    printf("ready to mount\n");
    fat_mount(ROOTDEV, &fat);
    printf("mount done\n");
    myproc()->cwd = namee(NULL, "/");
    // LOOP();
    // fsinit(ROOTDEV);


  }

  usertrapret();
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  // Must acquire p->lock in order to
  // change p->state and then call sched.
  // Once we hold p->lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup locks p->lock),
  // so it's okay to release lk.

  if(lk != &p->lock){
    acquire(&p->lock);
    release(lk);
  }

  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  release(&p->lock);
  acquire(lk);
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
  struct proc *p = myproc();
  if(user_dst){
    return copyout(p->pagetable, dst, src, len);
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
  struct proc *p = myproc();
  if(user_src){
    return copyin(p->pagetable, dst, src, len);
  } else {
    memmove(dst, (char*)src, len);
    return 0;
  }
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
    printf("%d %s %s", p->pid, state, p->name);
    printf("\n");
  }
}
