#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "utils.h"
#include "defs.h"
#include "mm/vm.h"


#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "fs/file.h"

#define __MODULE_NAME__ PROC
#include "debug.h"

struct cpu cpus[NUM_CORES];

struct proc proc[NPROC];

struct proc *initproc;

int nextpid = 0;
struct spinlock pid_lock;

extern void forkret(void);
static void freeproc(struct proc *p);


// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
struct spinlock wait_lock;


// initialize the proc table at boot time.
void
procinit(void)
{
  struct proc *p;
  nextpid = 0;
  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  for(p = proc; p < &proc[NPROC]; p++) {
      initlock(&p->lock, "proc");
      p->state = UNUSED;
      p->mm = NULL;
      p->kstack = 0;
      p->trapframe = NULL;
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
  p->nfd = NOFILE;
  p->ext_ofile = NULL;
  p->mm = (mm_t *)kzalloc(sizeof(mm_t));
  p->fd_flags = 0;

  if((p->kstack = (uint64)kmalloc(KSTACK_SZ)) == 0) {
    debug("kstack alloc failure");
    goto bad;
  }

  if((p->trapframe = kalloc()) == 0) {
    debug("trapframe alloc failure");
    goto bad;
  }

  if(mmap_init(p->mm) == -1){
    debug("mmap_init failure");
    goto bad;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + KSTACK_SZ;

  // p->cur_mmap_sz = MMAP_BASE;
  return p;


bad:
  freeproc(p);
  release(&p->lock);
  return NULL;
}


// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
static void
freeproc(struct proc *p)
{
  mmap_free(&p->mm);
  if(p->kstack) kfree_safe(&p->kstack);
  if(p->trapframe) kfree_safe(&p->trapframe);
  if(p->ext_ofile) kfree_safe(&p->ext_ofile);
  
  memset(p->ofile, 0, NOFILE * sizeof(struct file *));
  p->pid = 0;
  p->parent = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = UNUSED;
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

  if(sizeof(initcode) >= PGSIZE)
    panic("inituvm: more than a page");

  switchuvm(p->mm);

  if(do_mmap_alloc(p->mm, PGSIZE, PGSIZE, 0, PROT_WRITE|PROT_READ|PROT_EXEC|PROT_USER) == -1) {
    panic("mmap1 failure");
  }

  if(mmap_map_stack_heap(p->mm, 2 * PGSIZE, USTACKSIZE, UHEAPSIZE) == -1) {
    panic("mmap2 failure");
  }

  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  enable_sum();
  #endif
  memmove((void *)PGSIZE, initcode, sizeof(initcode));
  #if PRIVILEGE_VERSION == PRIVILEGE_VERSION_1_12
  disable_sum();
  #endif

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
  if(mmap_dup(np->mm, p->mm) == -1){
    freeproc(np);
    release(&np->lock);
    return -1;
  }

  // 拷贝trapframe
  memcpy(proc_get_tf(np), proc_get_tf(p), sizeof(tf_t));

  // 将返回值置为0
  proc_get_tf(np)->a0 = 0;

  // 如果指定了栈，那么重设sp
  if(stack)
    proc_get_tf(np)->sp = stack;

  // 增加文件描述符引用 
  // TODO: mmap file dup
  if(fdrealloc(np, p->nfd) == -1) {
    freeproc(np);
    release(&np->lock);
    return -1;
  }

  for(i = 0; i < p->nfd; i++) {
    struct file *fp;
    if((fp = get_file(p, i)) != NULL)
      set_file(np, i, filedup(fp));
  }
  
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


  // np->cur_mmap_sz = p->cur_mmap_sz;
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

void proc_close_files(proc_t *p) {
  for(int fd = 0; fd < p->nfd; fd++){
    struct file *f;
    if((f = get_file(p, fd)) != NULL){
      fileclose(f);
    }
  }
}
void buddy_print_free();
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
  // mmap_print(p->mm);
  // buddy_print_free();
  // if(status == -1) {
  //   panic("exception quit");
  // }
  // Close all open files.
  proc_close_files(p);

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
    debug("ready to mount\n");
    fat_mount(ROOTDEV, &fat);
    debug("mount done\n");
    p->cwd = namee(NULL, "/");
    // init dir...
    entry_t *tmp = create(fat->root, "/tmp", T_DIR);
    if(tmp)
      eunlockput(tmp);
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
  p->mm = newmm;
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
    printf("%d %s %s", p->pid, state, p->name);
    printf("\n");
  }
}


struct file *get_file(proc_t *p, int fd) {
    if(fd < 0 || fd >= p->nfd)
        return NULL;
    // debug("nfd is %d fd is %d ext_ofile is %#x", p->nfd, fd, p->ext_ofile);
    if(fd < NOFILE) 
        return p->ofile[fd];
    else 
        return p->ext_ofile[fd - NOFILE];
}

void set_file(proc_t *p, int fd, struct file *f) {
    if(fd < 0 || fd >= p->nfd)
        return;
    if(fd < NOFILE)
        p->ofile[fd] = f;
    else 
        p->ext_ofile[fd - NOFILE] = f; 
}


int fdrealloc(proc_t *p, int nfd) {
    if(nfd > MAX_FD || nfd < NOFILE) 
        return -1;
    if(nfd == NOFILE)
      return 0;

    int oldsz = max(p->nfd - NOFILE, 0) * sizeof(struct file *);
    int newsz = (nfd - NOFILE) * sizeof(struct file *);
    struct file **newfdarray;

    if((newfdarray = (struct file **)kzalloc(newsz)) == NULL) {
        debug("alloc fail");
        return -1;
    }
    memmove(newfdarray, p->ext_ofile, min(oldsz, newsz));
    p->nfd = nfd;
    kfree((void *)p->ext_ofile);
    p->ext_ofile = newfdarray;
    // debug("alloc newfdarray %#lx nfd %d addr %lx", newfdarray, nfd, &p->ext_ofile);
    return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
int fdalloc(proc_t *p, struct file* f) {
    int fd;

    for (fd = 0; fd < NOFILE; fd++) {
        if (p->ofile[fd] == 0) {
            p->ofile[fd] = f;
            return fd;
        }
    }
    for (fd = 0; fd < p->nfd - NOFILE; fd++) {
        if (p->ext_ofile[fd] == 0) {
            p->ext_ofile[fd] = f;
            return fd + NOFILE;
        }
    }
    
    if(p->nfd == MAX_FD || fdrealloc(p, min(p->nfd + 10, MAX_FD)) < 0)
        return -1;

    for (; fd < p->nfd - NOFILE; fd++) {
        if (p->ext_ofile[fd] == 0) {
            p->ext_ofile[fd] = f;
            return fd + NOFILE;
        }
    }
    panic("unreached");
}
