#include "syscall.h"

void syscall(int num);
void write(int fd, char *addr, int size);
void exit();

void entry() {
    syscall(99);
    for(;;);
}


void exit() {
    syscall(sys_exit);
}

void syscall(int num) {
    asm ("mv a7, %0"::"r"(num):"x17");
    asm ("ecall");
}