#include "common.h"
#include "mm/io.h"
#include "mm/vm.h"
#include "mm/mmap.h"
#include "riscv.h"

#define QUIET
#define __MODULE_NAME__ IO
#include "debug.h"


extern uint64_t mmap_map_io(mm_t *mm, uint64_t addr, uint64_t len, uint64_t pa, int prot, int pg_spec);
extern mm_t *public_map;

static uint64_t head = IO_BASE;

void *vm_alloc(size_t size);

/* alloc virtual memory for I/O (not RAM) */
void *vm_alloc(size_t size) {
    uint64_t ret = head;
    head = PGROUNDUP_LARGE(head + size) + PGSIZE_LARGE; // 简单起见，虚拟地址以2MB为单位进行分配 间隔2M做为guard page
    return (void *)ret;
}

uint64_t ioremap(uint64_t pa, size_t size) {
    uint64_t rpa = PGROUNDDOWN_LARGE(pa);
    uint64_t end = PGROUNDUP_LARGE(pa + size);
    uint64_t rsize = end -  rpa;
    uint64_t va = (uint64_t)vm_alloc(rsize);
    mmap_map_io(public_map, va, rsize, rpa, PROT_READ|PROT_WRITE, PGSPEC_LARGE);
    debug("map pa@%#lx---->va@%#lx size %#lx", rpa, va, rsize);
    // sfence_vma();
    return va + (pa & (PGSIZE_LARGE - 1)); // pa可能不是页对齐的，因此在映射完毕后还需要加上页内偏移
}