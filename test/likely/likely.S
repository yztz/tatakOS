
likely.o:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <main>:

void foo();
void bar();

int main(char *argv[], int argc)
{
   0:	7179                	addi	sp,sp,-48
   2:	f406                	sd	ra,40(sp)
   4:	f022                	sd	s0,32(sp)
   6:	1800                	addi	s0,sp,48
   8:	fca43c23          	sd	a0,-40(s0)
   c:	87ae                	mv	a5,a1
   e:	fcf42a23          	sw	a5,-44(s0)
   int a = 3;
  12:	478d                	li	a5,3
  14:	fef42623          	sw	a5,-20(s0)
   int b;
   if (likely (argc == 2)) {
  18:	fd442783          	lw	a5,-44(s0)
  1c:	0007871b          	sext.w	a4,a5
  20:	4789                	li	a5,2
  22:	40f707b3          	sub	a5,a4,a5
  26:	0017b793          	seqz	a5,a5
  2a:	0ff7f793          	zext.b	a5,a5
  2e:	cb99                	beqz	a5,44 <.L2>
      foo();
  30:	00000097          	auipc	ra,0x0
  34:	000080e7          	jalr	ra # 30 <main+0x30>
      a++;
  38:	fec42783          	lw	a5,-20(s0)
  3c:	2785                	addiw	a5,a5,1
  3e:	fef42623          	sw	a5,-20(s0)
  42:	a809                	j	54 <.L3>

0000000000000044 <.L2>:
   }
   else {
      bar();
  44:	00000097          	auipc	ra,0x0
  48:	000080e7          	jalr	ra # 44 <.L2>
      b = a;
  4c:	fec42783          	lw	a5,-20(s0)
  50:	fef42423          	sw	a5,-24(s0)

0000000000000054 <.L3>:
  54:	4781                	li	a5,0
   }
  56:	853e                	mv	a0,a5
  58:	70a2                	ld	ra,40(sp)
  5a:	7402                	ld	s0,32(sp)
  5c:	6145                	addi	sp,sp,48
  5e:	8082                	ret
