#define syscall(NUM) {\
    asm ("mv a7, %0"::"r"(NUM):"a7");\
    asm ("ecall");}

#include "syscall.h"
void write(int fd, char *addr, int size);
void exec(char *path, char** argv);

// path here
char* path = "/init";

__attribute__((section(".startup"))) void main() {
    syscall(NR_test);
    char *argv[2];
    // set arg
    argv[0] = path;
    argv[1] = 0;
    exec(path, argv);
    for(;;);
}



// void entry() {
//     syscall(NR_test);
//     for(;;);
// }

void exec(char *path, char** argv) {
    syscall(NR_test);
    syscall(NR_exec);
}

void exit() {
    syscall(NR_exit);
}
