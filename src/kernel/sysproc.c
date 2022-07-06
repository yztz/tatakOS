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

#define QUIET
#define __MODULE_NAME__ SYS_PROC
#include "debug.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n << 8); // ref: WEXITSTATUS
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_getppid(void)
{
  return myproc()->parent->pid;
}

uint64
sys_fork(void)
{
  return do_clone(0);
}


uint64
sys_clone(void)
{
  int flag; // ignored
  uint64_t stack;
  // debug("clone: enter");
  if(argint(0, &flag) < 0 || argaddr(1, &stack) < 0)
    return -1;
  debug("clone: stack is %x", stack);
  return do_clone(stack);
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return waitpid(-1, p);
}


uint64
sys_wait4(void)
{
  int pid;
  uint64_t status;
  int options; // ignored

  if(argint(0, &pid) < 0 || argaddr(1, &status) || argint(2, &options) < 0)
    return -1;

  if(options > 0) 
    panic("not support");

  return waitpid(pid, status);

}

uint64
sys_sbrk(void)
{
  int n;

  if(argint(0, &n) < 0)
    return -1;
    
  return growproc(n);
}

uint64
sys_nanosleep(void)
{
  timespec_t time;
  uint64_t addr;
  uint ticks0;

  if(argaddr(0, &addr) < 0)
    return -1;
  if(copy_from_user(&time, (void *)addr, sizeof(timespec_t)) == -1)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < SEC2TICK(time.tv_sec)){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_sched_yield(void)
{
  yield();
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
