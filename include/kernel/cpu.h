#ifndef _H_CPU_
#define _H_CPU_

#include "types.h"
#include "riscv.h"

/**
 * @brief Saved registers for kernel context switches.
 */
struct context {
    uint64_t ra;
    uint64_t sp;

    // callee-saved
    uint64_t s0;
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
};

/**
 * @brief Per-CPU state.
 */
struct cpu {
    /// @brief The process running on this cpu, or null.
    struct proc *proc;
    /// @brief swtch() here to enter scheduler().
    struct context scheduler;
    /// @brief Depth of push_off() nesting.
    int noff;
    /// @brief Were interrupts enabled before push_off()?
    int intena;
};

typedef struct cpu cpu_t;

void cpu_init();

extern struct cpu cpus[NUM_CORES];
extern void panic(char *s);

/**
 * @brief get current cpu id
 * @note called after intr_off
 * 
 * @return int 
 */
static inline int cpuid() {
    return r_tp();
}

/**
 * @brief return this CPU's cpu struct.
 * @note called after intr_off
 * 
 * @return cpu_t* 
 */
static inline cpu_t *mycpu() {
    return &cpus[cpuid()];
}



#endif