#include "types.h"
#include "printf.h"
#include "driver/timer.h"

#ifdef profile
#undef profile
#endif

#ifdef profile_void
#undef profile_void
#endif

#ifndef profile_name
#define profile_name(func) counter_##func
#endif

#ifdef profile_time_delta
#undef profile_time_delta
#endif

#ifdef PROFILE_PER_CALL
#define profile_time_delta(func, t1, t2) {printf("-"#func" used %ld\n", t2-t1);}
#else
#define profile_time_delta(func, t1, t2)
#endif

#define profile(func, ...) ({    \
    extern uint64_t profile_name(func);  \
    uint64_t __timer1 = get_time(); \
    typeof(func(__VA_ARGS__)) __res = func(__VA_ARGS__);          \
    uint64_t __timer2 = get_time(); \
    profile_name(func) += __timer2 - __timer1; \
    profile_time_delta(func, __timer1, __timer2) \
    __res;    \
})

#define profile_void(func, ...) {    \
    extern uint64_t profile_name(func);  \
    uint64_t __timer1 = get_time(); \
    func(__VA_ARGS__);          \
    uint64_t __timer2 = get_time(); \
    profile_name(func) += __timer2 - __timer1; \
    profile_time_delta(func, __timer1, __timer2) \
}


#ifdef DEBUG 
#ifndef PROFILE_DEF
#define PROFILE_DEF
#endif
#include "generated/profile_gen.h"
#endif

extern uint64_t sys_profile(void);