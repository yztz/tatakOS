#include "kernel/sys.h"
#include "mm/buddy.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "kernel/syscall.h"
#include "sys/resource.h"
#include "driver/timer.h"

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
    .release = "5.0",
    .version = "0.0.0",
};


uint64_t sys_sysinfo(void) {
    uint64_t addr;

    if (argaddr(0, &addr) < 0)
        return -1;

    struct sysinfo si = {
      .uptime = TICK2SEC(ticks),
      .loads = {0},
      .totalram = get_total_mem(),
      .freeram = get_free_mem(),
      .sharedram = 0,
      .bufferram = 0,
      .totalswap = 0,
      .freeswap = 0,
      .procs = get_proc_cnt(),
      .totalhigh = 0,
      .freehigh = 0,
      .mem_unit = 1,
    };

    return copy_to_user(addr, &si, sizeof(struct sysinfo));
}


uint64 sys_timetag(void) {
    return ticks;
}



uint64_t sys_memuse(void) {
    buddy_print_free();
    return 0;
}

uint64_t sys_times(void) {
    uint64_t addr;
    uint64_t ret = 0;

    if (argaddr(0, &addr) < 0)
        return -1;

    if (copyout(addr, (char*)&ticks, sizeof(ticks)) == -1)
        ret = -1;

    return ret;
}



uint64_t sys_halt(void) {
    sbi_shutdown();
    LOOP();
    return 0;
}

uint64_t sys_uname(void) {
    uint64_t addr;

    if (argaddr(0, &addr) < 0) {
        return -1;
    }

    return copyout(addr, (char*)&sysname, sizeof(utsname_t));
}
uint64_t sys_gettimeofday(void) {
    timespec_t time;
    uint64_t addr;

    if (argaddr(0, &addr) < 0)
        return -1;

    time = TICK2TIMESPEC(ticks);

    if (copyout(addr, (char*)&time, sizeof(time)) == -1) {
        return -1;
    }
    return 0;
}



uint64 sys_clock_gettime(void) {
    // int clockid;
    uint64_t addr;

    if (argaddr(1, &addr) < 0)
        return -1;

    timespec_t time = RT2TIMESPEC(get_time(), CLOCK_FREQ);

    if (copy_to_user(addr, &time, sizeof(time)) < 0) {
        debug("copy fail");
        return -1;
    }

    return 0;
}
