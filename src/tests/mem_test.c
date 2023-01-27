#include "mm/alloc.h"
#include "riscv.h"
#include "common.h"

#define __MODULE_NAME__ MEMTEST
#include "debug.h"

// #define TEST_MULT_PAGE
#define TEST_SLOB
#define report_point_mod (1000)
#define cycle (100000)

static uint64_t blk_sizes[] = {
#ifdef TEST_MULT_PAGE
    PGSIZE * 4, PGSIZE * 3, PGSIZE * 2, 
#endif
    PGSIZE, 
#ifdef TEST_SLOB
    PGSIZE / 2, PGSIZE / 4, 512, 256, 128, 64, 32, 16, 8, 7, 6, 5, 4, 3, 2, 1,
#endif    
};

static void rand_sleep(int rand) {
    rand = rand * 10000;
    while (rand > 0)
        rand--;
}



typedef struct mm_info {
    uint64_t addr;
    uint64_t size;
} mm_info_t;


void mm_test() {
    push_off();
    printf("---MM_TEST---\n");
    int tid = cpuid();
    int rand = tid + 1;
    int i = rand % NELEM(blk_sizes);

#define JUNK_0 (tid << 4 | 0xe)
#define JUNK_1 (tid << 4 | 0xf)
    
    int pool_size = rand * 2;
    int pool_usage = 0;
    mm_info_t *hold_pool = alloca(pool_size * sizeof(mm_info_t));

    int free_flag = 0;

    info("[Thread %d] Start", tid);
    int count = 0;
    while (count++ < cycle) {
        if (count % report_point_mod == 0) {
            info("[Thread %d] Cycle %d", tid, count);
        }

        if (pool_usage == pool_size) {
            free_flag = 1;
        } else if (pool_usage == 0){
            free_flag = 0;
        }

        if (free_flag) {
            int slot = pool_usage - 1;
            mm_info_t info = hold_pool[slot];
            memset((void *)info.addr, JUNK_1, info.size);
            kfree((void *)info.addr);
            pool_usage--;
            debug("[Thread %d] Free Memory %#lx Size %d", tid, info.addr, info.size);
        } else {
            int slot = pool_usage;
            int size = blk_sizes[i];
            void *addr = kmalloc(blk_sizes[i]);
            hold_pool[slot] = (mm_info_t){.addr=(uint64_t)addr, .size=size};
            pool_usage++;
            memset(addr, JUNK_0, size);
            debug("[Thread %d] Alloc Memory %#lx Size %d", tid, addr, size);
        }

        rand_sleep(rand);
        i = (i + 1) % NELEM(blk_sizes);
    }
    

    pop_off();
}