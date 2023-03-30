#include "riscv.h"
#include "mm/mmap.h"

static char descs[2][16][32]   = {
    {
        [0] "Instruction address misaligned",
        [1] "Instruction access fault",
        [2] "Illegal instruction",
        [3] "Breakpoint",
        [4] "Load address misaligned",
        [5] "Load access fault",
        [6] "Store/AMO address misaligned",
        [7] "Store/AMO access fault",
        [8] "Environment call from U-mode",
        [9] "Environment call from S-mode",
        [12] "Instruction page fault",
        [13] "Load page fault",
        [15] "Store/AMO page fault",
    },
    {
        [1] "Supervisor software interrupt",
        [5] "Supervisor timer interrupt",
        [9] "Supervisor external interrupt",
    },
};


char *riscv_cause2str(uint64_t scause) {
    int intr = (scause & INTERRUPT) > 0 ? 1 : 0;
    return descs[intr][scause % 16];
}


uint32_t riscv_map_prot(uint32_t linux_prot) {
    uint32_t prot = 0;

    if (linux_prot & PROT_COW) {
        linux_prot &= ~PROT_COW;
        prot |= PTE_COW;
    }
    return prot | linux_prot << 1;
}