#ifndef _H_SBI_
#define _H_SBI_

#include "types.h"
#include "riscv.h"

extern void panic(char *s);

#define BASE_EXT 0x10L
#define TIMER_EXT 0x54494D45L
#define HSM_EXT 0x48534DL
#define PMU_EXT 0x504D55L
#define CONSOLE_PUT_CHAR_EXT 0x01L
#define CONSOLE_GET_CHAR_EXT 0x02L
#define LEGACY_SET_TIMER 0x00L


typedef struct sbiret {
    uint64 error;
    uint64 value;
}sbiret_t;

#define SBI_CALL(module, funct, arg0, arg1, arg2, arg3) ({ \
    register uint64 a0 asm ("a0") = (uint64)(arg0); \
    register uint64 a1 asm ("a1") = (uint64)(arg1); \
    register uint64 a2 asm ("a2") = (uint64)(arg2); \
    register uint64 a3 asm ("a3") = (uint64)(arg3); \
    register uint64 a6 asm ("a6") = (uint64)(funct); \
    register uint64 a7 asm ("a7") = (uint64)(module); \
    asm volatile ("ecall" \
        : "+r" (a0), "+r" (a1) \
        : "r" (a1), "r" (a2), "r" (a3), "r" (a6), "r" (a7) \
        : "memory"); \
    (struct sbiret) {a0, a1}; \
})

#define SBI_CALL_4(module, funct, arg0, arg1, arg2, arg3) SBI_CALL(module, funct, arg0, arg1, arg2, arg3)
#define SBI_CALL_3(module, funct, arg0, arg1, arg2) SBI_CALL(module, funct, arg0, arg1, arg2, 0)
#define SBI_CALL_2(module, funct, arg0, arg1) SBI_CALL(module, funct, arg0, arg1, 0, 0)
#define SBI_CALL_1(module, funct, arg0) SBI_CALL(module, funct, arg0, 0, 0, 0)
#define SBI_CALL_0(module, funct) SBI_CALL(module, funct, 0, 0, 0, 0)


static inline struct sbiret sbi_set_timer(uint64 stime_value) {

    return SBI_CALL_1(TIMER_EXT, 0, stime_value);
}

static inline struct sbiret sbi_legacy_set_timer(uint64 stime_value) {
    return SBI_CALL_1(LEGACY_SET_TIMER, 0, stime_value);
}

static inline struct sbiret sbi_putchar(int ch) {
    return SBI_CALL_1(CONSOLE_PUT_CHAR_EXT, 0, ch);
}

static inline char sbi_getchar() {
    return SBI_CALL_0(CONSOLE_GET_CHAR_EXT, 0).error;
}

static inline struct sbiret sbi_set_mext() {
    return SBI_CALL_0(0x0A000009, 0x210);
}

/* Not implemented */
static inline struct sbiret sbi_pmu_num_counters() {
    return SBI_CALL_0(PMU_EXT, 0);
}


#endif