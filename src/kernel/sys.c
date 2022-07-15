#include "kernel/sys.h"
#include "kernel/time.h"
#include "defs.h"
#include "profile.h"
#include "mm/buddy.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "sys/resource.h"

#define __MODULE_NAME__ SYS
#include "debug.h"

typedef struct {
    uint64_t hit;
    uint64_t miss;
} cache_rate_t;


utsname_t sysname = {
  .domainname = "tatak.os",
  .machine = "xxxk210xxx",
  .nodename = "wtf",
  .release = "no release",
  .version = "0.0.0.0.0.0.0.0.1"
};



uint64 sys_timetag(void) {
  return ticks;
}

uint64 sys_bio_cache(void) {
  uint64_t addr;

  if(argaddr(0, &addr) < 0)
    return -1;

  #ifdef DEBUG
  cache_rate_t rate;
  extern uint64_t bio_cache_hit, bio_cache_miss;
  rate.hit = bio_cache_hit;
  rate.miss = bio_cache_miss;
  return copyout(addr, (char *)&rate, sizeof(rate));
  #endif


  return -1;
}


uint64_t sys_prlimit64(void) {
  int pid;
  int res;
  uint64_t oldrl, newrl;
  struct rlimit rl;
  proc_t *p = myproc();

  if(argint(0, &pid) < 0 || argint(1, &res) < 0 || argaddr(2, &newrl) < 0 || argaddr(3, &oldrl) < 0) {
    return -1;
  }

  if(res != RLIMIT_STACK) {
    debug("ukres %d", res);
    return -1;
  }
    

  if(oldrl) {
    rl.rlim_cur = p->mm->ustack->len;
    rl.rlim_max = 30 * PGSIZE;
    if(copyout(oldrl, (char *)&rl, sizeof(struct rlimit)) < 0) {
      return -1;
    }
  }
  

  if(newrl) {
    if(copy_from_user(&rl, newrl, sizeof(struct rlimit)) < 0)
      return -1;
    if(mmap_ext_stack(p->mm, rl.rlim_cur) < 0) 
      return -1;
  }
  
  return 0;
}


uint64_t sys_memuse(void) {
  buddy_print_free();
  return 0;
}

uint64_t sys_times(void) {
  uint64_t addr;
  uint64_t ret = 0;

  if(argaddr(0, &addr) < 0) 
    return -1;
  
  if(copyout(addr, (char *)&ticks, sizeof(ticks)) == -1)  
    ret = -1;

  return ret;
}

uint64_t sys_uname(void) {
  uint64_t addr;

  if(argaddr(0, &addr) < 0) {
    return -1;
  }

  return copyout(addr, (char *)&sysname, sizeof(utsname_t));
}
uint64_t sys_gettimeofday(void) {
  timespec_t time;
  uint64_t addr;

  if(argaddr(0, &addr) < 0) 
    return -1;

  time = TICK2TIMESPEC(ticks);

  if(copyout(addr, (char *)&time, sizeof(time)) == -1) {
    return -1;
  } 
  return 0;
}

uint64 sys_clock_gettime(void) {
  // int clockid;
  uint64_t addr;
  timespec_t time;

  if(argaddr(1, &addr) < 0) 
    return -1;

  if(copyout(addr, (char *)&time, sizeof(time)) == -1) {
    return -1;
  } 
  return 0;
}