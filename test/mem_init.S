	.file	"mem_init.c"
	.option nopic
	.attribute arch, "rv64i2p0_m2p0_a2p0_f2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.local	pointers
	.comm	pointers,80,8
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-16
	sd	s0,8(sp)
	addi	s0,sp,16
	li	a5,0
	mv	a0,a5
	ld	s0,8(sp)
	addi	sp,sp,16
	jr	ra
	.size	main, .-main
	.ident	"GCC: (gca312387ab1) 10.2.0"
