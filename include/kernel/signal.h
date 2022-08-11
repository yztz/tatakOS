#ifndef _H_SIGNAL_
#define _H_SIGNAL_

#include "common.h"
#include "atomic/spinlock.h"
#include "list.h"


#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGBUS		 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO
/*
#define SIGLOST		29
*/
#define SIGPWR		30
#define SIGSYS		31
#define	SIGUNUSED	31

/* These should not be considered constants from userland.  */
#define SIGRTMIN	32
#define SIGRTMAX    64

#define SA_NOCLDSTOP	0x00000001
#define SA_NOCLDWAIT	0x00000002
#define SA_SIGINFO	    0x00000004
#define SA_ONSTACK	    0x08000000
#define SA_RESTART	    0x10000000
#define SA_NODEFER	    0x40000000
#define SA_RESETHAND	0x80000000

#define SA_NOMASK	SA_NODEFER
#define SA_ONESHOT	SA_RESETHAND

#define SIG_DFL	    ((void (*)(int))0)	    /* default signal handling */
#define SIG_IGN	    ((void (*)(int))1)	    /* ignore signal */
#define SIG_ERR	    ((void (*)(int))-1)	/* error return from signal */

#define SIG_BLOCK     0
#define SIG_UNBLOCK   1
#define SIG_SETMASK   2


// typedef struct sigset {
//      unsigned long __bits[2]; 
// } sigset_t;

// struct sigpending {
//     int signo;
//     int senderpid;
//     list_head_t head;
// };

typedef uint64_t sigset_t;

typedef struct siginfo {
	int si_signo;
	int si_code;
	int si_errno;
} siginfo_t;

typedef struct sigaction {
    void (*handler)(int);
	unsigned long flags;
	void (*restorer)(void);
	unsigned mask[2];
} sigaction_t;

typedef struct signal {
    int ref;
    spinlock_t siglock;

    sigaction_t actions[sizeof(sigset_t) * 8];
    // sigaction_t 
    // sigset_t pending;
} signal_t;

typedef unsigned long __riscv_mc_gp_state[32];

struct __riscv_mc_f_ext_state {
	unsigned int __f[32];
	unsigned int __fcsr;
};

struct __riscv_mc_d_ext_state {
	unsigned long long __f[32];
	unsigned int __fcsr;
};

struct __riscv_mc_q_ext_state {
	unsigned long long __f[64] __attribute__((aligned(16)));
	unsigned int __fcsr;
	unsigned int __reserved[3];
};

union __riscv_mc_fp_state {
	struct __riscv_mc_f_ext_state __f;
	struct __riscv_mc_d_ext_state __d;
	struct __riscv_mc_q_ext_state __q;
};

typedef struct mcontext_t {
	__riscv_mc_gp_state __gregs;
	union __riscv_mc_fp_state __fpregs;
} mcontext_t;


struct sigaltstack {
	void *ss_sp;
	int ss_flags;
	size_t ss_size;
};

typedef struct __ucontext
{
	unsigned long uc_flags;
	struct __ucontext *uc_link;
	struct sigaltstack uc_stack;
	struct { unsigned long __bits[128/sizeof(long)]; } uc_sigmask;
	mcontext_t uc_mcontext;
} ucontext_t;


signal_t *sig_new();
void sig_free(signal_t **pself);
signal_t *sig_clone(signal_t *self);
void sig_ref(signal_t *self);
void sig_deref(signal_t *self);
sigaction_t *sig_getaction(signal_t *self, int signum);
void sig_setaction(signal_t *self, int signum, sigaction_t *act, sigaction_t *old);

void sig_handle(signal_t *self);
void sig_reset(signal_t *self);

#endif