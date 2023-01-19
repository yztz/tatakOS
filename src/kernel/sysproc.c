#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "common.h"
#include "kernel/time.h"
#include "mm/vm.h"
#include "driver/timer.h"

#define QUIET
#define __MODULE_NAME__ SYS_PROC
#include "debug.h"
#include "mm/mm.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n << 8); // ref: WEXITSTATUS
  return 0;  // not reached
}

uint64 sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return do_clone(myproc(), 0, 0, 0, 0, 0);
}

uint64
sys_set_tid_address(void)
{
  proc_t *p = myproc();
  uint64 tidaddr;

  if(argaddr(0, &tidaddr) < 0) {
    return -1;
  }

  p->clear_tid_addr = tidaddr;

  return p->pid;
}


uint64
sys_clone(void)
{
  uint64 stack;
  int flags;
  uint64_t ptid;
  uint64_t tls;
  uint64_t ctid;

  // debug("clone: enter");
  if(argint(0, &flags) < 0 || argaddr(1, &stack) < 0 || 
    argaddr(2, &ptid) < 0 || argaddr(3, &tls) < 0 || argaddr(4, &ctid) < 0)
      return -1;
  debug("clone: flags %x", flags);
  return do_clone(myproc(), stack, flags, ptid, tls, ctid);
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return waitpid(-1, p, 0);
}


uint64
sys_wait4(void)
{
  int pid;
  uint64_t status;
  int options; // ignored

  if(argint(0, &pid) < 0 || argaddr(1, &status) || argint(2, &options) < 0)
    return -1;

  // debug("option is %x", options);

  return waitpid(pid, status, options);

}

uint64
sys_brk(void)
{
  uint64_t brkaddr;

  if(argaddr(0, &brkaddr) < 0)
    return -1;
  // debug("brk addr is %#lx", brkaddr);

  return growproc(brkaddr);
}

uint64
sys_nanosleep(void)
{
  timespec_t time;
  uint64_t addr;
  proc_t *p = myproc();

  if(argaddr(0, &addr) < 0)
    return -1;
  if(copy_from_user(&time, addr, sizeof(timespec_t)) == -1)
    return -1;

  uint64_t timeout = time.ts_sec * 1000 + time.ts_nsec / 1000000;
  acquire(&p->lock);
  pstate_migrate(p, SLEEPING);
  sched_timeout(timeout);
  release(&p->lock);
  return 0;
}