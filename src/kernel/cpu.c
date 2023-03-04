#include "common.h"
#include "kernel/cpu.h"
#include "riscv.h"

#define __MODULE_NAME__ CPU
#include "debug.h"

struct cpu cpus[NUM_CORES];

void cpu_init() {
    for (int i = 0; i < NUM_CORES; i++) {
        cpus[i].intena = 0;
        cpus[i].noff = 0;
        cpus[i].proc = NULL;
    }
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
cpu_t *mycpu() {
    int id = cpuid();
#ifdef QEMU 
    assert_s(id < NUM_CORES, "CPUS change detected. Please clean and remake.");
#endif
    struct cpu *c = &cpus[id];
    return c;
}
