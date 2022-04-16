#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#define PGSIZE 4096 // bytes per page
#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define IO_BASE_ADDRESS 0xFF00000000L
static uint64_t head = IO_BASE_ADDRESS;
void *vmalloc(size_t size) {
    uint64_t ret = head;
    head += PGROUNDUP(size);
    return (void *)ret;
}

int main() {
    int size;
    void *addr;
    while(1) {
        scanf("%d", &size);
        addr = vmalloc(size);
        printf("addr: 0x%lX\n", (uint64_t)addr);
    }
    scanf("%d", &size);
    return 0;
}

