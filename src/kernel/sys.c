#include "kernel/sys.h"
#include "kernel/time.h"
#include "defs.h"
#include "profile.h"
#include "mm/buddy.h"
#include "mm/vm.h"

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
  uint64_t ret = 0;

  if(argaddr(0, &addr) < 0) 
    return -1;

  time = TICK2TIMESPEC(ticks);

  if(copyout(addr, (char *)&time, sizeof(time)) == -1) {
    ret = -1;
  } 
  return ret;
}