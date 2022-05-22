
build/user_prog/_mmap:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <test_mmap>:
 * "  Hello, mmap success"
 * 测试失败时输出：
 * "mmap error."
 */
static struct kstat kst;
void test_mmap(void){
       0:	fe010113          	addi	sp,sp,-32
       4:	00113c23          	sd	ra,24(sp)
       8:	00813823          	sd	s0,16(sp)
       c:	02010413          	addi	s0,sp,32
    // char *array1;
    // char *array2;
    // char *array3;
    // char *array4;
    // char *array5;
    const char *str = "  Hello, mmap successfully!";
      10:	00001797          	auipc	a5,0x1
      14:	29078793          	addi	a5,a5,656 # 12a0 <timetag+0xc>
      18:	fef43423          	sd	a5,-24(s0)
    int fd;

    // printf(rd("maptest begin !!\n"));
    fd = open("test_mmap.txt", O_RDWR | O_CREATE);
      1c:	04200593          	li	a1,66
      20:	00001517          	auipc	a0,0x1
      24:	2a050513          	addi	a0,a0,672 # 12c0 <timetag+0x2c>
      28:	00001097          	auipc	ra,0x1
      2c:	cc4080e7          	jalr	-828(ra) # cec <open>
      30:	00050793          	mv	a5,a0
      34:	fef42223          	sw	a5,-28(s0)
    write(fd, str, strlen(str));
      38:	fe843503          	ld	a0,-24(s0)
      3c:	00000097          	auipc	ra,0x0
      40:	7bc080e7          	jalr	1980(ra) # 7f8 <strlen>
      44:	00050793          	mv	a5,a0
      48:	0007879b          	sext.w	a5,a5
      4c:	0007871b          	sext.w	a4,a5
      50:	fe442783          	lw	a5,-28(s0)
      54:	00070613          	mv	a2,a4
      58:	fe843583          	ld	a1,-24(s0)
      5c:	00078513          	mv	a0,a5
      60:	00001097          	auipc	ra,0x1
      64:	1c8080e7          	jalr	456(ra) # 1228 <write>
    fstat(fd, &kst);
      68:	fe442783          	lw	a5,-28(s0)
      6c:	00001597          	auipc	a1,0x1
      70:	29458593          	addi	a1,a1,660 # 1300 <kst>
      74:	00078513          	mv	a0,a5
      78:	00001097          	auipc	ra,0x1
      7c:	1bc080e7          	jalr	444(ra) # 1234 <fstat>
    printf("file len: %d\n", kst.st_size);
      80:	00001797          	auipc	a5,0x1
      84:	28078793          	addi	a5,a5,640 # 1300 <kst>
      88:	0307b783          	ld	a5,48(a5)
      8c:	00078593          	mv	a1,a5
      90:	00001517          	auipc	a0,0x1
      94:	24050513          	addi	a0,a0,576 # 12d0 <timetag+0x3c>
      98:	00000097          	auipc	ra,0x0
      9c:	614080e7          	jalr	1556(ra) # 6ac <printf>

	// munmap(array, kst.st_size);
    // }

//   for(;;);
  close(fd);
      a0:	fe442783          	lw	a5,-28(s0)
      a4:	00078513          	mv	a0,a5
      a8:	00001097          	auipc	ra,0x1
      ac:	150080e7          	jalr	336(ra) # 11f8 <close>
//   for(;;);
    // TEST_END(__func__);
}
      b0:	00000013          	nop
      b4:	01813083          	ld	ra,24(sp)
      b8:	01013403          	ld	s0,16(sp)
      bc:	02010113          	addi	sp,sp,32
      c0:	00008067          	ret

00000000000000c4 <main>:

int main(void){
      c4:	ff010113          	addi	sp,sp,-16
      c8:	00113423          	sd	ra,8(sp)
      cc:	00813023          	sd	s0,0(sp)
      d0:	01010413          	addi	s0,sp,16
    test_mmap();
      d4:	00000097          	auipc	ra,0x0
      d8:	f2c080e7          	jalr	-212(ra) # 0 <test_mmap>
    return 0;
      dc:	00000793          	li	a5,0
      e0:	00078513          	mv	a0,a5
      e4:	00813083          	ld	ra,8(sp)
      e8:	00013403          	ld	s0,0(sp)
      ec:	01010113          	addi	sp,sp,16
      f0:	00008067          	ret

00000000000000f4 <putc>:

static char digits[] = "0123456789ABCDEF";

static void
putc(int fd, char c)
{
      f4:	fe010113          	addi	sp,sp,-32
      f8:	00113c23          	sd	ra,24(sp)
      fc:	00813823          	sd	s0,16(sp)
     100:	02010413          	addi	s0,sp,32
     104:	00050793          	mv	a5,a0
     108:	00058713          	mv	a4,a1
     10c:	fef42623          	sw	a5,-20(s0)
     110:	00070793          	mv	a5,a4
     114:	fef405a3          	sb	a5,-21(s0)
  write(fd, &c, 1);
     118:	feb40713          	addi	a4,s0,-21
     11c:	fec42783          	lw	a5,-20(s0)
     120:	00100613          	li	a2,1
     124:	00070593          	mv	a1,a4
     128:	00078513          	mv	a0,a5
     12c:	00001097          	auipc	ra,0x1
     130:	0fc080e7          	jalr	252(ra) # 1228 <write>
}
     134:	00000013          	nop
     138:	01813083          	ld	ra,24(sp)
     13c:	01013403          	ld	s0,16(sp)
     140:	02010113          	addi	sp,sp,32
     144:	00008067          	ret

0000000000000148 <printint>:

static void
printint(int fd, int xx, int base, int sgn)
{
     148:	fc010113          	addi	sp,sp,-64
     14c:	02113c23          	sd	ra,56(sp)
     150:	02813823          	sd	s0,48(sp)
     154:	04010413          	addi	s0,sp,64
     158:	00050793          	mv	a5,a0
     15c:	00068713          	mv	a4,a3
     160:	fcf42623          	sw	a5,-52(s0)
     164:	00058793          	mv	a5,a1
     168:	fcf42423          	sw	a5,-56(s0)
     16c:	00060793          	mv	a5,a2
     170:	fcf42223          	sw	a5,-60(s0)
     174:	00070793          	mv	a5,a4
     178:	fcf42023          	sw	a5,-64(s0)
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
     17c:	fe042423          	sw	zero,-24(s0)
  if(sgn && xx < 0){
     180:	fc042783          	lw	a5,-64(s0)
     184:	0007879b          	sext.w	a5,a5
     188:	02078663          	beqz	a5,1b4 <printint+0x6c>
     18c:	fc842783          	lw	a5,-56(s0)
     190:	0007879b          	sext.w	a5,a5
     194:	0207d063          	bgez	a5,1b4 <printint+0x6c>
    neg = 1;
     198:	00100793          	li	a5,1
     19c:	fef42423          	sw	a5,-24(s0)
    x = -xx;
     1a0:	fc842783          	lw	a5,-56(s0)
     1a4:	40f007bb          	negw	a5,a5
     1a8:	0007879b          	sext.w	a5,a5
     1ac:	fef42223          	sw	a5,-28(s0)
     1b0:	00c0006f          	j	1bc <printint+0x74>
  } else {
    x = xx;
     1b4:	fc842783          	lw	a5,-56(s0)
     1b8:	fef42223          	sw	a5,-28(s0)
  }

  i = 0;
     1bc:	fe042623          	sw	zero,-20(s0)
  do{
    buf[i++] = digits[x % base];
     1c0:	fc442783          	lw	a5,-60(s0)
     1c4:	fe442703          	lw	a4,-28(s0)
     1c8:	02f777bb          	remuw	a5,a4,a5
     1cc:	0007861b          	sext.w	a2,a5
     1d0:	fec42783          	lw	a5,-20(s0)
     1d4:	0017871b          	addiw	a4,a5,1
     1d8:	fee42623          	sw	a4,-20(s0)
     1dc:	00001697          	auipc	a3,0x1
     1e0:	10c68693          	addi	a3,a3,268 # 12e8 <digits>
     1e4:	02061713          	slli	a4,a2,0x20
     1e8:	02075713          	srli	a4,a4,0x20
     1ec:	00e68733          	add	a4,a3,a4
     1f0:	00074703          	lbu	a4,0(a4)
     1f4:	ff078793          	addi	a5,a5,-16
     1f8:	008787b3          	add	a5,a5,s0
     1fc:	fee78023          	sb	a4,-32(a5)
  }while((x /= base) != 0);
     200:	fc442783          	lw	a5,-60(s0)
     204:	fe442703          	lw	a4,-28(s0)
     208:	02f757bb          	divuw	a5,a4,a5
     20c:	fef42223          	sw	a5,-28(s0)
     210:	fe442783          	lw	a5,-28(s0)
     214:	0007879b          	sext.w	a5,a5
     218:	fa0794e3          	bnez	a5,1c0 <printint+0x78>
  if(neg)
     21c:	fe842783          	lw	a5,-24(s0)
     220:	0007879b          	sext.w	a5,a5
     224:	04078463          	beqz	a5,26c <printint+0x124>
    buf[i++] = '-';
     228:	fec42783          	lw	a5,-20(s0)
     22c:	0017871b          	addiw	a4,a5,1
     230:	fee42623          	sw	a4,-20(s0)
     234:	ff078793          	addi	a5,a5,-16
     238:	008787b3          	add	a5,a5,s0
     23c:	02d00713          	li	a4,45
     240:	fee78023          	sb	a4,-32(a5)

  while(--i >= 0)
     244:	0280006f          	j	26c <printint+0x124>
    putc(fd, buf[i]);
     248:	fec42783          	lw	a5,-20(s0)
     24c:	ff078793          	addi	a5,a5,-16
     250:	008787b3          	add	a5,a5,s0
     254:	fe07c703          	lbu	a4,-32(a5)
     258:	fcc42783          	lw	a5,-52(s0)
     25c:	00070593          	mv	a1,a4
     260:	00078513          	mv	a0,a5
     264:	00000097          	auipc	ra,0x0
     268:	e90080e7          	jalr	-368(ra) # f4 <putc>
  while(--i >= 0)
     26c:	fec42783          	lw	a5,-20(s0)
     270:	fff7879b          	addiw	a5,a5,-1
     274:	fef42623          	sw	a5,-20(s0)
     278:	fec42783          	lw	a5,-20(s0)
     27c:	0007879b          	sext.w	a5,a5
     280:	fc07d4e3          	bgez	a5,248 <printint+0x100>
}
     284:	00000013          	nop
     288:	00000013          	nop
     28c:	03813083          	ld	ra,56(sp)
     290:	03013403          	ld	s0,48(sp)
     294:	04010113          	addi	sp,sp,64
     298:	00008067          	ret

000000000000029c <printptr>:

static void
printptr(int fd, uint64 x) {
     29c:	fd010113          	addi	sp,sp,-48
     2a0:	02113423          	sd	ra,40(sp)
     2a4:	02813023          	sd	s0,32(sp)
     2a8:	03010413          	addi	s0,sp,48
     2ac:	00050793          	mv	a5,a0
     2b0:	fcb43823          	sd	a1,-48(s0)
     2b4:	fcf42e23          	sw	a5,-36(s0)
  int i;
  putc(fd, '0');
     2b8:	fdc42783          	lw	a5,-36(s0)
     2bc:	03000593          	li	a1,48
     2c0:	00078513          	mv	a0,a5
     2c4:	00000097          	auipc	ra,0x0
     2c8:	e30080e7          	jalr	-464(ra) # f4 <putc>
  putc(fd, 'x');
     2cc:	fdc42783          	lw	a5,-36(s0)
     2d0:	07800593          	li	a1,120
     2d4:	00078513          	mv	a0,a5
     2d8:	00000097          	auipc	ra,0x0
     2dc:	e1c080e7          	jalr	-484(ra) # f4 <putc>
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
     2e0:	fe042623          	sw	zero,-20(s0)
     2e4:	0480006f          	j	32c <printptr+0x90>
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
     2e8:	fd043783          	ld	a5,-48(s0)
     2ec:	03c7d793          	srli	a5,a5,0x3c
     2f0:	00001717          	auipc	a4,0x1
     2f4:	ff870713          	addi	a4,a4,-8 # 12e8 <digits>
     2f8:	00f707b3          	add	a5,a4,a5
     2fc:	0007c703          	lbu	a4,0(a5)
     300:	fdc42783          	lw	a5,-36(s0)
     304:	00070593          	mv	a1,a4
     308:	00078513          	mv	a0,a5
     30c:	00000097          	auipc	ra,0x0
     310:	de8080e7          	jalr	-536(ra) # f4 <putc>
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
     314:	fec42783          	lw	a5,-20(s0)
     318:	0017879b          	addiw	a5,a5,1
     31c:	fef42623          	sw	a5,-20(s0)
     320:	fd043783          	ld	a5,-48(s0)
     324:	00479793          	slli	a5,a5,0x4
     328:	fcf43823          	sd	a5,-48(s0)
     32c:	fec42783          	lw	a5,-20(s0)
     330:	00078713          	mv	a4,a5
     334:	00f00793          	li	a5,15
     338:	fae7f8e3          	bgeu	a5,a4,2e8 <printptr+0x4c>
}
     33c:	00000013          	nop
     340:	00000013          	nop
     344:	02813083          	ld	ra,40(sp)
     348:	02013403          	ld	s0,32(sp)
     34c:	03010113          	addi	sp,sp,48
     350:	00008067          	ret

0000000000000354 <vprintf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
vprintf(int fd, const char *fmt, va_list ap)
{
     354:	fb010113          	addi	sp,sp,-80
     358:	04113423          	sd	ra,72(sp)
     35c:	04813023          	sd	s0,64(sp)
     360:	05010413          	addi	s0,sp,80
     364:	00050793          	mv	a5,a0
     368:	fcb43023          	sd	a1,-64(s0)
     36c:	fac43c23          	sd	a2,-72(s0)
     370:	fcf42623          	sw	a5,-52(s0)
  char *s;
  int c, i, state;

  state = 0;
     374:	fe042023          	sw	zero,-32(s0)
  for(i = 0; fmt[i]; i++){
     378:	fe042223          	sw	zero,-28(s0)
     37c:	28c0006f          	j	608 <vprintf+0x2b4>
    c = fmt[i] & 0xff;
     380:	fe442783          	lw	a5,-28(s0)
     384:	fc043703          	ld	a4,-64(s0)
     388:	00f707b3          	add	a5,a4,a5
     38c:	0007c783          	lbu	a5,0(a5)
     390:	fcf42e23          	sw	a5,-36(s0)
    if(state == 0){
     394:	fe042783          	lw	a5,-32(s0)
     398:	0007879b          	sext.w	a5,a5
     39c:	04079063          	bnez	a5,3dc <vprintf+0x88>
      if(c == '%'){
     3a0:	fdc42783          	lw	a5,-36(s0)
     3a4:	0007871b          	sext.w	a4,a5
     3a8:	02500793          	li	a5,37
     3ac:	00f71863          	bne	a4,a5,3bc <vprintf+0x68>
        state = '%';
     3b0:	02500793          	li	a5,37
     3b4:	fef42023          	sw	a5,-32(s0)
     3b8:	2440006f          	j	5fc <vprintf+0x2a8>
      } else {
        putc(fd, c);
     3bc:	fdc42783          	lw	a5,-36(s0)
     3c0:	0ff7f713          	zext.b	a4,a5
     3c4:	fcc42783          	lw	a5,-52(s0)
     3c8:	00070593          	mv	a1,a4
     3cc:	00078513          	mv	a0,a5
     3d0:	00000097          	auipc	ra,0x0
     3d4:	d24080e7          	jalr	-732(ra) # f4 <putc>
     3d8:	2240006f          	j	5fc <vprintf+0x2a8>
      }
    } else if(state == '%'){
     3dc:	fe042783          	lw	a5,-32(s0)
     3e0:	0007871b          	sext.w	a4,a5
     3e4:	02500793          	li	a5,37
     3e8:	20f71a63          	bne	a4,a5,5fc <vprintf+0x2a8>
      if(c == 'd'){
     3ec:	fdc42783          	lw	a5,-36(s0)
     3f0:	0007871b          	sext.w	a4,a5
     3f4:	06400793          	li	a5,100
     3f8:	02f71a63          	bne	a4,a5,42c <vprintf+0xd8>
        printint(fd, va_arg(ap, int), 10, 1);
     3fc:	fb843783          	ld	a5,-72(s0)
     400:	00878713          	addi	a4,a5,8
     404:	fae43c23          	sd	a4,-72(s0)
     408:	0007a703          	lw	a4,0(a5)
     40c:	fcc42783          	lw	a5,-52(s0)
     410:	00100693          	li	a3,1
     414:	00a00613          	li	a2,10
     418:	00070593          	mv	a1,a4
     41c:	00078513          	mv	a0,a5
     420:	00000097          	auipc	ra,0x0
     424:	d28080e7          	jalr	-728(ra) # 148 <printint>
     428:	1d00006f          	j	5f8 <vprintf+0x2a4>
      } else if(c == 'l') {
     42c:	fdc42783          	lw	a5,-36(s0)
     430:	0007871b          	sext.w	a4,a5
     434:	06c00793          	li	a5,108
     438:	02f71c63          	bne	a4,a5,470 <vprintf+0x11c>
        printint(fd, va_arg(ap, uint64), 10, 0);
     43c:	fb843783          	ld	a5,-72(s0)
     440:	00878713          	addi	a4,a5,8
     444:	fae43c23          	sd	a4,-72(s0)
     448:	0007b783          	ld	a5,0(a5)
     44c:	0007871b          	sext.w	a4,a5
     450:	fcc42783          	lw	a5,-52(s0)
     454:	00000693          	li	a3,0
     458:	00a00613          	li	a2,10
     45c:	00070593          	mv	a1,a4
     460:	00078513          	mv	a0,a5
     464:	00000097          	auipc	ra,0x0
     468:	ce4080e7          	jalr	-796(ra) # 148 <printint>
     46c:	18c0006f          	j	5f8 <vprintf+0x2a4>
      } else if(c == 'x') {
     470:	fdc42783          	lw	a5,-36(s0)
     474:	0007871b          	sext.w	a4,a5
     478:	07800793          	li	a5,120
     47c:	02f71a63          	bne	a4,a5,4b0 <vprintf+0x15c>
        printint(fd, va_arg(ap, int), 16, 0);
     480:	fb843783          	ld	a5,-72(s0)
     484:	00878713          	addi	a4,a5,8
     488:	fae43c23          	sd	a4,-72(s0)
     48c:	0007a703          	lw	a4,0(a5)
     490:	fcc42783          	lw	a5,-52(s0)
     494:	00000693          	li	a3,0
     498:	01000613          	li	a2,16
     49c:	00070593          	mv	a1,a4
     4a0:	00078513          	mv	a0,a5
     4a4:	00000097          	auipc	ra,0x0
     4a8:	ca4080e7          	jalr	-860(ra) # 148 <printint>
     4ac:	14c0006f          	j	5f8 <vprintf+0x2a4>
      } else if(c == 'p') {
     4b0:	fdc42783          	lw	a5,-36(s0)
     4b4:	0007871b          	sext.w	a4,a5
     4b8:	07000793          	li	a5,112
     4bc:	02f71663          	bne	a4,a5,4e8 <vprintf+0x194>
        printptr(fd, va_arg(ap, uint64));
     4c0:	fb843783          	ld	a5,-72(s0)
     4c4:	00878713          	addi	a4,a5,8
     4c8:	fae43c23          	sd	a4,-72(s0)
     4cc:	0007b703          	ld	a4,0(a5)
     4d0:	fcc42783          	lw	a5,-52(s0)
     4d4:	00070593          	mv	a1,a4
     4d8:	00078513          	mv	a0,a5
     4dc:	00000097          	auipc	ra,0x0
     4e0:	dc0080e7          	jalr	-576(ra) # 29c <printptr>
     4e4:	1140006f          	j	5f8 <vprintf+0x2a4>
      } else if(c == 's'){
     4e8:	fdc42783          	lw	a5,-36(s0)
     4ec:	0007871b          	sext.w	a4,a5
     4f0:	07300793          	li	a5,115
     4f4:	06f71463          	bne	a4,a5,55c <vprintf+0x208>
        s = va_arg(ap, char*);
     4f8:	fb843783          	ld	a5,-72(s0)
     4fc:	00878713          	addi	a4,a5,8
     500:	fae43c23          	sd	a4,-72(s0)
     504:	0007b783          	ld	a5,0(a5)
     508:	fef43423          	sd	a5,-24(s0)
        if(s == 0)
     50c:	fe843783          	ld	a5,-24(s0)
     510:	02079e63          	bnez	a5,54c <vprintf+0x1f8>
          s = "(null)";
     514:	00001797          	auipc	a5,0x1
     518:	dcc78793          	addi	a5,a5,-564 # 12e0 <timetag+0x4c>
     51c:	fef43423          	sd	a5,-24(s0)
        while(*s != 0){
     520:	02c0006f          	j	54c <vprintf+0x1f8>
          putc(fd, *s);
     524:	fe843783          	ld	a5,-24(s0)
     528:	0007c703          	lbu	a4,0(a5)
     52c:	fcc42783          	lw	a5,-52(s0)
     530:	00070593          	mv	a1,a4
     534:	00078513          	mv	a0,a5
     538:	00000097          	auipc	ra,0x0
     53c:	bbc080e7          	jalr	-1092(ra) # f4 <putc>
          s++;
     540:	fe843783          	ld	a5,-24(s0)
     544:	00178793          	addi	a5,a5,1
     548:	fef43423          	sd	a5,-24(s0)
        while(*s != 0){
     54c:	fe843783          	ld	a5,-24(s0)
     550:	0007c783          	lbu	a5,0(a5)
     554:	fc0798e3          	bnez	a5,524 <vprintf+0x1d0>
     558:	0a00006f          	j	5f8 <vprintf+0x2a4>
        }
      } else if(c == 'c'){
     55c:	fdc42783          	lw	a5,-36(s0)
     560:	0007871b          	sext.w	a4,a5
     564:	06300793          	li	a5,99
     568:	02f71863          	bne	a4,a5,598 <vprintf+0x244>
        putc(fd, va_arg(ap, uint));
     56c:	fb843783          	ld	a5,-72(s0)
     570:	00878713          	addi	a4,a5,8
     574:	fae43c23          	sd	a4,-72(s0)
     578:	0007a783          	lw	a5,0(a5)
     57c:	0ff7f713          	zext.b	a4,a5
     580:	fcc42783          	lw	a5,-52(s0)
     584:	00070593          	mv	a1,a4
     588:	00078513          	mv	a0,a5
     58c:	00000097          	auipc	ra,0x0
     590:	b68080e7          	jalr	-1176(ra) # f4 <putc>
     594:	0640006f          	j	5f8 <vprintf+0x2a4>
      } else if(c == '%'){
     598:	fdc42783          	lw	a5,-36(s0)
     59c:	0007871b          	sext.w	a4,a5
     5a0:	02500793          	li	a5,37
     5a4:	02f71263          	bne	a4,a5,5c8 <vprintf+0x274>
        putc(fd, c);
     5a8:	fdc42783          	lw	a5,-36(s0)
     5ac:	0ff7f713          	zext.b	a4,a5
     5b0:	fcc42783          	lw	a5,-52(s0)
     5b4:	00070593          	mv	a1,a4
     5b8:	00078513          	mv	a0,a5
     5bc:	00000097          	auipc	ra,0x0
     5c0:	b38080e7          	jalr	-1224(ra) # f4 <putc>
     5c4:	0340006f          	j	5f8 <vprintf+0x2a4>
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
     5c8:	fcc42783          	lw	a5,-52(s0)
     5cc:	02500593          	li	a1,37
     5d0:	00078513          	mv	a0,a5
     5d4:	00000097          	auipc	ra,0x0
     5d8:	b20080e7          	jalr	-1248(ra) # f4 <putc>
        putc(fd, c);
     5dc:	fdc42783          	lw	a5,-36(s0)
     5e0:	0ff7f713          	zext.b	a4,a5
     5e4:	fcc42783          	lw	a5,-52(s0)
     5e8:	00070593          	mv	a1,a4
     5ec:	00078513          	mv	a0,a5
     5f0:	00000097          	auipc	ra,0x0
     5f4:	b04080e7          	jalr	-1276(ra) # f4 <putc>
      }
      state = 0;
     5f8:	fe042023          	sw	zero,-32(s0)
  for(i = 0; fmt[i]; i++){
     5fc:	fe442783          	lw	a5,-28(s0)
     600:	0017879b          	addiw	a5,a5,1
     604:	fef42223          	sw	a5,-28(s0)
     608:	fe442783          	lw	a5,-28(s0)
     60c:	fc043703          	ld	a4,-64(s0)
     610:	00f707b3          	add	a5,a4,a5
     614:	0007c783          	lbu	a5,0(a5)
     618:	d60794e3          	bnez	a5,380 <vprintf+0x2c>
    }
  }
}
     61c:	00000013          	nop
     620:	00000013          	nop
     624:	04813083          	ld	ra,72(sp)
     628:	04013403          	ld	s0,64(sp)
     62c:	05010113          	addi	sp,sp,80
     630:	00008067          	ret

0000000000000634 <fprintf>:

void
fprintf(int fd, const char *fmt, ...)
{
     634:	f9010113          	addi	sp,sp,-112
     638:	02113c23          	sd	ra,56(sp)
     63c:	02813823          	sd	s0,48(sp)
     640:	04010413          	addi	s0,sp,64
     644:	fcb43823          	sd	a1,-48(s0)
     648:	00c43023          	sd	a2,0(s0)
     64c:	00d43423          	sd	a3,8(s0)
     650:	00e43823          	sd	a4,16(s0)
     654:	00f43c23          	sd	a5,24(s0)
     658:	03043023          	sd	a6,32(s0)
     65c:	03143423          	sd	a7,40(s0)
     660:	00050793          	mv	a5,a0
     664:	fcf42e23          	sw	a5,-36(s0)
  va_list ap;

  va_start(ap, fmt);
     668:	03040793          	addi	a5,s0,48
     66c:	fcf43423          	sd	a5,-56(s0)
     670:	fc843783          	ld	a5,-56(s0)
     674:	fd078793          	addi	a5,a5,-48
     678:	fef43423          	sd	a5,-24(s0)
  vprintf(fd, fmt, ap);
     67c:	fe843703          	ld	a4,-24(s0)
     680:	fdc42783          	lw	a5,-36(s0)
     684:	00070613          	mv	a2,a4
     688:	fd043583          	ld	a1,-48(s0)
     68c:	00078513          	mv	a0,a5
     690:	00000097          	auipc	ra,0x0
     694:	cc4080e7          	jalr	-828(ra) # 354 <vprintf>
}
     698:	00000013          	nop
     69c:	03813083          	ld	ra,56(sp)
     6a0:	03013403          	ld	s0,48(sp)
     6a4:	07010113          	addi	sp,sp,112
     6a8:	00008067          	ret

00000000000006ac <printf>:

void
printf(const char *fmt, ...)
{
     6ac:	f9010113          	addi	sp,sp,-112
     6b0:	02113423          	sd	ra,40(sp)
     6b4:	02813023          	sd	s0,32(sp)
     6b8:	03010413          	addi	s0,sp,48
     6bc:	fca43c23          	sd	a0,-40(s0)
     6c0:	00b43423          	sd	a1,8(s0)
     6c4:	00c43823          	sd	a2,16(s0)
     6c8:	00d43c23          	sd	a3,24(s0)
     6cc:	02e43023          	sd	a4,32(s0)
     6d0:	02f43423          	sd	a5,40(s0)
     6d4:	03043823          	sd	a6,48(s0)
     6d8:	03143c23          	sd	a7,56(s0)
  va_list ap;

  va_start(ap, fmt);
     6dc:	04040793          	addi	a5,s0,64
     6e0:	fcf43823          	sd	a5,-48(s0)
     6e4:	fd043783          	ld	a5,-48(s0)
     6e8:	fc878793          	addi	a5,a5,-56
     6ec:	fef43423          	sd	a5,-24(s0)
  vprintf(1, fmt, ap);
     6f0:	fe843783          	ld	a5,-24(s0)
     6f4:	00078613          	mv	a2,a5
     6f8:	fd843583          	ld	a1,-40(s0)
     6fc:	00100513          	li	a0,1
     700:	00000097          	auipc	ra,0x0
     704:	c54080e7          	jalr	-940(ra) # 354 <vprintf>
}
     708:	00000013          	nop
     70c:	02813083          	ld	ra,40(sp)
     710:	02013403          	ld	s0,32(sp)
     714:	07010113          	addi	sp,sp,112
     718:	00008067          	ret

000000000000071c <strcpy>:
#include "fs/fcntl.h"
#include "user.h"

char*
strcpy(char *s, const char *t)
{
     71c:	fd010113          	addi	sp,sp,-48
     720:	02813423          	sd	s0,40(sp)
     724:	03010413          	addi	s0,sp,48
     728:	fca43c23          	sd	a0,-40(s0)
     72c:	fcb43823          	sd	a1,-48(s0)
  char *os;

  os = s;
     730:	fd843783          	ld	a5,-40(s0)
     734:	fef43423          	sd	a5,-24(s0)
  while((*s++ = *t++) != 0)
     738:	00000013          	nop
     73c:	fd043703          	ld	a4,-48(s0)
     740:	00170793          	addi	a5,a4,1
     744:	fcf43823          	sd	a5,-48(s0)
     748:	fd843783          	ld	a5,-40(s0)
     74c:	00178693          	addi	a3,a5,1
     750:	fcd43c23          	sd	a3,-40(s0)
     754:	00074703          	lbu	a4,0(a4)
     758:	00e78023          	sb	a4,0(a5)
     75c:	0007c783          	lbu	a5,0(a5)
     760:	fc079ee3          	bnez	a5,73c <strcpy+0x20>
    ;
  return os;
     764:	fe843783          	ld	a5,-24(s0)
}
     768:	00078513          	mv	a0,a5
     76c:	02813403          	ld	s0,40(sp)
     770:	03010113          	addi	sp,sp,48
     774:	00008067          	ret

0000000000000778 <strcmp>:

int
strcmp(const char *p, const char *q)
{
     778:	fe010113          	addi	sp,sp,-32
     77c:	00813c23          	sd	s0,24(sp)
     780:	02010413          	addi	s0,sp,32
     784:	fea43423          	sd	a0,-24(s0)
     788:	feb43023          	sd	a1,-32(s0)
  while(*p && *p == *q)
     78c:	01c0006f          	j	7a8 <strcmp+0x30>
    p++, q++;
     790:	fe843783          	ld	a5,-24(s0)
     794:	00178793          	addi	a5,a5,1
     798:	fef43423          	sd	a5,-24(s0)
     79c:	fe043783          	ld	a5,-32(s0)
     7a0:	00178793          	addi	a5,a5,1
     7a4:	fef43023          	sd	a5,-32(s0)
  while(*p && *p == *q)
     7a8:	fe843783          	ld	a5,-24(s0)
     7ac:	0007c783          	lbu	a5,0(a5)
     7b0:	00078c63          	beqz	a5,7c8 <strcmp+0x50>
     7b4:	fe843783          	ld	a5,-24(s0)
     7b8:	0007c703          	lbu	a4,0(a5)
     7bc:	fe043783          	ld	a5,-32(s0)
     7c0:	0007c783          	lbu	a5,0(a5)
     7c4:	fcf706e3          	beq	a4,a5,790 <strcmp+0x18>
  return (uchar)*p - (uchar)*q;
     7c8:	fe843783          	ld	a5,-24(s0)
     7cc:	0007c783          	lbu	a5,0(a5)
     7d0:	0007871b          	sext.w	a4,a5
     7d4:	fe043783          	ld	a5,-32(s0)
     7d8:	0007c783          	lbu	a5,0(a5)
     7dc:	0007879b          	sext.w	a5,a5
     7e0:	40f707bb          	subw	a5,a4,a5
     7e4:	0007879b          	sext.w	a5,a5
}
     7e8:	00078513          	mv	a0,a5
     7ec:	01813403          	ld	s0,24(sp)
     7f0:	02010113          	addi	sp,sp,32
     7f4:	00008067          	ret

00000000000007f8 <strlen>:

uint
strlen(const char *s)
{
     7f8:	fd010113          	addi	sp,sp,-48
     7fc:	02813423          	sd	s0,40(sp)
     800:	03010413          	addi	s0,sp,48
     804:	fca43c23          	sd	a0,-40(s0)
  int n;

  for(n = 0; s[n]; n++)
     808:	fe042623          	sw	zero,-20(s0)
     80c:	0100006f          	j	81c <strlen+0x24>
     810:	fec42783          	lw	a5,-20(s0)
     814:	0017879b          	addiw	a5,a5,1
     818:	fef42623          	sw	a5,-20(s0)
     81c:	fec42783          	lw	a5,-20(s0)
     820:	fd843703          	ld	a4,-40(s0)
     824:	00f707b3          	add	a5,a4,a5
     828:	0007c783          	lbu	a5,0(a5)
     82c:	fe0792e3          	bnez	a5,810 <strlen+0x18>
    ;
  return n;
     830:	fec42783          	lw	a5,-20(s0)
}
     834:	00078513          	mv	a0,a5
     838:	02813403          	ld	s0,40(sp)
     83c:	03010113          	addi	sp,sp,48
     840:	00008067          	ret

0000000000000844 <memset>:

void*
memset(void *dst, int c, uint n)
{
     844:	fd010113          	addi	sp,sp,-48
     848:	02813423          	sd	s0,40(sp)
     84c:	03010413          	addi	s0,sp,48
     850:	fca43c23          	sd	a0,-40(s0)
     854:	00058793          	mv	a5,a1
     858:	00060713          	mv	a4,a2
     85c:	fcf42a23          	sw	a5,-44(s0)
     860:	00070793          	mv	a5,a4
     864:	fcf42823          	sw	a5,-48(s0)
  char *cdst = (char *) dst;
     868:	fd843783          	ld	a5,-40(s0)
     86c:	fef43023          	sd	a5,-32(s0)
  int i;
  for(i = 0; i < n; i++){
     870:	fe042623          	sw	zero,-20(s0)
     874:	0280006f          	j	89c <memset+0x58>
    cdst[i] = c;
     878:	fec42783          	lw	a5,-20(s0)
     87c:	fe043703          	ld	a4,-32(s0)
     880:	00f707b3          	add	a5,a4,a5
     884:	fd442703          	lw	a4,-44(s0)
     888:	0ff77713          	zext.b	a4,a4
     88c:	00e78023          	sb	a4,0(a5)
  for(i = 0; i < n; i++){
     890:	fec42783          	lw	a5,-20(s0)
     894:	0017879b          	addiw	a5,a5,1
     898:	fef42623          	sw	a5,-20(s0)
     89c:	fec42703          	lw	a4,-20(s0)
     8a0:	fd042783          	lw	a5,-48(s0)
     8a4:	0007879b          	sext.w	a5,a5
     8a8:	fcf768e3          	bltu	a4,a5,878 <memset+0x34>
  }
  return dst;
     8ac:	fd843783          	ld	a5,-40(s0)
}
     8b0:	00078513          	mv	a0,a5
     8b4:	02813403          	ld	s0,40(sp)
     8b8:	03010113          	addi	sp,sp,48
     8bc:	00008067          	ret

00000000000008c0 <strchr>:

char*
strchr(const char *s, char c)
{
     8c0:	fe010113          	addi	sp,sp,-32
     8c4:	00813c23          	sd	s0,24(sp)
     8c8:	02010413          	addi	s0,sp,32
     8cc:	fea43423          	sd	a0,-24(s0)
     8d0:	00058793          	mv	a5,a1
     8d4:	fef403a3          	sb	a5,-25(s0)
  for(; *s; s++)
     8d8:	02c0006f          	j	904 <strchr+0x44>
    if(*s == c)
     8dc:	fe843783          	ld	a5,-24(s0)
     8e0:	0007c703          	lbu	a4,0(a5)
     8e4:	fe744783          	lbu	a5,-25(s0)
     8e8:	0ff7f793          	zext.b	a5,a5
     8ec:	00e79663          	bne	a5,a4,8f8 <strchr+0x38>
      return (char*)s;
     8f0:	fe843783          	ld	a5,-24(s0)
     8f4:	0200006f          	j	914 <strchr+0x54>
  for(; *s; s++)
     8f8:	fe843783          	ld	a5,-24(s0)
     8fc:	00178793          	addi	a5,a5,1
     900:	fef43423          	sd	a5,-24(s0)
     904:	fe843783          	ld	a5,-24(s0)
     908:	0007c783          	lbu	a5,0(a5)
     90c:	fc0798e3          	bnez	a5,8dc <strchr+0x1c>
  return 0;
     910:	00000793          	li	a5,0
}
     914:	00078513          	mv	a0,a5
     918:	01813403          	ld	s0,24(sp)
     91c:	02010113          	addi	sp,sp,32
     920:	00008067          	ret

0000000000000924 <gets>:

char*
gets(char *buf, int max)
{
     924:	fd010113          	addi	sp,sp,-48
     928:	02113423          	sd	ra,40(sp)
     92c:	02813023          	sd	s0,32(sp)
     930:	03010413          	addi	s0,sp,48
     934:	fca43c23          	sd	a0,-40(s0)
     938:	00058793          	mv	a5,a1
     93c:	fcf42a23          	sw	a5,-44(s0)
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
     940:	fe042623          	sw	zero,-20(s0)
     944:	0700006f          	j	9b4 <gets+0x90>
    cc = read(0, &c, 1);
     948:	fe740793          	addi	a5,s0,-25
     94c:	00100613          	li	a2,1
     950:	00078593          	mv	a1,a5
     954:	00000513          	li	a0,0
     958:	00001097          	auipc	ra,0x1
     95c:	8c4080e7          	jalr	-1852(ra) # 121c <read>
     960:	00050793          	mv	a5,a0
     964:	fef42423          	sw	a5,-24(s0)
    if(cc < 1)
     968:	fe842783          	lw	a5,-24(s0)
     96c:	0007879b          	sext.w	a5,a5
     970:	06f05063          	blez	a5,9d0 <gets+0xac>
      break;
    buf[i++] = c;
     974:	fec42783          	lw	a5,-20(s0)
     978:	0017871b          	addiw	a4,a5,1
     97c:	fee42623          	sw	a4,-20(s0)
     980:	00078713          	mv	a4,a5
     984:	fd843783          	ld	a5,-40(s0)
     988:	00e787b3          	add	a5,a5,a4
     98c:	fe744703          	lbu	a4,-25(s0)
     990:	00e78023          	sb	a4,0(a5)
    if(c == '\n' || c == '\r')
     994:	fe744783          	lbu	a5,-25(s0)
     998:	00078713          	mv	a4,a5
     99c:	00a00793          	li	a5,10
     9a0:	02f70a63          	beq	a4,a5,9d4 <gets+0xb0>
     9a4:	fe744783          	lbu	a5,-25(s0)
     9a8:	00078713          	mv	a4,a5
     9ac:	00d00793          	li	a5,13
     9b0:	02f70263          	beq	a4,a5,9d4 <gets+0xb0>
  for(i=0; i+1 < max; ){
     9b4:	fec42783          	lw	a5,-20(s0)
     9b8:	0017879b          	addiw	a5,a5,1
     9bc:	0007871b          	sext.w	a4,a5
     9c0:	fd442783          	lw	a5,-44(s0)
     9c4:	0007879b          	sext.w	a5,a5
     9c8:	f8f740e3          	blt	a4,a5,948 <gets+0x24>
     9cc:	0080006f          	j	9d4 <gets+0xb0>
      break;
     9d0:	00000013          	nop
      break;
  }
  buf[i] = '\0';
     9d4:	fec42783          	lw	a5,-20(s0)
     9d8:	fd843703          	ld	a4,-40(s0)
     9dc:	00f707b3          	add	a5,a4,a5
     9e0:	00078023          	sb	zero,0(a5)
  return buf;
     9e4:	fd843783          	ld	a5,-40(s0)
}
     9e8:	00078513          	mv	a0,a5
     9ec:	02813083          	ld	ra,40(sp)
     9f0:	02013403          	ld	s0,32(sp)
     9f4:	03010113          	addi	sp,sp,48
     9f8:	00008067          	ret

00000000000009fc <stat>:

int
stat(const char *n, struct kstat *st)
{
     9fc:	fd010113          	addi	sp,sp,-48
     a00:	02113423          	sd	ra,40(sp)
     a04:	02813023          	sd	s0,32(sp)
     a08:	03010413          	addi	s0,sp,48
     a0c:	fca43c23          	sd	a0,-40(s0)
     a10:	fcb43823          	sd	a1,-48(s0)
  int fd;
  int r;

  fd = open(n, O_RDONLY);
     a14:	00000593          	li	a1,0
     a18:	fd843503          	ld	a0,-40(s0)
     a1c:	00000097          	auipc	ra,0x0
     a20:	2d0080e7          	jalr	720(ra) # cec <open>
     a24:	00050793          	mv	a5,a0
     a28:	fef42623          	sw	a5,-20(s0)
  if(fd < 0)
     a2c:	fec42783          	lw	a5,-20(s0)
     a30:	0007879b          	sext.w	a5,a5
     a34:	0007d663          	bgez	a5,a40 <stat+0x44>
    return -1;
     a38:	fff00793          	li	a5,-1
     a3c:	0340006f          	j	a70 <stat+0x74>
  r = fstat(fd, st);
     a40:	fec42783          	lw	a5,-20(s0)
     a44:	fd043583          	ld	a1,-48(s0)
     a48:	00078513          	mv	a0,a5
     a4c:	00000097          	auipc	ra,0x0
     a50:	7e8080e7          	jalr	2024(ra) # 1234 <fstat>
     a54:	00050793          	mv	a5,a0
     a58:	fef42423          	sw	a5,-24(s0)
  close(fd);
     a5c:	fec42783          	lw	a5,-20(s0)
     a60:	00078513          	mv	a0,a5
     a64:	00000097          	auipc	ra,0x0
     a68:	794080e7          	jalr	1940(ra) # 11f8 <close>
  return r;
     a6c:	fe842783          	lw	a5,-24(s0)
}
     a70:	00078513          	mv	a0,a5
     a74:	02813083          	ld	ra,40(sp)
     a78:	02013403          	ld	s0,32(sp)
     a7c:	03010113          	addi	sp,sp,48
     a80:	00008067          	ret

0000000000000a84 <atoi>:

int
atoi(const char *s)
{
     a84:	fd010113          	addi	sp,sp,-48
     a88:	02813423          	sd	s0,40(sp)
     a8c:	03010413          	addi	s0,sp,48
     a90:	fca43c23          	sd	a0,-40(s0)
  int n;

  n = 0;
     a94:	fe042623          	sw	zero,-20(s0)
  while('0' <= *s && *s <= '9')
     a98:	0440006f          	j	adc <atoi+0x58>
    n = n*10 + *s++ - '0';
     a9c:	fec42783          	lw	a5,-20(s0)
     aa0:	00078713          	mv	a4,a5
     aa4:	00070793          	mv	a5,a4
     aa8:	0027979b          	slliw	a5,a5,0x2
     aac:	00e787bb          	addw	a5,a5,a4
     ab0:	0017979b          	slliw	a5,a5,0x1
     ab4:	0007871b          	sext.w	a4,a5
     ab8:	fd843783          	ld	a5,-40(s0)
     abc:	00178693          	addi	a3,a5,1
     ac0:	fcd43c23          	sd	a3,-40(s0)
     ac4:	0007c783          	lbu	a5,0(a5)
     ac8:	0007879b          	sext.w	a5,a5
     acc:	00f707bb          	addw	a5,a4,a5
     ad0:	0007879b          	sext.w	a5,a5
     ad4:	fd07879b          	addiw	a5,a5,-48
     ad8:	fef42623          	sw	a5,-20(s0)
  while('0' <= *s && *s <= '9')
     adc:	fd843783          	ld	a5,-40(s0)
     ae0:	0007c783          	lbu	a5,0(a5)
     ae4:	00078713          	mv	a4,a5
     ae8:	02f00793          	li	a5,47
     aec:	00e7fc63          	bgeu	a5,a4,b04 <atoi+0x80>
     af0:	fd843783          	ld	a5,-40(s0)
     af4:	0007c783          	lbu	a5,0(a5)
     af8:	00078713          	mv	a4,a5
     afc:	03900793          	li	a5,57
     b00:	f8e7fee3          	bgeu	a5,a4,a9c <atoi+0x18>
  return n;
     b04:	fec42783          	lw	a5,-20(s0)
}
     b08:	00078513          	mv	a0,a5
     b0c:	02813403          	ld	s0,40(sp)
     b10:	03010113          	addi	sp,sp,48
     b14:	00008067          	ret

0000000000000b18 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
     b18:	fc010113          	addi	sp,sp,-64
     b1c:	02813c23          	sd	s0,56(sp)
     b20:	04010413          	addi	s0,sp,64
     b24:	fca43c23          	sd	a0,-40(s0)
     b28:	fcb43823          	sd	a1,-48(s0)
     b2c:	00060793          	mv	a5,a2
     b30:	fcf42623          	sw	a5,-52(s0)
  char *dst;
  const char *src;

  dst = vdst;
     b34:	fd843783          	ld	a5,-40(s0)
     b38:	fef43423          	sd	a5,-24(s0)
  src = vsrc;
     b3c:	fd043783          	ld	a5,-48(s0)
     b40:	fef43023          	sd	a5,-32(s0)
  if (src > dst) {
     b44:	fe043703          	ld	a4,-32(s0)
     b48:	fe843783          	ld	a5,-24(s0)
     b4c:	02e7fe63          	bgeu	a5,a4,b88 <memmove+0x70>
    while(n-- > 0)
     b50:	0240006f          	j	b74 <memmove+0x5c>
      *dst++ = *src++;
     b54:	fe043703          	ld	a4,-32(s0)
     b58:	00170793          	addi	a5,a4,1
     b5c:	fef43023          	sd	a5,-32(s0)
     b60:	fe843783          	ld	a5,-24(s0)
     b64:	00178693          	addi	a3,a5,1
     b68:	fed43423          	sd	a3,-24(s0)
     b6c:	00074703          	lbu	a4,0(a4)
     b70:	00e78023          	sb	a4,0(a5)
    while(n-- > 0)
     b74:	fcc42783          	lw	a5,-52(s0)
     b78:	fff7871b          	addiw	a4,a5,-1
     b7c:	fce42623          	sw	a4,-52(s0)
     b80:	fcf04ae3          	bgtz	a5,b54 <memmove+0x3c>
     b84:	0600006f          	j	be4 <memmove+0xcc>
  } else {
    dst += n;
     b88:	fcc42783          	lw	a5,-52(s0)
     b8c:	fe843703          	ld	a4,-24(s0)
     b90:	00f707b3          	add	a5,a4,a5
     b94:	fef43423          	sd	a5,-24(s0)
    src += n;
     b98:	fcc42783          	lw	a5,-52(s0)
     b9c:	fe043703          	ld	a4,-32(s0)
     ba0:	00f707b3          	add	a5,a4,a5
     ba4:	fef43023          	sd	a5,-32(s0)
    while(n-- > 0)
     ba8:	02c0006f          	j	bd4 <memmove+0xbc>
      *--dst = *--src;
     bac:	fe043783          	ld	a5,-32(s0)
     bb0:	fff78793          	addi	a5,a5,-1
     bb4:	fef43023          	sd	a5,-32(s0)
     bb8:	fe843783          	ld	a5,-24(s0)
     bbc:	fff78793          	addi	a5,a5,-1
     bc0:	fef43423          	sd	a5,-24(s0)
     bc4:	fe043783          	ld	a5,-32(s0)
     bc8:	0007c703          	lbu	a4,0(a5)
     bcc:	fe843783          	ld	a5,-24(s0)
     bd0:	00e78023          	sb	a4,0(a5)
    while(n-- > 0)
     bd4:	fcc42783          	lw	a5,-52(s0)
     bd8:	fff7871b          	addiw	a4,a5,-1
     bdc:	fce42623          	sw	a4,-52(s0)
     be0:	fcf046e3          	bgtz	a5,bac <memmove+0x94>
  }
  return vdst;
     be4:	fd843783          	ld	a5,-40(s0)
}
     be8:	00078513          	mv	a0,a5
     bec:	03813403          	ld	s0,56(sp)
     bf0:	04010113          	addi	sp,sp,64
     bf4:	00008067          	ret

0000000000000bf8 <memcmp>:

int
memcmp(const void *s1, const void *s2, uint n)
{
     bf8:	fc010113          	addi	sp,sp,-64
     bfc:	02813c23          	sd	s0,56(sp)
     c00:	04010413          	addi	s0,sp,64
     c04:	fca43c23          	sd	a0,-40(s0)
     c08:	fcb43823          	sd	a1,-48(s0)
     c0c:	00060793          	mv	a5,a2
     c10:	fcf42623          	sw	a5,-52(s0)
  const char *p1 = s1, *p2 = s2;
     c14:	fd843783          	ld	a5,-40(s0)
     c18:	fef43423          	sd	a5,-24(s0)
     c1c:	fd043783          	ld	a5,-48(s0)
     c20:	fef43023          	sd	a5,-32(s0)
  while (n-- > 0) {
     c24:	0540006f          	j	c78 <memcmp+0x80>
    if (*p1 != *p2) {
     c28:	fe843783          	ld	a5,-24(s0)
     c2c:	0007c703          	lbu	a4,0(a5)
     c30:	fe043783          	ld	a5,-32(s0)
     c34:	0007c783          	lbu	a5,0(a5)
     c38:	02f70463          	beq	a4,a5,c60 <memcmp+0x68>
      return *p1 - *p2;
     c3c:	fe843783          	ld	a5,-24(s0)
     c40:	0007c783          	lbu	a5,0(a5)
     c44:	0007871b          	sext.w	a4,a5
     c48:	fe043783          	ld	a5,-32(s0)
     c4c:	0007c783          	lbu	a5,0(a5)
     c50:	0007879b          	sext.w	a5,a5
     c54:	40f707bb          	subw	a5,a4,a5
     c58:	0007879b          	sext.w	a5,a5
     c5c:	0300006f          	j	c8c <memcmp+0x94>
    }
    p1++;
     c60:	fe843783          	ld	a5,-24(s0)
     c64:	00178793          	addi	a5,a5,1
     c68:	fef43423          	sd	a5,-24(s0)
    p2++;
     c6c:	fe043783          	ld	a5,-32(s0)
     c70:	00178793          	addi	a5,a5,1
     c74:	fef43023          	sd	a5,-32(s0)
  while (n-- > 0) {
     c78:	fcc42783          	lw	a5,-52(s0)
     c7c:	fff7871b          	addiw	a4,a5,-1
     c80:	fce42623          	sw	a4,-52(s0)
     c84:	fa0792e3          	bnez	a5,c28 <memcmp+0x30>
  }
  return 0;
     c88:	00000793          	li	a5,0
}
     c8c:	00078513          	mv	a0,a5
     c90:	03813403          	ld	s0,56(sp)
     c94:	04010113          	addi	sp,sp,64
     c98:	00008067          	ret

0000000000000c9c <memcpy>:

void *
memcpy(void *dst, const void *src, uint n)
{
     c9c:	fd010113          	addi	sp,sp,-48
     ca0:	02113423          	sd	ra,40(sp)
     ca4:	02813023          	sd	s0,32(sp)
     ca8:	03010413          	addi	s0,sp,48
     cac:	fea43423          	sd	a0,-24(s0)
     cb0:	feb43023          	sd	a1,-32(s0)
     cb4:	00060793          	mv	a5,a2
     cb8:	fcf42e23          	sw	a5,-36(s0)
  return memmove(dst, src, n);
     cbc:	fdc42783          	lw	a5,-36(s0)
     cc0:	00078613          	mv	a2,a5
     cc4:	fe043583          	ld	a1,-32(s0)
     cc8:	fe843503          	ld	a0,-24(s0)
     ccc:	00000097          	auipc	ra,0x0
     cd0:	e4c080e7          	jalr	-436(ra) # b18 <memmove>
     cd4:	00050793          	mv	a5,a0
}
     cd8:	00078513          	mv	a0,a5
     cdc:	02813083          	ld	ra,40(sp)
     ce0:	02013403          	ld	s0,32(sp)
     ce4:	03010113          	addi	sp,sp,48
     ce8:	00008067          	ret

0000000000000cec <open>:

int open(const char *filename, int flag) {
     cec:	fe010113          	addi	sp,sp,-32
     cf0:	00113c23          	sd	ra,24(sp)
     cf4:	00813823          	sd	s0,16(sp)
     cf8:	02010413          	addi	s0,sp,32
     cfc:	fea43423          	sd	a0,-24(s0)
     d00:	00058793          	mv	a5,a1
     d04:	fef42223          	sw	a5,-28(s0)
    return openat(AT_FDCWD, filename, flag, 0600);
     d08:	fe442783          	lw	a5,-28(s0)
     d0c:	18000693          	li	a3,384
     d10:	00078613          	mv	a2,a5
     d14:	fe843583          	ld	a1,-24(s0)
     d18:	f9c00513          	li	a0,-100
     d1c:	00000097          	auipc	ra,0x0
     d20:	4d0080e7          	jalr	1232(ra) # 11ec <openat>
     d24:	00050793          	mv	a5,a0
     d28:	00078513          	mv	a0,a5
     d2c:	01813083          	ld	ra,24(sp)
     d30:	01013403          	ld	s0,16(sp)
     d34:	02010113          	addi	sp,sp,32
     d38:	00008067          	ret

0000000000000d3c <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
     d3c:	fd010113          	addi	sp,sp,-48
     d40:	02813423          	sd	s0,40(sp)
     d44:	03010413          	addi	s0,sp,48
     d48:	fca43c23          	sd	a0,-40(s0)
  Header *bp, *p;

  bp = (Header*)ap - 1;
     d4c:	fd843783          	ld	a5,-40(s0)
     d50:	ff078793          	addi	a5,a5,-16
     d54:	fef43023          	sd	a5,-32(s0)
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
     d58:	00000797          	auipc	a5,0x0
     d5c:	63878793          	addi	a5,a5,1592 # 1390 <freep>
     d60:	0007b783          	ld	a5,0(a5)
     d64:	fef43423          	sd	a5,-24(s0)
     d68:	03c0006f          	j	da4 <free+0x68>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
     d6c:	fe843783          	ld	a5,-24(s0)
     d70:	0007b783          	ld	a5,0(a5)
     d74:	fe843703          	ld	a4,-24(s0)
     d78:	02f76063          	bltu	a4,a5,d98 <free+0x5c>
     d7c:	fe043703          	ld	a4,-32(s0)
     d80:	fe843783          	ld	a5,-24(s0)
     d84:	02e7ee63          	bltu	a5,a4,dc0 <free+0x84>
     d88:	fe843783          	ld	a5,-24(s0)
     d8c:	0007b783          	ld	a5,0(a5)
     d90:	fe043703          	ld	a4,-32(s0)
     d94:	02f76663          	bltu	a4,a5,dc0 <free+0x84>
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
     d98:	fe843783          	ld	a5,-24(s0)
     d9c:	0007b783          	ld	a5,0(a5)
     da0:	fef43423          	sd	a5,-24(s0)
     da4:	fe043703          	ld	a4,-32(s0)
     da8:	fe843783          	ld	a5,-24(s0)
     dac:	fce7f0e3          	bgeu	a5,a4,d6c <free+0x30>
     db0:	fe843783          	ld	a5,-24(s0)
     db4:	0007b783          	ld	a5,0(a5)
     db8:	fe043703          	ld	a4,-32(s0)
     dbc:	faf778e3          	bgeu	a4,a5,d6c <free+0x30>
      break;
  if(bp + bp->s.size == p->s.ptr){
     dc0:	fe043783          	ld	a5,-32(s0)
     dc4:	0087a783          	lw	a5,8(a5)
     dc8:	02079793          	slli	a5,a5,0x20
     dcc:	0207d793          	srli	a5,a5,0x20
     dd0:	00479793          	slli	a5,a5,0x4
     dd4:	fe043703          	ld	a4,-32(s0)
     dd8:	00f70733          	add	a4,a4,a5
     ddc:	fe843783          	ld	a5,-24(s0)
     de0:	0007b783          	ld	a5,0(a5)
     de4:	04f71063          	bne	a4,a5,e24 <free+0xe8>
    bp->s.size += p->s.ptr->s.size;
     de8:	fe043783          	ld	a5,-32(s0)
     dec:	0087a703          	lw	a4,8(a5)
     df0:	fe843783          	ld	a5,-24(s0)
     df4:	0007b783          	ld	a5,0(a5)
     df8:	0087a783          	lw	a5,8(a5)
     dfc:	00f707bb          	addw	a5,a4,a5
     e00:	0007871b          	sext.w	a4,a5
     e04:	fe043783          	ld	a5,-32(s0)
     e08:	00e7a423          	sw	a4,8(a5)
    bp->s.ptr = p->s.ptr->s.ptr;
     e0c:	fe843783          	ld	a5,-24(s0)
     e10:	0007b783          	ld	a5,0(a5)
     e14:	0007b703          	ld	a4,0(a5)
     e18:	fe043783          	ld	a5,-32(s0)
     e1c:	00e7b023          	sd	a4,0(a5)
     e20:	0140006f          	j	e34 <free+0xf8>
  } else
    bp->s.ptr = p->s.ptr;
     e24:	fe843783          	ld	a5,-24(s0)
     e28:	0007b703          	ld	a4,0(a5)
     e2c:	fe043783          	ld	a5,-32(s0)
     e30:	00e7b023          	sd	a4,0(a5)
  if(p + p->s.size == bp){
     e34:	fe843783          	ld	a5,-24(s0)
     e38:	0087a783          	lw	a5,8(a5)
     e3c:	02079793          	slli	a5,a5,0x20
     e40:	0207d793          	srli	a5,a5,0x20
     e44:	00479793          	slli	a5,a5,0x4
     e48:	fe843703          	ld	a4,-24(s0)
     e4c:	00f707b3          	add	a5,a4,a5
     e50:	fe043703          	ld	a4,-32(s0)
     e54:	02f71c63          	bne	a4,a5,e8c <free+0x150>
    p->s.size += bp->s.size;
     e58:	fe843783          	ld	a5,-24(s0)
     e5c:	0087a703          	lw	a4,8(a5)
     e60:	fe043783          	ld	a5,-32(s0)
     e64:	0087a783          	lw	a5,8(a5)
     e68:	00f707bb          	addw	a5,a4,a5
     e6c:	0007871b          	sext.w	a4,a5
     e70:	fe843783          	ld	a5,-24(s0)
     e74:	00e7a423          	sw	a4,8(a5)
    p->s.ptr = bp->s.ptr;
     e78:	fe043783          	ld	a5,-32(s0)
     e7c:	0007b703          	ld	a4,0(a5)
     e80:	fe843783          	ld	a5,-24(s0)
     e84:	00e7b023          	sd	a4,0(a5)
     e88:	0100006f          	j	e98 <free+0x15c>
  } else
    p->s.ptr = bp;
     e8c:	fe843783          	ld	a5,-24(s0)
     e90:	fe043703          	ld	a4,-32(s0)
     e94:	00e7b023          	sd	a4,0(a5)
  freep = p;
     e98:	00000797          	auipc	a5,0x0
     e9c:	4f878793          	addi	a5,a5,1272 # 1390 <freep>
     ea0:	fe843703          	ld	a4,-24(s0)
     ea4:	00e7b023          	sd	a4,0(a5)
}
     ea8:	00000013          	nop
     eac:	02813403          	ld	s0,40(sp)
     eb0:	03010113          	addi	sp,sp,48
     eb4:	00008067          	ret

0000000000000eb8 <morecore>:

static Header*
morecore(uint nu)
{
     eb8:	fd010113          	addi	sp,sp,-48
     ebc:	02113423          	sd	ra,40(sp)
     ec0:	02813023          	sd	s0,32(sp)
     ec4:	03010413          	addi	s0,sp,48
     ec8:	00050793          	mv	a5,a0
     ecc:	fcf42e23          	sw	a5,-36(s0)
  char *p;
  Header *hp;

  if(nu < 4096)
     ed0:	fdc42783          	lw	a5,-36(s0)
     ed4:	0007871b          	sext.w	a4,a5
     ed8:	000017b7          	lui	a5,0x1
     edc:	00f77663          	bgeu	a4,a5,ee8 <morecore+0x30>
    nu = 4096;
     ee0:	000017b7          	lui	a5,0x1
     ee4:	fcf42e23          	sw	a5,-36(s0)
  p = sbrk(nu * sizeof(Header));
     ee8:	fdc42783          	lw	a5,-36(s0)
     eec:	0047979b          	slliw	a5,a5,0x4
     ef0:	0007879b          	sext.w	a5,a5
     ef4:	0007879b          	sext.w	a5,a5
     ef8:	00078513          	mv	a0,a5
     efc:	00000097          	auipc	ra,0x0
     f00:	344080e7          	jalr	836(ra) # 1240 <sbrk>
     f04:	fea43423          	sd	a0,-24(s0)
  if(p == (char*)-1)
     f08:	fe843703          	ld	a4,-24(s0)
     f0c:	fff00793          	li	a5,-1
     f10:	00f71663          	bne	a4,a5,f1c <morecore+0x64>
    return 0;
     f14:	00000793          	li	a5,0
     f18:	0380006f          	j	f50 <morecore+0x98>
  hp = (Header*)p;
     f1c:	fe843783          	ld	a5,-24(s0)
     f20:	fef43023          	sd	a5,-32(s0)
  hp->s.size = nu;
     f24:	fe043783          	ld	a5,-32(s0)
     f28:	fdc42703          	lw	a4,-36(s0)
     f2c:	00e7a423          	sw	a4,8(a5) # 1008 <malloc+0xa4>
  free((void*)(hp + 1));
     f30:	fe043783          	ld	a5,-32(s0)
     f34:	01078793          	addi	a5,a5,16
     f38:	00078513          	mv	a0,a5
     f3c:	00000097          	auipc	ra,0x0
     f40:	e00080e7          	jalr	-512(ra) # d3c <free>
  return freep;
     f44:	00000797          	auipc	a5,0x0
     f48:	44c78793          	addi	a5,a5,1100 # 1390 <freep>
     f4c:	0007b783          	ld	a5,0(a5)
}
     f50:	00078513          	mv	a0,a5
     f54:	02813083          	ld	ra,40(sp)
     f58:	02013403          	ld	s0,32(sp)
     f5c:	03010113          	addi	sp,sp,48
     f60:	00008067          	ret

0000000000000f64 <malloc>:

void*
malloc(uint nbytes)
{
     f64:	fc010113          	addi	sp,sp,-64
     f68:	02113c23          	sd	ra,56(sp)
     f6c:	02813823          	sd	s0,48(sp)
     f70:	04010413          	addi	s0,sp,64
     f74:	00050793          	mv	a5,a0
     f78:	fcf42623          	sw	a5,-52(s0)
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
     f7c:	fcc46783          	lwu	a5,-52(s0)
     f80:	00f78793          	addi	a5,a5,15
     f84:	0047d793          	srli	a5,a5,0x4
     f88:	0007879b          	sext.w	a5,a5
     f8c:	0017879b          	addiw	a5,a5,1
     f90:	fcf42e23          	sw	a5,-36(s0)
  if((prevp = freep) == 0){
     f94:	00000797          	auipc	a5,0x0
     f98:	3fc78793          	addi	a5,a5,1020 # 1390 <freep>
     f9c:	0007b783          	ld	a5,0(a5)
     fa0:	fef43023          	sd	a5,-32(s0)
     fa4:	fe043783          	ld	a5,-32(s0)
     fa8:	04079263          	bnez	a5,fec <malloc+0x88>
    base.s.ptr = freep = prevp = &base;
     fac:	00000797          	auipc	a5,0x0
     fb0:	3d478793          	addi	a5,a5,980 # 1380 <base>
     fb4:	fef43023          	sd	a5,-32(s0)
     fb8:	00000797          	auipc	a5,0x0
     fbc:	3d878793          	addi	a5,a5,984 # 1390 <freep>
     fc0:	fe043703          	ld	a4,-32(s0)
     fc4:	00e7b023          	sd	a4,0(a5)
     fc8:	00000797          	auipc	a5,0x0
     fcc:	3c878793          	addi	a5,a5,968 # 1390 <freep>
     fd0:	0007b703          	ld	a4,0(a5)
     fd4:	00000797          	auipc	a5,0x0
     fd8:	3ac78793          	addi	a5,a5,940 # 1380 <base>
     fdc:	00e7b023          	sd	a4,0(a5)
    base.s.size = 0;
     fe0:	00000797          	auipc	a5,0x0
     fe4:	3a078793          	addi	a5,a5,928 # 1380 <base>
     fe8:	0007a423          	sw	zero,8(a5)
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
     fec:	fe043783          	ld	a5,-32(s0)
     ff0:	0007b783          	ld	a5,0(a5)
     ff4:	fef43423          	sd	a5,-24(s0)
    if(p->s.size >= nunits){
     ff8:	fe843783          	ld	a5,-24(s0)
     ffc:	0087a703          	lw	a4,8(a5)
    1000:	fdc42783          	lw	a5,-36(s0)
    1004:	0007879b          	sext.w	a5,a5
    1008:	08f76863          	bltu	a4,a5,1098 <malloc+0x134>
      if(p->s.size == nunits)
    100c:	fe843783          	ld	a5,-24(s0)
    1010:	0087a703          	lw	a4,8(a5)
    1014:	fdc42783          	lw	a5,-36(s0)
    1018:	0007879b          	sext.w	a5,a5
    101c:	00e79c63          	bne	a5,a4,1034 <malloc+0xd0>
        prevp->s.ptr = p->s.ptr;
    1020:	fe843783          	ld	a5,-24(s0)
    1024:	0007b703          	ld	a4,0(a5)
    1028:	fe043783          	ld	a5,-32(s0)
    102c:	00e7b023          	sd	a4,0(a5)
    1030:	04c0006f          	j	107c <malloc+0x118>
      else {
        p->s.size -= nunits;
    1034:	fe843783          	ld	a5,-24(s0)
    1038:	0087a783          	lw	a5,8(a5)
    103c:	fdc42703          	lw	a4,-36(s0)
    1040:	40e787bb          	subw	a5,a5,a4
    1044:	0007871b          	sext.w	a4,a5
    1048:	fe843783          	ld	a5,-24(s0)
    104c:	00e7a423          	sw	a4,8(a5)
        p += p->s.size;
    1050:	fe843783          	ld	a5,-24(s0)
    1054:	0087a783          	lw	a5,8(a5)
    1058:	02079793          	slli	a5,a5,0x20
    105c:	0207d793          	srli	a5,a5,0x20
    1060:	00479793          	slli	a5,a5,0x4
    1064:	fe843703          	ld	a4,-24(s0)
    1068:	00f707b3          	add	a5,a4,a5
    106c:	fef43423          	sd	a5,-24(s0)
        p->s.size = nunits;
    1070:	fe843783          	ld	a5,-24(s0)
    1074:	fdc42703          	lw	a4,-36(s0)
    1078:	00e7a423          	sw	a4,8(a5)
      }
      freep = prevp;
    107c:	00000797          	auipc	a5,0x0
    1080:	31478793          	addi	a5,a5,788 # 1390 <freep>
    1084:	fe043703          	ld	a4,-32(s0)
    1088:	00e7b023          	sd	a4,0(a5)
      return (void*)(p + 1);
    108c:	fe843783          	ld	a5,-24(s0)
    1090:	01078793          	addi	a5,a5,16
    1094:	0540006f          	j	10e8 <malloc+0x184>
    }
    if(p == freep)
    1098:	00000797          	auipc	a5,0x0
    109c:	2f878793          	addi	a5,a5,760 # 1390 <freep>
    10a0:	0007b783          	ld	a5,0(a5)
    10a4:	fe843703          	ld	a4,-24(s0)
    10a8:	02f71463          	bne	a4,a5,10d0 <malloc+0x16c>
      if((p = morecore(nunits)) == 0)
    10ac:	fdc42783          	lw	a5,-36(s0)
    10b0:	00078513          	mv	a0,a5
    10b4:	00000097          	auipc	ra,0x0
    10b8:	e04080e7          	jalr	-508(ra) # eb8 <morecore>
    10bc:	fea43423          	sd	a0,-24(s0)
    10c0:	fe843783          	ld	a5,-24(s0)
    10c4:	00079663          	bnez	a5,10d0 <malloc+0x16c>
        return 0;
    10c8:	00000793          	li	a5,0
    10cc:	01c0006f          	j	10e8 <malloc+0x184>
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    10d0:	fe843783          	ld	a5,-24(s0)
    10d4:	fef43023          	sd	a5,-32(s0)
    10d8:	fe843783          	ld	a5,-24(s0)
    10dc:	0007b783          	ld	a5,0(a5)
    10e0:	fef43423          	sd	a5,-24(s0)
    if(p->s.size >= nunits){
    10e4:	f15ff06f          	j	ff8 <malloc+0x94>
  }
}
    10e8:	00078513          	mv	a0,a5
    10ec:	03813083          	ld	ra,56(sp)
    10f0:	03013403          	ld	s0,48(sp)
    10f4:	04010113          	addi	sp,sp,64
    10f8:	00008067          	ret

00000000000010fc <fork>:

#ifndef __SYS_CALL
#define __SYS_CALL
#endif

__SYS_CALL(1, fork, sys_fork)
    10fc:	00100893          	li	a7,1
    1100:	00000073          	ecall
    1104:	00008067          	ret

0000000000001108 <exit>:
__SYS_CALL(93, exit, sys_exit)
    1108:	05d00893          	li	a7,93
    110c:	00000073          	ecall
    1110:	00008067          	ret

0000000000001114 <wait>:
__SYS_CALL(3, wait, sys_wait)
    1114:	00300893          	li	a7,3
    1118:	00000073          	ecall
    111c:	00008067          	ret

0000000000001120 <kill>:
__SYS_CALL(6, kill, sys_kill)
    1120:	00600893          	li	a7,6
    1124:	00000073          	ecall
    1128:	00008067          	ret

000000000000112c <uptime>:
__SYS_CALL(14, uptime, sys_uptime)
    112c:	00e00893          	li	a7,14
    1130:	00000073          	ecall
    1134:	00008067          	ret

0000000000001138 <nanosleep>:
__SYS_CALL(101, nanosleep, sys_nanosleep)
    1138:	06500893          	li	a7,101
    113c:	00000073          	ecall
    1140:	00008067          	ret

0000000000001144 <yield>:
__SYS_CALL(124, yield, sys_sched_yield)
    1144:	07c00893          	li	a7,124
    1148:	00000073          	ecall
    114c:	00008067          	ret

0000000000001150 <getpid>:
__SYS_CALL(172, getpid, sys_getpid)
    1150:	0ac00893          	li	a7,172
    1154:	00000073          	ecall
    1158:	00008067          	ret

000000000000115c <getppid>:
__SYS_CALL(173, getppid, sys_getppid)
    115c:	0ad00893          	li	a7,173
    1160:	00000073          	ecall
    1164:	00008067          	ret

0000000000001168 <clone>:
__SYS_CALL(220, clone, sys_clone)
    1168:	0dc00893          	li	a7,220
    116c:	00000073          	ecall
    1170:	00008067          	ret

0000000000001174 <exec>:
__SYS_CALL(221, exec, sys_exec)
    1174:	0dd00893          	li	a7,221
    1178:	00000073          	ecall
    117c:	00008067          	ret

0000000000001180 <wait4>:
__SYS_CALL(260, wait4, sys_wait4)
    1180:	10400893          	li	a7,260
    1184:	00000073          	ecall
    1188:	00008067          	ret

000000000000118c <getcwd>:
__SYS_CALL(17, getcwd, sys_getcwd)
    118c:	01100893          	li	a7,17
    1190:	00000073          	ecall
    1194:	00008067          	ret

0000000000001198 <dup>:
__SYS_CALL(23, dup, sys_dup)
    1198:	01700893          	li	a7,23
    119c:	00000073          	ecall
    11a0:	00008067          	ret

00000000000011a4 <dup3>:
__SYS_CALL(24, dup3, sys_dup3)
    11a4:	01800893          	li	a7,24
    11a8:	00000073          	ecall
    11ac:	00008067          	ret

00000000000011b0 <mkdirat>:
__SYS_CALL(34, mkdirat, sys_mkdirat)
    11b0:	02200893          	li	a7,34
    11b4:	00000073          	ecall
    11b8:	00008067          	ret

00000000000011bc <unlinkat>:
__SYS_CALL(35, unlinkat, sys_unlinkat)
    11bc:	02300893          	li	a7,35
    11c0:	00000073          	ecall
    11c4:	00008067          	ret

00000000000011c8 <umount2>:
__SYS_CALL(39, umount2, sys_umount)
    11c8:	02700893          	li	a7,39
    11cc:	00000073          	ecall
    11d0:	00008067          	ret

00000000000011d4 <mount>:
__SYS_CALL(40, mount, sys_mount)
    11d4:	02800893          	li	a7,40
    11d8:	00000073          	ecall
    11dc:	00008067          	ret

00000000000011e0 <chdir>:
__SYS_CALL(49, chdir, sys_chdir)
    11e0:	03100893          	li	a7,49
    11e4:	00000073          	ecall
    11e8:	00008067          	ret

00000000000011ec <openat>:
__SYS_CALL(56, openat, sys_openat)
    11ec:	03800893          	li	a7,56
    11f0:	00000073          	ecall
    11f4:	00008067          	ret

00000000000011f8 <close>:
__SYS_CALL(57, close, sys_close)
    11f8:	03900893          	li	a7,57
    11fc:	00000073          	ecall
    1200:	00008067          	ret

0000000000001204 <pipe2>:
__SYS_CALL(59, pipe2, sys_pipe2)
    1204:	03b00893          	li	a7,59
    1208:	00000073          	ecall
    120c:	00008067          	ret

0000000000001210 <getdents64>:
__SYS_CALL(61, getdents64, sys_getdents64)
    1210:	03d00893          	li	a7,61
    1214:	00000073          	ecall
    1218:	00008067          	ret

000000000000121c <read>:
__SYS_CALL(63, read, sys_read)
    121c:	03f00893          	li	a7,63
    1220:	00000073          	ecall
    1224:	00008067          	ret

0000000000001228 <write>:
__SYS_CALL(64, write, sys_write)
    1228:	04000893          	li	a7,64
    122c:	00000073          	ecall
    1230:	00008067          	ret

0000000000001234 <fstat>:
__SYS_CALL(80, fstat, sys_fstat)
    1234:	05000893          	li	a7,80
    1238:	00000073          	ecall
    123c:	00008067          	ret

0000000000001240 <sbrk>:
__SYS_CALL(214, sbrk, sys_sbrk)
    1240:	0d600893          	li	a7,214
    1244:	00000073          	ecall
    1248:	00008067          	ret

000000000000124c <munmap>:
__SYS_CALL(215, munmap, sys_munmap)
    124c:	0d700893          	li	a7,215
    1250:	00000073          	ecall
    1254:	00008067          	ret

0000000000001258 <mmap>:
__SYS_CALL(222, mmap, sys_mmap)
    1258:	0de00893          	li	a7,222
    125c:	00000073          	ecall
    1260:	00008067          	ret

0000000000001264 <times>:
__SYS_CALL(153, times, sys_times)
    1264:	09900893          	li	a7,153
    1268:	00000073          	ecall
    126c:	00008067          	ret

0000000000001270 <uname>:
__SYS_CALL(160, uname, sys_uname)
    1270:	0a000893          	li	a7,160
    1274:	00000073          	ecall
    1278:	00008067          	ret

000000000000127c <gettimeofday>:
__SYS_CALL(169, gettimeofday, sys_gettimeofday)
    127c:	0a900893          	li	a7,169
    1280:	00000073          	ecall
    1284:	00008067          	ret

0000000000001288 <ktest>:
__SYS_CALL(88, ktest, sys_ktest)
    1288:	05800893          	li	a7,88
    128c:	00000073          	ecall
    1290:	00008067          	ret

0000000000001294 <timetag>:
    1294:	05900893          	li	a7,89
    1298:	00000073          	ecall
    129c:	00008067          	ret
