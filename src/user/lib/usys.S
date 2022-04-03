.macro syscall num name
.global \name
\name:
    li a7, \num
    ecall
    ret
.endm

#define __SYS_CALL(NUM, NAME, FUNC) syscall NUM, NAME
#include "kernel/syscall_gen.h"