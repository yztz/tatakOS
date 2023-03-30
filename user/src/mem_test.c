#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"

#define PGSIZE 4096

void test_mem(){
    uint64_t cur_pos;
    syscall(NR_memuse);
    cur_pos = brk(0);
    while((cur_pos = brk((void *)(cur_pos + PGSIZE))) != -1) {
        *(uint32_t *)(cur_pos - PGSIZE) = 0xabcd;
    }
    syscall(NR_memuse);
}

int main(void) {
    test_mem();
    return 0;
}