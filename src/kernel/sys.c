#include "kernel/sys.h"
#include "kernel/time.h"
#include "defs.h"
#include "profile.h"
#include "mm/buddy.h"

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
  // printf("tick %ld\n", ticks);
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
  return copyout(myproc()->mm->pagetable, addr, (char *)&rate, sizeof(rate));
  #endif


  return -1;
}


extern void print_buddy();
uint64_t sys_memuse(void) {
  buddy_print_free();
  print_buddy();
  return 0;
}

uint64_t sys_times(void) {
  uint64_t addr;
  uint64_t ret = 0;

  if(argaddr(0, &addr) < 0) 
    return -1;
  
  acquire(&tickslock);
  if(copyout(myproc()->mm->pagetable, addr, (char *)&ticks, sizeof(ticks)) == -1)  
    ret = -1;
  release(&tickslock);

  return ret;
}

uint64_t sys_uname(void) {
  uint64_t addr;

  if(argaddr(0, &addr) < 0) {
    return -1;
  }

  return copyout(myproc()->mm->pagetable, addr, (char *)&sysname, sizeof(utsname_t));
}
uint64_t sys_gettimeofday(void) {
  timespec_t time;
  uint64_t addr;
  uint64_t ret = 0;

  if(argaddr(0, &addr) < 0) 
    return -1;

  acquire(&tickslock);
  time = TICK2TIMESPEC(ticks);
  release(&tickslock);
  // uint64_t old_ticks = ticks;
  // while(ticks - old_ticks < 10); // 测试用例不合理

  if(copyout(myproc()->mm->pagetable, addr, (char *)&time, sizeof(time)) == -1) {
    ret = -1;
  } 
  return ret;
}