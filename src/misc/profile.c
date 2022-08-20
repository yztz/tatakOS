#include "common.h"

#ifdef DEBUG

#define PROFILE_DEF
#define PROFILE_CALL

#define profile(func, ...) uint64 counter_##func = 0;
#define profile_void(func, ...) uint64 counter_##func = 0;

#include "generated/profile_gen.h"

#undef profile
#undef profile_void
#endif

#include "kernel/time.h"
uint64_t sys_profile(void) {
    #ifdef DEBUG
    #define print_profile(func) printf("%-11s used %10ldus\n", #func, RT2US(counter_##func, CLOCK_FREQ));
    #define profile(func, ...) print_profile(func)
    #define profile_void(func, ...) print_profile(func)
    printf("-----------Profiler start-----------\n");
    #include "generated/profile_gen.h"
    printf("-----------Profiler end-------------\n");
    return 0;
    #else 
    printf("warning: profile is only available in DEBUG\n");
    return -1;
    #endif
}