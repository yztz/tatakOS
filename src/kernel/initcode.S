# #include "syscall.h"

# # exec(init, argv)
# .globl entry
# entry:
#         la a0, init
#         la a1, argv
#         li a7, SYS_exec
#         ecall

# # for(;;) exit();
# exit:
#         li a7, SYS_exit
#         ecall
#         jal exit

# # char init[] = "/init\0";
# init:
#   .string "/init\0"

# # char *argv[] = { init, 0 };
# .p2align 2
# argv:
#   .long init
#   .long 0

#include "syscall.h"

# exec(init, argv)
.globl entry
entry:
        la a1, letters
        jal putc
# for(;;)
exit:
        jal exit
putc:
  li a2, 1
  li a0, 1
  li a7, SYS_write
  ecall
  ret

letters:
.string "ABCDEFGHIJKLMNOPQRSTUVWXYZ"