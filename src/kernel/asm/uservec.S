#define BASE_OFFSET 0
#define STRUCT_OFFSET(x) (x + BASE_OFFSET)

.globl uservec
uservec:    
	#
        # trap.c sets stvec to point here, so
        # traps from user space start here,
        # in supervisor mode, but with a
        # user page table.
        #
        # sscratch points to p.trapframe
        #
        
	# swap a0 and sscratch
        # so that a0 is TRAPFRAME
        csrrw a0, sscratch, a0

        # save the user registers in TRAPFRAME
        sd ra, STRUCT_OFFSET(40)(a0)
        sd sp, STRUCT_OFFSET(48)(a0)
        sd gp, STRUCT_OFFSET(56)(a0)
        sd tp, STRUCT_OFFSET(64)(a0)
        sd t0, STRUCT_OFFSET(72)(a0)
        sd t1, STRUCT_OFFSET(80)(a0)
        sd t2, STRUCT_OFFSET(88)(a0)
        sd s0, STRUCT_OFFSET(96)(a0)
        sd s1, STRUCT_OFFSET(104)(a0)
        sd a1, STRUCT_OFFSET(120)(a0)
        sd a2, STRUCT_OFFSET(128)(a0)
        sd a3, STRUCT_OFFSET(136)(a0)
        sd a4, STRUCT_OFFSET(144)(a0)
        sd a5, STRUCT_OFFSET(152)(a0)
        sd a6, STRUCT_OFFSET(160)(a0)
        sd a7, STRUCT_OFFSET(168)(a0)
        sd s2, STRUCT_OFFSET(176)(a0)
        sd s3, STRUCT_OFFSET(184)(a0)
        sd s4, STRUCT_OFFSET(192)(a0)
        sd s5, STRUCT_OFFSET(200)(a0)
        sd s6, STRUCT_OFFSET(208)(a0)
        sd s7, STRUCT_OFFSET(216)(a0)
        sd s8, STRUCT_OFFSET(224)(a0)
        sd s9, STRUCT_OFFSET(232)(a0)
        sd s10, STRUCT_OFFSET(240)(a0)
        sd s11, STRUCT_OFFSET(248)(a0)
        sd t3, STRUCT_OFFSET(256)(a0)
        sd t4, STRUCT_OFFSET(264)(a0)
        sd t5, STRUCT_OFFSET(272)(a0)
        sd t6, STRUCT_OFFSET(280)(a0)

	# save the user a0 in p->trapframe->a0
        csrr t0, sscratch
        sd t0, STRUCT_OFFSET(112)(a0)

        # restore kernel stack pointer from p->trapframe->kernel_sp
        ld sp, STRUCT_OFFSET(8)(a0)

        # make tp hold the current hartid, from p->trapframe->kernel_hartid
        ld tp, STRUCT_OFFSET(32)(a0)

        # load the proc reference
        ld t0, STRUCT_OFFSET(0)(a0)
        csrw sscratch, t0

        # load the address of usertrap(), p->trapframe->kernel_trap
        ld t0, STRUCT_OFFSET(16)(a0)

        # jump to usertrap(), which does not return
        jr t0

.globl userret
userret:
        # userret(struct trapframe *trapfram)
        # switch from kernel to user.
        # usertrapret() calls here.

        # put the saved user a0 in sscratch, so we
        # can swap it with our a0 (trapframe) in the last step.
        ld t0, STRUCT_OFFSET(112)(a0)
        csrw sscratch, t0

        # restore all but a0 from trapframe
        ld ra, STRUCT_OFFSET(40)(a0)
        ld sp, STRUCT_OFFSET(48)(a0)
        ld gp, STRUCT_OFFSET(56)(a0)
        ld tp, STRUCT_OFFSET(64)(a0)
        ld t0, STRUCT_OFFSET(72)(a0)
        ld t1, STRUCT_OFFSET(80)(a0)
        ld t2, STRUCT_OFFSET(88)(a0)
        ld s0, STRUCT_OFFSET(96)(a0)
        ld s1, STRUCT_OFFSET(104)(a0)
        ld a1, STRUCT_OFFSET(120)(a0)
        ld a2, STRUCT_OFFSET(128)(a0)
        ld a3, STRUCT_OFFSET(136)(a0)
        ld a4, STRUCT_OFFSET(144)(a0)
        ld a5, STRUCT_OFFSET(152)(a0)
        ld a6, STRUCT_OFFSET(160)(a0)
        ld a7, STRUCT_OFFSET(168)(a0)
        ld s2, STRUCT_OFFSET(176)(a0)
        ld s3, STRUCT_OFFSET(184)(a0)
        ld s4, STRUCT_OFFSET(192)(a0)
        ld s5, STRUCT_OFFSET(200)(a0)
        ld s6, STRUCT_OFFSET(208)(a0)
        ld s7, STRUCT_OFFSET(216)(a0)
        ld s8, STRUCT_OFFSET(224)(a0)
        ld s9, STRUCT_OFFSET(232)(a0)
        ld s10, STRUCT_OFFSET(240)(a0)
        ld s11, STRUCT_OFFSET(248)(a0)
        ld t3, STRUCT_OFFSET(256)(a0)
        ld t4, STRUCT_OFFSET(264)(a0)
        ld t5, STRUCT_OFFSET(272)(a0)
        ld t6, STRUCT_OFFSET(280)(a0)

	# restore user a0, and save trapframe in sscratch
        csrrw a0, sscratch, a0
        
        # return to user mode and user pc.
        # usertrapret() set up sstatus and sepc.
        sret
