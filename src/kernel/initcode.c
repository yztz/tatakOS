#include "types.h"
#define syscall(num, arg0, arg1, arg2, arg3) ({ \
    register uint64 a0 asm ("a0") = (uint64)(arg0); \
    register uint64 a1 asm ("a1") = (uint64)(arg1); \
    register uint64 a2 asm ("a2") = (uint64)(arg2); \
    register uint64 a3 asm ("a3") = (uint64)(arg3); \
    register uint64 a7 asm ("a7") = (uint64)(num); \
    asm volatile ("ecall" \
        : "+r" (a0) \
        : "r" (a1), "r" (a2), "r" (a3), "r" (a7) \
        : "memory"); \
    a0; \
})

#include "generated/syscall.h"
void ktest();
void write(int fd, char *addr, int size);
void exec(char *path, char** argv);

// path here
char* path = "/init";

__attribute__((section(".startup"))) void main() {
    __attribute__((unused)) char *argv[2];
    // set arg
    // for(;;);
    argv[0] = path;
    argv[1] = 0;
    ktest();
    exec(path, argv);
    for(;;);
}



// void entry() {
//     syscall(NR_test);
//     for(;;);
// }

void exec(char *path, char** argv) {
    syscall(NR_exec, path, argv, 0, 0);
}

void ktest() {
    syscall(NR_ktest, 0, 0, 0, 0);
}

// void exit() {
//     syscall(NR_exit);
// }
