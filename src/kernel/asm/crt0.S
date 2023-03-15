.section .text.entry
.global _entry
# a1=dtb  a0=hartid
_entry: 
	# set up a stack for C.
        # stack0 is declared in main.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + (hartid * 4096)
        mv tp, a0
#ifdef CPUS
        # check hartid
        li t0, CPUS
        bge tp, t0, set_bad_core
#endif
        # init stack
        la sp, stack0
        li t0, 4096
        addi t1, a0, 1
        mul t0, t0, t1
        add sp, sp, t0

        call main

set_bad_core:
        la t0, bad_core
        li t1, 1
        sw t1, (t0)
spin:
        j spin

.global bad_core
.section .data
bad_core: .word 0