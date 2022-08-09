#include "kernel/signal.h"
#include "kernel/proc.h"
#include "mm/vm.h"
#include "defs.h"

#define __MODULE_NAME__ SIGNAL
#include "debug.h"

static void sig_dfl(int signum) {
    debug("exit...");
    exit(signum);
}




static inline void sigmaskadd(proc_t *p, sigset_t set) {
    p->sig_mask |= set;
}

static inline void sigmaskclear(proc_t *p, sigset_t set) {
    p->sig_mask &= ~set;
}

static inline void sigmaskset(proc_t *p, sigset_t set) {
    p->sig_mask = set;
}



void sig_ref(signal_t *self) {
    acquire(&self->siglock);
    self->ref++;
    release(&self->siglock);
}

void sig_deref(signal_t *self) {
    acquire(&self->siglock);
    self->ref--;
    release(&self->siglock);
}


signal_t *sig_new() {
    signal_t *newsig = kzalloc(sizeof(signal_t));
    if(newsig == NULL) 
        return NULL;
    initlock(&newsig->siglock, "siglock");

    return newsig;
}

void sig_free(signal_t **pself) {
    if(pself == NULL || *pself == NULL)
        return;
    signal_t *self = *pself;

    if(self->ref == 0)
        panic("ref");
    
    sig_deref(self);
    if(self->ref > 0)
        return;

    kfree_safe(pself);
}

signal_t *sig_clone(signal_t *self) {
    signal_t *newsig = sig_new();
    if(newsig == NULL)
        return NULL;
    
    for(int i = 0; i < sizeof(sigset_t) * 8; i++) {
        newsig->actions[i] = self->actions[i];
    }

    return newsig;
}


struct start_args {
	void *(*start_func)(void *);
	void *start_arg;
	volatile int control;
	// unsigned long sig_mask[_NSIG/8/sizeof(long)];
};


static char sigret_code[] = { 0x93, 0x08, 0xB0, 0x08, 0x73, 0x00, 0x00, 0x00 };

static void __sig_handle(proc_t *p, signal_t *signal, int signum, sigaction_t *act) {
    tf_t *tf = proc_get_tf(p);

    tf_backup(tf);
    
    // 设置sigret
    tf->sp -= sizeof(sigret_code);

    if(copy_to_user(tf->sp, sigret_code, sizeof(sigret_code)) < 0) {
        debug("bad copy?");
    }

    tf->ra = tf->sp;
    // 传递参数 (pass context anyway)
    // ucontext_t *context = kzalloc(sizeof(ucontext_t));
    // if(context == NULL)
    //     debug("bad alloc?");
    
    tf->sp -= sizeof(ucontext_t);
    tf->sp -= tf->sp % 16; // 对齐

    memset_user(tf->sp, 0, sizeof(ucontext_t));
    // context->uc_sigmask = p->sig_mask;
    // context->uc_mcontext.__gregs[0] = tf->epc;

    // debug("context addr is %#lx", tf->sp);

    // if(copy_to_user(tf->sp, context, sizeof(ucontext_t)) < 0) {
    //     debug("bad copy?");
    // }
    
    tf->a0 = signum;
    tf->a1 = 0;
    tf->a2 = tf->sp;

    // 跳转处理函数
    tf->epc = (uint64_t)act->handler;

    // debug("sigmask is %#lx", sigmaskget(signal));

    // debug("handler epc is %#lx", act->handler);
    
    // sigmaskadd(signal, *(sigset_t *)act->mask);

    // kfree(context);

    // tf_print(tf);

    // struct start_args args;
    // if(copy_from_user(&args, 0xF00022B00, sizeof(struct start_args)) >= 0)
    //     debug("func %#lx param %#lx", args.start_func, args.start_arg);
}

uint64_t sys_rt_sigreturn(void) {
    proc_t *p = myproc();
    tf_t *tf =proc_get_tf(p);

    if(p->signaling == 0) {
        debug("not from signal?");
        return -1;
    }

    // struct start_args args;
    // pte_t *pte = walk(p->mm->pagetable, 0xF00022B00, 0);
    // if(copy_from_user(&args, 0xF00022B00, sizeof(struct start_args)) >= 0)
    //     debug("func %#lx param %#lx", args.start_func, args.start_arg);
    // // short code[8];
    // uint64_t pa = PTE2PA(*pte);
    // debug("now pa is %#lx", pa);
    // copy_from_user(code, 0x35288, 16);
    // for(int i = 0; i < 8; i++) {
    //     printf("code %#x\n", code[i]);
    // }
    

    ucontext_t *context = kmalloc(sizeof(ucontext_t));
    if(context == NULL) {
        debug("bad alloc");
        return -1;
    }
    if(copy_from_user(context, tf->sp, sizeof(ucontext_t)) < 0)
        return -1;
    // debug("sigmask is %#lx", context->uc_sigmask);
    // debug("epc addr is %#lx", context->uc_mcontext.__gregs[0]);
    // debug("epc addr is %#lx", &(((ucontext_t *)tf->sp)->uc_mcontext.MC_PC));
    // sigmaskset(p->signal, context->uc_sigmask);
    acquire(&p->lock);
    p->sig_pending &= ~(1L << (p->signaling - 1));
    release(&p->lock);

    tf_restore(tf);

    // if(tf->epc == 0)
    //     panic("0?");
    
    if(context->uc_mcontext.__gregs[0]) {
        debug("switch? %#lx", context->uc_mcontext.__gregs[0]);
        tf->epc = context->uc_mcontext.__gregs[0];
    }

    debug("epc is %#lx", tf->epc);

    kfree(context);

    p->signaling = 0;
    
    // 避免a0被覆盖
    return tf->a0;
}


void sig_send(proc_t *p, int signum) {
    acquire(&p->lock);
    p->sig_pending |= (1L << (signum - 1));
    release(&p->lock);
}


// 这个实现在proc里还是signal里似乎都可以接受...
void sig_handle(signal_t *self) {
    proc_t *p = myproc();

    if(p->signaling) return;
    // debug("mask is %#lx", self->mask);
    // debug("pending is %#lx", p->sig_pending);
    for(int i = 0; i < sizeof(sigset_t) * 8; i++) {
        if((p->sig_mask & (1L << i)) > 0) continue;
        if((p->sig_pending & (1L << i)) == 0) continue;
        int signum = i + 1;
        debug("ready to handle sig %d", i + 1);
        
        sigaction_t *act = sig_getaction(self, signum);
        if(act->handler == SIG_DFL) {
            debug("sig handle DFL");
            sig_dfl(signum);
        } else if(act->handler == SIG_IGN) {
            debug("no dfl ign handler impl");
        } else {
            __sig_handle(p, self, i + 1, act);
            p->signaling = signum;
            break;
        }
    }
}

extern proc_t proc[];
uint64_t sys_tkill(void) {
    int tid;
    int sig;
    proc_t *p;

    if(argint(0, &tid) < 0 || argint(1, &sig) < 0) 
        return -1;

    debug("tid is %d sig is %d", tid, sig);

    for(p = proc; p < &proc[NPROC]; p++) {
        if(p->pid == tid) {
            sig_send(p, sig);
            return 0;
        }
    }

    return -1;
}

uint64_t sys_kill(void) {
    int pid;
    int sig;
    proc_t *p;

    if(argint(0, &pid) < 0 || argint(1, &sig) < 0) 
        return -1;

    for(p = proc; p < &proc[NPROC]; p++) {
        if(p->pid == pid) {
            sig_send(p, sig);
            return 0;
        }
    }

    return 0;
}

uint64_t sys_tgkill(void) {
    debug("called but not impl");
    return 0;
}



sigaction_t *sig_getaction(signal_t *self, int signum) {
    if(signum <= 0 || signum > SIGRTMAX)
        return NULL;
    return &self->actions[signum];
}

void sig_setaction(signal_t *self, int signum, sigaction_t *act, sigaction_t *old) {
    if(signum <= 0 || signum > SIGRTMAX)
        return;
    acquire(&self->siglock);
    if(old) {
        *old = self->actions[signum];
    }

    self->actions[signum] = *act;
    release(&self->siglock);
}



uint64_t sys_rt_sigaction(void) {
    int signum;
    uint64_t act_addr, oldact_addr;
    proc_t *p = myproc();

    sigaction_t act;
    sigaction_t oldact;

    if(argint(0, &signum) < 0 || argaddr(1, &act_addr) < 0 || argaddr(2, &oldact_addr) < 0)
        return -1;


    if(act_addr) {
        if(copy_from_user(&act, act_addr, sizeof(sigaction_t)) < 0)
            return -1;

        // debug("register for pid %d signum is %d flags is %#lx handler is %#lx", p->pid, signum, act.flags, act.handler);

        sig_setaction(p->signal, signum, &act, &oldact);
    }
    

    if(oldact_addr) {
        if(copy_to_user(oldact_addr, &oldact, sizeof(sigaction_t)) < 0)
            return -1;
    }

    return 0;
}

uint64_t sys_rt_sigprocmask(void) {
    int how;
    uint64_t newset_addr, oldset_addr;
    size_t size;
    proc_t *p = myproc();

    if(argint(0, &how) < 0 || argaddr(1, &newset_addr) < 0 || 
      argaddr(2, &oldset_addr) < 0 || argaddr(3, &size) < 0) {
        return -1;
    }
    
    assert(size <= sizeof(sigset_t));

    if(oldset_addr) {
        if(copy_to_user(oldset_addr, &p->sig_mask, size) < 0)
            return -1;
    }

    if(newset_addr) {
        sigset_t newset;
        if(copy_from_user(&newset, newset_addr, size) < 0)
            return -1;
        switch(how) {
            case SIG_BLOCK:
                sigmaskadd(p, newset);
                break;
            case SIG_UNBLOCK:
                sigmaskclear(p, newset);
                break;
            case SIG_SETMASK:
                sigmaskset(p, newset);
                break;
        }
        // debug("newset is %#lx", newset);
    }


    // debug("how is %d size is %ld", how, size);

    return 0;
}

uint64_t sys_rt_sigtimedwait(void) {
    return 0;
}


