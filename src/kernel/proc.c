#include "memlayout.h"
#include "common.h"
#include "kernel/sched.h"
#include "kernel/taskqueue.h"
#include "defs.h"
#include "mm/vm.h"
#include "driver/timer.h"


#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/fs.h"
#include "fs/file.h"

#define QUIET
#define __MODULE_NAME__ PROC
#include "debug.h"

struct cpu cpus[NUM_CORES];

struct proc proc[NPROC];

struct proc *initproc;

atomic_t proc_cnt = INIT_ATOMIC();

static atomic_t nextpid = INIT_ATOMIC();

#define get_next_pid() (atomic_inc(&nextpid))

#define IS_MAIN_THREAD(p) ((p)->tg->master_pid==p->pid)

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
struct spinlock wait_lock;


static inline void procstate_init() {
    for (struct proc *p = proc; p < &proc[NPROC]; p++) {
        initlock(&p->lock, "proc");
        p->__state = UNUSED;
        p->wait_channel = NULL;
        p->mm = NULL;
        p->kstack = 0;
        p->trapframe = NULL;
        pstate_list_offer(UNUSED, p);
    }
}

static inline void cpustate_init() {
    for (int i = 0; i < NUM_CORES; i++) {
        cpus[i].intena = 0;
        cpus[i].noff = 0;
        cpus[i].proc = NULL;
    }
}

// initialize the proc table at boot time.
void procinit(void) {
    initlock(&wait_lock, "wait_lock");
    procstate_init();
    cpustate_init();
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
struct cpu *mycpu(void) {
    int id = cpuid();
    struct cpu *c = &cpus[id];
    return c;
}


struct proc *myproc(void) {
    push_off();
    struct cpu *c = mycpu();
    struct proc *p = c->proc;
    struct proc *cur = current;
    pop_off();
    assert(cur == p);
    return p;
}

struct pagevec;

struct pagevec *my_inactive_pvec() {
    push_off();
    struct cpu *c = mycpu();
    struct pagevec *inactive_pvec = c->inactive_pvec;
    // pop_off();
    return inactive_pvec;
}

struct pagevec *my_active_pvec() {
    push_off();
    struct cpu *c = mycpu();
    struct pagevec *active_pvec = c->active_pvec;
    // pop_off();
    return active_pvec;
}

int get_proc_cnt() {
    return atomic_get(&proc_cnt);
}

void pstate() {
    for (struct proc *p = proc; p < &proc[NPROC]; p++) {
        if (p->state_head.next == NULL) ER();
    }
}


void newproc_callback_stage1(kthread_callback callback) {
    proc_t *p = myproc();
    release(&p->lock);

    callback(p);

    panic("bad ret");
}

extern void newproc_callback_stage0();

proc_t *proc_new(kthread_callback callback) {
    struct proc *p;

    pstate_list_lock(UNUSED);
    if ((p = pstate_list_poll(UNUSED)) == NULL) {
        pstate_list_unlock(UNUSED);
        return NULL;
    }
    acquire(&p->lock);
    pstate_list_unlock(UNUSED);

    // printf("kill is %d and kill addr is %#lx\n", p->killed, &p->killed);
    p->pid = get_next_pid();
    // p->tid = alloctid();
    p->killed = 0;
    p->sig_pending = 0;
    p->sig_mask = 0;
    p->signaling = 0;
    p->set_tid_addr = 0;
    p->clear_tid_addr = 0;
    p->u_time = 0;
    p->s_time = 0;
    p->stub_time = ticks;

    INIT_LIST_HEAD(&p->thrd_head);

    if ((p->kstack = (uint64)kmalloc(KSTACK_SZ)) == 0) {
        debug("kstack alloc failure");
        goto bad;
    }

    memset(&p->context, 0, sizeof(p->context));
    // We want to release the proc lock automatically, 
    // so a middle stage(newproc_callback_stage1) need to be called
    // and the real callback should be passed as a param.
    // Out of a0 is not a callee-saved register, so we do it with the help of s1.
    // An asm routine(newproc_callback_stage0 in Swtch.S) is written to help us pass the param
    p->context.ra = (uint64)newproc_callback_stage0;
    p->context.s1 = (uint64)callback;
    p->context.sp = p->kstack + KSTACK_SZ;

    atomic_inc(&proc_cnt);

    pstate_migrate(p, USED);

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

    if (p->kstack) kfree_safe(&p->kstack);
    if (p->exe) eput(p->exe);

    pstate_migrate(p, UNUSED);

    p->exe = NULL;
    p->pid = -1;
    p->parent = 0;
    p->name[0] = 0;
    p->chan = 0;
    p->futex_chan = 0;
    p->killed = 0;
    p->xstate = 0;

    atomic_dec(&proc_cnt);
}

// a user program that calls exec("/init")
// od -t xC initcode
uchar initcode[] = {
    #include "generated/initcode_bin.h"
};


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

static void __userinit(proc_t *p) {
    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    extern fat32_t *fat;
    fat_mount(ROOTDEV, &fat);
    p->cwd = namee(NULL, "/");
    forkret(p);
}

// Set up first user process.
void userinit(void) {
    struct proc *p;
    mm_t *mm;
    fdtable_t *fdtable;
    signal_t *sig;
    tg_t *tg;
    tf_t *tf;

    p = proc_new(__userinit);
    initproc = p;

    if (p == NULL) {
        panic("alloc");
    }

    if ((mm = mmap_new()) == NULL ||
        (fdtable = fdtbl_new()) == NULL ||
        (sig = sig_new()) == NULL ||
        (tg = tg_new(p)) == NULL ||
        (tf = tf_new(p)) == NULL) {
        panic("alloc");
    }

    proc_switchmm(p, mm);
    proc_setfdtbl(p, fdtable);
    proc_setsig(p, sig);
    proc_settg(p, tg);
    proc_settf(p, tf);

    const int USER_SIZE = 4 * PGSIZE;

    if (sizeof(initcode) >= USER_SIZE)
        panic("inituvm: too large");

    // debug("initcode size: %d", sizeof(initcode));

    // if(do_mmap_alloc(p->mm, 0, USER_SIZE, 0, PROT_WRITE|PROT_READ|PROT_EXEC|PROT_USER) == -1) {
    if (do_mmap_alloc(p->mm, PGSIZE, USER_SIZE, 0, PROT_WRITE | PROT_READ | PROT_EXEC | PROT_USER) == -1) {
        panic("mmap1 failure");
    }

    if (mmap_map_stack(p->mm, USTACKSIZE) == -1) {
        panic("mmap2 failure");
    }

    enable_sum();
    memmove((void *)PGSIZE, initcode, sizeof(initcode));
    // memmove((void *)0, initcode, sizeof(initcode));
    disable_sum();

    // prepare for the very first "return" from kernel to user.
    proc_get_tf(p)->epc = PGSIZE;      // user program counter
    proc_get_tf(p)->sp = USERSPACE_END;  // user stack pointer

    safestrcpy(p->name, "initcode", sizeof(p->name));

    init_std(p);

    pstate_migrate(p, RUNNABLE);

    release(&p->lock);
}

uint64 growproc(uint64_t newbreak) {
    struct proc *p = myproc();
    if (newbreak == 0) {
        return PROGRAM_BREAK(p->mm);
    }
    if (mmap_ext_heap(p->mm, newbreak) == -1) {
        return -1;
    }

    return PROGRAM_BREAK(p->mm);
}


int kthread_create(char *name, kthread_callback callback) {
    if (initproc == NULL)
        panic("initproc required");

    struct proc *np = proc_new(callback);

    if (np == NULL)
        return -1;

    proc_setmm(np, initproc->mm);
    strncpy(np->name, name, 20);
    pstate_migrate(np, RUNNABLE);

    release(&np->lock);

    return 0;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
void reparent(struct proc *p) {
    struct proc *pp;

    for (pp = proc; pp < &proc[NPROC]; pp++) {
        if (pp->parent == p) {
            pp->parent = initproc;
            wakeup(initproc);
        }
    }
}

#include "kernel/futex.h"

void exit(int status) {
    proc_t *p = myproc();
    // tg_t *thrdgrp = p->tg;
    // proc_t *mp = tg_main_thrd(thrdgrp);
    int thrdcnt = tg_quit(p->tg);

    if (p == initproc) {
        panic("init exiting");
    }

    debug("PID %d EXIT %d", p->pid, status);

    if (thrdcnt == 0) {
        fdtbl_closeall(p->fdtable);
    }

    eput(p->cwd);
    p->cwd = NULL;


    acquire(&wait_lock);

    // Give any children to init.
    reparent(p);

    // Parent might be sleeping in wait().
    if (thrdcnt == 0) {
        wakeup(p->parent);
        sig_send(p->parent, SIGCHLD);
    }

    acquire(&p->lock);

    p->xstate = status;
    pstate_migrate(p, ZOMBIE);

    release(&wait_lock);

    if (p->clear_tid_addr) {
        memset_user(p->clear_tid_addr, 0, sizeof(int));
        acquire(&p->tg->lock);
        futex_wake((void *)p->clear_tid_addr, 1);
        release(&p->tg->lock);
    }

    // Jump into the scheduler, never to return.
    sched();
    panic("zombie exit");
}

void sig_send(proc_t *p, int signum) {
    acquire(&p->lock);
    if (signum == SIGKILL) {
        p->killed = 1;
    }
    p->sig_pending |= (1L << (signum - 1));
    if (p->state == SLEEPING) pstate_migrate(p, RUNNABLE);
    release(&p->lock);
}


extern void lru_add_drain();

int freechild() {
    proc_t *p = myproc();
    proc_t *np;
    int cnt = 0;
    for (np = proc; np < &proc[NPROC]; np++) {
        if (np->parent == p) {
            // Make sure the child isn't still in exit() or swtch().
            acquire(&np->lock);
            if (np->state == ZOMBIE) {
                // Found one.
                /* 进程退出的时候，有些页还在pagevec中，没有释放，看上去好像内存泄露了，所以这里加上这句。 */
                // lru_add_drain();
                freeproc(np);
                // release(&np->lock);
                cnt++;
            }
            release(&np->lock);
        }
    }
    return cnt;
}
// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int waitpid(int cid, uint64 addr, int options) {
    struct proc *np;
    int havekids, pid, haveckid;
    struct proc *p = myproc();

    acquire(&wait_lock);

    for (;;) {
        // Scan through table looking for exited children.
        havekids = 0; // 表示子进程存在
        haveckid = 0; // 表示指定的cid存在
        for (np = proc; np < &proc[NPROC]; np++) {
            if (np->parent == p) {
                // Make sure the child isn't still in exit() or swtch().
                acquire(&np->lock);

                havekids = 1;

                pid = np->pid;
                if (cid != -1 && cid != pid) {
                    release(&np->lock);
                    continue;
                }

                haveckid = 1;
                if (np->state == ZOMBIE) {
                    // Found one.

                    /* 进程退出的时候，有些页还在pagevec中，没有释放，看上去好像内存泄露了，所以这里加上这句。 */
                    // lru_add_drain();
                    if (addr != 0 && copyout(addr, (char *)&np->xstate,
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
        if (!havekids || !haveckid || p->killed) {
            release(&wait_lock);
            return -1;
        }

        // Wait for a child to exit.
        sleep(p, &wait_lock);
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

void proc_settf(proc_t *p, tf_t *tf) {
    p->trapframe = tf;
}

void proc_setfdtbl(proc_t *p, fdtable_t *fdtbl) {
    fdtbl_ref(fdtbl);
    p->fdtable = fdtbl;
}

tf_t *proc_get_tf(proc_t *p) {
    return p->trapframe;
}




static void process_timeout(void *p) {
    proc_t *proc = (proc_t *)p;
    wake_up_process(proc);
}

int sched_timeout(int timeout) {
    timer_t timer;
    set_timer(&timer, process_timeout, timeout, myproc());
    sched();
    // May not be woken because of timeout, so we need clear the timer
    remove_timer(&timer);

    return timer.expires == 0;
}



void proc_switchmm(proc_t *p, mm_t *newmm) {
    proc_setmm(p, newmm);
    switchuvm(p->mm);
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void procdump(void) {
    static char *states[] = {
        [UNUSED]          "UNUSED        ",
        [SLEEPING]        "SLEEPING      ",
        [RUNNABLE]        "RUNNABLE      ",
        [RUNNING]         "RUNNING       ",
        [ZOMBIE]          "ZOMBIE        ",
    };
    struct proc *p;
    char *state;

    printf("\n");
    for (p = proc; p < &proc[NPROC]; p++) {
        if (p->state == UNUSED)
            continue;
        if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
            state = states[p->state];
        else
            state = "???";
        printf("%d %s %s chan %#lx futex %#lx\n", p->pid, state, p->name, p->chan, p->futex_chan);
        if (p->state == SLEEPING && p->pid > 3) 
            backtrace_fp(p->context.s0);
    }
}
