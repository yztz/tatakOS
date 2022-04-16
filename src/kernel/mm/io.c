#include "io.h"
#include "memlayout.h"
#include "mm.h"
#include "riscv.h"
#include "printf.h"

static uint64_t head = IO_BASE_ADDRESS;

void *vm_alloc(size_t size);

/* alloc virtual memory for I/O (not RAM) */
void *vm_alloc(size_t size) {
    uint64_t ret = head;
    head = PGROUNDUP_LARGE(head + size); // 简单起见，虚拟地址以2MB为单位进行分配
    return (void *)ret;
}

uint64 ioremap(uint64_t pa, size_t size) {
    uint64_t va = (uint64_t)vm_alloc(size);
    // printf("alloc size is: %ld, alloc va is: %p pa is: %p\n", size, va, pa);
    kvmmap(va, pa, size, PTE_R | PTE_W, PGSPEC_LARGE);
    // sfence_vma();
    return va + (pa & (PGSIZE_LARGE - 1)); // pa可能不是页对齐的，因此在映射完毕后还需要加上页内偏移
}