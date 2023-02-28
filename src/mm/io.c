#include "common.h"
#include "mm/io.h"
#include "mm/vm.h"
#include "mm/mmap.h"
#include "riscv.h"

#define QUIET
#define __MODULE_NAME__ IO
#include "debug.h"

static uint64_t head = IO_BASE;

void *vm_alloc(size_t size);

/* alloc virtual memory for I/O (not RAM) */
void *vm_alloc(size_t size) {
    uint64_t ret = head;
    head = PGROUNDUP_LARGE(head + size) + 0x200000; // 简单起见，虚拟地址以2MB为单位进行分配 间隔2M做为guard page
    return (void *)ret;
}

uint64 ioremap(uint64_t pa, size_t size) {
    uint64_t va = (uint64_t)vm_alloc(size);
    kvmmap(va, pa, size, PROT_READ | PROT_WRITE, PGSPEC_LARGE);
    debug("map pa@%#lx---->va@%#lx size %#lx", pa, va, size);
    // sfence_vma();
    return va + (pa & (PGSIZE_LARGE - 1)); // pa可能不是页对齐的，因此在映射完毕后还需要加上页内偏移
}