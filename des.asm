
user/src/raw/runtest.exe:     file format elf64-littleriscv


Disassembly of section .text:

0000000000010120 <exit>:
   10120:	1101                	addi	sp,sp,-32
   10122:	e42a                	sd	a0,8(sp)
   10124:	ec06                	sd	ra,24(sp)
   10126:	67b000ef          	jal	ra,10fa0 <__funcs_on_exit>
   1012a:	679000ef          	jal	ra,10fa2 <__libc_exit_fini>
   1012e:	2f0060ef          	jal	ra,1641e <__stdio_exit>
   10132:	6522                	ld	a0,8(sp)
   10134:	0d8040ef          	jal	ra,1420c <_Exit>

0000000000010138 <main>:
	t_error("usage: %s [-t timeoutsec] [-w wrapcmd] cmd [args..]\n", argv[0]);
	exit(-1);
}

int main(int argc, char *argv[])
{
   10138:	7151                	addi	sp,sp,-240
   1013a:	e9ca                	sd	s2,208(sp)
	char *wrap = "";
   1013c:	6965                	lui	s2,0x19
{
   1013e:	f1a2                	sd	s0,224(sp)
   10140:	eda6                	sd	s1,216(sp)
   10142:	e5ce                	sd	s3,200(sp)
   10144:	e1d2                	sd	s4,192(sp)
   10146:	fd56                	sd	s5,184(sp)
   10148:	f95a                	sd	s6,176(sp)
   1014a:	f55e                	sd	s7,168(sp)
   1014c:	f162                	sd	s8,160(sp)
   1014e:	f586                	sd	ra,232(sp)
   10150:	84aa                	mv	s1,a0
   10152:	842e                	mv	s0,a1
	int timeoutsec = 5;
   10154:	4a15                	li	s4,5
	char *wrap = "";
   10156:	86890913          	addi	s2,s2,-1944 # 18868 <__clzdi2+0x262>
	int status;
	sigset_t set;
	int opt;
	int pid;

	while ((opt = getopt(argc, argv, "w:t:")) != -1) {
   1015a:	6b61                	lui	s6,0x18
   1015c:	5afd                	li	s5,-1
		switch (opt) {
   1015e:	07400b93          	li	s7,116
   10162:	07700c13          	li	s8,119
	while ((opt = getopt(argc, argv, "w:t:")) != -1) {
   10166:	6a0b0613          	addi	a2,s6,1696 # 186a0 <__clzdi2+0x9a>
   1016a:	85a2                	mv	a1,s0
   1016c:	8526                	mv	a0,s1
   1016e:	6ce020ef          	jal	ra,1283c <getopt>
   10172:	87aa                	mv	a5,a0
   10174:	01551f63          	bne	a0,s5,10192 <main+0x5a>
			break;
		default:
			usage(argv);
		}
	}
	if (optind >= argc)
   10178:	9641a583          	lw	a1,-1692(gp) # 1c164 <optind>
   1017c:	0295c863          	blt	a1,s1,101ac <main+0x74>
	t_error("usage: %s [-t timeoutsec] [-w wrapcmd] cmd [args..]\n", argv[0]);
   10180:	600c                	ld	a1,0(s0)
   10182:	6561                	lui	a0,0x18
   10184:	65050513          	addi	a0,a0,1616 # 18650 <__clzdi2+0x4a>
   10188:	664000ef          	jal	ra,107ec <t_printf>
	exit(-1);
   1018c:	557d                	li	a0,-1
		exit(1);
   1018e:	f93ff0ef          	jal	ra,10120 <exit>
		switch (opt) {
   10192:	01778763          	beq	a5,s7,101a0 <main+0x68>
   10196:	ff8795e3          	bne	a5,s8,10180 <main+0x48>
			wrap = optarg;
   1019a:	1f81b903          	ld	s2,504(gp) # 1c9f8 <optarg>
			break;
   1019e:	b7e1                	j	10166 <main+0x2e>
			timeoutsec = atoi(optarg);
   101a0:	1f81b503          	ld	a0,504(gp) # 1c9f8 <optarg>
   101a4:	214030ef          	jal	ra,133b8 <atoi>
   101a8:	8a2a                	mv	s4,a0
			break;
   101aa:	bf75                	j	10166 <main+0x2e>
		usage(argv);
	argv += optind;
   101ac:	058e                	slli	a1,a1,0x3
	sigemptyset(&set);
   101ae:	1008                	addi	a0,sp,32
	argv += optind;
   101b0:	942e                	add	s0,s0,a1
	sigemptyset(&set);
   101b2:	4f3020ef          	jal	ra,12ea4 <sigemptyset>
	sigaddset(&set, SIGCHLD);
   101b6:	45c5                	li	a1,17
   101b8:	1008                	addi	a0,sp,32
   101ba:	4b3020ef          	jal	ra,12e6c <sigaddset>
	sigprocmask(SIG_BLOCK, &set, 0);
   101be:	4601                	li	a2,0
   101c0:	100c                	addi	a1,sp,32
   101c2:	4501                	li	a0,0
   101c4:	529020ef          	jal	ra,12eec <sigprocmask>
	signal(SIGCHLD, handler);
   101c8:	65c1                	lui	a1,0x10
   101ca:	46c58593          	addi	a1,a1,1132 # 1046c <handler>
   101ce:	4545                	li	a0,17
   101d0:	4dd020ef          	jal	ra,12eac <signal>
	t_printf("========== START %s %s ==========\n", wrap, argv[0]);
   101d4:	6010                	ld	a2,0(s0)
   101d6:	6561                	lui	a0,0x18
   101d8:	85ca                	mv	a1,s2
   101da:	6a850513          	addi	a0,a0,1704 # 186a8 <__clzdi2+0xa2>
   101de:	60e000ef          	jal	ra,107ec <t_printf>
	pid = fork();
   101e2:	30f020ef          	jal	ra,12cf0 <fork>
   101e6:	84aa                	mv	s1,a0
	if (pid == 0) {
   101e8:	ed1d                	bnez	a0,10226 <main+0xee>
		t_setrlim(RLIMIT_STACK, 100*1024);
   101ea:	65e5                	lui	a1,0x19
   101ec:	450d                	li	a0,3
   101ee:	0d9000ef          	jal	ra,10ac6 <t_setrlim>
		if (*wrap) {
   101f2:	00094783          	lbu	a5,0(s2)
   101f6:	c781                	beqz	a5,101fe <main+0xc6>
			argv[0] = wrap;
   101f8:	ff243c23          	sd	s2,-8(s0)
			argv--;
   101fc:	1461                	addi	s0,s0,-8
		execv(argv[0], argv);
   101fe:	6008                	ld	a0,0(s0)
   10200:	85a2                	mv	a1,s0
   10202:	2cd020ef          	jal	ra,12cce <execv>
		t_error("%s exec failed: %s\n", argv[0], strerror(errno));
   10206:	600c                	ld	a1,0(s0)
   10208:	e42e                	sd	a1,8(sp)
   1020a:	563000ef          	jal	ra,10f6c <__errno_location>
   1020e:	4108                	lw	a0,0(a0)
   10210:	589000ef          	jal	ra,10f98 <strerror>
   10214:	65a2                	ld	a1,8(sp)
   10216:	862a                	mv	a2,a0
   10218:	6561                	lui	a0,0x18
   1021a:	6d050513          	addi	a0,a0,1744 # 186d0 <__clzdi2+0xca>
   1021e:	5ce000ef          	jal	ra,107ec <t_printf>
		exit(1);
   10222:	4505                	li	a0,1
   10224:	b7ad                	j	1018e <main+0x56>
	pid = start(wrap, argv);
	int err = 0;
   10226:	4981                	li	s3,0
	if (pid == -1) {
   10228:	03551763          	bne	a0,s5,10256 <main+0x11e>
		t_error("%s fork failed: %s\n", argv[0], strerror(errno));
   1022c:	600c                	ld	a1,0(s0)
		t_printf("FAIL %s [internal]\n", argv[0]);
		err = 1;
   1022e:	4985                	li	s3,1
		t_error("%s fork failed: %s\n", argv[0], strerror(errno));
   10230:	e42e                	sd	a1,8(sp)
   10232:	53b000ef          	jal	ra,10f6c <__errno_location>
   10236:	4108                	lw	a0,0(a0)
   10238:	561000ef          	jal	ra,10f98 <strerror>
   1023c:	65a2                	ld	a1,8(sp)
   1023e:	862a                	mv	a2,a0
   10240:	6561                	lui	a0,0x18
   10242:	70050513          	addi	a0,a0,1792 # 18700 <__clzdi2+0xfa>
   10246:	5a6000ef          	jal	ra,107ec <t_printf>
		t_printf("FAIL %s [internal]\n", argv[0]);
   1024a:	600c                	ld	a1,0(s0)
   1024c:	6561                	lui	a0,0x18
   1024e:	73050513          	addi	a0,a0,1840 # 18730 <__clzdi2+0x12a>
   10252:	59a000ef          	jal	ra,107ec <t_printf>
	}
	if (sigtimedwait(&set, 0, &(struct timespec){timeoutsec,0}) == -1) {
   10256:	0810                	addi	a2,sp,16
   10258:	4581                	li	a1,0
   1025a:	1008                	addi	a0,sp,32
   1025c:	ec02                	sd	zero,24(sp)
   1025e:	e852                	sd	s4,16(sp)
   10260:	4ad020ef          	jal	ra,12f0c <sigtimedwait>
   10264:	57fd                	li	a5,-1
	int timeout = 0;
   10266:	4a81                	li	s5,0
	if (sigtimedwait(&set, 0, &(struct timespec){timeoutsec,0}) == -1) {
   10268:	04f51c63          	bne	a0,a5,102c0 <main+0x188>
		if (errno == EAGAIN)
   1026c:	501000ef          	jal	ra,10f6c <__errno_location>
   10270:	8a2a                	mv	s4,a0
   10272:	4108                	lw	a0,0(a0)
   10274:	47ad                	li	a5,11
			timeout = 1;
   10276:	4a85                	li	s5,1
		if (errno == EAGAIN)
   10278:	00f50f63          	beq	a0,a5,10296 <main+0x15e>
		else {
			t_error("%s sigtimedwait failed: %s\n", argv[0], strerror(errno));
   1027c:	600c                	ld	a1,0(s0)
			err = 1;
   1027e:	4985                	li	s3,1
	int timeout = 0;
   10280:	4a81                	li	s5,0
			t_error("%s sigtimedwait failed: %s\n", argv[0], strerror(errno));
   10282:	e42e                	sd	a1,8(sp)
   10284:	515000ef          	jal	ra,10f98 <strerror>
   10288:	65a2                	ld	a1,8(sp)
   1028a:	862a                	mv	a2,a0
   1028c:	6561                	lui	a0,0x18
   1028e:	74850513          	addi	a0,a0,1864 # 18748 <__clzdi2+0x142>
   10292:	55a000ef          	jal	ra,107ec <t_printf>
		}
		if (kill(pid, SIGKILL) == -1) {
   10296:	45a5                	li	a1,9
   10298:	8526                	mv	a0,s1
   1029a:	3bb020ef          	jal	ra,12e54 <kill>
   1029e:	57fd                	li	a5,-1
   102a0:	02f51063          	bne	a0,a5,102c0 <main+0x188>
			t_error("%s kill failed: %s\n", argv[0], strerror(errno));
   102a4:	600c                	ld	a1,0(s0)
   102a6:	000a2503          	lw	a0,0(s4)
			err = 1;
   102aa:	4985                	li	s3,1
			t_error("%s kill failed: %s\n", argv[0], strerror(errno));
   102ac:	e42e                	sd	a1,8(sp)
   102ae:	4eb000ef          	jal	ra,10f98 <strerror>
   102b2:	65a2                	ld	a1,8(sp)
   102b4:	862a                	mv	a2,a0
   102b6:	6561                	lui	a0,0x18
   102b8:	78050513          	addi	a0,a0,1920 # 18780 <__clzdi2+0x17a>
   102bc:	530000ef          	jal	ra,107ec <t_printf>
		}
	}
	if (waitpid(pid, &status, 0) != pid) {
   102c0:	4601                	li	a2,0
   102c2:	080c                	addi	a1,sp,16
   102c4:	8526                	mv	a0,s1
   102c6:	32d020ef          	jal	ra,12df2 <waitpid>
   102ca:	02950763          	beq	a0,s1,102f8 <main+0x1c0>
		t_error("%s waitpid failed: %s\n", argv[0], strerror(errno));
   102ce:	600c                	ld	a1,0(s0)
		t_printf("FAIL %s [internal]\n", argv[0]);
		err = 1;
   102d0:	4985                	li	s3,1
		t_error("%s waitpid failed: %s\n", argv[0], strerror(errno));
   102d2:	e42e                	sd	a1,8(sp)
   102d4:	499000ef          	jal	ra,10f6c <__errno_location>
   102d8:	4108                	lw	a0,0(a0)
   102da:	4bf000ef          	jal	ra,10f98 <strerror>
   102de:	65a2                	ld	a1,8(sp)
   102e0:	862a                	mv	a2,a0
   102e2:	6561                	lui	a0,0x18
   102e4:	7b050513          	addi	a0,a0,1968 # 187b0 <__clzdi2+0x1aa>
   102e8:	504000ef          	jal	ra,107ec <t_printf>
		t_printf("FAIL %s [internal]\n", argv[0]);
   102ec:	600c                	ld	a1,0(s0)
   102ee:	6561                	lui	a0,0x18
   102f0:	73050513          	addi	a0,a0,1840 # 18730 <__clzdi2+0x12a>
   102f4:	4f8000ef          	jal	ra,107ec <t_printf>
	}
	if (WIFEXITED(status)) {
   102f8:	4642                	lw	a2,16(sp)
   102fa:	07f67513          	andi	a0,a2,127
   102fe:	e121                	bnez	a0,1033e <main+0x206>
		if (WEXITSTATUS(status) != 0) {
   10300:	4086561b          	sraiw	a2,a2,0x8
   10304:	0ff67613          	andi	a2,a2,255
   10308:	ca35                	beqz	a2,1037c <main+0x244>
			t_printf("FAIL %s [status %d]\n", argv[0], WEXITSTATUS(status));
   1030a:	600c                	ld	a1,0(s0)
   1030c:	6561                	lui	a0,0x18
   1030e:	7e050513          	addi	a0,a0,2016 # 187e0 <__clzdi2+0x1da>
   10312:	4da000ef          	jal	ra,107ec <t_printf>


	if (err == 0) {
		t_printf("Pass!\n");
	}
	t_printf("========== END %s %s ==========\n", wrap, argv[0]);
   10316:	6010                	ld	a2,0(s0)
   10318:	6565                	lui	a0,0x19
   1031a:	85ca                	mv	a1,s2
   1031c:	84850513          	addi	a0,a0,-1976 # 18848 <__clzdi2+0x242>
   10320:	4cc000ef          	jal	ra,107ec <t_printf>
	return 1;
}
   10324:	70ae                	ld	ra,232(sp)
   10326:	740e                	ld	s0,224(sp)
   10328:	64ee                	ld	s1,216(sp)
   1032a:	694e                	ld	s2,208(sp)
   1032c:	69ae                	ld	s3,200(sp)
   1032e:	6a0e                	ld	s4,192(sp)
   10330:	7aea                	ld	s5,184(sp)
   10332:	7b4a                	ld	s6,176(sp)
   10334:	7baa                	ld	s7,168(sp)
   10336:	7c0a                	ld	s8,160(sp)
   10338:	4505                	li	a0,1
   1033a:	616d                	addi	sp,sp,240
   1033c:	8082                	ret
	} else if (timeout) {
   1033e:	600c                	ld	a1,0(s0)
   10340:	000a8863          	beqz	s5,10350 <main+0x218>
		t_printf("FAIL %s [timed out]\n", argv[0]);
   10344:	6561                	lui	a0,0x18
   10346:	7f850513          	addi	a0,a0,2040 # 187f8 <__clzdi2+0x1f2>
		t_printf("FAIL %s [unknown]\n", argv[0]);
   1034a:	4a2000ef          	jal	ra,107ec <t_printf>
	if (err == 0) {
   1034e:	b7e1                	j	10316 <main+0x1de>
	} else if (WIFSIGNALED(status)) {
   10350:	1642                	slli	a2,a2,0x30
   10352:	9241                	srli	a2,a2,0x30
   10354:	367d                	addiw	a2,a2,-1
   10356:	0fe00793          	li	a5,254
   1035a:	00c7ed63          	bltu	a5,a2,10374 <main+0x23c>
   1035e:	e42e                	sd	a1,8(sp)
		t_printf("FAIL %s [signal %s]\n", argv[0], strsignal(WTERMSIG(status)));
   10360:	708030ef          	jal	ra,13a68 <strsignal>
   10364:	65a2                	ld	a1,8(sp)
   10366:	862a                	mv	a2,a0
   10368:	6565                	lui	a0,0x19
   1036a:	81050513          	addi	a0,a0,-2032 # 18810 <__clzdi2+0x20a>
   1036e:	47e000ef          	jal	ra,107ec <t_printf>
	if (err == 0) {
   10372:	b755                	j	10316 <main+0x1de>
		t_printf("FAIL %s [unknown]\n", argv[0]);
   10374:	6565                	lui	a0,0x19
   10376:	82850513          	addi	a0,a0,-2008 # 18828 <__clzdi2+0x222>
   1037a:	bfc1                	j	1034a <main+0x212>
	if (err == 0) {
   1037c:	f8099de3          	bnez	s3,10316 <main+0x1de>
		t_printf("Pass!\n");
   10380:	6565                	lui	a0,0x19
   10382:	84050513          	addi	a0,a0,-1984 # 18840 <__clzdi2+0x23a>
   10386:	466000ef          	jal	ra,107ec <t_printf>
   1038a:	b771                	j	10316 <main+0x1de>

000000000001038c <_start>:
   1038c:	0000c197          	auipc	gp,0xc
   10390:	47418193          	addi	gp,gp,1140 # 1c800 <__global_pointer$>
   10394:	850a                	mv	a0,sp
   10396:	ffff0597          	auipc	a1,0xffff0
   1039a:	c6a58593          	addi	a1,a1,-918 # 0 <exit-0x10120>
   1039e:	ff017113          	andi	sp,sp,-16
   103a2:	a009                	j	103a4 <_start_c>

00000000000103a4 <_start_c>:
   103a4:	410c                	lw	a1,0(a0)
   103a6:	6745                	lui	a4,0x11
   103a8:	66c5                	lui	a3,0x11
   103aa:	00850613          	addi	a2,a0,8
   103ae:	6541                	lui	a0,0x10
   103b0:	4781                	li	a5,0
   103b2:	fa070713          	addi	a4,a4,-96 # 10fa0 <__funcs_on_exit>
   103b6:	db868693          	addi	a3,a3,-584 # 10db8 <_init>
   103ba:	13850513          	addi	a0,a0,312 # 10138 <main>
   103be:	37d0006f          	j	10f3a <__libc_start_main>

00000000000103c2 <deregister_tm_clones>:
   103c2:	8e818793          	addi	a5,gp,-1816 # 1c0e8 <__TMC_END__>
   103c6:	8e818713          	addi	a4,gp,-1816 # 1c0e8 <__TMC_END__>
   103ca:	00f70b63          	beq	a4,a5,103e0 <deregister_tm_clones+0x1e>
   103ce:	00000337          	lui	t1,0x0
   103d2:	00030313          	mv	t1,t1
   103d6:	00030563          	beqz	t1,103e0 <deregister_tm_clones+0x1e>
   103da:	8e818513          	addi	a0,gp,-1816 # 1c0e8 <__TMC_END__>
   103de:	8302                	jr	t1
   103e0:	8082                	ret

00000000000103e2 <register_tm_clones>:
   103e2:	8e818593          	addi	a1,gp,-1816 # 1c0e8 <__TMC_END__>
   103e6:	8e818793          	addi	a5,gp,-1816 # 1c0e8 <__TMC_END__>
   103ea:	8f8d                	sub	a5,a5,a1
   103ec:	4037d713          	srai	a4,a5,0x3
   103f0:	03f7d593          	srli	a1,a5,0x3f
   103f4:	95ba                	add	a1,a1,a4
   103f6:	8585                	srai	a1,a1,0x1
   103f8:	c991                	beqz	a1,1040c <register_tm_clones+0x2a>
   103fa:	00000337          	lui	t1,0x0
   103fe:	00030313          	mv	t1,t1
   10402:	00030563          	beqz	t1,1040c <register_tm_clones+0x2a>
   10406:	8e818513          	addi	a0,gp,-1816 # 1c0e8 <__TMC_END__>
   1040a:	8302                	jr	t1
   1040c:	8082                	ret

000000000001040e <__do_global_dtors_aux>:
   1040e:	1141                	addi	sp,sp,-16
   10410:	e022                	sd	s0,0(sp)
   10412:	a101c783          	lbu	a5,-1520(gp) # 1c210 <completed.4703>
   10416:	e406                	sd	ra,8(sp)
   10418:	e395                	bnez	a5,1043c <__do_global_dtors_aux+0x2e>
   1041a:	fa9ff0ef          	jal	ra,103c2 <deregister_tm_clones>
   1041e:	000007b7          	lui	a5,0x0
   10422:	00078793          	mv	a5,a5
   10426:	cb81                	beqz	a5,10436 <__do_global_dtors_aux+0x28>
   10428:	6569                	lui	a0,0x1a
   1042a:	24850513          	addi	a0,a0,584 # 1a248 <__FRAME_END__>
   1042e:	00000097          	auipc	ra,0x0
   10432:	000000e7          	jalr	zero # 0 <exit-0x10120>
   10436:	4785                	li	a5,1
   10438:	a0f18823          	sb	a5,-1520(gp) # 1c210 <completed.4703>
   1043c:	60a2                	ld	ra,8(sp)
   1043e:	6402                	ld	s0,0(sp)
   10440:	0141                	addi	sp,sp,16
   10442:	8082                	ret

0000000000010444 <frame_dummy>:
   10444:	000007b7          	lui	a5,0x0
   10448:	00078793          	mv	a5,a5
   1044c:	cf99                	beqz	a5,1046a <frame_dummy+0x26>
   1044e:	6569                	lui	a0,0x1a
   10450:	1141                	addi	sp,sp,-16
   10452:	a1818593          	addi	a1,gp,-1512 # 1c218 <object.4708>
   10456:	24850513          	addi	a0,a0,584 # 1a248 <__FRAME_END__>
   1045a:	e406                	sd	ra,8(sp)
   1045c:	00000097          	auipc	ra,0x0
   10460:	000000e7          	jalr	zero # 0 <exit-0x10120>
   10464:	60a2                	ld	ra,8(sp)
   10466:	0141                	addi	sp,sp,16
   10468:	bfad                	j	103e2 <register_tm_clones>
   1046a:	bfa5                	j	103e2 <register_tm_clones>

000000000001046c <handler>:
}
   1046c:	8082                	ret

000000000001046e <t_fdfill>:
#include <unistd.h>
#include <errno.h>
#include "test.h"

void t_fdfill(void)
{
   1046e:	1101                	addi	sp,sp,-32
	int fd = 1;
	if (dup(fd) == -1) {
   10470:	4505                	li	a0,1
{
   10472:	e822                	sd	s0,16(sp)
   10474:	ec06                	sd	ra,24(sp)
   10476:	e426                	sd	s1,8(sp)
	if (dup(fd) == -1) {
   10478:	357030ef          	jal	ra,13fce <dup>
   1047c:	57fd                	li	a5,-1
   1047e:	4405                	li	s0,1
   10480:	00f51f63          	bne	a0,a5,1049e <t_fdfill+0x30>
		if (errno == EMFILE)
   10484:	2e9000ef          	jal	ra,10f6c <__errno_location>
   10488:	4118                	lw	a4,0(a0)
   1048a:	47e1                	li	a5,24
   1048c:	00f70f63          	beq	a4,a5,104aa <t_fdfill+0x3c>
			return;
		fd = open("/dev/null", O_RDONLY);
   10490:	6565                	lui	a0,0x19
   10492:	4581                	li	a1,0
   10494:	87050513          	addi	a0,a0,-1936 # 18870 <__clzdi2+0x26a>
   10498:	337000ef          	jal	ra,10fce <open>
   1049c:	842a                	mv	s0,a0
	}
	while(dup(fd) != -1);
   1049e:	54fd                	li	s1,-1
   104a0:	8522                	mv	a0,s0
   104a2:	32d030ef          	jal	ra,13fce <dup>
   104a6:	fe951de3          	bne	a0,s1,104a0 <t_fdfill+0x32>
}
   104aa:	60e2                	ld	ra,24(sp)
   104ac:	6442                	ld	s0,16(sp)
   104ae:	64a2                	ld	s1,8(sp)
   104b0:	6105                	addi	sp,sp,32
   104b2:	8082                	ret

00000000000104b4 <t_memfill>:
#include <errno.h>
#include <sys/resource.h>
#include "test.h"

int t_memfill()
{
   104b4:	1141                	addi	sp,sp,-16
	int r = 0;
	/* alloc mmap space with PROT_NONE */
	if (t_vmfill(0,0,0) < 0) {
   104b6:	4601                	li	a2,0
   104b8:	4581                	li	a1,0
   104ba:	4501                	li	a0,0
{
   104bc:	e022                	sd	s0,0(sp)
   104be:	e406                	sd	ra,8(sp)
	if (t_vmfill(0,0,0) < 0) {
   104c0:	708000ef          	jal	ra,10bc8 <t_vmfill>
   104c4:	4401                	li	s0,0
   104c6:	00055e63          	bgez	a0,104e2 <t_memfill+0x2e>
		t_error("vmfill failed: %s\n", strerror(errno));
   104ca:	2a3000ef          	jal	ra,10f6c <__errno_location>
   104ce:	4108                	lw	a0,0(a0)
		r = -1;
   104d0:	547d                	li	s0,-1
		t_error("vmfill failed: %s\n", strerror(errno));
   104d2:	2c7000ef          	jal	ra,10f98 <strerror>
   104d6:	85aa                	mv	a1,a0
   104d8:	6565                	lui	a0,0x19
   104da:	88050513          	addi	a0,a0,-1920 # 18880 <__clzdi2+0x27a>
   104de:	30e000ef          	jal	ra,107ec <t_printf>
	}
	/* limit brk space */
	if (t_setrlim(RLIMIT_DATA, 0) < 0)
   104e2:	4581                	li	a1,0
   104e4:	4509                	li	a0,2
   104e6:	5e0000ef          	jal	ra,10ac6 <t_setrlim>
   104ea:	00054c63          	bltz	a0,10502 <t_memfill+0x4e>
		r = -1;
	if (!r)
   104ee:	e811                	bnez	s0,10502 <t_memfill+0x4e>
		/* use up libc reserves if any */
		while (malloc(1));
   104f0:	4505                	li	a0,1
   104f2:	0c8010ef          	jal	ra,115ba <malloc>
   104f6:	fd6d                	bnez	a0,104f0 <t_memfill+0x3c>
	return r;
}
   104f8:	60a2                	ld	ra,8(sp)
   104fa:	8522                	mv	a0,s0
   104fc:	6402                	ld	s0,0(sp)
   104fe:	0141                	addi	sp,sp,16
   10500:	8082                	ret
		r = -1;
   10502:	547d                	li	s0,-1
   10504:	bfd5                	j	104f8 <t_memfill+0x44>

0000000000010506 <eulpf>:
#include <stdio.h>
#include <stdint.h>
#include "mtest.h"

int eulpf(float x)
{
   10506:	e0050553          	fmv.x.w	a0,fa0
	union { float f; uint32_t i; } u = { x };
	int e = u.i>>23 & 0xff;
   1050a:	0175551b          	srliw	a0,a0,0x17
   1050e:	0ff57513          	andi	a0,a0,255

	if (!e)
   10512:	e111                	bnez	a0,10516 <eulpf+0x10>
		e++;
   10514:	4505                	li	a0,1
	return e - 0x7f - 23;
}
   10516:	f6a5051b          	addiw	a0,a0,-150
   1051a:	8082                	ret

000000000001051c <eulp>:

int eulp(double x)
{
   1051c:	e20507d3          	fmv.x.d	a5,fa0
	union { double f; uint64_t i; } u = { x };
	int e = u.i>>52 & 0x7ff;
   10520:	93d1                	srli	a5,a5,0x34

	if (!e)
   10522:	03579713          	slli	a4,a5,0x35
	int e = u.i>>52 & 0x7ff;
   10526:	7ff7f513          	andi	a0,a5,2047
	if (!e)
   1052a:	e311                	bnez	a4,1052e <eulp+0x12>
		e++;
   1052c:	4505                	li	a0,1
	return e - 0x3ff - 52;
}
   1052e:	bcd5051b          	addiw	a0,a0,-1075
   10532:	8082                	ret

0000000000010534 <eulpl>:
	return e - 0x3fff - 63;
#else
	// TODO
	return 0;
#endif
}
   10534:	4501                	li	a0,0
   10536:	8082                	ret

0000000000010538 <ulperrf>:

float ulperrf(float got, float want, float dwant)
{
   10538:	7179                	addi	sp,sp,-48
   1053a:	00a12627          	fsw	fa0,12(sp)
int __fpclassifyl(long double);

static __inline unsigned __FLOAT_BITS(float __f)
{
	union {float __f; unsigned __i;} __u;
	__u.__f = __f;
   1053e:	4732                	lw	a4,12(sp)
	if (isnan(got) && isnan(want))
   10540:	800007b7          	lui	a5,0x80000
   10544:	fff7c793          	not	a5,a5
{
   10548:	f022                	sd	s0,32(sp)
   1054a:	ac22                	fsd	fs0,24(sp)
   1054c:	f406                	sd	ra,40(sp)
	if (isnan(got) && isnan(want))
   1054e:	00f77633          	and	a2,a4,a5
   10552:	7f8006b7          	lui	a3,0x7f800
{
   10556:	e0058453          	fmv.x.w	s0,fa1
   1055a:	20c60453          	fmv.s	fs0,fa2
	if (isnan(got) && isnan(want))
   1055e:	00c6f963          	bgeu	a3,a2,10570 <ulperrf+0x38>
   10562:	8fe1                	and	a5,a5,s0
   10564:	2781                	sext.w	a5,a5
   10566:	08f6f263          	bgeu	a3,a5,105ea <ulperrf+0xb2>
		return 0;
   1056a:	f0000453          	fmv.w.x	fs0,zero
   1056e:	a831                	j	1058a <ulperrf+0x52>
	if (got == want) {
   10570:	00c12787          	flw	fa5,12(sp)
   10574:	a0b7a7d3          	feq.s	a5,fa5,fa1
   10578:	c385                	beqz	a5,10598 <ulperrf+0x60>
		if (signbit(got) == signbit(want))
   1057a:	01f7571b          	srliw	a4,a4,0x1f
   1057e:	01f4541b          	srliw	s0,s0,0x1f
   10582:	00870463          	beq	a4,s0,1058a <ulperrf+0x52>
			return dwant;
		return inf;
   10586:	9401a407          	flw	fs0,-1728(gp) # 1c140 <__SDATA_BEGIN__+0x38>
	if (isinf(got)) {
		got = copysignf(0x1p127, got);
		want *= 0.5;
	}
	return scalbn(got - want, -eulpf(want)) + dwant;
}
   1058a:	70a2                	ld	ra,40(sp)
   1058c:	7402                	ld	s0,32(sp)
   1058e:	20840553          	fmv.s	fa0,fs0
   10592:	2462                	fld	fs0,24(sp)
   10594:	6145                	addi	sp,sp,48
   10596:	8082                	ret
	if (isinf(got)) {
   10598:	02d61263          	bne	a2,a3,105bc <ulperrf+0x84>
		got = copysignf(0x1p127, got);
   1059c:	9441a507          	flw	fa0,-1724(gp) # 1c144 <__SDATA_BEGIN__+0x3c>
   105a0:	00c12587          	flw	fa1,12(sp)
   105a4:	182020ef          	jal	ra,12726 <copysignf>
		want *= 0.5;
   105a8:	f0040753          	fmv.w.x	fa4,s0
   105ac:	9481a787          	flw	fa5,-1720(gp) # 1c148 <__SDATA_BEGIN__+0x40>
		got = copysignf(0x1p127, got);
   105b0:	00a12627          	fsw	fa0,12(sp)
		want *= 0.5;
   105b4:	10f777d3          	fmul.s	fa5,fa4,fa5
   105b8:	e0078453          	fmv.x.w	s0,fa5
	return scalbn(got - want, -eulpf(want)) + dwant;
   105bc:	f0040553          	fmv.w.x	fa0,s0
   105c0:	f47ff0ef          	jal	ra,10506 <eulpf>
   105c4:	00c12787          	flw	fa5,12(sp)
   105c8:	f0040753          	fmv.w.x	fa4,s0
   105cc:	40a0053b          	negw	a0,a0
   105d0:	08e7f553          	fsub.s	fa0,fa5,fa4
   105d4:	42050553          	fcvt.d.s	fa0,fa0
   105d8:	154020ef          	jal	ra,1272c <scalbn>
   105dc:	42040453          	fcvt.d.s	fs0,fs0
   105e0:	02a47453          	fadd.d	fs0,fs0,fa0
   105e4:	40147453          	fcvt.s.d	fs0,fs0
   105e8:	b74d                	j	1058a <ulperrf+0x52>
	if (got == want) {
   105ea:	00c12787          	flw	fa5,12(sp)
   105ee:	a0b7a7d3          	feq.s	a5,fa5,fa1
   105f2:	f7c1                	bnez	a5,1057a <ulperrf+0x42>
   105f4:	b7e1                	j	105bc <ulperrf+0x84>

00000000000105f6 <ulperr>:

float ulperr(double got, double want, float dwant)
{
   105f6:	e20507d3          	fmv.x.d	a5,fa0
	if (isnan(got) && isnan(want))
   105fa:	577d                	li	a4,-1
{
   105fc:	1101                	addi	sp,sp,-32
	if (isnan(got) && isnan(want))
   105fe:	8305                	srli	a4,a4,0x1
   10600:	7ff00693          	li	a3,2047
{
   10604:	e822                	sd	s0,16(sp)
   10606:	a026                	fsd	fs1,0(sp)
   10608:	ec06                	sd	ra,24(sp)
   1060a:	a422                	fsd	fs0,8(sp)
	if (isnan(got) && isnan(want))
   1060c:	00f775b3          	and	a1,a4,a5
   10610:	16d2                	slli	a3,a3,0x34
{
   10612:	e2058453          	fmv.x.d	s0,fa1
   10616:	20c604d3          	fmv.s	fs1,fa2
	return __u.__i;
}
static __inline unsigned long long __DOUBLE_BITS(double __f)
{
	union {double __f; unsigned long long __i;} __u;
	__u.__f = __f;
   1061a:	863e                	mv	a2,a5
	if (isnan(got) && isnan(want))
   1061c:	00b6f863          	bgeu	a3,a1,1062c <ulperr+0x36>
   10620:	8f61                	and	a4,a4,s0
   10622:	08e6f163          	bgeu	a3,a4,106a4 <ulperr+0xae>
		return 0;
   10626:	f00004d3          	fmv.w.x	fs1,zero
   1062a:	a821                	j	10642 <ulperr+0x4c>
	if (got == want) {
   1062c:	a2b52753          	feq.d	a4,fa0,fa1
   10630:	c30d                	beqz	a4,10652 <ulperr+0x5c>
		if (signbit(got) == signbit(want))
   10632:	00062613          	slti	a2,a2,0
   10636:	00042413          	slti	s0,s0,0
   1063a:	00860463          	beq	a2,s0,10642 <ulperr+0x4c>
			return dwant;
		return inf; // treat 0 sign errors badly
   1063e:	9401a487          	flw	fs1,-1728(gp) # 1c140 <__SDATA_BEGIN__+0x38>
	if (isinf(got)) {
		got = copysign(0x1p1023, got);
		want *= 0.5;
	}
	return scalbn(got - want, -eulp(want)) + dwant;
}
   10642:	60e2                	ld	ra,24(sp)
   10644:	6442                	ld	s0,16(sp)
   10646:	20948553          	fmv.s	fa0,fs1
   1064a:	2422                	fld	fs0,8(sp)
   1064c:	2482                	fld	fs1,0(sp)
   1064e:	6105                	addi	sp,sp,32
   10650:	8082                	ret
	if (isinf(got)) {
   10652:	02d59263          	bne	a1,a3,10676 <ulperr+0x80>
		got = copysign(0x1p1023, got);
   10656:	22a505d3          	fmv.d	fa1,fa0
   1065a:	9081b507          	fld	fa0,-1784(gp) # 1c108 <__SDATA_BEGIN__>
   1065e:	0c2020ef          	jal	ra,12720 <copysign>
		want *= 0.5;
   10662:	f2040753          	fmv.d.x	fa4,s0
   10666:	9101b787          	fld	fa5,-1776(gp) # 1c110 <__SDATA_BEGIN__+0x8>
		got = copysign(0x1p1023, got);
   1066a:	e20507d3          	fmv.x.d	a5,fa0
		want *= 0.5;
   1066e:	12f777d3          	fmul.d	fa5,fa4,fa5
   10672:	e2078453          	fmv.x.d	s0,fa5
	return scalbn(got - want, -eulp(want)) + dwant;
   10676:	f20787d3          	fmv.d.x	fa5,a5
   1067a:	f2040753          	fmv.d.x	fa4,s0
   1067e:	f2040553          	fmv.d.x	fa0,s0
   10682:	0ae7f453          	fsub.d	fs0,fa5,fa4
   10686:	e97ff0ef          	jal	ra,1051c <eulp>
   1068a:	40a0053b          	negw	a0,a0
   1068e:	22840553          	fmv.d	fa0,fs0
   10692:	09a020ef          	jal	ra,1272c <scalbn>
   10696:	420484d3          	fcvt.d.s	fs1,fs1
   1069a:	02a4f4d3          	fadd.d	fs1,fs1,fa0
   1069e:	4014f4d3          	fcvt.s.d	fs1,fs1
   106a2:	b745                	j	10642 <ulperr+0x4c>
	if (got == want) {
   106a4:	a2b52753          	feq.d	a4,fa0,fa1
   106a8:	f749                	bnez	a4,10632 <ulperr+0x3c>
   106aa:	b7f1                	j	10676 <ulperr+0x80>

00000000000106ac <ulperrl>:
	return scalbnl(got - want, -eulpl(want)) + dwant;
#else
	// TODO
	return inf;
#endif
}
   106ac:	9401a507          	flw	fa0,-1728(gp) # 1c140 <__SDATA_BEGIN__+0x38>
   106b0:	8082                	ret

00000000000106b2 <estr>:
	flag(UNDERFLOW),
	flag(OVERFLOW)
};

char *estr(int f)
{
   106b2:	711d                	addi	sp,sp,-96
   106b4:	e4a6                	sd	s1,72(sp)
   106b6:	64e5                	lui	s1,0x19
   106b8:	f852                	sd	s4,48(sp)
   106ba:	f456                	sd	s5,40(sp)
   106bc:	f05a                	sd	s6,32(sp)
   106be:	94048493          	addi	s1,s1,-1728 # 18940 <eflags>
	char *p = buf;
	int i, all = 0;

	for (i = 0; i < length(eflags); i++)
		if (f & eflags[i].flag) {
			p += sprintf(p, "%s%s", all ? "|" : "", eflags[i].s);
   106c2:	6ae5                	lui	s5,0x19
   106c4:	6b65                	lui	s6,0x19
{
   106c6:	e8a2                	sd	s0,80(sp)
   106c8:	e0ca                	sd	s2,64(sp)
   106ca:	fc4e                	sd	s3,56(sp)
   106cc:	e862                	sd	s8,16(sp)
   106ce:	e466                	sd	s9,8(sp)
   106d0:	ec86                	sd	ra,88(sp)
   106d2:	ec5e                	sd	s7,24(sp)
   106d4:	89aa                	mv	s3,a0
   106d6:	05048c13          	addi	s8,s1,80
	int i, all = 0;
   106da:	4401                	li	s0,0
	char *p = buf;
   106dc:	a4818913          	addi	s2,gp,-1464 # 1c248 <buf.2024>
			p += sprintf(p, "%s%s", all ? "|" : "", eflags[i].s);
   106e0:	6ce5                	lui	s9,0x19
   106e2:	8b0a8a93          	addi	s5,s5,-1872 # 188b0 <__clzdi2+0x2aa>
   106e6:	868b0b13          	addi	s6,s6,-1944 # 18868 <__clzdi2+0x262>
		if (f & eflags[i].flag) {
   106ea:	0004ab83          	lw	s7,0(s1)
   106ee:	013bf7b3          	and	a5,s7,s3
   106f2:	cf89                	beqz	a5,1070c <estr+0x5a>
			p += sprintf(p, "%s%s", all ? "|" : "", eflags[i].s);
   106f4:	8656                	mv	a2,s5
   106f6:	e011                	bnez	s0,106fa <estr+0x48>
   106f8:	865a                	mv	a2,s6
   106fa:	6494                	ld	a3,8(s1)
   106fc:	854a                	mv	a0,s2
   106fe:	8c0c8593          	addi	a1,s9,-1856 # 188c0 <__clzdi2+0x2ba>
   10702:	389020ef          	jal	ra,1328a <sprintf>
   10706:	992a                	add	s2,s2,a0
			all |= eflags[i].flag;
   10708:	008be433          	or	s0,s7,s0
	for (i = 0; i < length(eflags); i++)
   1070c:	04c1                	addi	s1,s1,16
   1070e:	fd849ee3          	bne	s1,s8,106ea <estr+0x38>
		}
	if (all != f) {
   10712:	03340163          	beq	s0,s3,10734 <estr+0x82>
		p += sprintf(p, "%s%d", all ? "|" : "", f & ~all);
   10716:	e829                	bnez	s0,10768 <estr+0xb6>
   10718:	6665                	lui	a2,0x19
   1071a:	86860613          	addi	a2,a2,-1944 # 18868 <__clzdi2+0x262>
   1071e:	fff44693          	not	a3,s0
   10722:	65e5                	lui	a1,0x19
   10724:	854a                	mv	a0,s2
   10726:	0136f6b3          	and	a3,a3,s3
   1072a:	8c858593          	addi	a1,a1,-1848 # 188c8 <__clzdi2+0x2c2>
   1072e:	35d020ef          	jal	ra,1328a <sprintf>
   10732:	992a                	add	s2,s2,a0
		all = f;
	}
	p += sprintf(p, "%s", all ? "" : "0");
   10734:	02099e63          	bnez	s3,10770 <estr+0xbe>
   10738:	6665                	lui	a2,0x19
   1073a:	8b860613          	addi	a2,a2,-1864 # 188b8 <__clzdi2+0x2b2>
   1073e:	65e5                	lui	a1,0x19
   10740:	854a                	mv	a0,s2
   10742:	8d058593          	addi	a1,a1,-1840 # 188d0 <__clzdi2+0x2ca>
   10746:	345020ef          	jal	ra,1328a <sprintf>
	return buf;
}
   1074a:	60e6                	ld	ra,88(sp)
   1074c:	6446                	ld	s0,80(sp)
   1074e:	64a6                	ld	s1,72(sp)
   10750:	6906                	ld	s2,64(sp)
   10752:	79e2                	ld	s3,56(sp)
   10754:	7aa2                	ld	s5,40(sp)
   10756:	7b02                	ld	s6,32(sp)
   10758:	6be2                	ld	s7,24(sp)
   1075a:	6c42                	ld	s8,16(sp)
   1075c:	6ca2                	ld	s9,8(sp)
   1075e:	a4818513          	addi	a0,gp,-1464 # 1c248 <buf.2024>
   10762:	7a42                	ld	s4,48(sp)
   10764:	6125                	addi	sp,sp,96
   10766:	8082                	ret
		p += sprintf(p, "%s%d", all ? "|" : "", f & ~all);
   10768:	6665                	lui	a2,0x19
   1076a:	8b060613          	addi	a2,a2,-1872 # 188b0 <__clzdi2+0x2aa>
   1076e:	bf45                	j	1071e <estr+0x6c>
	p += sprintf(p, "%s", all ? "" : "0");
   10770:	6665                	lui	a2,0x19
   10772:	86860613          	addi	a2,a2,-1944 # 18868 <__clzdi2+0x262>
   10776:	b7e1                	j	1073e <estr+0x8c>

0000000000010778 <rstr>:

char *rstr(int r)
{
	switch (r) {
   10778:	478d                	li	a5,3
   1077a:	00a7ea63          	bltu	a5,a0,1078e <rstr+0x16>
   1077e:	1502                	slli	a0,a0,0x20
   10780:	67e5                	lui	a5,0x19
   10782:	94078793          	addi	a5,a5,-1728 # 18940 <eflags>
   10786:	8175                	srli	a0,a0,0x1d
   10788:	953e                	add	a0,a0,a5
   1078a:	6928                	ld	a0,80(a0)
   1078c:	8082                	ret
{
   1078e:	6565                	lui	a0,0x19
   10790:	8d850513          	addi	a0,a0,-1832 # 188d8 <__clzdi2+0x2d2>
#ifdef FE_DOWNWARD
	case RD: return "RD";
#endif
	}
	return "R?";
}
   10794:	8082                	ret

0000000000010796 <t_pathrel>:
#include <stdio.h>
#include "test.h"

/* relative path to p */
char *t_pathrel(char *buf, size_t n, char *argv0, char *p)
{
   10796:	7179                	addi	sp,sp,-48
   10798:	f022                	sd	s0,32(sp)
   1079a:	ec26                	sd	s1,24(sp)
   1079c:	842a                	mv	s0,a0
   1079e:	84ae                	mv	s1,a1
	char *s = strrchr(argv0, '/');
   107a0:	8532                	mv	a0,a2
   107a2:	02f00593          	li	a1,47
{
   107a6:	e436                	sd	a3,8(sp)
   107a8:	f406                	sd	ra,40(sp)
	char *s = strrchr(argv0, '/');
   107aa:	e032                	sd	a2,0(sp)
   107ac:	298030ef          	jal	ra,13a44 <strrchr>
	int k;

	if (s)
   107b0:	66a2                	ld	a3,8(sp)
   107b2:	c50d                	beqz	a0,107dc <t_pathrel+0x46>
		k = snprintf(buf, n, "%.*s/%s", (int)(s-argv0), argv0, p);
   107b4:	6702                	ld	a4,0(sp)
   107b6:	6665                	lui	a2,0x19
   107b8:	87b6                	mv	a5,a3
   107ba:	9b060613          	addi	a2,a2,-1616 # 189b0 <CSWTCH.18+0x20>
   107be:	40e506bb          	subw	a3,a0,a4
   107c2:	85a6                	mv	a1,s1
   107c4:	8522                	mv	a0,s0
   107c6:	2a9020ef          	jal	ra,1326e <snprintf>
	else
		k = snprintf(buf, n, "./%s", p);
	if ((size_t)k >= n)
   107ca:	00956363          	bltu	a0,s1,107d0 <t_pathrel+0x3a>
		return 0;
   107ce:	4401                	li	s0,0
	return buf;
}
   107d0:	70a2                	ld	ra,40(sp)
   107d2:	8522                	mv	a0,s0
   107d4:	7402                	ld	s0,32(sp)
   107d6:	64e2                	ld	s1,24(sp)
   107d8:	6145                	addi	sp,sp,48
   107da:	8082                	ret
		k = snprintf(buf, n, "./%s", p);
   107dc:	6665                	lui	a2,0x19
   107de:	9b860613          	addi	a2,a2,-1608 # 189b8 <CSWTCH.18+0x28>
   107e2:	85a6                	mv	a1,s1
   107e4:	8522                	mv	a0,s0
   107e6:	289020ef          	jal	ra,1326e <snprintf>
   107ea:	b7c5                	j	107ca <t_pathrel+0x34>

00000000000107ec <t_printf>:
#include "test.h"

volatile int t_status = 0;

int t_printf(const char *s, ...)
{
   107ec:	da010113          	addi	sp,sp,-608
   107f0:	22b13423          	sd	a1,552(sp)
   107f4:	22c13823          	sd	a2,560(sp)
   107f8:	22d13c23          	sd	a3,568(sp)
	char buf[512];
	int n;

	t_status = 1;
	va_start(ap, s);
	n = vsnprintf(buf, sizeof buf, s, ap);
   107fc:	862a                	mv	a2,a0
{
   107fe:	24e13023          	sd	a4,576(sp)
   10802:	24f13423          	sd	a5,584(sp)
	t_status = 1;
   10806:	4705                	li	a4,1
	va_start(ap, s);
   10808:	1434                	addi	a3,sp,552
	n = vsnprintf(buf, sizeof buf, s, ap);
   1080a:	20000593          	li	a1,512
   1080e:	0808                	addi	a0,sp,16
{
   10810:	20113c23          	sd	ra,536(sp)
   10814:	25013823          	sd	a6,592(sp)
   10818:	25113c23          	sd	a7,600(sp)
	t_status = 1;
   1081c:	96e1ac23          	sw	a4,-1672(gp) # 1c178 <_edata>
	va_start(ap, s);
   10820:	e436                	sd	a3,8(sp)
	n = vsnprintf(buf, sizeof buf, s, ap);
   10822:	30d020ef          	jal	ra,1332e <vsnprintf>
	va_end(ap);
	if (n < 0)
		n = 0;
   10826:	4601                	li	a2,0
	if (n < 0)
   10828:	00054f63          	bltz	a0,10846 <t_printf+0x5a>
	else if (n >= sizeof buf) {
   1082c:	1ff00793          	li	a5,511
   10830:	862a                	mv	a2,a0
   10832:	00a7da63          	bge	a5,a0,10846 <t_printf+0x5a>
		n = sizeof buf;
		buf[n - 1] = '\n';
		buf[n - 2] = '.';
		buf[n - 3] = '.';
		buf[n - 4] = '.';
   10836:	0a2e37b7          	lui	a5,0xa2e3
   1083a:	e2e7879b          	addiw	a5,a5,-466
   1083e:	20f12623          	sw	a5,524(sp)
		n = sizeof buf;
   10842:	20000613          	li	a2,512
	}
	return write(1, buf, n);
   10846:	080c                	addi	a1,sp,16
   10848:	4505                	li	a0,1
   1084a:	79a030ef          	jal	ra,13fe4 <write>
}
   1084e:	21813083          	ld	ra,536(sp)
   10852:	2501                	sext.w	a0,a0
   10854:	26010113          	addi	sp,sp,608
   10858:	8082                	ret

000000000001085a <rand32>:

// TODO: use large period prng
static uint64_t seed = -1;
static uint32_t rand32(void)
{
	seed = 6364136223846793005ULL*seed + 1;
   1085a:	9581b503          	ld	a0,-1704(gp) # 1c158 <seed>
   1085e:	9181b703          	ld	a4,-1768(gp) # 1c118 <__SDATA_BEGIN__+0x10>
   10862:	02e50533          	mul	a0,a0,a4
   10866:	0505                	addi	a0,a0,1
   10868:	94a1bc23          	sd	a0,-1704(gp) # 1c158 <seed>
	return seed >> 32;
}
   1086c:	9501                	srai	a0,a0,0x20
   1086e:	8082                	ret

0000000000010870 <t_randseed>:
;
}

void t_randseed(uint64_t s)
{
	seed = s;
   10870:	94a1bc23          	sd	a0,-1704(gp) # 1c158 <seed>
}
   10874:	8082                	ret

0000000000010876 <t_randn>:

/* uniform random in [0,n), n > 0 must hold */
uint64_t t_randn(uint64_t n)
{
   10876:	1101                	addi	sp,sp,-32
   10878:	e426                	sd	s1,8(sp)
	uint64_t r, m;

	/* m is the largest multiple of n */
	m = -1;
	m -= m%n;
   1087a:	54fd                	li	s1,-1
   1087c:	02a4f4b3          	remu	s1,s1,a0
{
   10880:	e04a                	sd	s2,0(sp)
   10882:	ec06                	sd	ra,24(sp)
   10884:	e822                	sd	s0,16(sp)
   10886:	892a                	mv	s2,a0
	m -= m%n;
   10888:	fff4c493          	not	s1,s1
	uint64_t u = rand32();
   1088c:	fcfff0ef          	jal	ra,1085a <rand32>
   10890:	0005041b          	sext.w	s0,a0
	return u<<32 | rand32();
   10894:	fc7ff0ef          	jal	ra,1085a <rand32>
   10898:	02051793          	slli	a5,a0,0x20
   1089c:	1402                	slli	s0,s0,0x20
   1089e:	9381                	srli	a5,a5,0x20
   108a0:	8fc1                	or	a5,a5,s0
	while ((r = rand64()) >= m);
   108a2:	fe97f5e3          	bgeu	a5,s1,1088c <t_randn+0x16>
	return r%n;
}
   108a6:	60e2                	ld	ra,24(sp)
   108a8:	6442                	ld	s0,16(sp)
   108aa:	0327f533          	remu	a0,a5,s2
   108ae:	64a2                	ld	s1,8(sp)
   108b0:	6902                	ld	s2,0(sp)
   108b2:	6105                	addi	sp,sp,32
   108b4:	8082                	ret

00000000000108b6 <shuffle2>:
	return rand64();
}

/* shuffle the elements of p and q until the elements in p are well shuffled */
static void shuffle2(uint64_t *p, uint64_t *q, size_t np, size_t nq)
{
   108b6:	7179                	addi	sp,sp,-48
   108b8:	ec26                	sd	s1,24(sp)
   108ba:	00361493          	slli	s1,a2,0x3
   108be:	f022                	sd	s0,32(sp)
   108c0:	e84a                	sd	s2,16(sp)
   108c2:	e44e                	sd	s3,8(sp)
   108c4:	e052                	sd	s4,0(sp)
   108c6:	f406                	sd	ra,40(sp)
   108c8:	89aa                	mv	s3,a0
   108ca:	8a2e                	mv	s4,a1
   108cc:	8432                	mv	s0,a2
   108ce:	8936                	mv	s2,a3
   108d0:	94aa                	add	s1,s1,a0
	size_t r;
	uint64_t t;

	while (np) {
   108d2:	e809                	bnez	s0,108e4 <shuffle2+0x2e>
		} else {
			p[np] = p[r-nq];
			p[r-nq] = t;
		}
	}
}
   108d4:	70a2                	ld	ra,40(sp)
   108d6:	7402                	ld	s0,32(sp)
   108d8:	64e2                	ld	s1,24(sp)
   108da:	6942                	ld	s2,16(sp)
   108dc:	69a2                	ld	s3,8(sp)
   108de:	6a02                	ld	s4,0(sp)
   108e0:	6145                	addi	sp,sp,48
   108e2:	8082                	ret
		r = t_randn(nq+np--);
   108e4:	01240533          	add	a0,s0,s2
   108e8:	f8fff0ef          	jal	ra,10876 <t_randn>
		t = p[np];
   108ec:	ff84b703          	ld	a4,-8(s1)
		if (r < nq) {
   108f0:	01257b63          	bgeu	a0,s2,10906 <shuffle2+0x50>
			p[np] = q[r];
   108f4:	00351793          	slli	a5,a0,0x3
   108f8:	97d2                	add	a5,a5,s4
			p[np] = p[r-nq];
   108fa:	6394                	ld	a3,0(a5)
   108fc:	147d                	addi	s0,s0,-1
   108fe:	14e1                	addi	s1,s1,-8
   10900:	e094                	sd	a3,0(s1)
			p[r-nq] = t;
   10902:	e398                	sd	a4,0(a5)
   10904:	b7f9                	j	108d2 <shuffle2+0x1c>
			p[np] = p[r-nq];
   10906:	412507b3          	sub	a5,a0,s2
   1090a:	078e                	slli	a5,a5,0x3
   1090c:	97ce                	add	a5,a5,s3
   1090e:	b7f5                	j	108fa <shuffle2+0x44>

0000000000010910 <t_randint>:
{
   10910:	1141                	addi	sp,sp,-16
	uint64_t n = b - a + 1;
   10912:	8d89                	sub	a1,a1,a0
{
   10914:	e022                	sd	s0,0(sp)
   10916:	e406                	sd	ra,8(sp)
   10918:	842a                	mv	s0,a0
	uint64_t n = b - a + 1;
   1091a:	00158513          	addi	a0,a1,1
	if (n)
   1091e:	c901                	beqz	a0,1092e <t_randint+0x1e>
		return a + t_randn(n);
   10920:	f57ff0ef          	jal	ra,10876 <t_randn>
   10924:	9522                	add	a0,a0,s0
}
   10926:	60a2                	ld	ra,8(sp)
   10928:	6402                	ld	s0,0(sp)
   1092a:	0141                	addi	sp,sp,16
   1092c:	8082                	ret
	uint64_t u = rand32();
   1092e:	f2dff0ef          	jal	ra,1085a <rand32>
   10932:	0005041b          	sext.w	s0,a0
	return u<<32 | rand32();
   10936:	f25ff0ef          	jal	ra,1085a <rand32>
   1093a:	1502                	slli	a0,a0,0x20
   1093c:	1402                	slli	s0,s0,0x20
   1093e:	9101                	srli	a0,a0,0x20
   10940:	8d41                	or	a0,a0,s0
	return rand64();
   10942:	b7d5                	j	10926 <t_randint+0x16>

0000000000010944 <t_shuffle>:

/* shuffle the elements of p */
void t_shuffle(uint64_t *p, size_t n)
{
   10944:	862e                	mv	a2,a1
	shuffle2(p,0,n,0);
   10946:	4681                	li	a3,0
   10948:	4581                	li	a1,0
   1094a:	b7b5                	j	108b6 <shuffle2>

000000000001094c <t_randrange>:
}

void t_randrange(uint64_t *p, size_t n)
{
   1094c:	87ae                	mv	a5,a1
	size_t i;
	for (i = 0; i < n; i++)
   1094e:	4581                	li	a1,0
   10950:	00f59363          	bne	a1,a5,10956 <t_randrange+0xa>
		p[i] = i;
	t_shuffle(p, n);
   10954:	bfc5                	j	10944 <t_shuffle>
		p[i] = i;
   10956:	00359713          	slli	a4,a1,0x3
   1095a:	972a                	add	a4,a4,a0
   1095c:	e30c                	sd	a1,0(a4)
	for (i = 0; i < n; i++)
   1095e:	0585                	addi	a1,a1,1
   10960:	bfc5                	j	10950 <t_randrange+0x4>

0000000000010962 <t_choose>:
int t_choose(uint64_t n, size_t k, uint64_t *p)
{
	uint64_t *tab;
	size_t i, j, len;

	if (n < k)
   10962:	00b57f63          	bgeu	a0,a1,10980 <t_choose+0x1e>
		return -1;
   10966:	557d                	li	a0,-1
	for (i = 0; i < len; i++)
		if (tab[i])
			*p++ = tab[i]-1;
	free(tab);
	return 0;
}
   10968:	8082                	ret
		return -1;
   1096a:	557d                	li	a0,-1
}
   1096c:	60a6                	ld	ra,72(sp)
   1096e:	6406                	ld	s0,64(sp)
   10970:	74e2                	ld	s1,56(sp)
   10972:	7942                	ld	s2,48(sp)
   10974:	79a2                	ld	s3,40(sp)
   10976:	7a02                	ld	s4,32(sp)
   10978:	6ae2                	ld	s5,24(sp)
   1097a:	6b42                	ld	s6,16(sp)
   1097c:	6161                	addi	sp,sp,80
   1097e:	8082                	ret
{
   10980:	715d                	addi	sp,sp,-80
   10982:	e0a2                	sd	s0,64(sp)
   10984:	fc26                	sd	s1,56(sp)
   10986:	f84a                	sd	s2,48(sp)
   10988:	e486                	sd	ra,72(sp)
   1098a:	f44e                	sd	s3,40(sp)
   1098c:	f052                	sd	s4,32(sp)
   1098e:	ec56                	sd	s5,24(sp)
   10990:	e85a                	sd	s6,16(sp)
	if (n < 16) {
   10992:	47bd                	li	a5,15
   10994:	84aa                	mv	s1,a0
   10996:	842e                	mv	s0,a1
   10998:	8932                	mv	s2,a2
   1099a:	02a7e463          	bltu	a5,a0,109c2 <t_choose+0x60>
		while (k)
   1099e:	e019                	bnez	s0,109a4 <t_choose+0x42>
	return 0;
   109a0:	4501                	li	a0,0
   109a2:	b7e9                	j	1096c <t_choose+0xa>
			if (t_randn(n--) < k)
   109a4:	8526                	mv	a0,s1
   109a6:	fff48993          	addi	s3,s1,-1
   109aa:	ecdff0ef          	jal	ra,10876 <t_randn>
   109ae:	00857863          	bgeu	a0,s0,109be <t_choose+0x5c>
				p[--k] = n;
   109b2:	147d                	addi	s0,s0,-1
   109b4:	00341793          	slli	a5,s0,0x3
   109b8:	97ca                	add	a5,a5,s2
   109ba:	0137b023          	sd	s3,0(a5) # a2e3000 <__BSS_END__+0xa2c65e0>
{
   109be:	84ce                	mv	s1,s3
   109c0:	bff9                	j	1099e <t_choose+0x3c>
	if (k < 8) {
   109c2:	479d                	li	a5,7
   109c4:	02b7eb63          	bltu	a5,a1,109fa <t_choose+0x98>
		for (i = 0; i < k;) {
   109c8:	4981                	li	s3,0
   109ca:	a01d                	j	109f0 <t_choose+0x8e>
			p[i] = t_randn(n);
   109cc:	00399a13          	slli	s4,s3,0x3
   109d0:	8526                	mv	a0,s1
   109d2:	ea5ff0ef          	jal	ra,10876 <t_randn>
   109d6:	9a4a                	add	s4,s4,s2
   109d8:	00aa3023          	sd	a0,0(s4)
			for (j = 0; p[j] != p[i]; j++);
   109dc:	4781                	li	a5,0
   109de:	00379713          	slli	a4,a5,0x3
   109e2:	974a                	add	a4,a4,s2
   109e4:	6318                	ld	a4,0(a4)
   109e6:	00e51863          	bne	a0,a4,109f6 <t_choose+0x94>
			if (j == i)
   109ea:	00f99363          	bne	s3,a5,109f0 <t_choose+0x8e>
				i++;
   109ee:	0985                	addi	s3,s3,1
		for (i = 0; i < k;) {
   109f0:	fc89eee3          	bltu	s3,s0,109cc <t_choose+0x6a>
   109f4:	b775                	j	109a0 <t_choose+0x3e>
			for (j = 0; p[j] != p[i]; j++);
   109f6:	0785                	addi	a5,a5,1
   109f8:	b7dd                	j	109de <t_choose+0x7c>
	if (n < 5*k && (n-k)*sizeof *tab < (size_t)-1) {
   109fa:	4795                	li	a5,5
   109fc:	02f587b3          	mul	a5,a1,a5
   10a00:	04f56063          	bltu	a0,a5,10a40 <t_choose+0xde>
	for (len = 16; len < 2*k; len *= 2);
   10a04:	00159793          	slli	a5,a1,0x1
   10a08:	4a41                	li	s4,16
   10a0a:	08fa6663          	bltu	s4,a5,10a96 <t_choose+0x134>
	tab = calloc(len, sizeof *tab);
   10a0e:	45a1                	li	a1,8
   10a10:	8552                	mv	a0,s4
   10a12:	0c9000ef          	jal	ra,112da <calloc>
   10a16:	89aa                	mv	s3,a0
	if (!tab)
   10a18:	d929                	beqz	a0,1096a <t_choose+0x8>
	for (i = 0; i < k; i++)
   10a1a:	4a81                	li	s5,0
	size_t i = v & (len-1);
   10a1c:	fffa0b13          	addi	s6,s4,-1
	for (i = 0; i < k; i++)
   10a20:	068a9d63          	bne	s5,s0,10a9a <t_choose+0x138>
	for (i = 0; i < len; i++)
   10a24:	4781                	li	a5,0
   10a26:	05478663          	beq	a5,s4,10a72 <t_choose+0x110>
		if (tab[i])
   10a2a:	00379713          	slli	a4,a5,0x3
   10a2e:	974e                	add	a4,a4,s3
   10a30:	6318                	ld	a4,0(a4)
   10a32:	c709                	beqz	a4,10a3c <t_choose+0xda>
			*p++ = tab[i]-1;
   10a34:	177d                	addi	a4,a4,-1
   10a36:	00e93023          	sd	a4,0(s2)
   10a3a:	0921                	addi	s2,s2,8
	for (i = 0; i < len; i++)
   10a3c:	0785                	addi	a5,a5,1
   10a3e:	b7e5                	j	10a26 <t_choose+0xc4>
	if (n < 5*k && (n-k)*sizeof *tab < (size_t)-1) {
   10a40:	40b50633          	sub	a2,a0,a1
		tab = malloc((n-k) * sizeof *tab);
   10a44:	00361513          	slli	a0,a2,0x3
   10a48:	e432                	sd	a2,8(sp)
   10a4a:	371000ef          	jal	ra,115ba <malloc>
   10a4e:	89aa                	mv	s3,a0
		if (!tab)
   10a50:	dd09                	beqz	a0,1096a <t_choose+0x8>
   10a52:	6622                	ld	a2,8(sp)
		for (i = 0; i < k; i++)
   10a54:	4781                	li	a5,0
   10a56:	02879263          	bne	a5,s0,10a7a <t_choose+0x118>
   10a5a:	874e                	mv	a4,s3
   10a5c:	87a2                	mv	a5,s0
		for (; i < n; i++)
   10a5e:	02979463          	bne	a5,s1,10a86 <t_choose+0x124>
		if (k < n-k)
   10a62:	02c47663          	bgeu	s0,a2,10a8e <t_choose+0x12c>
			shuffle2(p, tab, k, n-k);
   10a66:	86b2                	mv	a3,a2
   10a68:	85ce                	mv	a1,s3
   10a6a:	8622                	mv	a2,s0
   10a6c:	854a                	mv	a0,s2
			shuffle2(tab, p, n-k, k);
   10a6e:	e49ff0ef          	jal	ra,108b6 <shuffle2>
	free(tab);
   10a72:	854e                	mv	a0,s3
   10a74:	113000ef          	jal	ra,11386 <free>
   10a78:	b725                	j	109a0 <t_choose+0x3e>
			p[i] = i;
   10a7a:	00379713          	slli	a4,a5,0x3
   10a7e:	974a                	add	a4,a4,s2
   10a80:	e31c                	sd	a5,0(a4)
		for (i = 0; i < k; i++)
   10a82:	0785                	addi	a5,a5,1
   10a84:	bfc9                	j	10a56 <t_choose+0xf4>
			tab[i-k] = i;
   10a86:	e31c                	sd	a5,0(a4)
		for (; i < n; i++)
   10a88:	0785                	addi	a5,a5,1
   10a8a:	0721                	addi	a4,a4,8
   10a8c:	bfc9                	j	10a5e <t_choose+0xfc>
			shuffle2(tab, p, n-k, k);
   10a8e:	86a2                	mv	a3,s0
   10a90:	85ca                	mv	a1,s2
   10a92:	854e                	mv	a0,s3
   10a94:	bfe9                	j	10a6e <t_choose+0x10c>
	for (len = 16; len < 2*k; len *= 2);
   10a96:	0a06                	slli	s4,s4,0x1
   10a98:	bf8d                	j	10a0a <t_choose+0xa8>
		while (insert(tab, len, t_randn(n)+1));
   10a9a:	8526                	mv	a0,s1
   10a9c:	ddbff0ef          	jal	ra,10876 <t_randn>
   10aa0:	0505                	addi	a0,a0,1
	size_t i = v & (len-1);
   10aa2:	016577b3          	and	a5,a0,s6
	size_t j = 1;
   10aa6:	4685                	li	a3,1
	while (tab[i]) {
   10aa8:	00379713          	slli	a4,a5,0x3
   10aac:	974e                	add	a4,a4,s3
   10aae:	6310                	ld	a2,0(a4)
   10ab0:	e601                	bnez	a2,10ab8 <t_choose+0x156>
	tab[i] = v;
   10ab2:	e308                	sd	a0,0(a4)
	for (i = 0; i < k; i++)
   10ab4:	0a85                	addi	s5,s5,1
   10ab6:	b7ad                	j	10a20 <t_choose+0xbe>
		if (tab[i] == v)
   10ab8:	fec501e3          	beq	a0,a2,10a9a <t_choose+0x138>
		i += j++;
   10abc:	97b6                	add	a5,a5,a3
		i &= len-1;
   10abe:	00fb77b3          	and	a5,s6,a5
		i += j++;
   10ac2:	0685                	addi	a3,a3,1
   10ac4:	b7d5                	j	10aa8 <t_choose+0x146>

0000000000010ac6 <t_setrlim>:
#include <errno.h>
#include <sys/resource.h>
#include "test.h"

int t_setrlim(int r, long lim)
{
   10ac6:	7179                	addi	sp,sp,-48
   10ac8:	f022                	sd	s0,32(sp)
   10aca:	842e                	mv	s0,a1
	struct rlimit rl;

	if (getrlimit(r, &rl)) {
   10acc:	858a                	mv	a1,sp
{
   10ace:	e84a                	sd	s2,16(sp)
   10ad0:	f406                	sd	ra,40(sp)
   10ad2:	ec26                	sd	s1,24(sp)
   10ad4:	892a                	mv	s2,a0
	if (getrlimit(r, &rl)) {
   10ad6:	727010ef          	jal	ra,129fc <getrlimit>
   10ada:	c50d                	beqz	a0,10b04 <t_setrlim+0x3e>
		t_error("getrlimit %d: %s\n", r, strerror(errno));
   10adc:	490000ef          	jal	ra,10f6c <__errno_location>
   10ae0:	4108                	lw	a0,0(a0)
   10ae2:	4b6000ef          	jal	ra,10f98 <strerror>
   10ae6:	862a                	mv	a2,a0
   10ae8:	6565                	lui	a0,0x19
   10aea:	85ca                	mv	a1,s2
   10aec:	9c050513          	addi	a0,a0,-1600 # 189c0 <CSWTCH.18+0x30>
   10af0:	cfdff0ef          	jal	ra,107ec <t_printf>
		return -1;
	}
	if (lim > rl.rlim_max)
		return -1;
   10af4:	54fd                	li	s1,-1
	if (setrlimit(r, &rl)) {
		t_error("setrlimit(%d, %ld): %s\n", r, lim, strerror(errno));
		return -1;
	}
	return 0;
}
   10af6:	70a2                	ld	ra,40(sp)
   10af8:	7402                	ld	s0,32(sp)
   10afa:	6942                	ld	s2,16(sp)
   10afc:	8526                	mv	a0,s1
   10afe:	64e2                	ld	s1,24(sp)
   10b00:	6145                	addi	sp,sp,48
   10b02:	8082                	ret
	if (lim > rl.rlim_max)
   10b04:	67a2                	ld	a5,8(sp)
   10b06:	fe87e7e3          	bltu	a5,s0,10af4 <t_setrlim+0x2e>
   10b0a:	84aa                	mv	s1,a0
	if (lim == rl.rlim_max && lim == rl.rlim_cur)
   10b0c:	00879563          	bne	a5,s0,10b16 <t_setrlim+0x50>
   10b10:	6782                	ld	a5,0(sp)
   10b12:	fef402e3          	beq	s0,a5,10af6 <t_setrlim+0x30>
	if (setrlimit(r, &rl)) {
   10b16:	858a                	mv	a1,sp
   10b18:	854a                	mv	a0,s2
	rl.rlim_max = lim;
   10b1a:	e422                	sd	s0,8(sp)
	rl.rlim_cur = lim;
   10b1c:	e022                	sd	s0,0(sp)
	if (setrlimit(r, &rl)) {
   10b1e:	75f010ef          	jal	ra,12a7c <setrlimit>
   10b22:	d971                	beqz	a0,10af6 <t_setrlim+0x30>
		t_error("setrlimit(%d, %ld): %s\n", r, lim, strerror(errno));
   10b24:	448000ef          	jal	ra,10f6c <__errno_location>
   10b28:	4108                	lw	a0,0(a0)
   10b2a:	46e000ef          	jal	ra,10f98 <strerror>
   10b2e:	86aa                	mv	a3,a0
   10b30:	6565                	lui	a0,0x19
   10b32:	8622                	mv	a2,s0
   10b34:	85ca                	mv	a1,s2
   10b36:	9f050513          	addi	a0,a0,-1552 # 189f0 <CSWTCH.18+0x60>
   10b3a:	cb3ff0ef          	jal	ra,107ec <t_printf>
		return -1;
   10b3e:	bf5d                	j	10af4 <t_setrlim+0x2e>

0000000000010b40 <t_setutf8>:
#include "test.h"

int t_setutf8()
{
	(void)(
	setlocale(LC_CTYPE, "C.UTF-8") ||
   10b40:	65e5                	lui	a1,0x19
{
   10b42:	1141                	addi	sp,sp,-16
	setlocale(LC_CTYPE, "C.UTF-8") ||
   10b44:	a2858593          	addi	a1,a1,-1496 # 18a28 <CSWTCH.18+0x98>
   10b48:	4501                	li	a0,0
{
   10b4a:	e022                	sd	s0,0(sp)
   10b4c:	e406                	sd	ra,8(sp)
	setlocale(LC_CTYPE, "C.UTF-8") ||
   10b4e:	606000ef          	jal	ra,11154 <setlocale>
   10b52:	6465                	lui	s0,0x19
	(void)(
   10b54:	e139                	bnez	a0,10b9a <t_setutf8+0x5a>
	setlocale(LC_CTYPE, "POSIX.UTF-8") ||
   10b56:	65e5                	lui	a1,0x19
   10b58:	a3058593          	addi	a1,a1,-1488 # 18a30 <CSWTCH.18+0xa0>
   10b5c:	5f8000ef          	jal	ra,11154 <setlocale>
	setlocale(LC_CTYPE, "C.UTF-8") ||
   10b60:	ed0d                	bnez	a0,10b9a <t_setutf8+0x5a>
	setlocale(LC_CTYPE, "en_US.UTF-8") ||
   10b62:	65e5                	lui	a1,0x19
   10b64:	a4058593          	addi	a1,a1,-1472 # 18a40 <CSWTCH.18+0xb0>
   10b68:	5ec000ef          	jal	ra,11154 <setlocale>
	setlocale(LC_CTYPE, "POSIX.UTF-8") ||
   10b6c:	e51d                	bnez	a0,10b9a <t_setutf8+0x5a>
	setlocale(LC_CTYPE, "en_GB.UTF-8") ||
   10b6e:	65e5                	lui	a1,0x19
   10b70:	a5058593          	addi	a1,a1,-1456 # 18a50 <CSWTCH.18+0xc0>
   10b74:	5e0000ef          	jal	ra,11154 <setlocale>
	setlocale(LC_CTYPE, "en_US.UTF-8") ||
   10b78:	e10d                	bnez	a0,10b9a <t_setutf8+0x5a>
	setlocale(LC_CTYPE, "en.UTF-8") ||
   10b7a:	65e5                	lui	a1,0x19
   10b7c:	a6058593          	addi	a1,a1,-1440 # 18a60 <CSWTCH.18+0xd0>
   10b80:	5d4000ef          	jal	ra,11154 <setlocale>
	setlocale(LC_CTYPE, "en_GB.UTF-8") ||
   10b84:	e919                	bnez	a0,10b9a <t_setutf8+0x5a>
	setlocale(LC_CTYPE, "UTF-8") ||
   10b86:	a7040593          	addi	a1,s0,-1424 # 18a70 <CSWTCH.18+0xe0>
   10b8a:	5ca000ef          	jal	ra,11154 <setlocale>
	setlocale(LC_CTYPE, "en.UTF-8") ||
   10b8e:	e511                	bnez	a0,10b9a <t_setutf8+0x5a>
	setlocale(LC_CTYPE, "") );
   10b90:	65e5                	lui	a1,0x19
   10b92:	86858593          	addi	a1,a1,-1944 # 18868 <__clzdi2+0x262>
   10b96:	5be000ef          	jal	ra,11154 <setlocale>
	
	if (strcmp(nl_langinfo(CODESET), "UTF-8"))
   10b9a:	4539                	li	a0,14
   10b9c:	5b0000ef          	jal	ra,1114c <__nl_langinfo>
   10ba0:	a7040593          	addi	a1,s0,-1424
   10ba4:	62d020ef          	jal	ra,139d0 <strcmp>
   10ba8:	cd01                	beqz	a0,10bc0 <t_setutf8+0x80>
		return t_error("cannot set UTF-8 locale for test (codeset=%s)\n", nl_langinfo(CODESET));
   10baa:	4539                	li	a0,14
   10bac:	5a0000ef          	jal	ra,1114c <__nl_langinfo>

	return 0;
}
   10bb0:	6402                	ld	s0,0(sp)
   10bb2:	60a2                	ld	ra,8(sp)
		return t_error("cannot set UTF-8 locale for test (codeset=%s)\n", nl_langinfo(CODESET));
   10bb4:	85aa                	mv	a1,a0
   10bb6:	6565                	lui	a0,0x19
   10bb8:	a7850513          	addi	a0,a0,-1416 # 18a78 <CSWTCH.18+0xe8>
}
   10bbc:	0141                	addi	sp,sp,16
		return t_error("cannot set UTF-8 locale for test (codeset=%s)\n", nl_langinfo(CODESET));
   10bbe:	b13d                	j	107ec <t_printf>
}
   10bc0:	60a2                	ld	ra,8(sp)
   10bc2:	6402                	ld	s0,0(sp)
   10bc4:	0141                	addi	sp,sp,16
   10bc6:	8082                	ret

0000000000010bc8 <t_vmfill>:
fills the virtual memory with anonymous PROT_NONE mmaps,
returns the mappings in *p and *n in decreasing size order,
the return value is the number of mappings or -1 on failure.
*/
int t_vmfill(void **p, size_t *n, int len)
{
   10bc8:	715d                	addi	sp,sp,-80
   10bca:	fc26                	sd	s1,56(sp)
	int fd = MAP_ANONYMOUS ? -1 : open("/dev/zero", O_RDWR);
	size_t start = SIZE_MAX/2 + 1;
   10bcc:	54fd                	li	s1,-1
{
   10bce:	f84a                	sd	s2,48(sp)
   10bd0:	f44e                	sd	s3,40(sp)
   10bd2:	f052                	sd	s4,32(sp)
   10bd4:	ec56                	sd	s5,24(sp)
   10bd6:	e486                	sd	ra,72(sp)
   10bd8:	e0a2                	sd	s0,64(sp)
   10bda:	e85a                	sd	s6,16(sp)
   10bdc:	e45e                	sd	s7,8(sp)
   10bde:	e062                	sd	s8,0(sp)
   10be0:	89aa                	mv	s3,a0
   10be2:	8a2e                	mv	s4,a1
   10be4:	8ab2                	mv	s5,a2
   10be6:	4901                	li	s2,0
	size_t start = SIZE_MAX/2 + 1;
   10be8:	14fe                	slli	s1,s1,0x3f
   10bea:	00090b1b          	sext.w	s6,s2
	for (i=n=*start; i>=PAGE_SIZE; i/=2) {
   10bee:	8426                	mv	s0,s1
   10bf0:	8ba6                	mv	s7,s1
   10bf2:	4579                	li	a0,30
   10bf4:	094000ef          	jal	ra,10c88 <sysconf>
   10bf8:	04abf063          	bgeu	s7,a0,10c38 <t_vmfill+0x70>
	return n & -PAGE_SIZE;
   10bfc:	4579                	li	a0,30
   10bfe:	08a000ef          	jal	ra,10c88 <sysconf>
   10c02:	40a007b3          	neg	a5,a0
   10c06:	8c7d                	and	s0,s0,a5
	void *q;
	int i;

	for (i=0;;i++) {
		m = mmax(fd, &start);
		if (!m)
   10c08:	c03d                	beqz	s0,10c6e <t_vmfill+0xa6>
			break;
		q = mmap(0, m, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, fd, 0);
   10c0a:	4781                	li	a5,0
   10c0c:	577d                	li	a4,-1
   10c0e:	02200693          	li	a3,34
   10c12:	4601                	li	a2,0
   10c14:	85a2                	mv	a1,s0
   10c16:	4501                	li	a0,0
   10c18:	6df010ef          	jal	ra,12af6 <__mmap>
		if (q == MAP_FAILED)
   10c1c:	57fd                	li	a5,-1
   10c1e:	04f50763          	beq	a0,a5,10c6c <t_vmfill+0xa4>
			return -1;
		if (i < len) {
   10c22:	015b5963          	bge	s6,s5,10c34 <t_vmfill+0x6c>
			p[i] = q;
   10c26:	00391793          	slli	a5,s2,0x3
   10c2a:	00f98733          	add	a4,s3,a5
   10c2e:	e308                	sd	a0,0(a4)
			n[i] = m;
   10c30:	97d2                	add	a5,a5,s4
   10c32:	e380                	sd	s0,0(a5)
	for (i=0;;i++) {
   10c34:	0905                	addi	s2,s2,1
		m = mmax(fd, &start);
   10c36:	bf55                	j	10bea <t_vmfill+0x22>
		if ((p=mmap(0, n, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, fd, 0)) == MAP_FAILED) {
   10c38:	4781                	li	a5,0
   10c3a:	577d                	li	a4,-1
   10c3c:	02200693          	li	a3,34
   10c40:	4601                	li	a2,0
   10c42:	85a2                	mv	a1,s0
   10c44:	4501                	li	a0,0
   10c46:	6b1010ef          	jal	ra,12af6 <__mmap>
   10c4a:	57fd                	li	a5,-1
   10c4c:	001bdc13          	srli	s8,s7,0x1
   10c50:	00f51663          	bne	a0,a5,10c5c <t_vmfill+0x94>
			n -= i/2;
   10c54:	41840433          	sub	s0,s0,s8
	for (i=n=*start; i>=PAGE_SIZE; i/=2) {
   10c58:	8be2                	mv	s7,s8
   10c5a:	bf61                	j	10bf2 <t_vmfill+0x2a>
			munmap(p, n);
   10c5c:	85a2                	mv	a1,s0
   10c5e:	749010ef          	jal	ra,12ba6 <__munmap>
			if (n == i)
   10c62:	01741363          	bne	s0,s7,10c68 <t_vmfill+0xa0>
   10c66:	84a2                	mv	s1,s0
			n += i/2;
   10c68:	9462                	add	s0,s0,s8
   10c6a:	b7fd                	j	10c58 <t_vmfill+0x90>
			return -1;
   10c6c:	5b7d                	li	s6,-1
		}
	}
	return i;
}
   10c6e:	60a6                	ld	ra,72(sp)
   10c70:	6406                	ld	s0,64(sp)
   10c72:	74e2                	ld	s1,56(sp)
   10c74:	7942                	ld	s2,48(sp)
   10c76:	79a2                	ld	s3,40(sp)
   10c78:	7a02                	ld	s4,32(sp)
   10c7a:	6ae2                	ld	s5,24(sp)
   10c7c:	6ba2                	ld	s7,8(sp)
   10c7e:	6c02                	ld	s8,0(sp)
   10c80:	855a                	mv	a0,s6
   10c82:	6b42                	ld	s6,16(sp)
   10c84:	6161                	addi	sp,sp,80
   10c86:	8082                	ret

0000000000010c88 <sysconf>:
   10c88:	7165                	addi	sp,sp,-400
   10c8a:	e706                	sd	ra,392(sp)
   10c8c:	e322                	sd	s0,384(sp)
   10c8e:	fea6                	sd	s1,376(sp)
   10c90:	0f800793          	li	a5,248
   10c94:	00a7ec63          	bltu	a5,a0,10cac <sysconf+0x24>
   10c98:	6765                	lui	a4,0x19
   10c9a:	00151793          	slli	a5,a0,0x1
   10c9e:	af070713          	addi	a4,a4,-1296 # 18af0 <values.2211>
   10ca2:	97ba                	add	a5,a5,a4
   10ca4:	842a                	mv	s0,a0
   10ca6:	00079503          	lh	a0,0(a5)
   10caa:	e919                	bnez	a0,10cc0 <sysconf+0x38>
   10cac:	2c0000ef          	jal	ra,10f6c <__errno_location>
   10cb0:	47d9                	li	a5,22
   10cb2:	c11c                	sw	a5,0(a0)
   10cb4:	557d                	li	a0,-1
   10cb6:	60ba                	ld	ra,392(sp)
   10cb8:	641a                	ld	s0,384(sp)
   10cba:	74f6                	ld	s1,376(sp)
   10cbc:	6159                	addi	sp,sp,400
   10cbe:	8082                	ret
   10cc0:	54fd                	li	s1,-1
   10cc2:	0005071b          	sext.w	a4,a0
   10cc6:	fe9558e3          	bge	a0,s1,10cb6 <sysconf+0x2e>
   10cca:	f0000793          	li	a5,-256
   10cce:	02f75163          	bge	a4,a5,10cf0 <sysconf+0x68>
   10cd2:	154a                	slli	a0,a0,0x32
   10cd4:	858a                	mv	a1,sp
   10cd6:	9149                	srli	a0,a0,0x32
   10cd8:	525010ef          	jal	ra,129fc <getrlimit>
   10cdc:	6782                	ld	a5,0(sp)
   10cde:	557d                	li	a0,-1
   10ce0:	fc978be3          	beq	a5,s1,10cb6 <sysconf+0x2e>
   10ce4:	853e                	mv	a0,a5
   10ce6:	fc07d8e3          	bgez	a5,10cb6 <sysconf+0x2e>
   10cea:	0014d513          	srli	a0,s1,0x1
   10cee:	b7e1                	j	10cb6 <sysconf+0x2e>
   10cf0:	fff5079b          	addiw	a5,a0,-1
   10cf4:	0ff7f793          	andi	a5,a5,255
   10cf8:	4729                	li	a4,10
   10cfa:	faf76ee3          	bltu	a4,a5,10cb6 <sysconf+0x2e>
   10cfe:	6765                	lui	a4,0x19
   10d00:	078a                	slli	a5,a5,0x2
   10d02:	ac070713          	addi	a4,a4,-1344 # 18ac0 <CSWTCH.18+0x130>
   10d06:	97ba                	add	a5,a5,a4
   10d08:	439c                	lw	a5,0(a5)
   10d0a:	8782                	jr	a5
   10d0c:	00020537          	lui	a0,0x20
   10d10:	b75d                	j	10cb6 <sysconf+0x2e>
   10d12:	6521                	lui	a0,0x8
   10d14:	b74d                	j	10cb6 <sysconf+0x2e>
   10d16:	1b01b503          	ld	a0,432(gp) # 1c9b0 <__libc+0x30>
   10d1a:	bf71                	j	10cb6 <sysconf+0x2e>
   10d1c:	80000537          	lui	a0,0x80000
   10d20:	fff54513          	not	a0,a0
   10d24:	bf49                	j	10cb6 <sysconf+0x2e>
   10d26:	4785                	li	a5,1
   10d28:	07800613          	li	a2,120
   10d2c:	4581                	li	a1,0
   10d2e:	0028                	addi	a0,sp,8
   10d30:	e03e                	sd	a5,0(sp)
   10d32:	32b020ef          	jal	ra,1385c <memset>
   10d36:	07b00893          	li	a7,123
   10d3a:	4501                	li	a0,0
   10d3c:	08000593          	li	a1,128
   10d40:	860a                	mv	a2,sp
   10d42:	00000073          	ecall
   10d46:	4701                	li	a4,0
   10d48:	4501                	li	a0,0
   10d4a:	08000613          	li	a2,128
   10d4e:	1a9c                	addi	a5,sp,368
   10d50:	00e786b3          	add	a3,a5,a4
   10d54:	e906c783          	lbu	a5,-368(a3) # 7f7ffe90 <__BSS_END__+0x7f7e3470>
   10d58:	e789                	bnez	a5,10d62 <sysconf+0xda>
   10d5a:	2705                	addiw	a4,a4,1
   10d5c:	fec719e3          	bne	a4,a2,10d4e <sysconf+0xc6>
   10d60:	bf99                	j	10cb6 <sysconf+0x2e>
   10d62:	fff7859b          	addiw	a1,a5,-1
   10d66:	8fed                	and	a5,a5,a1
   10d68:	e8f68823          	sb	a5,-368(a3)
   10d6c:	2505                	addiw	a0,a0,1
   10d6e:	b7c5                	j	10d4e <sysconf+0xc6>
   10d70:	850a                	mv	a0,sp
   10d72:	2fa000ef          	jal	ra,1106c <__lsysinfo>
   10d76:	57a6                	lw	a5,104(sp)
   10d78:	e399                	bnez	a5,10d7e <sysconf+0xf6>
   10d7a:	4785                	li	a5,1
   10d7c:	d4be                	sw	a5,104(sp)
   10d7e:	05500793          	li	a5,85
   10d82:	02f41063          	bne	s0,a5,10da2 <sysconf+0x11a>
   10d86:	7782                	ld	a5,32(sp)
   10d88:	06816503          	lwu	a0,104(sp)
   10d8c:	02f50533          	mul	a0,a0,a5
   10d90:	1b01b783          	ld	a5,432(gp) # 1c9b0 <__libc+0x30>
   10d94:	02f55533          	divu	a0,a0,a5
   10d98:	f0055fe3          	bgez	a0,10cb6 <sysconf+0x2e>
   10d9c:	557d                	li	a0,-1
   10d9e:	8105                	srli	a0,a0,0x1
   10da0:	bf19                	j	10cb6 <sysconf+0x2e>
   10da2:	7522                	ld	a0,40(sp)
   10da4:	77e2                	ld	a5,56(sp)
   10da6:	97aa                	add	a5,a5,a0
   10da8:	b7c5                	j	10d88 <sysconf+0x100>
   10daa:	4501                	li	a0,0
   10dac:	b729                	j	10cb6 <sysconf+0x2e>
   10dae:	00031537          	lui	a0,0x31
   10db2:	06950513          	addi	a0,a0,105 # 31069 <__BSS_END__+0x14649>
   10db6:	b701                	j	10cb6 <sysconf+0x2e>

0000000000010db8 <_init>:
   10db8:	8082                	ret

0000000000010dba <__init_ssp>:
   10dba:	8082                	ret

0000000000010dbc <__init_libc>:
   10dbc:	7109                	addi	sp,sp,-384
   10dbe:	f2ca                	sd	s2,352(sp)
   10dc0:	03010913          	addi	s2,sp,48
   10dc4:	faa2                	sd	s0,368(sp)
   10dc6:	f6a6                	sd	s1,360(sp)
   10dc8:	842e                	mv	s0,a1
   10dca:	84aa                	mv	s1,a0
   10dcc:	13000613          	li	a2,304
   10dd0:	4581                	li	a1,0
   10dd2:	854a                	mv	a0,s2
   10dd4:	fe86                	sd	ra,376(sp)
   10dd6:	287020ef          	jal	ra,1385c <memset>
   10dda:	9c91bc23          	sd	s1,-1576(gp) # 1c1d8 <__environ>
   10dde:	4501                	li	a0,0
   10de0:	00351793          	slli	a5,a0,0x3
   10de4:	97a6                	add	a5,a5,s1
   10de6:	639c                	ld	a5,0(a5)
   10de8:	0505                	addi	a0,a0,1
   10dea:	fbfd                	bnez	a5,10de0 <__init_libc+0x24>
   10dec:	050e                	slli	a0,a0,0x3
   10dee:	9526                	add	a0,a0,s1
   10df0:	18018793          	addi	a5,gp,384 # 1c980 <__libc>
   10df4:	e788                	sd	a0,8(a5)
   10df6:	18018493          	addi	s1,gp,384 # 1c980 <__libc>
   10dfa:	02500713          	li	a4,37
   10dfe:	611c                	ld	a5,0(a0)
   10e00:	e7d5                	bnez	a5,10eac <__init_libc+0xf0>
   10e02:	774a                	ld	a4,176(sp)
   10e04:	16e1bc23          	sd	a4,376(gp) # 1c978 <__hwcap>
   10e08:	77d2                	ld	a5,304(sp)
   10e0a:	c399                	beqz	a5,10e10 <__init_libc+0x54>
   10e0c:	16f1b823          	sd	a5,368(gp) # 1c970 <__sysinfo>
   10e10:	7786                	ld	a5,96(sp)
   10e12:	f89c                	sd	a5,48(s1)
   10e14:	e411                	bnez	s0,10e20 <__init_libc+0x64>
   10e16:	7432                	ld	s0,296(sp)
   10e18:	e401                	bnez	s0,10e20 <__init_libc+0x64>
   10e1a:	6465                	lui	s0,0x19
   10e1c:	86840413          	addi	s0,s0,-1944 # 18868 <__clzdi2+0x262>
   10e20:	9881b423          	sd	s0,-1656(gp) # 1c188 <__progname_full>
   10e24:	9881b023          	sd	s0,-1664(gp) # 1c180 <__progname>
   10e28:	02f00693          	li	a3,47
   10e2c:	0405                	addi	s0,s0,1
   10e2e:	fff44703          	lbu	a4,-1(s0)
   10e32:	e759                	bnez	a4,10ec0 <__init_libc+0x104>
   10e34:	854a                	mv	a0,s2
   10e36:	2b0030ef          	jal	ra,140e6 <__init_tls>
   10e3a:	756e                	ld	a0,248(sp)
   10e3c:	f7fff0ef          	jal	ra,10dba <__init_ssp>
   10e40:	672a                	ld	a4,136(sp)
   10e42:	67ca                	ld	a5,144(sp)
   10e44:	00f71863          	bne	a4,a5,10e54 <__init_libc+0x98>
   10e48:	676a                	ld	a4,152(sp)
   10e4a:	778a                	ld	a5,160(sp)
   10e4c:	00f71463          	bne	a4,a5,10e54 <__init_libc+0x98>
   10e50:	77ae                	ld	a5,232(sp)
   10e52:	c3d9                	beqz	a5,10ed8 <__init_libc+0x11c>
   10e54:	f002                	sd	zero,32(sp)
   10e56:	4785                	li	a5,1
   10e58:	f402                	sd	zero,40(sp)
   10e5a:	d03e                	sw	a5,32(sp)
   10e5c:	4789                	li	a5,2
   10e5e:	ec02                	sd	zero,24(sp)
   10e60:	d43e                	sw	a5,40(sp)
   10e62:	e402                	sd	zero,8(sp)
   10e64:	e802                	sd	zero,16(sp)
   10e66:	04900893          	li	a7,73
   10e6a:	0828                	addi	a0,sp,24
   10e6c:	458d                	li	a1,3
   10e6e:	0030                	addi	a2,sp,8
   10e70:	4681                	li	a3,0
   10e72:	4721                	li	a4,8
   10e74:	00000073          	ecall
   10e78:	02051713          	slli	a4,a0,0x20
   10e7c:	083c                	addi	a5,sp,24
   10e7e:	02074463          	bltz	a4,10ea6 <__init_libc+0xea>
   10e82:	6621                	lui	a2,0x8
   10e84:	66e5                	lui	a3,0x19
   10e86:	0609                	addi	a2,a2,2
   10e88:	0067d703          	lhu	a4,6(a5)
   10e8c:	02077713          	andi	a4,a4,32
   10e90:	cf15                	beqz	a4,10ecc <__init_libc+0x110>
   10e92:	03800893          	li	a7,56
   10e96:	f9c00513          	li	a0,-100
   10e9a:	87068593          	addi	a1,a3,-1936 # 18870 <__clzdi2+0x26a>
   10e9e:	00000073          	ecall
   10ea2:	02055563          	bgez	a0,10ecc <__init_libc+0x110>
   10ea6:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   10eaa:	9002                	ebreak
   10eac:	00f76863          	bltu	a4,a5,10ebc <__init_libc+0x100>
   10eb0:	1294                	addi	a3,sp,352
   10eb2:	078e                	slli	a5,a5,0x3
   10eb4:	97b6                	add	a5,a5,a3
   10eb6:	6514                	ld	a3,8(a0)
   10eb8:	ecd7b823          	sd	a3,-304(a5)
   10ebc:	0541                	addi	a0,a0,16
   10ebe:	b781                	j	10dfe <__init_libc+0x42>
   10ec0:	00d71463          	bne	a4,a3,10ec8 <__init_libc+0x10c>
   10ec4:	9881b023          	sd	s0,-1664(gp) # 1c180 <__progname>
   10ec8:	0405                	addi	s0,s0,1
   10eca:	b795                	j	10e2e <__init_libc+0x72>
   10ecc:	07a1                	addi	a5,a5,8
   10ece:	faf91de3          	bne	s2,a5,10e88 <__init_libc+0xcc>
   10ed2:	4785                	li	a5,1
   10ed4:	00f48123          	sb	a5,2(s1)
   10ed8:	70f6                	ld	ra,376(sp)
   10eda:	7456                	ld	s0,368(sp)
   10edc:	74b6                	ld	s1,360(sp)
   10ede:	7916                	ld	s2,352(sp)
   10ee0:	6119                	addi	sp,sp,384
   10ee2:	8082                	ret

0000000000010ee4 <__libc_start_init>:
   10ee4:	1101                	addi	sp,sp,-32
   10ee6:	e822                	sd	s0,16(sp)
   10ee8:	e426                	sd	s1,8(sp)
   10eea:	6471                	lui	s0,0x1c
   10eec:	64f1                	lui	s1,0x1c
   10eee:	ec06                	sd	ra,24(sp)
   10ef0:	ff040413          	addi	s0,s0,-16 # 1bff0 <__frame_dummy_init_array_entry>
   10ef4:	ec5ff0ef          	jal	ra,10db8 <_init>
   10ef8:	ff848493          	addi	s1,s1,-8 # 1bff8 <__init_array_end>
   10efc:	00946763          	bltu	s0,s1,10f0a <__libc_start_init+0x26>
   10f00:	60e2                	ld	ra,24(sp)
   10f02:	6442                	ld	s0,16(sp)
   10f04:	64a2                	ld	s1,8(sp)
   10f06:	6105                	addi	sp,sp,32
   10f08:	8082                	ret
   10f0a:	601c                	ld	a5,0(s0)
   10f0c:	0421                	addi	s0,s0,8
   10f0e:	9782                	jalr	a5
   10f10:	b7f5                	j	10efc <__libc_start_init+0x18>

0000000000010f12 <libc_start_main_stage2>:
   10f12:	7179                	addi	sp,sp,-48
   10f14:	f022                	sd	s0,32(sp)
   10f16:	00158413          	addi	s0,a1,1
   10f1a:	040e                	slli	s0,s0,0x3
   10f1c:	f406                	sd	ra,40(sp)
   10f1e:	ec26                	sd	s1,24(sp)
   10f20:	e84a                	sd	s2,16(sp)
   10f22:	84ae                	mv	s1,a1
   10f24:	892a                	mv	s2,a0
   10f26:	9432                	add	s0,s0,a2
   10f28:	e432                	sd	a2,8(sp)
   10f2a:	fbbff0ef          	jal	ra,10ee4 <__libc_start_init>
   10f2e:	65a2                	ld	a1,8(sp)
   10f30:	8622                	mv	a2,s0
   10f32:	8526                	mv	a0,s1
   10f34:	9902                	jalr	s2
   10f36:	9eaff0ef          	jal	ra,10120 <exit>

0000000000010f3a <__libc_start_main>:
   10f3a:	7179                	addi	sp,sp,-48
   10f3c:	f022                	sd	s0,32(sp)
   10f3e:	ec26                	sd	s1,24(sp)
   10f40:	842e                	mv	s0,a1
   10f42:	84aa                	mv	s1,a0
   10f44:	00158513          	addi	a0,a1,1
   10f48:	620c                	ld	a1,0(a2)
   10f4a:	050e                	slli	a0,a0,0x3
   10f4c:	9532                	add	a0,a0,a2
   10f4e:	f406                	sd	ra,40(sp)
   10f50:	e432                	sd	a2,8(sp)
   10f52:	e6bff0ef          	jal	ra,10dbc <__init_libc>
   10f56:	6345                	lui	t1,0x11
   10f58:	f1230313          	addi	t1,t1,-238 # 10f12 <libc_start_main_stage2>
   10f5c:	85a2                	mv	a1,s0
   10f5e:	7402                	ld	s0,32(sp)
   10f60:	6622                	ld	a2,8(sp)
   10f62:	70a2                	ld	ra,40(sp)
   10f64:	8526                	mv	a0,s1
   10f66:	64e2                	ld	s1,24(sp)
   10f68:	6145                	addi	sp,sp,48
   10f6a:	8302                	jr	t1

0000000000010f6c <__errno_location>:
   10f6c:	8512                	mv	a0,tp
   10f6e:	f5c50513          	addi	a0,a0,-164
   10f72:	8082                	ret

0000000000010f74 <__strerror_l>:
   10f74:	08300793          	li	a5,131
   10f78:	00a7f363          	bgeu	a5,a0,10f7e <__strerror_l+0xa>
   10f7c:	4501                	li	a0,0
   10f7e:	67e5                	lui	a5,0x19
   10f80:	ce878793          	addi	a5,a5,-792 # 18ce8 <errmsgidx>
   10f84:	0506                	slli	a0,a0,0x1
   10f86:	953e                	add	a0,a0,a5
   10f88:	00055503          	lhu	a0,0(a0)
   10f8c:	67e5                	lui	a5,0x19
   10f8e:	758c                	ld	a1,40(a1)
   10f90:	df078793          	addi	a5,a5,-528 # 18df0 <errmsgstr>
   10f94:	953e                	add	a0,a0,a5
   10f96:	a8c5                	j	11086 <__lctrans>

0000000000010f98 <strerror>:
   10f98:	8792                	mv	a5,tp
   10f9a:	fd07b583          	ld	a1,-48(a5)
   10f9e:	bfd9                	j	10f74 <__strerror_l>

0000000000010fa0 <__funcs_on_exit>:
   10fa0:	8082                	ret

0000000000010fa2 <__libc_exit_fini>:
   10fa2:	1101                	addi	sp,sp,-32
   10fa4:	e822                	sd	s0,16(sp)
   10fa6:	e426                	sd	s1,8(sp)
   10fa8:	6471                	lui	s0,0x1c
   10faa:	64f1                	lui	s1,0x1c
   10fac:	ec06                	sd	ra,24(sp)
   10fae:	00040413          	mv	s0,s0
   10fb2:	ff848493          	addi	s1,s1,-8 # 1bff8 <__init_array_end>
   10fb6:	0084e763          	bltu	s1,s0,10fc4 <__libc_exit_fini+0x22>
   10fba:	6442                	ld	s0,16(sp)
   10fbc:	60e2                	ld	ra,24(sp)
   10fbe:	64a2                	ld	s1,8(sp)
   10fc0:	6105                	addi	sp,sp,32
   10fc2:	bff9                	j	10fa0 <__funcs_on_exit>
   10fc4:	ff843783          	ld	a5,-8(s0) # 1bff8 <__init_array_end>
   10fc8:	1461                	addi	s0,s0,-8
   10fca:	9782                	jalr	a5
   10fcc:	b7ed                	j	10fb6 <__libc_exit_fini+0x14>

0000000000010fce <open>:
   10fce:	715d                	addi	sp,sp,-80
   10fd0:	e822                	sd	s0,16(sp)
   10fd2:	fc3e                	sd	a5,56(sp)
   10fd4:	ec06                	sd	ra,24(sp)
   10fd6:	f032                	sd	a2,32(sp)
   10fd8:	f436                	sd	a3,40(sp)
   10fda:	f83a                	sd	a4,48(sp)
   10fdc:	e0c2                	sd	a6,64(sp)
   10fde:	e4c6                	sd	a7,72(sp)
   10fe0:	0405f793          	andi	a5,a1,64
   10fe4:	842e                	mv	s0,a1
   10fe6:	eb89                	bnez	a5,10ff8 <open+0x2a>
   10fe8:	004107b7          	lui	a5,0x410
   10fec:	8fed                	and	a5,a5,a1
   10fee:	004106b7          	lui	a3,0x410
   10ff2:	4701                	li	a4,0
   10ff4:	00d79563          	bne	a5,a3,10ffe <open+0x30>
   10ff8:	5702                	lw	a4,32(sp)
   10ffa:	101c                	addi	a5,sp,32
   10ffc:	e43e                	sd	a5,8(sp)
   10ffe:	1702                	slli	a4,a4,0x20
   11000:	66a1                	lui	a3,0x8
   11002:	4781                	li	a5,0
   11004:	9301                	srli	a4,a4,0x20
   11006:	862a                	mv	a2,a0
   11008:	4801                	li	a6,0
   1100a:	8ec1                	or	a3,a3,s0
   1100c:	f9c00593          	li	a1,-100
   11010:	03800513          	li	a0,56
   11014:	3dd020ef          	jal	ra,13bf0 <__syscall_cp>
   11018:	02051713          	slli	a4,a0,0x20
   1101c:	0005079b          	sext.w	a5,a0
   11020:	00074c63          	bltz	a4,11038 <open+0x6a>
   11024:	02c41713          	slli	a4,s0,0x2c
   11028:	00075863          	bgez	a4,11038 <open+0x6a>
   1102c:	48e5                	li	a7,25
   1102e:	853e                	mv	a0,a5
   11030:	4589                	li	a1,2
   11032:	4605                	li	a2,1
   11034:	00000073          	ecall
   11038:	853e                	mv	a0,a5
   1103a:	00e000ef          	jal	ra,11048 <__syscall_ret>
   1103e:	60e2                	ld	ra,24(sp)
   11040:	6442                	ld	s0,16(sp)
   11042:	2501                	sext.w	a0,a0
   11044:	6161                	addi	sp,sp,80
   11046:	8082                	ret

0000000000011048 <__syscall_ret>:
   11048:	1141                	addi	sp,sp,-16
   1104a:	e022                	sd	s0,0(sp)
   1104c:	e406                	sd	ra,8(sp)
   1104e:	77fd                	lui	a5,0xfffff
   11050:	842a                	mv	s0,a0
   11052:	00a7e663          	bltu	a5,a0,1105e <__syscall_ret+0x16>
   11056:	60a2                	ld	ra,8(sp)
   11058:	6402                	ld	s0,0(sp)
   1105a:	0141                	addi	sp,sp,16
   1105c:	8082                	ret
   1105e:	f0fff0ef          	jal	ra,10f6c <__errno_location>
   11062:	4080043b          	negw	s0,s0
   11066:	c100                	sw	s0,0(a0)
   11068:	557d                	li	a0,-1
   1106a:	b7f5                	j	11056 <__syscall_ret+0xe>

000000000001106c <__lsysinfo>:
   1106c:	1141                	addi	sp,sp,-16
   1106e:	e406                	sd	ra,8(sp)
   11070:	0b300893          	li	a7,179
   11074:	00000073          	ecall
   11078:	fd1ff0ef          	jal	ra,11048 <__syscall_ret>
   1107c:	60a2                	ld	ra,8(sp)
   1107e:	2501                	sext.w	a0,a0
   11080:	0141                	addi	sp,sp,16
   11082:	8082                	ret

0000000000011084 <dummy>:
   11084:	8082                	ret

0000000000011086 <__lctrans>:
   11086:	1980306f          	j	1421e <__lctrans_impl>

000000000001108a <__lctrans_cur>:
   1108a:	8792                	mv	a5,tp
   1108c:	fd07b783          	ld	a5,-48(a5) # ffffffffffffefd0 <__BSS_END__+0xfffffffffffe25b0>
   11090:	778c                	ld	a1,40(a5)
   11092:	18c0306f          	j	1421e <__lctrans_impl>

0000000000011096 <__nl_langinfo_l>:
   11096:	6741                	lui	a4,0x10
   11098:	fff70613          	addi	a2,a4,-1 # ffff <exit-0x121>
   1109c:	4839                	li	a6,14
   1109e:	41055793          	srai	a5,a0,0x10
   110a2:	00c576b3          	and	a3,a0,a2
   110a6:	01051863          	bne	a0,a6,110b6 <__nl_langinfo_l+0x20>
   110aa:	619c                	ld	a5,0(a1)
   110ac:	ebc1                	bnez	a5,1113c <__nl_langinfo_l+0xa6>
   110ae:	6565                	lui	a0,0x19
   110b0:	57850513          	addi	a0,a0,1400 # 19578 <errmsgstr+0x788>
   110b4:	8082                	ret
   110b6:	00c69b63          	bne	a3,a2,110cc <__nl_langinfo_l+0x36>
   110ba:	4715                	li	a4,5
   110bc:	02f74063          	blt	a4,a5,110dc <__nl_langinfo_l+0x46>
   110c0:	078e                	slli	a5,a5,0x3
   110c2:	97ae                	add	a5,a5,a1
   110c4:	6388                	ld	a0,0(a5)
   110c6:	cd3d                	beqz	a0,11144 <__nl_langinfo_l+0xae>
   110c8:	0541                	addi	a0,a0,16
   110ca:	8082                	ret
   110cc:	4609                	li	a2,2
   110ce:	04c78963          	beq	a5,a2,11120 <__nl_langinfo_l+0x8a>
   110d2:	00f64963          	blt	a2,a5,110e4 <__nl_langinfo_l+0x4e>
   110d6:	4605                	li	a2,1
   110d8:	02c78363          	beq	a5,a2,110fe <__nl_langinfo_l+0x68>
   110dc:	6565                	lui	a0,0x19
   110de:	86850513          	addi	a0,a0,-1944 # 18868 <__clzdi2+0x262>
   110e2:	8082                	ret
   110e4:	4611                	li	a2,4
   110e6:	fec78be3          	beq	a5,a2,110dc <__nl_langinfo_l+0x46>
   110ea:	4615                	li	a2,5
   110ec:	fec798e3          	bne	a5,a2,110dc <__nl_langinfo_l+0x46>
   110f0:	1771                	addi	a4,a4,-4
   110f2:	8d79                	and	a0,a0,a4
   110f4:	f565                	bnez	a0,110dc <__nl_langinfo_l+0x46>
   110f6:	6565                	lui	a0,0x19
   110f8:	58050513          	addi	a0,a0,1408 # 19580 <c_messages>
   110fc:	a039                	j	1110a <__nl_langinfo_l+0x74>
   110fe:	1779                	addi	a4,a4,-2
   11100:	8d79                	and	a0,a0,a4
   11102:	fd69                	bnez	a0,110dc <__nl_langinfo_l+0x46>
   11104:	6569                	lui	a0,0x1a
   11106:	20050513          	addi	a0,a0,512 # 1a200 <c_numeric>
   1110a:	e29d                	bnez	a3,11130 <__nl_langinfo_l+0x9a>
   1110c:	4705                	li	a4,1
   1110e:	fce78ae3          	beq	a5,a4,110e2 <__nl_langinfo_l+0x4c>
   11112:	00054703          	lbu	a4,0(a0)
   11116:	d771                	beqz	a4,110e2 <__nl_langinfo_l+0x4c>
   11118:	078e                	slli	a5,a5,0x3
   1111a:	97ae                	add	a5,a5,a1
   1111c:	638c                	ld	a1,0(a5)
   1111e:	b7a5                	j	11086 <__lctrans>
   11120:	03100713          	li	a4,49
   11124:	fad74ce3          	blt	a4,a3,110dc <__nl_langinfo_l+0x46>
   11128:	6565                	lui	a0,0x19
   1112a:	59850513          	addi	a0,a0,1432 # 19598 <c_time>
   1112e:	bff1                	j	1110a <__nl_langinfo_l+0x74>
   11130:	00054703          	lbu	a4,0(a0)
   11134:	0505                	addi	a0,a0,1
   11136:	ff6d                	bnez	a4,11130 <__nl_langinfo_l+0x9a>
   11138:	36fd                	addiw	a3,a3,-1
   1113a:	bfc1                	j	1110a <__nl_langinfo_l+0x74>
   1113c:	6565                	lui	a0,0x19
   1113e:	a7050513          	addi	a0,a0,-1424 # 18a70 <CSWTCH.18+0xe0>
   11142:	8082                	ret
   11144:	6565                	lui	a0,0x19
   11146:	57050513          	addi	a0,a0,1392 # 19570 <errmsgstr+0x780>
   1114a:	8082                	ret

000000000001114c <__nl_langinfo>:
   1114c:	8792                	mv	a5,tp
   1114e:	fd07b583          	ld	a1,-48(a5)
   11152:	b791                	j	11096 <__nl_langinfo_l>

0000000000011154 <setlocale>:
   11154:	7171                	addi	sp,sp,-176
   11156:	e152                	sd	s4,128(sp)
   11158:	f506                	sd	ra,168(sp)
   1115a:	f122                	sd	s0,160(sp)
   1115c:	ed26                	sd	s1,152(sp)
   1115e:	e94a                	sd	s2,144(sp)
   11160:	e54e                	sd	s3,136(sp)
   11162:	fcd6                	sd	s5,120(sp)
   11164:	f8da                	sd	s6,112(sp)
   11166:	f4de                	sd	s7,104(sp)
   11168:	f0e2                	sd	s8,96(sp)
   1116a:	ece6                	sd	s9,88(sp)
   1116c:	e8ea                	sd	s10,80(sp)
   1116e:	4a19                	li	s4,6
   11170:	06aa6b63          	bltu	s4,a0,111e6 <setlocale+0x92>
   11174:	842a                	mv	s0,a0
   11176:	21018513          	addi	a0,gp,528 # 1ca10 <__locale_lock>
   1117a:	84ae                	mv	s1,a1
   1117c:	894e                	mv	s2,s3
   1117e:	129020ef          	jal	ra,13aa6 <__lock>
   11182:	11441763          	bne	s0,s4,11290 <setlocale+0x13c>
   11186:	ccbd                	beqz	s1,11204 <setlocale+0xb0>
   11188:	67e5                	lui	a5,0x19
   1118a:	6d87b783          	ld	a5,1752(a5) # 196d8 <c_time+0x140>
   1118e:	02010a93          	addi	s5,sp,32
   11192:	e802                	sd	zero,16(sp)
   11194:	e43e                	sd	a5,8(sp)
   11196:	ec02                	sd	zero,24(sp)
   11198:	4a01                	li	s4,0
   1119a:	8b56                	mv	s6,s5
   1119c:	4c5d                	li	s8,23
   1119e:	5cfd                	li	s9,-1
   111a0:	4b99                	li	s7,6
   111a2:	03b00593          	li	a1,59
   111a6:	8526                	mv	a0,s1
   111a8:	784020ef          	jal	ra,1392c <__strchrnul>
   111ac:	409509b3          	sub	s3,a0,s1
   111b0:	842a                	mv	s0,a0
   111b2:	033c4063          	blt	s8,s3,111d2 <setlocale+0x7e>
   111b6:	864e                	mv	a2,s3
   111b8:	85a6                	mv	a1,s1
   111ba:	0028                	addi	a0,sp,8
   111bc:	258020ef          	jal	ra,13414 <memcpy>
   111c0:	089c                	addi	a5,sp,80
   111c2:	99be                	add	s3,s3,a5
   111c4:	fa098c23          	sb	zero,-72(s3)
   111c8:	00044783          	lbu	a5,0(s0)
   111cc:	c399                	beqz	a5,111d2 <setlocale+0x7e>
   111ce:	00140493          	addi	s1,s0,1
   111d2:	002c                	addi	a1,sp,8
   111d4:	8552                	mv	a0,s4
   111d6:	06c030ef          	jal	ra,14242 <__get_locale>
   111da:	01951863          	bne	a0,s9,111ea <setlocale+0x96>
   111de:	21018513          	addi	a0,gp,528 # 1ca10 <__locale_lock>
   111e2:	19d020ef          	jal	ra,13b7e <__unlock>
   111e6:	4401                	li	s0,0
   111e8:	a069                	j	11272 <setlocale+0x11e>
   111ea:	00aab023          	sd	a0,0(s5)
   111ee:	2a05                	addiw	s4,s4,1
   111f0:	0aa1                	addi	s5,s5,8
   111f2:	fb7a18e3          	bne	s4,s7,111a2 <setlocale+0x4e>
   111f6:	03000613          	li	a2,48
   111fa:	85da                	mv	a1,s6
   111fc:	1b818513          	addi	a0,gp,440 # 1c9b8 <__libc+0x38>
   11200:	214020ef          	jal	ra,13414 <memcpy>
   11204:	18018a13          	addi	s4,gp,384 # 1c980 <__libc>
   11208:	6c65                	lui	s8,0x19
   1120a:	b4818993          	addi	s3,gp,-1208 # 1c348 <buf>
   1120e:	030a0c93          	addi	s9,s4,48
   11212:	4b81                	li	s7,0
   11214:	18018b13          	addi	s6,gp,384 # 1c980 <__libc>
   11218:	b4818a93          	addi	s5,gp,-1208 # 1c348 <buf>
   1121c:	570c0c13          	addi	s8,s8,1392 # 19570 <errmsgstr+0x780>
   11220:	03b00d13          	li	s10,59
   11224:	038a3783          	ld	a5,56(s4)
   11228:	038b3703          	ld	a4,56(s6)
   1122c:	00f71363          	bne	a4,a5,11232 <setlocale+0xde>
   11230:	2b85                	addiw	s7,s7,1
   11232:	8462                	mv	s0,s8
   11234:	c399                	beqz	a5,1123a <setlocale+0xe6>
   11236:	01078413          	addi	s0,a5,16
   1123a:	8522                	mv	a0,s0
   1123c:	7b2020ef          	jal	ra,139ee <strlen>
   11240:	84aa                	mv	s1,a0
   11242:	862a                	mv	a2,a0
   11244:	85a2                	mv	a1,s0
   11246:	854e                	mv	a0,s3
   11248:	1cc020ef          	jal	ra,13414 <memcpy>
   1124c:	009987b3          	add	a5,s3,s1
   11250:	01a78023          	sb	s10,0(a5)
   11254:	0485                	addi	s1,s1,1
   11256:	0a21                	addi	s4,s4,8
   11258:	99a6                	add	s3,s3,s1
   1125a:	fd4c95e3          	bne	s9,s4,11224 <setlocale+0xd0>
   1125e:	fe098fa3          	sb	zero,-1(s3)
   11262:	21018513          	addi	a0,gp,528 # 1ca10 <__locale_lock>
   11266:	119020ef          	jal	ra,13b7e <__unlock>
   1126a:	4799                	li	a5,6
   1126c:	00fb8363          	beq	s7,a5,11272 <setlocale+0x11e>
   11270:	8456                	mv	s0,s5
   11272:	70aa                	ld	ra,168(sp)
   11274:	8522                	mv	a0,s0
   11276:	740a                	ld	s0,160(sp)
   11278:	64ea                	ld	s1,152(sp)
   1127a:	694a                	ld	s2,144(sp)
   1127c:	69aa                	ld	s3,136(sp)
   1127e:	6a0a                	ld	s4,128(sp)
   11280:	7ae6                	ld	s5,120(sp)
   11282:	7b46                	ld	s6,112(sp)
   11284:	7ba6                	ld	s7,104(sp)
   11286:	7c06                	ld	s8,96(sp)
   11288:	6ce6                	ld	s9,88(sp)
   1128a:	6d46                	ld	s10,80(sp)
   1128c:	614d                	addi	sp,sp,176
   1128e:	8082                	ret
   11290:	cc85                	beqz	s1,112c8 <setlocale+0x174>
   11292:	85a6                	mv	a1,s1
   11294:	8522                	mv	a0,s0
   11296:	7ad020ef          	jal	ra,14242 <__get_locale>
   1129a:	57fd                	li	a5,-1
   1129c:	00f51563          	bne	a0,a5,112a6 <setlocale+0x152>
   112a0:	21018513          	addi	a0,gp,528 # 1ca10 <__locale_lock>
   112a4:	bf3d                	j	111e2 <setlocale+0x8e>
   112a6:	0419                	addi	s0,s0,6
   112a8:	040e                	slli	s0,s0,0x3
   112aa:	18018793          	addi	a5,gp,384 # 1c980 <__libc>
   112ae:	943e                	add	s0,s0,a5
   112b0:	e408                	sd	a0,8(s0)
   112b2:	01050413          	addi	s0,a0,16
   112b6:	e501                	bnez	a0,112be <setlocale+0x16a>
   112b8:	6465                	lui	s0,0x19
   112ba:	57040413          	addi	s0,s0,1392 # 19570 <errmsgstr+0x780>
   112be:	21018513          	addi	a0,gp,528 # 1ca10 <__locale_lock>
   112c2:	0bd020ef          	jal	ra,13b7e <__unlock>
   112c6:	b775                	j	11272 <setlocale+0x11e>
   112c8:	0419                	addi	s0,s0,6
   112ca:	040e                	slli	s0,s0,0x3
   112cc:	18018793          	addi	a5,gp,384 # 1c980 <__libc>
   112d0:	943e                	add	s0,s0,a5
   112d2:	6408                	ld	a0,8(s0)
   112d4:	bff9                	j	112b2 <setlocale+0x15e>

00000000000112d6 <allzerop>:
   112d6:	4501                	li	a0,0
   112d8:	8082                	ret

00000000000112da <calloc>:
   112da:	1101                	addi	sp,sp,-32
   112dc:	ec06                	sd	ra,24(sp)
   112de:	e822                	sd	s0,16(sp)
   112e0:	e426                	sd	s1,8(sp)
   112e2:	e04a                	sd	s2,0(sp)
   112e4:	c581                	beqz	a1,112ec <calloc+0x12>
   112e6:	02b537b3          	mulhu	a5,a0,a1
   112ea:	e3d1                	bnez	a5,1136e <calloc+0x94>
   112ec:	02a58433          	mul	s0,a1,a0
   112f0:	8522                	mv	a0,s0
   112f2:	2c8000ef          	jal	ra,115ba <malloc>
   112f6:	84aa                	mv	s1,a0
   112f8:	c141                	beqz	a0,11378 <calloc+0x9e>
   112fa:	1ec1a783          	lw	a5,492(gp) # 1c9ec <__malloc_replaced>
   112fe:	e781                	bnez	a5,11306 <calloc+0x2c>
   11300:	2f2010ef          	jal	ra,125f2 <__malloc_allzerop>
   11304:	e935                	bnez	a0,11378 <calloc+0x9e>
   11306:	6605                	lui	a2,0x1
   11308:	04c46963          	bltu	s0,a2,1135a <calloc+0x80>
   1130c:	9426                	add	s0,s0,s1
   1130e:	167d                	addi	a2,a2,-1
   11310:	8e61                	and	a2,a2,s0
   11312:	6905                	lui	s2,0x1
   11314:	40c40533          	sub	a0,s0,a2
   11318:	4581                	li	a1,0
   1131a:	542020ef          	jal	ra,1385c <memset>
   1131e:	40950433          	sub	s0,a0,s1
   11322:	03246c63          	bltu	s0,s2,1135a <calloc+0x80>
   11326:	842a                	mv	s0,a0
   11328:	408906b3          	sub	a3,s2,s0
   1132c:	41250533          	sub	a0,a0,s2
   11330:	ff843703          	ld	a4,-8(s0)
   11334:	ff043583          	ld	a1,-16(s0)
   11338:	00d40633          	add	a2,s0,a3
   1133c:	8f4d                	or	a4,a4,a1
   1133e:	fb79                	bnez	a4,11314 <calloc+0x3a>
   11340:	1441                	addi	s0,s0,-16
   11342:	fe8517e3          	bne	a0,s0,11330 <calloc+0x56>
   11346:	4601                	li	a2,0
   11348:	40c40533          	sub	a0,s0,a2
   1134c:	4581                	li	a1,0
   1134e:	50e020ef          	jal	ra,1385c <memset>
   11352:	40950433          	sub	s0,a0,s1
   11356:	fd2478e3          	bgeu	s0,s2,11326 <calloc+0x4c>
   1135a:	8622                	mv	a2,s0
   1135c:	6442                	ld	s0,16(sp)
   1135e:	60e2                	ld	ra,24(sp)
   11360:	6902                	ld	s2,0(sp)
   11362:	8526                	mv	a0,s1
   11364:	64a2                	ld	s1,8(sp)
   11366:	4581                	li	a1,0
   11368:	6105                	addi	sp,sp,32
   1136a:	4f20206f          	j	1385c <memset>
   1136e:	bffff0ef          	jal	ra,10f6c <__errno_location>
   11372:	47b1                	li	a5,12
   11374:	c11c                	sw	a5,0(a0)
   11376:	4481                	li	s1,0
   11378:	60e2                	ld	ra,24(sp)
   1137a:	6442                	ld	s0,16(sp)
   1137c:	6902                	ld	s2,0(sp)
   1137e:	8526                	mv	a0,s1
   11380:	64a2                	ld	s1,8(sp)
   11382:	6105                	addi	sp,sp,32
   11384:	8082                	ret

0000000000011386 <free>:
   11386:	abe1                	j	1195e <__libc_free>

0000000000011388 <__simple_malloc>:
   11388:	715d                	addi	sp,sp,-80
   1138a:	e486                	sd	ra,72(sp)
   1138c:	e0a2                	sd	s0,64(sp)
   1138e:	fc26                	sd	s1,56(sp)
   11390:	f84a                	sd	s2,48(sp)
   11392:	f44e                	sd	s3,40(sp)
   11394:	f052                	sd	s4,32(sp)
   11396:	1c054f63          	bltz	a0,11574 <__simple_malloc+0x1ec>
   1139a:	8a2a                	mv	s4,a0
   1139c:	c971                	beqz	a0,11470 <__simple_malloc+0xe8>
   1139e:	4785                	li	a5,1
   113a0:	1ea7ff63          	bgeu	a5,a0,1159e <__simple_malloc+0x216>
   113a4:	4789                	li	a5,2
   113a6:	1ca7f363          	bgeu	a5,a0,1156c <__simple_malloc+0x1e4>
   113aa:	4791                	li	a5,4
   113ac:	1ea7fb63          	bgeu	a5,a0,115a2 <__simple_malloc+0x21a>
   113b0:	47a1                	li	a5,8
   113b2:	1ea7fc63          	bgeu	a5,a0,115aa <__simple_malloc+0x222>
   113b6:	47c1                	li	a5,16
   113b8:	443d                	li	s0,15
   113ba:	1aa7fa63          	bgeu	a5,a0,1156e <__simple_malloc+0x1e6>
   113be:	9a818513          	addi	a0,gp,-1624 # 1c1a8 <lock>
   113c2:	6e4020ef          	jal	ra,13aa6 <__lock>
   113c6:	99818913          	addi	s2,gp,-1640 # 1c198 <cur.1724>
   113ca:	00093683          	ld	a3,0(s2) # 1000 <exit-0xf120>
   113ce:	9a018993          	addi	s3,gp,-1632 # 1c1a0 <end.1725>
   113d2:	0009b803          	ld	a6,0(s3)
   113d6:	40d007b3          	neg	a5,a3
   113da:	8fe1                	and	a5,a5,s0
   113dc:	97b6                	add	a5,a5,a3
   113de:	00f93023          	sd	a5,0(s2)
   113e2:	40f80733          	sub	a4,a6,a5
   113e6:	13477d63          	bgeu	a4,s4,11520 <__simple_malloc+0x198>
   113ea:	18018e93          	addi	t4,gp,384 # 1c980 <__libc>
   113ee:	030eb683          	ld	a3,48(t4)
   113f2:	fff68713          	addi	a4,a3,-1 # 7fff <exit-0x8121>
   113f6:	40d00333          	neg	t1,a3
   113fa:	cfad                	beqz	a5,11474 <__simple_malloc+0xec>
   113fc:	99018e13          	addi	t3,gp,-1648 # 1c190 <brk.1723>
   11400:	000e3603          	ld	a2,0(t3)
   11404:	1ac80763          	beq	a6,a2,115b2 <__simple_malloc+0x22a>
   11408:	00ea05b3          	add	a1,s4,a4
   1140c:	0065f5b3          	and	a1,a1,t1
   11410:	41458733          	sub	a4,a1,s4
   11414:	0035d793          	srli	a5,a1,0x3
   11418:	02e7f363          	bgeu	a5,a4,1143e <__simple_malloc+0xb6>
   1141c:	9ac18713          	addi	a4,gp,-1620 # 1c1ac <mmap_step.1726>
   11420:	4308                	lw	a0,0(a4)
   11422:	0009b603          	ld	a2,0(s3)
   11426:	00093883          	ld	a7,0(s2)
   1142a:	0015579b          	srliw	a5,a0,0x1
   1142e:	00f697b3          	sll	a5,a3,a5
   11432:	41478833          	sub	a6,a5,s4
   11436:	411606b3          	sub	a3,a2,a7
   1143a:	0f06ec63          	bltu	a3,a6,11532 <__simple_malloc+0x1aa>
   1143e:	4781                	li	a5,0
   11440:	577d                	li	a4,-1
   11442:	02200693          	li	a3,34
   11446:	460d                	li	a2,3
   11448:	4501                	li	a0,0
   1144a:	6ac010ef          	jal	ra,12af6 <__mmap>
   1144e:	57fd                	li	a5,-1
   11450:	842a                	mv	s0,a0
   11452:	12f50763          	beq	a0,a5,11580 <__simple_malloc+0x1f8>
   11456:	9a818513          	addi	a0,gp,-1624 # 1c1a8 <lock>
   1145a:	724020ef          	jal	ra,13b7e <__unlock>
   1145e:	60a6                	ld	ra,72(sp)
   11460:	8522                	mv	a0,s0
   11462:	6406                	ld	s0,64(sp)
   11464:	74e2                	ld	s1,56(sp)
   11466:	7942                	ld	s2,48(sp)
   11468:	79a2                	ld	s3,40(sp)
   1146a:	7a02                	ld	s4,32(sp)
   1146c:	6161                	addi	sp,sp,80
   1146e:	8082                	ret
   11470:	4a05                	li	s4,1
   11472:	b735                	j	1139e <__simple_malloc+0x16>
   11474:	0d600893          	li	a7,214
   11478:	4501                	li	a0,0
   1147a:	00000073          	ecall
   1147e:	030eb683          	ld	a3,48(t4)
   11482:	40a00633          	neg	a2,a0
   11486:	fff68893          	addi	a7,a3,-1
   1148a:	01167633          	and	a2,a2,a7
   1148e:	962a                	add	a2,a2,a0
   11490:	99018e13          	addi	t3,gp,-1648 # 1c190 <brk.1723>
   11494:	00ce3023          	sd	a2,0(t3)
   11498:	00c9b023          	sd	a2,0(s3)
   1149c:	00c93023          	sd	a2,0(s2)
   114a0:	41070733          	sub	a4,a4,a6
   114a4:	9752                	add	a4,a4,s4
   114a6:	97ba                	add	a5,a5,a4
   114a8:	0067f5b3          	and	a1,a5,t1
   114ac:	fff64793          	not	a5,a2
   114b0:	0af5fa63          	bgeu	a1,a5,11564 <__simple_malloc+0x1dc>
   114b4:	008eb703          	ld	a4,8(t4)
   114b8:	00800837          	lui	a6,0x800
   114bc:	00c58533          	add	a0,a1,a2
   114c0:	87ba                	mv	a5,a4
   114c2:	0d076563          	bltu	a4,a6,1158c <__simple_malloc+0x204>
   114c6:	ff800837          	lui	a6,0xff800
   114ca:	97c2                	add	a5,a5,a6
   114cc:	00a7f463          	bgeu	a5,a0,114d4 <__simple_malloc+0x14c>
   114d0:	08e66a63          	bltu	a2,a4,11564 <__simple_malloc+0x1dc>
   114d4:	0838                	addi	a4,sp,24
   114d6:	00800837          	lui	a6,0x800
   114da:	87ba                	mv	a5,a4
   114dc:	0b076b63          	bltu	a4,a6,11592 <__simple_malloc+0x20a>
   114e0:	ff800837          	lui	a6,0xff800
   114e4:	97c2                	add	a5,a5,a6
   114e6:	00a7f463          	bgeu	a5,a0,114ee <__simple_malloc+0x166>
   114ea:	06e66d63          	bltu	a2,a4,11564 <__simple_malloc+0x1dc>
   114ee:	0d600893          	li	a7,214
   114f2:	00000073          	ecall
   114f6:	000e3783          	ld	a5,0(t3)
   114fa:	97ae                	add	a5,a5,a1
   114fc:	00f50963          	beq	a0,a5,1150e <__simple_malloc+0x186>
   11500:	030eb683          	ld	a3,48(t4)
   11504:	40d00333          	neg	t1,a3
   11508:	fff68713          	addi	a4,a3,-1
   1150c:	bdf5                	j	11408 <__simple_malloc+0x80>
   1150e:	0009b703          	ld	a4,0(s3)
   11512:	00093783          	ld	a5,0(s2)
   11516:	95ba                	add	a1,a1,a4
   11518:	00b9b023          	sd	a1,0(s3)
   1151c:	00be3023          	sd	a1,0(t3)
   11520:	843e                	mv	s0,a5
   11522:	97d2                	add	a5,a5,s4
   11524:	9a818513          	addi	a0,gp,-1624 # 1c1a8 <lock>
   11528:	00f93023          	sd	a5,0(s2)
   1152c:	652020ef          	jal	ra,13b7e <__unlock>
   11530:	b73d                	j	1145e <__simple_malloc+0xd6>
   11532:	00f5f663          	bgeu	a1,a5,1153e <__simple_malloc+0x1b6>
   11536:	46ad                	li	a3,11
   11538:	85be                	mv	a1,a5
   1153a:	04a6ff63          	bgeu	a3,a0,11598 <__simple_malloc+0x210>
   1153e:	4781                	li	a5,0
   11540:	577d                	li	a4,-1
   11542:	02200693          	li	a3,34
   11546:	460d                	li	a2,3
   11548:	4501                	li	a0,0
   1154a:	e42e                	sd	a1,8(sp)
   1154c:	5aa010ef          	jal	ra,12af6 <__mmap>
   11550:	57fd                	li	a5,-1
   11552:	65a2                	ld	a1,8(sp)
   11554:	842a                	mv	s0,a0
   11556:	02f50563          	beq	a0,a5,11580 <__simple_malloc+0x1f8>
   1155a:	95a2                	add	a1,a1,s0
   1155c:	87a2                	mv	a5,s0
   1155e:	00b9b023          	sd	a1,0(s3)
   11562:	b7c1                	j	11522 <__simple_malloc+0x19a>
   11564:	40d00333          	neg	t1,a3
   11568:	8746                	mv	a4,a7
   1156a:	bd79                	j	11408 <__simple_malloc+0x80>
   1156c:	4789                	li	a5,2
   1156e:	fff78413          	addi	s0,a5,-1
   11572:	b5b1                	j	113be <__simple_malloc+0x36>
   11574:	9f9ff0ef          	jal	ra,10f6c <__errno_location>
   11578:	47b1                	li	a5,12
   1157a:	c11c                	sw	a5,0(a0)
   1157c:	4401                	li	s0,0
   1157e:	b5c5                	j	1145e <__simple_malloc+0xd6>
   11580:	9a818513          	addi	a0,gp,-1624 # 1c1a8 <lock>
   11584:	5fa020ef          	jal	ra,13b7e <__unlock>
   11588:	4401                	li	s0,0
   1158a:	bdd1                	j	1145e <__simple_malloc+0xd6>
   1158c:	008007b7          	lui	a5,0x800
   11590:	bf1d                	j	114c6 <__simple_malloc+0x13e>
   11592:	008007b7          	lui	a5,0x800
   11596:	b7a9                	j	114e0 <__simple_malloc+0x158>
   11598:	2505                	addiw	a0,a0,1
   1159a:	c308                	sw	a0,0(a4)
   1159c:	b74d                	j	1153e <__simple_malloc+0x1b6>
   1159e:	4401                	li	s0,0
   115a0:	bd39                	j	113be <__simple_malloc+0x36>
   115a2:	4791                	li	a5,4
   115a4:	fff78413          	addi	s0,a5,-1 # 7fffff <__BSS_END__+0x7e35df>
   115a8:	bd19                	j	113be <__simple_malloc+0x36>
   115aa:	47a1                	li	a5,8
   115ac:	fff78413          	addi	s0,a5,-1
   115b0:	b539                	j	113be <__simple_malloc+0x36>
   115b2:	88ba                	mv	a7,a4
   115b4:	b5f5                	j	114a0 <__simple_malloc+0x118>

00000000000115b6 <__libc_malloc>:
   115b6:	6910006f          	j	12446 <__libc_malloc_impl>

00000000000115ba <malloc>:
   115ba:	68d0006f          	j	12446 <__libc_malloc_impl>

00000000000115be <queue>:
   115be:	659c                	ld	a5,8(a1)
   115c0:	c781                	beqz	a5,115c8 <queue+0xa>
   115c2:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   115c6:	9002                	ebreak
   115c8:	619c                	ld	a5,0(a1)
   115ca:	ffe5                	bnez	a5,115c2 <queue+0x4>
   115cc:	611c                	ld	a5,0(a0)
   115ce:	cb81                	beqz	a5,115de <queue+0x20>
   115d0:	e59c                	sd	a5,8(a1)
   115d2:	639c                	ld	a5,0(a5)
   115d4:	e19c                	sd	a5,0(a1)
   115d6:	e78c                	sd	a1,8(a5)
   115d8:	659c                	ld	a5,8(a1)
   115da:	e38c                	sd	a1,0(a5)
   115dc:	8082                	ret
   115de:	e58c                	sd	a1,8(a1)
   115e0:	e18c                	sd	a1,0(a1)
   115e2:	e10c                	sd	a1,0(a0)
   115e4:	8082                	ret

00000000000115e6 <get_meta>:
   115e6:	00f57793          	andi	a5,a0,15
   115ea:	c781                	beqz	a5,115f2 <get_meta+0xc>
   115ec:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   115f0:	9002                	ebreak
   115f2:	ffd54683          	lbu	a3,-3(a0)
   115f6:	ffc54783          	lbu	a5,-4(a0)
   115fa:	ffe55703          	lhu	a4,-2(a0)
   115fe:	01f6f593          	andi	a1,a3,31
   11602:	8afd                	andi	a3,a3,31
   11604:	c799                	beqz	a5,11612 <get_meta+0x2c>
   11606:	f37d                	bnez	a4,115ec <get_meta+0x6>
   11608:	ff852703          	lw	a4,-8(a0)
   1160c:	67c1                	lui	a5,0x10
   1160e:	fcf74fe3          	blt	a4,a5,115ec <get_meta+0x6>
   11612:	0047179b          	slliw	a5,a4,0x4
   11616:	8d1d                	sub	a0,a0,a5
   11618:	ff050793          	addi	a5,a0,-16
   1161c:	6388                	ld	a0,0(a5)
   1161e:	6910                	ld	a2,16(a0)
   11620:	fcf616e3          	bne	a2,a5,115ec <get_meta+0x6>
   11624:	711c                	ld	a5,32(a0)
   11626:	01f7f613          	andi	a2,a5,31
   1162a:	fcd641e3          	blt	a2,a3,115ec <get_meta+0x6>
   1162e:	4d10                	lw	a2,24(a0)
   11630:	00d6563b          	srlw	a2,a2,a3
   11634:	8a05                	andi	a2,a2,1
   11636:	fa5d                	bnez	a2,115ec <get_meta+0x6>
   11638:	4d50                	lw	a2,28(a0)
   1163a:	00d656bb          	srlw	a3,a2,a3
   1163e:	8a85                	andi	a3,a3,1
   11640:	f6d5                	bnez	a3,115ec <get_meta+0x6>
   11642:	76fd                	lui	a3,0xfffff
   11644:	8ee9                	and	a3,a3,a0
   11646:	6290                	ld	a2,0(a3)
   11648:	bd81b683          	ld	a3,-1064(gp) # 1c3d8 <__malloc_context>
   1164c:	fad610e3          	bne	a2,a3,115ec <get_meta+0x6>
   11650:	0067d693          	srli	a3,a5,0x6
   11654:	03f6f693          	andi	a3,a3,63
   11658:	02f00613          	li	a2,47
   1165c:	02d66963          	bltu	a2,a3,1168e <get_meta+0xa8>
   11660:	6665                	lui	a2,0x19
   11662:	0686                	slli	a3,a3,0x1
   11664:	6f060613          	addi	a2,a2,1776 # 196f0 <__malloc_size_classes>
   11668:	96b2                	add	a3,a3,a2
   1166a:	0006d683          	lhu	a3,0(a3) # fffffffffffff000 <__BSS_END__+0xfffffffffffe25e0>
   1166e:	02b685bb          	mulw	a1,a3,a1
   11672:	f6b74de3          	blt	a4,a1,115ec <get_meta+0x6>
   11676:	9ead                	addw	a3,a3,a1
   11678:	f6d75ae3          	bge	a4,a3,115ec <get_meta+0x6>
   1167c:	76fd                	lui	a3,0xfffff
   1167e:	8efd                	and	a3,a3,a5
   11680:	c691                	beqz	a3,1168c <get_meta+0xa6>
   11682:	83b1                	srli	a5,a5,0xc
   11684:	07a2                	slli	a5,a5,0x8
   11686:	17fd                	addi	a5,a5,-1
   11688:	f6e7e2e3          	bltu	a5,a4,115ec <get_meta+0x6>
   1168c:	8082                	ret
   1168e:	6685                	lui	a3,0x1
   11690:	fc068693          	addi	a3,a3,-64 # fc0 <exit-0xf160>
   11694:	00d7f633          	and	a2,a5,a3
   11698:	f4d61ae3          	bne	a2,a3,115ec <get_meta+0x6>
   1169c:	b7c5                	j	1167c <get_meta+0x96>

000000000001169e <get_stride>:
   1169e:	7108                	ld	a0,32(a0)
   116a0:	01f57793          	andi	a5,a0,31
   116a4:	eb81                	bnez	a5,116b4 <get_stride+0x16>
   116a6:	77fd                	lui	a5,0xfffff
   116a8:	8fe9                	and	a5,a5,a0
   116aa:	c789                	beqz	a5,116b4 <get_stride+0x16>
   116ac:	8131                	srli	a0,a0,0xc
   116ae:	0532                	slli	a0,a0,0xc
   116b0:	1541                	addi	a0,a0,-16
   116b2:	8082                	ret
   116b4:	67e5                	lui	a5,0x19
   116b6:	8115                	srli	a0,a0,0x5
   116b8:	6f078793          	addi	a5,a5,1776 # 196f0 <__malloc_size_classes>
   116bc:	07e57513          	andi	a0,a0,126
   116c0:	953e                	add	a0,a0,a5
   116c2:	00055503          	lhu	a0,0(a0)
   116c6:	0045151b          	slliw	a0,a0,0x4
   116ca:	8082                	ret

00000000000116cc <nontrivial_free>:
   116cc:	7159                	addi	sp,sp,-112
   116ce:	4d58                	lw	a4,28(a0)
   116d0:	4d1c                	lw	a5,24(a0)
   116d2:	e4ce                	sd	s3,72(sp)
   116d4:	02053983          	ld	s3,32(a0)
   116d8:	8fd9                	or	a5,a5,a4
   116da:	e0d2                	sd	s4,64(sp)
   116dc:	4709                	li	a4,2
   116de:	4a05                	li	s4,1
   116e0:	eca6                	sd	s1,88(sp)
   116e2:	00ba1a3b          	sllw	s4,s4,a1
   116e6:	0069d493          	srli	s1,s3,0x6
   116ea:	0137173b          	sllw	a4,a4,s3
   116ee:	f0a2                	sd	s0,96(sp)
   116f0:	fc56                	sd	s5,56(sp)
   116f2:	f45e                	sd	s7,40(sp)
   116f4:	03f4fa93          	andi	s5,s1,63
   116f8:	00078b9b          	sext.w	s7,a5
   116fc:	f486                	sd	ra,104(sp)
   116fe:	e8ca                	sd	s2,80(sp)
   11700:	f85a                	sd	s6,48(sp)
   11702:	00fa07bb          	addw	a5,s4,a5
   11706:	377d                	addiw	a4,a4,-1
   11708:	842a                	mv	s0,a0
   1170a:	84d6                	mv	s1,s5
   1170c:	1ee79863          	bne	a5,a4,118fc <nontrivial_free+0x230>
   11710:	0209f793          	andi	a5,s3,32
   11714:	1e078463          	beqz	a5,118fc <nontrivial_free+0x230>
   11718:	02f00793          	li	a5,47
   1171c:	00853903          	ld	s2,8(a0)
   11720:	2357c963          	blt	a5,s5,11952 <nontrivial_free+0x286>
   11724:	f7bff0ef          	jal	ra,1169e <get_stride>
   11728:	6765                	lui	a4,0x19
   1172a:	001a9793          	slli	a5,s5,0x1
   1172e:	6f070713          	addi	a4,a4,1776 # 196f0 <__malloc_size_classes>
   11732:	97ba                	add	a5,a5,a4
   11734:	0007d783          	lhu	a5,0(a5)
   11738:	0047979b          	slliw	a5,a5,0x4
   1173c:	20f56e63          	bltu	a0,a5,11958 <nontrivial_free+0x28c>
   11740:	77fd                	lui	a5,0xfffff
   11742:	01f9fb13          	andi	s6,s3,31
   11746:	00f9f9b3          	and	s3,s3,a5
   1174a:	20098763          	beqz	s3,11958 <nontrivial_free+0x28c>
   1174e:	21241563          	bne	s0,s2,11958 <nontrivial_free+0x28c>
   11752:	ff9a879b          	addiw	a5,s5,-7
   11756:	477d                	li	a4,31
   11758:	02f76c63          	bltu	a4,a5,11790 <nontrivial_free+0xc4>
   1175c:	bd818793          	addi	a5,gp,-1064 # 1c3d8 <__malloc_context>
   11760:	ff9a871b          	addiw	a4,s5,-7
   11764:	973e                	add	a4,a4,a5
   11766:	37074683          	lbu	a3,880(a4)
   1176a:	06300713          	li	a4,99
   1176e:	02d77163          	bgeu	a4,a3,11790 <nontrivial_free+0xc4>
   11772:	03aa8693          	addi	a3,s5,58
   11776:	068e                	slli	a3,a3,0x3
   11778:	97b6                	add	a5,a5,a3
   1177a:	001b071b          	addiw	a4,s6,1
   1177e:	46a5                	li	a3,9
   11780:	02d706b3          	mul	a3,a4,a3
   11784:	639c                	ld	a5,0(a5)
   11786:	1cd7e363          	bltu	a5,a3,1194c <nontrivial_free+0x280>
   1178a:	47cd                	li	a5,19
   1178c:	1ce7c063          	blt	a5,a4,1194c <nontrivial_free+0x280>
   11790:	04a9                	addi	s1,s1,10
   11792:	bd818713          	addi	a4,gp,-1064 # 1c3d8 <__malloc_context>
   11796:	00349693          	slli	a3,s1,0x3
   1179a:	9736                	add	a4,a4,a3
   1179c:	6314                	ld	a3,0(a4)
   1179e:	bd818793          	addi	a5,gp,-1064 # 1c3d8 <__malloc_context>
   117a2:	03240b63          	beq	s0,s2,117d8 <nontrivial_free+0x10c>
   117a6:	6010                	ld	a2,0(s0)
   117a8:	01263423          	sd	s2,8(a2)
   117ac:	00c93023          	sd	a2,0(s2)
   117b0:	00869463          	bne	a3,s0,117b8 <nontrivial_free+0xec>
   117b4:	01273023          	sd	s2,0(a4)
   117b8:	00043423          	sd	zero,8(s0)
   117bc:	00043023          	sd	zero,0(s0)
   117c0:	06869063          	bne	a3,s0,11820 <nontrivial_free+0x154>
   117c4:	048e                	slli	s1,s1,0x3
   117c6:	97a6                	add	a5,a5,s1
   117c8:	6398                	ld	a4,0(a5)
   117ca:	cb39                	beqz	a4,11820 <nontrivial_free+0x154>
   117cc:	4f1c                	lw	a5,24(a4)
   117ce:	2781                	sext.w	a5,a5
   117d0:	c799                	beqz	a5,117de <nontrivial_free+0x112>
   117d2:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   117d6:	9002                	ebreak
   117d8:	00073023          	sd	zero,0(a4)
   117dc:	bff1                	j	117b8 <nontrivial_free+0xec>
   117de:	6b1c                	ld	a5,16(a4)
   117e0:	01c70813          	addi	a6,a4,28
   117e4:	6794                	ld	a3,8(a5)
   117e6:	4789                	li	a5,2
   117e8:	00d797bb          	sllw	a5,a5,a3
   117ec:	fff7861b          	addiw	a2,a5,-1
   117f0:	40f006bb          	negw	a3,a5
   117f4:	4f5c                	lw	a5,28(a4)
   117f6:	0007851b          	sext.w	a0,a5
   117fa:	00d577b3          	and	a5,a0,a3
   117fe:	88aa                	mv	a7,a0
   11800:	2781                	sext.w	a5,a5
   11802:	160825af          	lr.w.aqrl	a1,(a6)
   11806:	00a59663          	bne	a1,a0,11812 <nontrivial_free+0x146>
   1180a:	1ef8232f          	sc.w.aqrl	t1,a5,(a6)
   1180e:	fe031ae3          	bnez	t1,11802 <nontrivial_free+0x136>
   11812:	2581                	sext.w	a1,a1
   11814:	feb510e3          	bne	a0,a1,117f4 <nontrivial_free+0x128>
   11818:	011677b3          	and	a5,a2,a7
   1181c:	2781                	sext.w	a5,a5
   1181e:	cf1c                	sw	a5,24(a4)
   11820:	7014                	ld	a3,32(s0)
   11822:	02f00713          	li	a4,47
   11826:	0066d793          	srli	a5,a3,0x6
   1182a:	03f7f593          	andi	a1,a5,63
   1182e:	87ae                	mv	a5,a1
   11830:	00b74f63          	blt	a4,a1,1184e <nontrivial_free+0x182>
   11834:	03a58613          	addi	a2,a1,58
   11838:	060e                	slli	a2,a2,0x3
   1183a:	bd818713          	addi	a4,gp,-1064 # 1c3d8 <__malloc_context>
   1183e:	9732                	add	a4,a4,a2
   11840:	01f6f613          	andi	a2,a3,31
   11844:	0016051b          	addiw	a0,a2,1
   11848:	6310                	ld	a2,0(a4)
   1184a:	8e09                	sub	a2,a2,a0
   1184c:	e310                	sd	a2,0(a4)
   1184e:	777d                	lui	a4,0xfffff
   11850:	8f75                	and	a4,a4,a3
   11852:	01043903          	ld	s2,16(s0)
   11856:	c749                	beqz	a4,118e0 <nontrivial_free+0x214>
   11858:	bd818693          	addi	a3,gp,-1064 # 1c3d8 <__malloc_context>
   1185c:	3906c603          	lbu	a2,912(a3)
   11860:	0ff00513          	li	a0,255
   11864:	06a61a63          	bne	a2,a0,118d8 <nontrivial_free+0x20c>
   11868:	bd818713          	addi	a4,gp,-1064 # 1c3d8 <__malloc_context>
   1186c:	02068613          	addi	a2,a3,32
   11870:	34070823          	sb	zero,848(a4) # fffffffffffff350 <__BSS_END__+0xfffffffffffe2930>
   11874:	0705                	addi	a4,a4,1
   11876:	fee61de3          	bne	a2,a4,11870 <nontrivial_free+0x1a4>
   1187a:	4705                	li	a4,1
   1187c:	38e68823          	sb	a4,912(a3)
   11880:	ff95871b          	addiw	a4,a1,-7
   11884:	467d                	li	a2,31
   11886:	00e66863          	bltu	a2,a4,11896 <nontrivial_free+0x1ca>
   1188a:	3906c703          	lbu	a4,912(a3)
   1188e:	37e5                	addiw	a5,a5,-7
   11890:	97b6                	add	a5,a5,a3
   11892:	34e78823          	sb	a4,848(a5) # fffffffffffff350 <__BSS_END__+0xfffffffffffe2930>
   11896:	7004                	ld	s1,32(s0)
   11898:	80b1                	srli	s1,s1,0xc
   1189a:	04b2                	slli	s1,s1,0xc
   1189c:	00043023          	sd	zero,0(s0)
   118a0:	00043423          	sd	zero,8(s0)
   118a4:	00043823          	sd	zero,16(s0)
   118a8:	00043c23          	sd	zero,24(s0)
   118ac:	02043023          	sd	zero,32(s0)
   118b0:	85a2                	mv	a1,s0
   118b2:	be818513          	addi	a0,gp,-1048 # 1c3e8 <__malloc_context+0x10>
   118b6:	d09ff0ef          	jal	ra,115be <queue>
   118ba:	e04a                	sd	s2,0(sp)
   118bc:	e426                	sd	s1,8(sp)
   118be:	70a6                	ld	ra,104(sp)
   118c0:	7406                	ld	s0,96(sp)
   118c2:	6502                	ld	a0,0(sp)
   118c4:	65a2                	ld	a1,8(sp)
   118c6:	64e6                	ld	s1,88(sp)
   118c8:	6946                	ld	s2,80(sp)
   118ca:	69a6                	ld	s3,72(sp)
   118cc:	6a06                	ld	s4,64(sp)
   118ce:	7ae2                	ld	s5,56(sp)
   118d0:	7b42                	ld	s6,48(sp)
   118d2:	7ba2                	ld	s7,40(sp)
   118d4:	6165                	addi	sp,sp,112
   118d6:	8082                	ret
   118d8:	2605                	addiw	a2,a2,1
   118da:	38c68823          	sb	a2,912(a3)
   118de:	b74d                	j	11880 <nontrivial_free+0x1b4>
   118e0:	854a                	mv	a0,s2
   118e2:	d05ff0ef          	jal	ra,115e6 <get_meta>
   118e6:	ffd94583          	lbu	a1,-3(s2)
   118ea:	681c                	ld	a5,16(s0)
   118ec:	89fd                	andi	a1,a1,31
   118ee:	0007b023          	sd	zero,0(a5)
   118f2:	ddbff0ef          	jal	ra,116cc <nontrivial_free>
   118f6:	892a                	mv	s2,a0
   118f8:	84ae                	mv	s1,a1
   118fa:	b74d                	j	1189c <nontrivial_free+0x1d0>
   118fc:	020b9563          	bnez	s7,11926 <nontrivial_free+0x25a>
   11900:	02f00793          	li	a5,47
   11904:	ec97c7e3          	blt	a5,s1,117d2 <nontrivial_free+0x106>
   11908:	04a9                	addi	s1,s1,10
   1190a:	bd818793          	addi	a5,gp,-1064 # 1c3d8 <__malloc_context>
   1190e:	048e                	slli	s1,s1,0x3
   11910:	94be                	add	s1,s1,a5
   11912:	6098                	ld	a4,0(s1)
   11914:	00870963          	beq	a4,s0,11926 <nontrivial_free+0x25a>
   11918:	00aa8513          	addi	a0,s5,10
   1191c:	050e                	slli	a0,a0,0x3
   1191e:	85a2                	mv	a1,s0
   11920:	953e                	add	a0,a0,a5
   11922:	c9dff0ef          	jal	ra,115be <queue>
   11926:	01c40693          	addi	a3,s0,28
   1192a:	4c5c                	lw	a5,28(s0)
   1192c:	2781                	sext.w	a5,a5
   1192e:	00fa6633          	or	a2,s4,a5
   11932:	1606a72f          	lr.w.aqrl	a4,(a3)
   11936:	00f71563          	bne	a4,a5,11940 <nontrivial_free+0x274>
   1193a:	1ec6a5af          	sc.w.aqrl	a1,a2,(a3)
   1193e:	f9f5                	bnez	a1,11932 <nontrivial_free+0x266>
   11940:	2701                	sext.w	a4,a4
   11942:	fee794e3          	bne	a5,a4,1192a <nontrivial_free+0x25e>
   11946:	e002                	sd	zero,0(sp)
   11948:	e402                	sd	zero,8(sp)
   1194a:	bf95                	j	118be <nontrivial_free+0x1f2>
   1194c:	fa0b8ee3          	beqz	s7,11908 <nontrivial_free+0x23c>
   11950:	bfd9                	j	11926 <nontrivial_free+0x25a>
   11952:	e80910e3          	bnez	s2,117d2 <nontrivial_free+0x106>
   11956:	b5e9                	j	11820 <nontrivial_free+0x154>
   11958:	ec0904e3          	beqz	s2,11820 <nontrivial_free+0x154>
   1195c:	bd15                	j	11790 <nontrivial_free+0xc4>

000000000001195e <__libc_free>:
   1195e:	1a050e63          	beqz	a0,11b1a <__libc_free+0x1bc>
   11962:	7159                	addi	sp,sp,-112
   11964:	e8ca                	sd	s2,80(sp)
   11966:	892a                	mv	s2,a0
   11968:	f486                	sd	ra,104(sp)
   1196a:	f0a2                	sd	s0,96(sp)
   1196c:	eca6                	sd	s1,88(sp)
   1196e:	e4ce                	sd	s3,72(sp)
   11970:	e0d2                	sd	s4,64(sp)
   11972:	f85a                	sd	s6,48(sp)
   11974:	fc56                	sd	s5,56(sp)
   11976:	f45e                	sd	s7,40(sp)
   11978:	f062                	sd	s8,32(sp)
   1197a:	c6dff0ef          	jal	ra,115e6 <get_meta>
   1197e:	ffd94983          	lbu	s3,-3(s2)
   11982:	84aa                	mv	s1,a0
   11984:	d1bff0ef          	jal	ra,1169e <get_stride>
   11988:	01f9fa13          	andi	s4,s3,31
   1198c:	8452                	mv	s0,s4
   1198e:	02aa0a33          	mul	s4,s4,a0
   11992:	0104bb03          	ld	s6,16(s1)
   11996:	ffc50593          	addi	a1,a0,-4
   1199a:	0059d993          	srli	s3,s3,0x5
   1199e:	0b41                	addi	s6,s6,16
   119a0:	4691                	li	a3,4
   119a2:	874e                	mv	a4,s3
   119a4:	9b52                	add	s6,s6,s4
   119a6:	00bb07b3          	add	a5,s6,a1
   119aa:	0336f563          	bgeu	a3,s3,119d4 <__libc_free+0x76>
   119ae:	4715                	li	a4,5
   119b0:	00e98563          	beq	s3,a4,119ba <__libc_free+0x5c>
   119b4:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   119b8:	9002                	ebreak
   119ba:	ffc7e703          	lwu	a4,-4(a5)
   119be:	00e6e563          	bltu	a3,a4,119c8 <__libc_free+0x6a>
   119c2:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   119c6:	9002                	ebreak
   119c8:	ffb7c683          	lbu	a3,-5(a5)
   119cc:	c681                	beqz	a3,119d4 <__libc_free+0x76>
   119ce:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   119d2:	9002                	ebreak
   119d4:	412786b3          	sub	a3,a5,s2
   119d8:	00e6f563          	bgeu	a3,a4,119e2 <__libc_free+0x84>
   119dc:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   119e0:	9002                	ebreak
   119e2:	40e78733          	sub	a4,a5,a4
   119e6:	00074703          	lbu	a4,0(a4)
   119ea:	c701                	beqz	a4,119f2 <__libc_free+0x94>
   119ec:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   119f0:	9002                	ebreak
   119f2:	0007c703          	lbu	a4,0(a5)
   119f6:	c701                	beqz	a4,119fe <__libc_free+0xa0>
   119f8:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   119fc:	9002                	ebreak
   119fe:	7098                	ld	a4,32(s1)
   11a00:	4a89                	li	s5,2
   11a02:	00ea9abb          	sllw	s5,s5,a4
   11a06:	577d                	li	a4,-1
   11a08:	fee90ea3          	sb	a4,-3(s2)
   11a0c:	18018713          	addi	a4,gp,384 # 1c980 <__libc>
   11a10:	7b18                	ld	a4,48(a4)
   11a12:	fffb0693          	addi	a3,s6,-1
   11a16:	4a05                	li	s4,1
   11a18:	8fb5                	xor	a5,a5,a3
   11a1a:	fe091f23          	sh	zero,-2(s2)
   11a1e:	00171693          	slli	a3,a4,0x1
   11a22:	008a1a3b          	sllw	s4,s4,s0
   11a26:	3afd                	addiw	s5,s5,-1
   11a28:	18018b93          	addi	s7,gp,384 # 1c980 <__libc>
   11a2c:	02d7ef63          	bltu	a5,a3,11a6a <__libc_free+0x10c>
   11a30:	709c                	ld	a5,32(s1)
   11a32:	8bfd                	andi	a5,a5,31
   11a34:	cb9d                	beqz	a5,11a6a <__libc_free+0x10c>
   11a36:	fff70793          	addi	a5,a4,-1
   11a3a:	41600933          	neg	s2,s6
   11a3e:	00f97933          	and	s2,s2,a5
   11a42:	412585b3          	sub	a1,a1,s2
   11a46:	40e00733          	neg	a4,a4
   11a4a:	8df9                	and	a1,a1,a4
   11a4c:	cd99                	beqz	a1,11a6a <__libc_free+0x10c>
   11a4e:	e42e                	sd	a1,8(sp)
   11a50:	d1cff0ef          	jal	ra,10f6c <__errno_location>
   11a54:	00052c03          	lw	s8,0(a0)
   11a58:	65a2                	ld	a1,8(sp)
   11a5a:	89aa                	mv	s3,a0
   11a5c:	4621                	li	a2,8
   11a5e:	012b0533          	add	a0,s6,s2
   11a62:	07a010ef          	jal	ra,12adc <__madvise>
   11a66:	0189a023          	sw	s8,0(s3)
   11a6a:	01c48613          	addi	a2,s1,28
   11a6e:	4cdc                	lw	a5,28(s1)
   11a70:	4c98                	lw	a4,24(s1)
   11a72:	0007869b          	sext.w	a3,a5
   11a76:	2701                	sext.w	a4,a4
   11a78:	8f55                	or	a4,a4,a3
   11a7a:	00ea75b3          	and	a1,s4,a4
   11a7e:	c581                	beqz	a1,11a86 <__libc_free+0x128>
   11a80:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   11a84:	9002                	ebreak
   11a86:	c6b9                	beqz	a3,11ad4 <__libc_free+0x176>
   11a88:	00ea073b          	addw	a4,s4,a4
   11a8c:	05570463          	beq	a4,s5,11ad4 <__libc_free+0x176>
   11a90:	003bc703          	lbu	a4,3(s7)
   11a94:	00da07bb          	addw	a5,s4,a3
   11a98:	0187171b          	slliw	a4,a4,0x18
   11a9c:	4187571b          	sraiw	a4,a4,0x18
   11aa0:	ef11                	bnez	a4,11abc <__libc_free+0x15e>
   11aa2:	ccdc                	sw	a5,28(s1)
   11aa4:	70a6                	ld	ra,104(sp)
   11aa6:	7406                	ld	s0,96(sp)
   11aa8:	64e6                	ld	s1,88(sp)
   11aaa:	6946                	ld	s2,80(sp)
   11aac:	69a6                	ld	s3,72(sp)
   11aae:	6a06                	ld	s4,64(sp)
   11ab0:	7ae2                	ld	s5,56(sp)
   11ab2:	7b42                	ld	s6,48(sp)
   11ab4:	7ba2                	ld	s7,40(sp)
   11ab6:	7c02                	ld	s8,32(sp)
   11ab8:	6165                	addi	sp,sp,112
   11aba:	8082                	ret
   11abc:	1606272f          	lr.w.aqrl	a4,(a2)
   11ac0:	00d71563          	bne	a4,a3,11aca <__libc_free+0x16c>
   11ac4:	1ef625af          	sc.w.aqrl	a1,a5,(a2)
   11ac8:	f9f5                	bnez	a1,11abc <__libc_free+0x15e>
   11aca:	0007079b          	sext.w	a5,a4
   11ace:	fcf68be3          	beq	a3,a5,11aa4 <__libc_free+0x146>
   11ad2:	bf71                	j	11a6e <__libc_free+0x110>
   11ad4:	003bc783          	lbu	a5,3(s7)
   11ad8:	0187979b          	slliw	a5,a5,0x18
   11adc:	4187d79b          	sraiw	a5,a5,0x18
   11ae0:	c789                	beqz	a5,11aea <__libc_free+0x18c>
   11ae2:	1e818513          	addi	a0,gp,488 # 1c9e8 <__malloc_lock>
   11ae6:	7c1010ef          	jal	ra,13aa6 <__lock>
   11aea:	85a2                	mv	a1,s0
   11aec:	8526                	mv	a0,s1
   11aee:	bdfff0ef          	jal	ra,116cc <nontrivial_free>
   11af2:	842e                	mv	s0,a1
   11af4:	e82a                	sd	a0,16(sp)
   11af6:	1e818513          	addi	a0,gp,488 # 1c9e8 <__malloc_lock>
   11afa:	ec2e                	sd	a1,24(sp)
   11afc:	082020ef          	jal	ra,13b7e <__unlock>
   11b00:	d055                	beqz	s0,11aa4 <__libc_free+0x146>
   11b02:	c6aff0ef          	jal	ra,10f6c <__errno_location>
   11b06:	00052903          	lw	s2,0(a0)
   11b0a:	84aa                	mv	s1,a0
   11b0c:	6542                	ld	a0,16(sp)
   11b0e:	85a2                	mv	a1,s0
   11b10:	096010ef          	jal	ra,12ba6 <__munmap>
   11b14:	0124a023          	sw	s2,0(s1)
   11b18:	b771                	j	11aa4 <__libc_free+0x146>
   11b1a:	8082                	ret

0000000000011b1c <queue>:
   11b1c:	659c                	ld	a5,8(a1)
   11b1e:	c781                	beqz	a5,11b26 <queue+0xa>
   11b20:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   11b24:	9002                	ebreak
   11b26:	619c                	ld	a5,0(a1)
   11b28:	ffe5                	bnez	a5,11b20 <queue+0x4>
   11b2a:	611c                	ld	a5,0(a0)
   11b2c:	cb81                	beqz	a5,11b3c <queue+0x20>
   11b2e:	e59c                	sd	a5,8(a1)
   11b30:	639c                	ld	a5,0(a5)
   11b32:	e19c                	sd	a5,0(a1)
   11b34:	e78c                	sd	a1,8(a5)
   11b36:	659c                	ld	a5,8(a1)
   11b38:	e38c                	sd	a1,0(a5)
   11b3a:	8082                	ret
   11b3c:	e58c                	sd	a1,8(a1)
   11b3e:	e18c                	sd	a1,0(a1)
   11b40:	e10c                	sd	a1,0(a0)
   11b42:	8082                	ret

0000000000011b44 <get_stride>:
   11b44:	7108                	ld	a0,32(a0)
   11b46:	01f57793          	andi	a5,a0,31
   11b4a:	eb81                	bnez	a5,11b5a <get_stride+0x16>
   11b4c:	77fd                	lui	a5,0xfffff
   11b4e:	8fe9                	and	a5,a5,a0
   11b50:	c789                	beqz	a5,11b5a <get_stride+0x16>
   11b52:	8131                	srli	a0,a0,0xc
   11b54:	0532                	slli	a0,a0,0xc
   11b56:	1541                	addi	a0,a0,-16
   11b58:	8082                	ret
   11b5a:	8115                	srli	a0,a0,0x5
   11b5c:	67e5                	lui	a5,0x19
   11b5e:	6f078793          	addi	a5,a5,1776 # 196f0 <__malloc_size_classes>
   11b62:	07e57513          	andi	a0,a0,126
   11b66:	953e                	add	a0,a0,a5
   11b68:	00055503          	lhu	a0,0(a0)
   11b6c:	0045151b          	slliw	a0,a0,0x4
   11b70:	8082                	ret

0000000000011b72 <enframe>:
   11b72:	7179                	addi	sp,sp,-48
   11b74:	f022                	sd	s0,32(sp)
   11b76:	ec26                	sd	s1,24(sp)
   11b78:	8436                	mv	s0,a3
   11b7a:	84ae                	mv	s1,a1
   11b7c:	e84a                	sd	s2,16(sp)
   11b7e:	e44e                	sd	s3,8(sp)
   11b80:	8932                	mv	s2,a2
   11b82:	89aa                	mv	s3,a0
   11b84:	f406                	sd	ra,40(sp)
   11b86:	fbfff0ef          	jal	ra,11b44 <get_stride>
   11b8a:	87aa                	mv	a5,a0
   11b8c:	02f486b3          	mul	a3,s1,a5
   11b90:	41250733          	sub	a4,a0,s2
   11b94:	0109b503          	ld	a0,16(s3)
   11b98:	1771                	addi	a4,a4,-4
   11b9a:	17f1                	addi	a5,a5,-4
   11b9c:	0541                	addi	a0,a0,16
   11b9e:	00475613          	srli	a2,a4,0x4
   11ba2:	9536                	add	a0,a0,a3
   11ba4:	ffd54583          	lbu	a1,-3(a0)
   11ba8:	97aa                	add	a5,a5,a0
   11baa:	0ff47693          	andi	a3,s0,255
   11bae:	c591                	beqz	a1,11bba <enframe+0x48>
   11bb0:	ffe55683          	lhu	a3,-2(a0)
   11bb4:	2685                	addiw	a3,a3,1
   11bb6:	0ff6f693          	andi	a3,a3,255
   11bba:	ffc54583          	lbu	a1,-4(a0)
   11bbe:	c581                	beqz	a1,11bc6 <enframe+0x54>
   11bc0:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   11bc4:	9002                	ebreak
   11bc6:	02d67863          	bgeu	a2,a3,11bf6 <enframe+0x84>
   11bca:	8315                	srli	a4,a4,0x5
   11bcc:	8f51                	or	a4,a4,a2
   11bce:	00275593          	srli	a1,a4,0x2
   11bd2:	8dd9                	or	a1,a1,a4
   11bd4:	0045d713          	srli	a4,a1,0x4
   11bd8:	8f4d                	or	a4,a4,a1
   11bda:	8ef9                	and	a3,a3,a4
   11bdc:	0006871b          	sext.w	a4,a3
   11be0:	1682                	slli	a3,a3,0x20
   11be2:	9281                	srli	a3,a3,0x20
   11be4:	00d67563          	bgeu	a2,a3,11bee <enframe+0x7c>
   11be8:	fff64693          	not	a3,a2
   11bec:	9f35                	addw	a4,a4,a3
   11bee:	0007069b          	sext.w	a3,a4
   11bf2:	fce667e3          	bltu	a2,a4,11bc0 <enframe+0x4e>
   11bf6:	ca99                	beqz	a3,11c0c <enframe+0x9a>
   11bf8:	fed51f23          	sh	a3,-2(a0)
   11bfc:	5701                	li	a4,-32
   11bfe:	0046969b          	slliw	a3,a3,0x4
   11c02:	fee50ea3          	sb	a4,-3(a0)
   11c06:	9536                	add	a0,a0,a3
   11c08:	fe050e23          	sb	zero,-4(a0)
   11c0c:	0109b703          	ld	a4,16(s3)
   11c10:	40a78633          	sub	a2,a5,a0
   11c14:	fe950ea3          	sb	s1,-3(a0)
   11c18:	0741                	addi	a4,a4,16
   11c1a:	40e50733          	sub	a4,a0,a4
   11c1e:	8311                	srli	a4,a4,0x4
   11c20:	fee51f23          	sh	a4,-2(a0)
   11c24:	4126073b          	subw	a4,a2,s2
   11c28:	cf11                	beqz	a4,11c44 <enframe+0xd2>
   11c2a:	40e006bb          	negw	a3,a4
   11c2e:	96be                	add	a3,a3,a5
   11c30:	00068023          	sb	zero,0(a3)
   11c34:	4691                	li	a3,4
   11c36:	00e6d763          	bge	a3,a4,11c44 <enframe+0xd2>
   11c3a:	fee7ae23          	sw	a4,-4(a5)
   11c3e:	fe078da3          	sb	zero,-5(a5)
   11c42:	4715                	li	a4,5
   11c44:	ffd54783          	lbu	a5,-3(a0)
   11c48:	0057171b          	slliw	a4,a4,0x5
   11c4c:	8bfd                	andi	a5,a5,31
   11c4e:	9f3d                	addw	a4,a4,a5
   11c50:	fee50ea3          	sb	a4,-3(a0)
   11c54:	70a2                	ld	ra,40(sp)
   11c56:	7402                	ld	s0,32(sp)
   11c58:	64e2                	ld	s1,24(sp)
   11c5a:	6942                	ld	s2,16(sp)
   11c5c:	69a2                	ld	s3,8(sp)
   11c5e:	6145                	addi	sp,sp,48
   11c60:	8082                	ret

0000000000011c62 <rdlock>:
   11c62:	18018793          	addi	a5,gp,384 # 1c980 <__libc>
   11c66:	0037c783          	lbu	a5,3(a5)
   11c6a:	0187979b          	slliw	a5,a5,0x18
   11c6e:	4187d79b          	sraiw	a5,a5,0x18
   11c72:	c789                	beqz	a5,11c7c <rdlock+0x1a>
   11c74:	1e818513          	addi	a0,gp,488 # 1c9e8 <__malloc_lock>
   11c78:	62f0106f          	j	13aa6 <__lock>
   11c7c:	8082                	ret

0000000000011c7e <step_seq>:
   11c7e:	bd818693          	addi	a3,gp,-1064 # 1c3d8 <__malloc_context>
   11c82:	3906c703          	lbu	a4,912(a3)
   11c86:	0ff00593          	li	a1,255
   11c8a:	bd818613          	addi	a2,gp,-1064 # 1c3d8 <__malloc_context>
   11c8e:	00b71f63          	bne	a4,a1,11cac <step_seq+0x2e>
   11c92:	bd818793          	addi	a5,gp,-1064 # 1c3d8 <__malloc_context>
   11c96:	02060713          	addi	a4,a2,32
   11c9a:	34078823          	sb	zero,848(a5)
   11c9e:	0785                	addi	a5,a5,1
   11ca0:	fee79de3          	bne	a5,a4,11c9a <step_seq+0x1c>
   11ca4:	4785                	li	a5,1
   11ca6:	38f60823          	sb	a5,912(a2)
   11caa:	8082                	ret
   11cac:	2705                	addiw	a4,a4,1
   11cae:	38e68823          	sb	a4,912(a3)
   11cb2:	8082                	ret

0000000000011cb4 <size_to_class>:
   11cb4:	050d                	addi	a0,a0,3
   11cb6:	8111                	srli	a0,a0,0x4
   11cb8:	47a5                	li	a5,9
   11cba:	00a7e463          	bltu	a5,a0,11cc2 <size_to_class+0xe>
   11cbe:	2501                	sext.w	a0,a0
   11cc0:	8082                	ret
   11cc2:	00150693          	addi	a3,a0,1
   11cc6:	0016d79b          	srliw	a5,a3,0x1
   11cca:	0026d71b          	srliw	a4,a3,0x2
   11cce:	8f5d                	or	a4,a4,a5
   11cd0:	0007079b          	sext.w	a5,a4
   11cd4:	0027571b          	srliw	a4,a4,0x2
   11cd8:	8fd9                	or	a5,a5,a4
   11cda:	0007871b          	sext.w	a4,a5
   11cde:	0047d79b          	srliw	a5,a5,0x4
   11ce2:	8f5d                	or	a4,a4,a5
   11ce4:	0007079b          	sext.w	a5,a4
   11ce8:	0087571b          	srliw	a4,a4,0x8
   11cec:	8fd9                	or	a5,a5,a4
   11cee:	0007871b          	sext.w	a4,a5
   11cf2:	0107d79b          	srliw	a5,a5,0x10
   11cf6:	8f5d                	or	a4,a4,a5
   11cf8:	0007079b          	sext.w	a5,a4
   11cfc:	fff7c793          	not	a5,a5
   11d00:	2705                	addiw	a4,a4,1
   11d02:	8ff9                	and	a5,a5,a4
   11d04:	076be737          	lui	a4,0x76be
   11d08:	6297071b          	addiw	a4,a4,1577
   11d0c:	02e787bb          	mulw	a5,a5,a4
   11d10:	6765                	lui	a4,0x19
   11d12:	75070713          	addi	a4,a4,1872 # 19750 <debruijn32.2704>
   11d16:	01b7d79b          	srliw	a5,a5,0x1b
   11d1a:	97ba                	add	a5,a5,a4
   11d1c:	0007c783          	lbu	a5,0(a5)
   11d20:	6765                	lui	a4,0x19
   11d22:	6f070613          	addi	a2,a4,1776 # 196f0 <__malloc_size_classes>
   11d26:	37fd                	addiw	a5,a5,-1
   11d28:	0027951b          	slliw	a0,a5,0x2
   11d2c:	0015059b          	addiw	a1,a0,1
   11d30:	0586                	slli	a1,a1,0x1
   11d32:	962e                	add	a2,a2,a1
   11d34:	00065603          	lhu	a2,0(a2)
   11d38:	6f070713          	addi	a4,a4,1776
   11d3c:	00d67363          	bgeu	a2,a3,11d42 <size_to_class+0x8e>
   11d40:	2509                	addiw	a0,a0,2
   11d42:	00151793          	slli	a5,a0,0x1
   11d46:	973e                	add	a4,a4,a5
   11d48:	00075783          	lhu	a5,0(a4)
   11d4c:	00d7f363          	bgeu	a5,a3,11d52 <size_to_class+0x9e>
   11d50:	2505                	addiw	a0,a0,1
   11d52:	8082                	ret

0000000000011d54 <__malloc_atfork>:
   11d54:	00055363          	bgez	a0,11d5a <__malloc_atfork+0x6>
   11d58:	b729                	j	11c62 <rdlock>
   11d5a:	c501                	beqz	a0,11d62 <__malloc_atfork+0xe>
   11d5c:	1e01a423          	sw	zero,488(gp) # 1c9e8 <__malloc_lock>
   11d60:	8082                	ret
   11d62:	1e818513          	addi	a0,gp,488 # 1c9e8 <__malloc_lock>
   11d66:	6190106f          	j	13b7e <__unlock>

0000000000011d6a <__malloc_alloc_meta>:
   11d6a:	7139                	addi	sp,sp,-64
   11d6c:	f822                	sd	s0,48(sp)
   11d6e:	bd818793          	addi	a5,gp,-1064 # 1c3d8 <__malloc_context>
   11d72:	479c                	lw	a5,8(a5)
   11d74:	fc06                	sd	ra,56(sp)
   11d76:	f426                	sd	s1,40(sp)
   11d78:	f04a                	sd	s2,32(sp)
   11d7a:	ec4e                	sd	s3,24(sp)
   11d7c:	e852                	sd	s4,16(sp)
   11d7e:	bd818413          	addi	s0,gp,-1064 # 1c3d8 <__malloc_context>
   11d82:	e79d                	bnez	a5,11db0 <__malloc_alloc_meta+0x46>
   11d84:	41c657b7          	lui	a5,0x41c65
   11d88:	00810993          	addi	s3,sp,8
   11d8c:	e6d78793          	addi	a5,a5,-403 # 41c64e6d <__BSS_END__+0x41c4844d>
   11d90:	02f987b3          	mul	a5,s3,a5
   11d94:	4481                	li	s1,0
   11d96:	18018913          	addi	s2,gp,384 # 1c980 <__libc>
   11d9a:	4a65                	li	s4,25
   11d9c:	e43e                	sd	a5,8(sp)
   11d9e:	00893783          	ld	a5,8(s2)
   11da2:	97a6                	add	a5,a5,s1
   11da4:	6398                	ld	a4,0(a5)
   11da6:	eb15                	bnez	a4,11dda <__malloc_alloc_meta+0x70>
   11da8:	67a2                	ld	a5,8(sp)
   11daa:	e01c                	sd	a5,0(s0)
   11dac:	4785                	li	a5,1
   11dae:	c41c                	sw	a5,8(s0)
   11db0:	01043983          	ld	s3,16(s0)
   11db4:	0e098b63          	beqz	s3,11eaa <__malloc_alloc_meta+0x140>
   11db8:	0089b783          	ld	a5,8(s3)
   11dbc:	02f98963          	beq	s3,a5,11dee <__malloc_alloc_meta+0x84>
   11dc0:	0009b703          	ld	a4,0(s3)
   11dc4:	e71c                	sd	a5,8(a4)
   11dc6:	e398                	sd	a4,0(a5)
   11dc8:	6818                	ld	a4,16(s0)
   11dca:	00e99363          	bne	s3,a4,11dd0 <__malloc_alloc_meta+0x66>
   11dce:	e81c                	sd	a5,16(s0)
   11dd0:	0009b423          	sd	zero,8(s3)
   11dd4:	0009b023          	sd	zero,0(s3)
   11dd8:	a071                	j	11e64 <__malloc_alloc_meta+0xfa>
   11dda:	01471863          	bne	a4,s4,11dea <__malloc_alloc_meta+0x80>
   11dde:	678c                	ld	a1,8(a5)
   11de0:	4621                	li	a2,8
   11de2:	854e                	mv	a0,s3
   11de4:	05a1                	addi	a1,a1,8
   11de6:	62e010ef          	jal	ra,13414 <memcpy>
   11dea:	04c1                	addi	s1,s1,16
   11dec:	bf4d                	j	11d9e <__malloc_alloc_meta+0x34>
   11dee:	00043823          	sd	zero,16(s0)
   11df2:	bff9                	j	11dd0 <__malloc_alloc_meta+0x66>
   11df4:	0d600893          	li	a7,214
   11df8:	4501                	li	a0,0
   11dfa:	00000073          	ecall
   11dfe:	40a00933          	neg	s2,a0
   11e02:	fff48793          	addi	a5,s1,-1
   11e06:	00f97933          	and	s2,s2,a5
   11e0a:	954a                	add	a0,a0,s2
   11e0c:	00149913          	slli	s2,s1,0x1
   11e10:	38a43c23          	sd	a0,920(s0)
   11e14:	992a                	add	s2,s2,a0
   11e16:	4785                	li	a5,1
   11e18:	a86d                	j	11ed2 <__malloc_alloc_meta+0x168>
   11e1a:	cb99                	beqz	a5,11e30 <__malloc_alloc_meta+0xc6>
   11e1c:	39843503          	ld	a0,920(s0)
   11e20:	4781                	li	a5,0
   11e22:	577d                	li	a4,-1
   11e24:	03200693          	li	a3,50
   11e28:	4601                	li	a2,0
   11e2a:	85a6                	mv	a1,s1
   11e2c:	4cb000ef          	jal	ra,12af6 <__mmap>
   11e30:	39243c23          	sd	s2,920(s0)
   11e34:	00c4d793          	srli	a5,s1,0xc
   11e38:	40990933          	sub	s2,s2,s1
   11e3c:	05243423          	sd	s2,72(s0)
   11e40:	f41c                	sd	a5,40(s0)
   11e42:	4a01                	li	s4,0
   11e44:	a055                	j	11ee8 <__malloc_alloc_meta+0x17e>
   11e46:	0e0a0763          	beqz	s4,11f34 <__malloc_alloc_meta+0x1ca>
   11e4a:	460d                	li	a2,3
   11e4c:	85a6                	mv	a1,s1
   11e4e:	854a                	mv	a0,s2
   11e50:	525000ef          	jal	ra,12b74 <__mprotect>
   11e54:	c165                	beqz	a0,11f34 <__malloc_alloc_meta+0x1ca>
   11e56:	916ff0ef          	jal	ra,10f6c <__errno_location>
   11e5a:	4118                	lw	a4,0(a0)
   11e5c:	02600793          	li	a5,38
   11e60:	0cf70a63          	beq	a4,a5,11f34 <__malloc_alloc_meta+0x1ca>
   11e64:	70e2                	ld	ra,56(sp)
   11e66:	7442                	ld	s0,48(sp)
   11e68:	74a2                	ld	s1,40(sp)
   11e6a:	7902                	ld	s2,32(sp)
   11e6c:	6a42                	ld	s4,16(sp)
   11e6e:	854e                	mv	a0,s3
   11e70:	69e2                	ld	s3,24(sp)
   11e72:	6121                	addi	sp,sp,64
   11e74:	8082                	ret
   11e76:	0127b423          	sd	s2,8(a5)
   11e7a:	601c                	ld	a5,0(s0)
   11e7c:	05243023          	sd	s2,64(s0)
   11e80:	0961                	addi	s2,s2,24
   11e82:	fef93423          	sd	a5,-24(s2)
   11e86:	06500793          	li	a5,101
   11e8a:	fef92c23          	sw	a5,-8(s2)
   11e8e:	06500793          	li	a5,101
   11e92:	f01c                	sd	a5,32(s0)
   11e94:	01243c23          	sd	s2,24(s0)
   11e98:	701c                	ld	a5,32(s0)
   11e9a:	01843983          	ld	s3,24(s0)
   11e9e:	17fd                	addi	a5,a5,-1
   11ea0:	f01c                	sd	a5,32(s0)
   11ea2:	02898793          	addi	a5,s3,40
   11ea6:	ec1c                	sd	a5,24(s0)
   11ea8:	b725                	j	11dd0 <__malloc_alloc_meta+0x66>
   11eaa:	701c                	ld	a5,32(s0)
   11eac:	f7f5                	bnez	a5,11e98 <__malloc_alloc_meta+0x12e>
   11eae:	1b01b483          	ld	s1,432(gp) # 1c9b0 <__libc+0x30>
   11eb2:	6785                	lui	a5,0x1
   11eb4:	00f4f363          	bgeu	s1,a5,11eba <__malloc_alloc_meta+0x150>
   11eb8:	6485                	lui	s1,0x1
   11eba:	741c                	ld	a5,40(s0)
   11ebc:	4a05                	li	s4,1
   11ebe:	e78d                	bnez	a5,11ee8 <__malloc_alloc_meta+0x17e>
   11ec0:	39843903          	ld	s2,920(s0)
   11ec4:	57fd                	li	a5,-1
   11ec6:	02f90163          	beq	s2,a5,11ee8 <__malloc_alloc_meta+0x17e>
   11eca:	f20905e3          	beqz	s2,11df4 <__malloc_alloc_meta+0x8a>
   11ece:	9926                	add	s2,s2,s1
   11ed0:	4781                	li	a5,0
   11ed2:	0d600893          	li	a7,214
   11ed6:	854a                	mv	a0,s2
   11ed8:	00000073          	ecall
   11edc:	f2a90fe3          	beq	s2,a0,11e1a <__malloc_alloc_meta+0xb0>
   11ee0:	57fd                	li	a5,-1
   11ee2:	38f43c23          	sd	a5,920(s0)
   11ee6:	4a05                	li	s4,1
   11ee8:	741c                	ld	a5,40(s0)
   11eea:	ef95                	bnez	a5,11f26 <__malloc_alloc_meta+0x1bc>
   11eec:	781c                	ld	a5,48(s0)
   11eee:	4909                	li	s2,2
   11ef0:	577d                	li	a4,-1
   11ef2:	00f91933          	sll	s2,s2,a5
   11ef6:	032485b3          	mul	a1,s1,s2
   11efa:	4781                	li	a5,0
   11efc:	02200693          	li	a3,34
   11f00:	4601                	li	a2,0
   11f02:	4501                	li	a0,0
   11f04:	3f3000ef          	jal	ra,12af6 <__mmap>
   11f08:	57fd                	li	a5,-1
   11f0a:	f4f50de3          	beq	a0,a5,11e64 <__malloc_alloc_meta+0xfa>
   11f0e:	00c4d793          	srli	a5,s1,0xc
   11f12:	197d                	addi	s2,s2,-1
   11f14:	02f90933          	mul	s2,s2,a5
   11f18:	781c                	ld	a5,48(s0)
   11f1a:	9526                	add	a0,a0,s1
   11f1c:	e428                	sd	a0,72(s0)
   11f1e:	0785                	addi	a5,a5,1
   11f20:	f81c                	sd	a5,48(s0)
   11f22:	03243423          	sd	s2,40(s0)
   11f26:	04843903          	ld	s2,72(s0)
   11f2a:	fff48793          	addi	a5,s1,-1 # fff <exit-0xf121>
   11f2e:	00f977b3          	and	a5,s2,a5
   11f32:	db91                	beqz	a5,11e46 <__malloc_alloc_meta+0xdc>
   11f34:	741c                	ld	a5,40(s0)
   11f36:	17fd                	addi	a5,a5,-1
   11f38:	f41c                	sd	a5,40(s0)
   11f3a:	6785                	lui	a5,0x1
   11f3c:	97ca                	add	a5,a5,s2
   11f3e:	e43c                	sd	a5,72(s0)
   11f40:	603c                	ld	a5,64(s0)
   11f42:	fb95                	bnez	a5,11e76 <__malloc_alloc_meta+0x10c>
   11f44:	03243c23          	sd	s2,56(s0)
   11f48:	bf0d                	j	11e7a <__malloc_alloc_meta+0x110>

0000000000011f4a <alloc_slot>:
   11f4a:	7119                	addi	sp,sp,-128
   11f4c:	ecce                	sd	s3,88(sp)
   11f4e:	e8d2                	sd	s4,80(sp)
   11f50:	00a50993          	addi	s3,a0,10
   11f54:	bd818713          	addi	a4,gp,-1064 # 1c3d8 <__malloc_context>
   11f58:	00399793          	slli	a5,s3,0x3
   11f5c:	973e                	add	a4,a4,a5
   11f5e:	631c                	ld	a5,0(a4)
   11f60:	f0ca                	sd	s2,96(sp)
   11f62:	e0da                	sd	s6,64(sp)
   11f64:	fc86                	sd	ra,120(sp)
   11f66:	f8a2                	sd	s0,112(sp)
   11f68:	f4a6                	sd	s1,104(sp)
   11f6a:	e4d6                	sd	s5,72(sp)
   11f6c:	fc5e                	sd	s7,56(sp)
   11f6e:	f862                	sd	s8,48(sp)
   11f70:	f466                	sd	s9,40(sp)
   11f72:	f06a                	sd	s10,32(sp)
   11f74:	ec6e                	sd	s11,24(sp)
   11f76:	892a                	mv	s2,a0
   11f78:	8b2e                	mv	s6,a1
   11f7a:	bd818a13          	addi	s4,gp,-1064 # 1c3d8 <__malloc_context>
   11f7e:	cb9d                	beqz	a5,11fb4 <alloc_slot+0x6a>
   11f80:	4f94                	lw	a3,24(a5)
   11f82:	2681                	sext.w	a3,a3
   11f84:	2e069763          	bnez	a3,12272 <alloc_slot+0x328>
   11f88:	4fd0                	lw	a2,28(a5)
   11f8a:	6794                	ld	a3,8(a5)
   11f8c:	2601                	sext.w	a2,a2
   11f8e:	1c061363          	bnez	a2,12154 <alloc_slot+0x20a>
   11f92:	1ad78e63          	beq	a5,a3,1214e <alloc_slot+0x204>
   11f96:	6390                	ld	a2,0(a5)
   11f98:	e614                	sd	a3,8(a2)
   11f9a:	6794                	ld	a3,8(a5)
   11f9c:	e290                	sd	a2,0(a3)
   11f9e:	e314                	sd	a3,0(a4)
   11fa0:	0007b023          	sd	zero,0(a5) # 1000 <exit-0xf120>
   11fa4:	0007b423          	sd	zero,8(a5)
   11fa8:	00399793          	slli	a5,s3,0x3
   11fac:	97d2                	add	a5,a5,s4
   11fae:	6394                	ld	a3,0(a5)
   11fb0:	1a069363          	bnez	a3,12156 <alloc_slot+0x20c>
   11fb4:	6de5                	lui	s11,0x19
   11fb6:	6f0d8793          	addi	a5,s11,1776 # 196f0 <__malloc_size_classes>
   11fba:	00191713          	slli	a4,s2,0x1
   11fbe:	97ba                	add	a5,a5,a4
   11fc0:	0007d783          	lhu	a5,0(a5)
   11fc4:	6f0d8d93          	addi	s11,s11,1776
   11fc8:	00479c1b          	slliw	s8,a5,0x4
   11fcc:	d9fff0ef          	jal	ra,11d6a <__malloc_alloc_meta>
   11fd0:	84aa                	mv	s1,a0
   11fd2:	557d                	li	a0,-1
   11fd4:	2c048963          	beqz	s1,122a6 <alloc_slot+0x35c>
   11fd8:	03a90b93          	addi	s7,s2,58
   11fdc:	003b9793          	slli	a5,s7,0x3
   11fe0:	97d2                	add	a5,a5,s4
   11fe2:	0007bd03          	ld	s10,0(a5)
   11fe6:	1b01bc83          	ld	s9,432(gp) # 1c9b0 <__libc+0x30>
   11fea:	47a1                	li	a5,8
   11fec:	2d27cc63          	blt	a5,s2,122c4 <alloc_slot+0x37a>
   11ff0:	470d                	li	a4,3
   11ff2:	02e90733          	mul	a4,s2,a4
   11ff6:	67e5                	lui	a5,0x19
   11ff8:	77078693          	addi	a3,a5,1904 # 19770 <small_cnt_tab>
   11ffc:	77078793          	addi	a5,a5,1904
   12000:	96ba                	add	a3,a3,a4
   12002:	0006c603          	lbu	a2,0(a3)
   12006:	4701                	li	a4,0
   12008:	0026161b          	slliw	a2,a2,0x2
   1200c:	00cd7963          	bgeu	s10,a2,1201e <alloc_slot+0xd4>
   12010:	0016c703          	lbu	a4,1(a3)
   12014:	0027171b          	slliw	a4,a4,0x2
   12018:	00ed3733          	sltu	a4,s10,a4
   1201c:	0705                	addi	a4,a4,1
   1201e:	468d                	li	a3,3
   12020:	02d906b3          	mul	a3,s2,a3
   12024:	97b6                	add	a5,a5,a3
   12026:	97ba                	add	a5,a5,a4
   12028:	0007c403          	lbu	s0,0(a5)
   1202c:	4705                	li	a4,1
   1202e:	001cd793          	srli	a5,s9,0x1
   12032:	00e41763          	bne	s0,a4,12040 <alloc_slot+0xf6>
   12036:	010c0713          	addi	a4,s8,16
   1203a:	00e7e363          	bltu	a5,a4,12040 <alloc_slot+0xf6>
   1203e:	4409                	li	s0,2
   12040:	03840ab3          	mul	s5,s0,s8
   12044:	010a8713          	addi	a4,s5,16
   12048:	38e7fa63          	bgeu	a5,a4,123dc <alloc_slot+0x492>
   1204c:	ff99079b          	addiw	a5,s2,-7
   12050:	477d                	li	a4,31
   12052:	4d81                	li	s11,0
   12054:	02f76a63          	bltu	a4,a5,12088 <alloc_slot+0x13e>
   12058:	97d2                	add	a5,a5,s4
   1205a:	3707c703          	lbu	a4,880(a5)
   1205e:	3507c683          	lbu	a3,848(a5)
   12062:	06300d93          	li	s11,99
   12066:	00edbdb3          	sltu	s11,s11,a4
   1206a:	8636                	mv	a2,a3
   1206c:	ce91                	beqz	a3,12088 <alloc_slot+0x13e>
   1206e:	390a4683          	lbu	a3,912(s4)
   12072:	9e91                	subw	a3,a3,a2
   12074:	4625                	li	a2,9
   12076:	00d64963          	blt	a2,a3,12088 <alloc_slot+0x13e>
   1207a:	06200693          	li	a3,98
   1207e:	26e6ed63          	bltu	a3,a4,122f8 <alloc_slot+0x3ae>
   12082:	2705                	addiw	a4,a4,1
   12084:	36e78823          	sb	a4,880(a5)
   12088:	bf7ff0ef          	jal	ra,11c7e <step_seq>
   1208c:	00197793          	andi	a5,s2,1
   12090:	ef81                	bnez	a5,120a8 <alloc_slot+0x15e>
   12092:	47fd                	li	a5,31
   12094:	0127ca63          	blt	a5,s2,120a8 <alloc_slot+0x15e>
   12098:	0019079b          	addiw	a5,s2,1
   1209c:	03a78793          	addi	a5,a5,58
   120a0:	078e                	slli	a5,a5,0x3
   120a2:	97d2                	add	a5,a5,s4
   120a4:	639c                	ld	a5,0(a5)
   120a6:	9d3e                	add	s10,s10,a5
   120a8:	0024179b          	slliw	a5,s0,0x2
   120ac:	00fd7d63          	bgeu	s10,a5,120c6 <alloc_slot+0x17c>
   120b0:	000d9b63          	bnez	s11,120c6 <alloc_slot+0x17c>
   120b4:	00397793          	andi	a5,s2,3
   120b8:	4705                	li	a4,1
   120ba:	24e79263          	bne	a5,a4,122fe <alloc_slot+0x3b4>
   120be:	003c9793          	slli	a5,s9,0x3
   120c2:	2757e363          	bltu	a5,s5,12328 <alloc_slot+0x3de>
   120c6:	038405b3          	mul	a1,s0,s8
   120ca:	5ac1                	li	s5,-16
   120cc:	fffc8793          	addi	a5,s9,-1
   120d0:	40ba8ab3          	sub	s5,s5,a1
   120d4:	01058713          	addi	a4,a1,16
   120d8:	00fafab3          	and	s5,s5,a5
   120dc:	9aba                	add	s5,s5,a4
   120de:	020d9b63          	bnez	s11,12114 <alloc_slot+0x1ca>
   120e2:	471d                	li	a4,7
   120e4:	02874863          	blt	a4,s0,12114 <alloc_slot+0x1ca>
   120e8:	55b1                	li	a1,-20
   120ea:	014b0713          	addi	a4,s6,20
   120ee:	41658b33          	sub	s6,a1,s6
   120f2:	00fb7b33          	and	s6,s6,a5
   120f6:	9b3a                	add	s6,s6,a4
   120f8:	010c0793          	addi	a5,s8,16
   120fc:	00fb6a63          	bltu	s6,a5,12110 <alloc_slot+0x1c6>
   12100:	002c9793          	slli	a5,s9,0x2
   12104:	00fb6863          	bltu	s6,a5,12114 <alloc_slot+0x1ca>
   12108:	00141793          	slli	a5,s0,0x1
   1210c:	00fd7463          	bgeu	s10,a5,12114 <alloc_slot+0x1ca>
   12110:	8ada                	mv	s5,s6
   12112:	4405                	li	s0,1
   12114:	4781                	li	a5,0
   12116:	577d                	li	a4,-1
   12118:	02200693          	li	a3,34
   1211c:	460d                	li	a2,3
   1211e:	85d6                	mv	a1,s5
   12120:	4501                	li	a0,0
   12122:	1d5000ef          	jal	ra,12af6 <__mmap>
   12126:	57fd                	li	a5,-1
   12128:	20f51263          	bne	a0,a5,1232c <alloc_slot+0x3e2>
   1212c:	010a0513          	addi	a0,s4,16
   12130:	0004b023          	sd	zero,0(s1)
   12134:	0004b423          	sd	zero,8(s1)
   12138:	0004b823          	sd	zero,16(s1)
   1213c:	0004bc23          	sd	zero,24(s1)
   12140:	0204b023          	sd	zero,32(s1)
   12144:	85a6                	mv	a1,s1
   12146:	9d7ff0ef          	jal	ra,11b1c <queue>
   1214a:	557d                	li	a0,-1
   1214c:	aaa9                	j	122a6 <alloc_slot+0x35c>
   1214e:	00073023          	sd	zero,0(a4)
   12152:	b5b9                	j	11fa0 <alloc_slot+0x56>
   12154:	e314                	sd	a3,0(a4)
   12156:	7290                	ld	a2,32(a3)
   12158:	4edc                	lw	a5,28(a3)
   1215a:	4709                	li	a4,2
   1215c:	00c7173b          	sllw	a4,a4,a2
   12160:	2781                	sext.w	a5,a5
   12162:	377d                	addiw	a4,a4,-1
   12164:	00f71c63          	bne	a4,a5,1217c <alloc_slot+0x232>
   12168:	02067613          	andi	a2,a2,32
   1216c:	ca01                	beqz	a2,1217c <alloc_slot+0x232>
   1216e:	6694                	ld	a3,8(a3)
   12170:	00399793          	slli	a5,s3,0x3
   12174:	97d2                	add	a5,a5,s4
   12176:	e394                	sd	a3,0(a5)
   12178:	4edc                	lw	a5,28(a3)
   1217a:	2781                	sext.w	a5,a5
   1217c:	6a8c                	ld	a1,16(a3)
   1217e:	4609                	li	a2,2
   12180:	6588                	ld	a0,8(a1)
   12182:	00a6163b          	sllw	a2,a2,a0
   12186:	367d                	addiw	a2,a2,-1
   12188:	8ff1                	and	a5,a5,a2
   1218a:	0007861b          	sext.w	a2,a5
   1218e:	01f57713          	andi	a4,a0,31
   12192:	87b6                	mv	a5,a3
   12194:	ea01                	bnez	a2,121a4 <alloc_slot+0x25a>
   12196:	669c                	ld	a5,8(a3)
   12198:	00f68c63          	beq	a3,a5,121b0 <alloc_slot+0x266>
   1219c:	00399713          	slli	a4,s3,0x3
   121a0:	9752                	add	a4,a4,s4
   121a2:	e31c                	sd	a5,0(a4)
   121a4:	4f98                	lw	a4,24(a5)
   121a6:	2701                	sext.w	a4,a4
   121a8:	c33d                	beqz	a4,1220e <alloc_slot+0x2c4>
   121aa:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   121ae:	9002                	ebreak
   121b0:	7394                	ld	a3,32(a5)
   121b2:	0027051b          	addiw	a0,a4,2
   121b6:	0056d613          	srli	a2,a3,0x5
   121ba:	07e67813          	andi	a6,a2,126
   121be:	6665                	lui	a2,0x19
   121c0:	6f060613          	addi	a2,a2,1776 # 196f0 <__malloc_size_classes>
   121c4:	9642                	add	a2,a2,a6
   121c6:	00065603          	lhu	a2,0(a2)
   121ca:	6805                	lui	a6,0x1
   121cc:	0046131b          	slliw	t1,a2,0x4
   121d0:	0265073b          	mulw	a4,a0,t1
   121d4:	2741                	addiw	a4,a4,16
   121d6:	00e308bb          	addw	a7,t1,a4
   121da:	fff8861b          	addiw	a2,a7,-1
   121de:	8f31                	xor	a4,a4,a2
   121e0:	2701                	sext.w	a4,a4
   121e2:	03074363          	blt	a4,a6,12208 <alloc_slot+0x2be>
   121e6:	01f6f713          	andi	a4,a3,31
   121ea:	0017069b          	addiw	a3,a4,1
   121ee:	8736                	mv	a4,a3
   121f0:	00d55363          	bge	a0,a3,121f6 <alloc_slot+0x2ac>
   121f4:	872a                	mv	a4,a0
   121f6:	0085c683          	lbu	a3,8(a1)
   121fa:	377d                	addiw	a4,a4,-1
   121fc:	8b7d                	andi	a4,a4,31
   121fe:	9a81                	andi	a3,a3,-32
   12200:	8f55                	or	a4,a4,a3
   12202:	00e58423          	sb	a4,8(a1)
   12206:	bf79                	j	121a4 <alloc_slot+0x25a>
   12208:	2505                	addiw	a0,a0,1
   1220a:	8746                	mv	a4,a7
   1220c:	b7e9                	j	121d6 <alloc_slot+0x28c>
   1220e:	6b98                	ld	a4,16(a5)
   12210:	01c78813          	addi	a6,a5,28
   12214:	6714                	ld	a3,8(a4)
   12216:	4709                	li	a4,2
   12218:	00d7173b          	sllw	a4,a4,a3
   1221c:	fff7069b          	addiw	a3,a4,-1
   12220:	40e0063b          	negw	a2,a4
   12224:	4fd8                	lw	a4,28(a5)
   12226:	0007051b          	sext.w	a0,a4
   1222a:	00c57733          	and	a4,a0,a2
   1222e:	88aa                	mv	a7,a0
   12230:	2701                	sext.w	a4,a4
   12232:	160825af          	lr.w.aqrl	a1,(a6)
   12236:	00a59663          	bne	a1,a0,12242 <alloc_slot+0x2f8>
   1223a:	1ee8232f          	sc.w.aqrl	t1,a4,(a6)
   1223e:	fe031ae3          	bnez	t1,12232 <alloc_slot+0x2e8>
   12242:	2581                	sext.w	a1,a1
   12244:	feb510e3          	bne	a0,a1,12224 <alloc_slot+0x2da>
   12248:	0116f733          	and	a4,a3,a7
   1224c:	0007069b          	sext.w	a3,a4
   12250:	cf94                	sw	a3,24(a5)
   12252:	dea1                	beqz	a3,121aa <alloc_slot+0x260>
   12254:	7398                	ld	a4,32(a5)
   12256:	467d                	li	a2,31
   12258:	8319                	srli	a4,a4,0x6
   1225a:	03f77713          	andi	a4,a4,63
   1225e:	3765                	addiw	a4,a4,-7
   12260:	00e66963          	bltu	a2,a4,12272 <alloc_slot+0x328>
   12264:	9752                	add	a4,a4,s4
   12266:	37074603          	lbu	a2,880(a4)
   1226a:	c601                	beqz	a2,12272 <alloc_slot+0x328>
   1226c:	367d                	addiw	a2,a2,-1
   1226e:	36c70823          	sb	a2,880(a4)
   12272:	40d0063b          	negw	a2,a3
   12276:	8e75                	and	a2,a2,a3
   12278:	9e91                	subw	a3,a3,a2
   1227a:	0006071b          	sext.w	a4,a2
   1227e:	cf94                	sw	a3,24(a5)
   12280:	d2070ae3          	beqz	a4,11fb4 <alloc_slot+0x6a>
   12284:	40e007bb          	negw	a5,a4
   12288:	8f7d                	and	a4,a4,a5
   1228a:	076be7b7          	lui	a5,0x76be
   1228e:	6297879b          	addiw	a5,a5,1577
   12292:	02f7073b          	mulw	a4,a4,a5
   12296:	67e5                	lui	a5,0x19
   12298:	75078793          	addi	a5,a5,1872 # 19750 <debruijn32.2704>
   1229c:	01b7571b          	srliw	a4,a4,0x1b
   122a0:	973e                	add	a4,a4,a5
   122a2:	00074503          	lbu	a0,0(a4)
   122a6:	70e6                	ld	ra,120(sp)
   122a8:	7446                	ld	s0,112(sp)
   122aa:	74a6                	ld	s1,104(sp)
   122ac:	7906                	ld	s2,96(sp)
   122ae:	69e6                	ld	s3,88(sp)
   122b0:	6a46                	ld	s4,80(sp)
   122b2:	6aa6                	ld	s5,72(sp)
   122b4:	6b06                	ld	s6,64(sp)
   122b6:	7be2                	ld	s7,56(sp)
   122b8:	7c42                	ld	s8,48(sp)
   122ba:	7ca2                	ld	s9,40(sp)
   122bc:	7d02                	ld	s10,32(sp)
   122be:	6de2                	ld	s11,24(sp)
   122c0:	6109                	addi	sp,sp,128
   122c2:	8082                	ret
   122c4:	6769                	lui	a4,0x1a
   122c6:	00397793          	andi	a5,s2,3
   122ca:	21070713          	addi	a4,a4,528 # 1a210 <med_cnt_tab>
   122ce:	97ba                	add	a5,a5,a4
   122d0:	0007c403          	lbu	s0,0(a5)
   122d4:	00147793          	andi	a5,s0,1
   122d8:	e789                	bnez	a5,122e2 <alloc_slot+0x398>
   122da:	0024179b          	slliw	a5,s0,0x2
   122de:	00fd6563          	bltu	s10,a5,122e8 <alloc_slot+0x39e>
   122e2:	001007b7          	lui	a5,0x100
   122e6:	a021                	j	122ee <alloc_slot+0x3a4>
   122e8:	8405                	srai	s0,s0,0x1
   122ea:	b7ed                	j	122d4 <alloc_slot+0x38a>
   122ec:	8405                	srai	s0,s0,0x1
   122ee:	03840733          	mul	a4,s0,s8
   122f2:	fef77de3          	bgeu	a4,a5,122ec <alloc_slot+0x3a2>
   122f6:	bb1d                	j	1202c <alloc_slot+0xe2>
   122f8:	f9600713          	li	a4,-106
   122fc:	b361                	j	12084 <alloc_slot+0x13a>
   122fe:	4709                	li	a4,2
   12300:	00e79863          	bne	a5,a4,12310 <alloc_slot+0x3c6>
   12304:	002c9793          	slli	a5,s9,0x2
   12308:	db57ffe3          	bgeu	a5,s5,120c6 <alloc_slot+0x17c>
   1230c:	440d                	li	s0,3
   1230e:	bb65                	j	120c6 <alloc_slot+0x17c>
   12310:	da079be3          	bnez	a5,120c6 <alloc_slot+0x17c>
   12314:	003c9793          	slli	a5,s9,0x3
   12318:	ff57eae3          	bltu	a5,s5,1230c <alloc_slot+0x3c2>
   1231c:	001c9793          	slli	a5,s9,0x1
   12320:	db57f3e3          	bgeu	a5,s5,120c6 <alloc_slot+0x17c>
   12324:	4415                	li	s0,5
   12326:	b345                	j	120c6 <alloc_slot+0x17c>
   12328:	4409                	li	s0,2
   1232a:	bb71                	j	120c6 <alloc_slot+0x17c>
   1232c:	6785                	lui	a5,0x1
   1232e:	fff78713          	addi	a4,a5,-1 # fff <exit-0xf121>
   12332:	17c1                	addi	a5,a5,-16
   12334:	0387d7b3          	divu	a5,a5,s8
   12338:	708c                	ld	a1,32(s1)
   1233a:	00cada93          	srli	s5,s5,0xc
   1233e:	0ab2                	slli	s5,s5,0xc
   12340:	8df9                	and	a1,a1,a4
   12342:	0155eab3          	or	s5,a1,s5
   12346:	0354b023          	sd	s5,32(s1)
   1234a:	00ca2703          	lw	a4,12(s4)
   1234e:	2705                	addiw	a4,a4,1
   12350:	00ea2623          	sw	a4,12(s4)
   12354:	37fd                	addiw	a5,a5,-1
   12356:	0087c463          	blt	a5,s0,1235e <alloc_slot+0x414>
   1235a:	fff4079b          	addiw	a5,s0,-1
   1235e:	873e                	mv	a4,a5
   12360:	0007d363          	bgez	a5,12366 <alloc_slot+0x41c>
   12364:	4701                	li	a4,0
   12366:	0007079b          	sext.w	a5,a4
   1236a:	0b8e                	slli	s7,s7,0x3
   1236c:	9bd2                	add	s7,s7,s4
   1236e:	000bb703          	ld	a4,0(s7)
   12372:	4689                	li	a3,2
   12374:	03f97913          	andi	s2,s2,63
   12378:	9722                	add	a4,a4,s0
   1237a:	00ebb023          	sd	a4,0(s7)
   1237e:	00f6973b          	sllw	a4,a3,a5
   12382:	377d                	addiw	a4,a4,-1
   12384:	cc98                	sw	a4,24(s1)
   12386:	4c98                	lw	a4,24(s1)
   12388:	347d                	addiw	s0,s0,-1
   1238a:	008696bb          	sllw	a3,a3,s0
   1238e:	fff74713          	not	a4,a4
   12392:	9f35                	addw	a4,a4,a3
   12394:	ccd8                	sw	a4,28(s1)
   12396:	00854703          	lbu	a4,8(a0)
   1239a:	887d                	andi	s0,s0,31
   1239c:	091a                	slli	s2,s2,0x6
   1239e:	02046413          	ori	s0,s0,32
   123a2:	9b01                	andi	a4,a4,-32
   123a4:	00896433          	or	s0,s2,s0
   123a8:	8bfd                	andi	a5,a5,31
   123aa:	0204d903          	lhu	s2,32(s1)
   123ae:	e888                	sd	a0,16(s1)
   123b0:	8fd9                	or	a5,a5,a4
   123b2:	00f50423          	sb	a5,8(a0)
   123b6:	77fd                	lui	a5,0xfffff
   123b8:	00f97933          	and	s2,s2,a5
   123bc:	4c9c                	lw	a5,24(s1)
   123be:	e104                	sd	s1,0(a0)
   123c0:	01246433          	or	s0,s0,s2
   123c4:	37fd                	addiw	a5,a5,-1
   123c6:	00399513          	slli	a0,s3,0x3
   123ca:	9552                	add	a0,a0,s4
   123cc:	02849023          	sh	s0,32(s1)
   123d0:	cc9c                	sw	a5,24(s1)
   123d2:	85a6                	mv	a1,s1
   123d4:	f48ff0ef          	jal	ra,11b1c <queue>
   123d8:	4501                	li	a0,0
   123da:	b5f1                	j	122a6 <alloc_slot+0x35c>
   123dc:	00ca8593          	addi	a1,s5,12
   123e0:	852e                	mv	a0,a1
   123e2:	e42e                	sd	a1,8(sp)
   123e4:	8d1ff0ef          	jal	ra,11cb4 <size_to_class>
   123e8:	65a2                	ld	a1,8(sp)
   123ea:	8aaa                	mv	s5,a0
   123ec:	b5fff0ef          	jal	ra,11f4a <alloc_slot>
   123f0:	85aa                	mv	a1,a0
   123f2:	d2054de3          	bltz	a0,1212c <alloc_slot+0x1e2>
   123f6:	001a9793          	slli	a5,s5,0x1
   123fa:	9dbe                	add	s11,s11,a5
   123fc:	0aa9                	addi	s5,s5,10
   123fe:	000dd603          	lhu	a2,0(s11)
   12402:	0a8e                	slli	s5,s5,0x3
   12404:	9ad2                	add	s5,s5,s4
   12406:	00ca2683          	lw	a3,12(s4)
   1240a:	000ab503          	ld	a0,0(s5)
   1240e:	0046161b          	slliw	a2,a2,0x4
   12412:	3671                	addiw	a2,a2,-4
   12414:	f5eff0ef          	jal	ra,11b72 <enframe>
   12418:	709c                	ld	a5,32(s1)
   1241a:	00c50713          	addi	a4,a0,12
   1241e:	17d2                	slli	a5,a5,0x34
   12420:	93d1                	srli	a5,a5,0x34
   12422:	f09c                	sd	a5,32(s1)
   12424:	ffd54783          	lbu	a5,-3(a0)
   12428:	8bfd                	andi	a5,a5,31
   1242a:	fc07e793          	ori	a5,a5,-64
   1242e:	fef50ea3          	sb	a5,-3(a0)
   12432:	4781                	li	a5,0
   12434:	00070023          	sb	zero,0(a4)
   12438:	2785                	addiw	a5,a5,1
   1243a:	9762                	add	a4,a4,s8
   1243c:	fef45ce3          	bge	s0,a5,12434 <alloc_slot+0x4ea>
   12440:	fff4079b          	addiw	a5,s0,-1
   12444:	b71d                	j	1236a <alloc_slot+0x420>

0000000000012446 <__libc_malloc_impl>:
   12446:	77f9                	lui	a5,0xffffe
   12448:	7139                	addi	sp,sp,-64
   1244a:	17f5                	addi	a5,a5,-3
   1244c:	fc06                	sd	ra,56(sp)
   1244e:	f822                	sd	s0,48(sp)
   12450:	f426                	sd	s1,40(sp)
   12452:	f04a                	sd	s2,32(sp)
   12454:	ec4e                	sd	s3,24(sp)
   12456:	8385                	srli	a5,a5,0x1
   12458:	0ca7e763          	bltu	a5,a0,12526 <__libc_malloc_impl+0xe0>
   1245c:	000207b7          	lui	a5,0x20
   12460:	17ad                	addi	a5,a5,-21
   12462:	892a                	mv	s2,a0
   12464:	0ca7ed63          	bltu	a5,a0,1253e <__libc_malloc_impl+0xf8>
   12468:	84dff0ef          	jal	ra,11cb4 <size_to_class>
   1246c:	84aa                	mv	s1,a0
   1246e:	ff4ff0ef          	jal	ra,11c62 <rdlock>
   12472:	00a48713          	addi	a4,s1,10
   12476:	bd818793          	addi	a5,gp,-1064 # 1c3d8 <__malloc_context>
   1247a:	070e                	slli	a4,a4,0x3
   1247c:	97ba                	add	a5,a5,a4
   1247e:	6380                	ld	s0,0(a5)
   12480:	bd818993          	addi	s3,gp,-1064 # 1c3d8 <__malloc_context>
   12484:	e42d                	bnez	s0,124ee <__libc_malloc_impl+0xa8>
   12486:	ffc4879b          	addiw	a5,s1,-4
   1248a:	46ed                	li	a3,27
   1248c:	4701                	li	a4,0
   1248e:	06f6e263          	bltu	a3,a5,124f2 <__libc_malloc_impl+0xac>
   12492:	4799                	li	a5,6
   12494:	8722                	mv	a4,s0
   12496:	04f48e63          	beq	s1,a5,124f2 <__libc_malloc_impl+0xac>
   1249a:	0014f693          	andi	a3,s1,1
   1249e:	eab1                	bnez	a3,124f2 <__libc_malloc_impl+0xac>
   124a0:	03a48793          	addi	a5,s1,58
   124a4:	078e                	slli	a5,a5,0x3
   124a6:	97ce                	add	a5,a5,s3
   124a8:	639c                	ld	a5,0(a5)
   124aa:	8736                	mv	a4,a3
   124ac:	e3b9                	bnez	a5,124f2 <__libc_malloc_impl+0xac>
   124ae:	0014e793          	ori	a5,s1,1
   124b2:	2781                	sext.w	a5,a5
   124b4:	03a78713          	addi	a4,a5,58 # 2003a <__BSS_END__+0x361a>
   124b8:	070e                	slli	a4,a4,0x3
   124ba:	974e                	add	a4,a4,s3
   124bc:	6314                	ld	a3,0(a4)
   124be:	00a78713          	addi	a4,a5,10
   124c2:	070e                	slli	a4,a4,0x3
   124c4:	974e                	add	a4,a4,s3
   124c6:	6318                	ld	a4,0(a4)
   124c8:	c719                	beqz	a4,124d6 <__libc_malloc_impl+0x90>
   124ca:	4f10                	lw	a2,24(a4)
   124cc:	2601                	sext.w	a2,a2
   124ce:	e609                	bnez	a2,124d8 <__libc_malloc_impl+0x92>
   124d0:	4f58                	lw	a4,28(a4)
   124d2:	2701                	sext.w	a4,a4
   124d4:	e311                	bnez	a4,124d8 <__libc_malloc_impl+0x92>
   124d6:	068d                	addi	a3,a3,3
   124d8:	4731                	li	a4,12
   124da:	00d76363          	bltu	a4,a3,124e0 <__libc_malloc_impl+0x9a>
   124de:	84be                	mv	s1,a5
   124e0:	00a48793          	addi	a5,s1,10
   124e4:	078e                	slli	a5,a5,0x3
   124e6:	97ce                	add	a5,a5,s3
   124e8:	6380                	ld	s0,0(a5)
   124ea:	4701                	li	a4,0
   124ec:	c019                	beqz	s0,124f2 <__libc_malloc_impl+0xac>
   124ee:	4c18                	lw	a4,24(s0)
   124f0:	2701                	sext.w	a4,a4
   124f2:	40e007bb          	negw	a5,a4
   124f6:	8ff9                	and	a5,a5,a4
   124f8:	2781                	sext.w	a5,a5
   124fa:	0c078b63          	beqz	a5,125d0 <__libc_malloc_impl+0x18a>
   124fe:	9f1d                	subw	a4,a4,a5
   12500:	cc18                	sw	a4,24(s0)
   12502:	40f0073b          	negw	a4,a5
   12506:	8ff9                	and	a5,a5,a4
   12508:	076be737          	lui	a4,0x76be
   1250c:	6297071b          	addiw	a4,a4,1577
   12510:	02e787bb          	mulw	a5,a5,a4
   12514:	6765                	lui	a4,0x19
   12516:	75070713          	addi	a4,a4,1872 # 19750 <debruijn32.2704>
   1251a:	01b7d79b          	srliw	a5,a5,0x1b
   1251e:	97ba                	add	a5,a5,a4
   12520:	0007c583          	lbu	a1,0(a5)
   12524:	a051                	j	125a8 <__libc_malloc_impl+0x162>
   12526:	a47fe0ef          	jal	ra,10f6c <__errno_location>
   1252a:	47b1                	li	a5,12
   1252c:	c11c                	sw	a5,0(a0)
   1252e:	70e2                	ld	ra,56(sp)
   12530:	7442                	ld	s0,48(sp)
   12532:	74a2                	ld	s1,40(sp)
   12534:	7902                	ld	s2,32(sp)
   12536:	69e2                	ld	s3,24(sp)
   12538:	4501                	li	a0,0
   1253a:	6121                	addi	sp,sp,64
   1253c:	8082                	ret
   1253e:	01450993          	addi	s3,a0,20
   12542:	4781                	li	a5,0
   12544:	577d                	li	a4,-1
   12546:	02200693          	li	a3,34
   1254a:	460d                	li	a2,3
   1254c:	85ce                	mv	a1,s3
   1254e:	4501                	li	a0,0
   12550:	5a6000ef          	jal	ra,12af6 <__mmap>
   12554:	57fd                	li	a5,-1
   12556:	84aa                	mv	s1,a0
   12558:	fcf50be3          	beq	a0,a5,1252e <__libc_malloc_impl+0xe8>
   1255c:	f06ff0ef          	jal	ra,11c62 <rdlock>
   12560:	f1eff0ef          	jal	ra,11c7e <step_seq>
   12564:	807ff0ef          	jal	ra,11d6a <__malloc_alloc_meta>
   12568:	842a                	mv	s0,a0
   1256a:	e911                	bnez	a0,1257e <__libc_malloc_impl+0x138>
   1256c:	1e818513          	addi	a0,gp,488 # 1c9e8 <__malloc_lock>
   12570:	60e010ef          	jal	ra,13b7e <__unlock>
   12574:	85ce                	mv	a1,s3
   12576:	8526                	mv	a0,s1
   12578:	62e000ef          	jal	ra,12ba6 <__munmap>
   1257c:	bf4d                	j	1252e <__libc_malloc_impl+0xe8>
   1257e:	6705                	lui	a4,0x1
   12580:	01370793          	addi	a5,a4,19 # 1013 <exit-0xf10d>
   12584:	97ca                	add	a5,a5,s2
   12586:	83b1                	srli	a5,a5,0xc
   12588:	1701                	addi	a4,a4,-32
   1258a:	e904                	sd	s1,16(a0)
   1258c:	07b2                	slli	a5,a5,0xc
   1258e:	e088                	sd	a0,0(s1)
   12590:	8fd9                	or	a5,a5,a4
   12592:	f11c                	sd	a5,32(a0)
   12594:	bd818793          	addi	a5,gp,-1064 # 1c3d8 <__malloc_context>
   12598:	47d8                	lw	a4,12(a5)
   1259a:	00052e23          	sw	zero,28(a0)
   1259e:	00052c23          	sw	zero,24(a0)
   125a2:	2705                	addiw	a4,a4,1
   125a4:	c7d8                	sw	a4,12(a5)
   125a6:	4581                	li	a1,0
   125a8:	be41a683          	lw	a3,-1052(gp) # 1c3e4 <__malloc_context+0xc>
   125ac:	1e818513          	addi	a0,gp,488 # 1c9e8 <__malloc_lock>
   125b0:	e42e                	sd	a1,8(sp)
   125b2:	e036                	sd	a3,0(sp)
   125b4:	5ca010ef          	jal	ra,13b7e <__unlock>
   125b8:	8522                	mv	a0,s0
   125ba:	7442                	ld	s0,48(sp)
   125bc:	6682                	ld	a3,0(sp)
   125be:	65a2                	ld	a1,8(sp)
   125c0:	70e2                	ld	ra,56(sp)
   125c2:	74a2                	ld	s1,40(sp)
   125c4:	69e2                	ld	s3,24(sp)
   125c6:	864a                	mv	a2,s2
   125c8:	7902                	ld	s2,32(sp)
   125ca:	6121                	addi	sp,sp,64
   125cc:	da6ff06f          	j	11b72 <enframe>
   125d0:	85ca                	mv	a1,s2
   125d2:	8526                	mv	a0,s1
   125d4:	977ff0ef          	jal	ra,11f4a <alloc_slot>
   125d8:	85aa                	mv	a1,a0
   125da:	00055763          	bgez	a0,125e8 <__libc_malloc_impl+0x1a2>
   125de:	1e818513          	addi	a0,gp,488 # 1c9e8 <__malloc_lock>
   125e2:	59c010ef          	jal	ra,13b7e <__unlock>
   125e6:	b7a1                	j	1252e <__libc_malloc_impl+0xe8>
   125e8:	04a9                	addi	s1,s1,10
   125ea:	048e                	slli	s1,s1,0x3
   125ec:	94ce                	add	s1,s1,s3
   125ee:	6080                	ld	s0,0(s1)
   125f0:	bf65                	j	125a8 <__libc_malloc_impl+0x162>

00000000000125f2 <__malloc_allzerop>:
   125f2:	00f57793          	andi	a5,a0,15
   125f6:	c781                	beqz	a5,125fe <__malloc_allzerop+0xc>
   125f8:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   125fc:	9002                	ebreak
   125fe:	ffd54603          	lbu	a2,-3(a0)
   12602:	ffc54783          	lbu	a5,-4(a0)
   12606:	ffe55703          	lhu	a4,-2(a0)
   1260a:	01f67813          	andi	a6,a2,31
   1260e:	8a7d                	andi	a2,a2,31
   12610:	cf89                	beqz	a5,1262a <__malloc_allzerop+0x38>
   12612:	c701                	beqz	a4,1261a <__malloc_allzerop+0x28>
   12614:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   12618:	9002                	ebreak
   1261a:	ff852703          	lw	a4,-8(a0)
   1261e:	67c1                	lui	a5,0x10
   12620:	00f75563          	bge	a4,a5,1262a <__malloc_allzerop+0x38>
   12624:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   12628:	9002                	ebreak
   1262a:	0047179b          	slliw	a5,a4,0x4
   1262e:	8d1d                	sub	a0,a0,a5
   12630:	ff053683          	ld	a3,-16(a0)
   12634:	ff050793          	addi	a5,a0,-16
   12638:	6a8c                	ld	a1,16(a3)
   1263a:	00b78563          	beq	a5,a1,12644 <__malloc_allzerop+0x52>
   1263e:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   12642:	9002                	ebreak
   12644:	729c                	ld	a5,32(a3)
   12646:	01f7f593          	andi	a1,a5,31
   1264a:	00c5d563          	bge	a1,a2,12654 <__malloc_allzerop+0x62>
   1264e:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   12652:	9002                	ebreak
   12654:	4e8c                	lw	a1,24(a3)
   12656:	00c5d5bb          	srlw	a1,a1,a2
   1265a:	8985                	andi	a1,a1,1
   1265c:	c581                	beqz	a1,12664 <__malloc_allzerop+0x72>
   1265e:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   12662:	9002                	ebreak
   12664:	4ecc                	lw	a1,28(a3)
   12666:	00c5d63b          	srlw	a2,a1,a2
   1266a:	8a05                	andi	a2,a2,1
   1266c:	c601                	beqz	a2,12674 <__malloc_allzerop+0x82>
   1266e:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   12672:	9002                	ebreak
   12674:	767d                	lui	a2,0xfffff
   12676:	8e75                	and	a2,a2,a3
   12678:	620c                	ld	a1,0(a2)
   1267a:	bd81b603          	ld	a2,-1064(gp) # 1c3d8 <__malloc_context>
   1267e:	00c58563          	beq	a1,a2,12688 <__malloc_allzerop+0x96>
   12682:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   12686:	9002                	ebreak
   12688:	1141                	addi	sp,sp,-16
   1268a:	e022                	sd	s0,0(sp)
   1268c:	0067d413          	srli	s0,a5,0x6
   12690:	e406                	sd	ra,8(sp)
   12692:	03f47413          	andi	s0,s0,63
   12696:	02f00613          	li	a2,47
   1269a:	02866763          	bltu	a2,s0,126c8 <__malloc_allzerop+0xd6>
   1269e:	65e5                	lui	a1,0x19
   126a0:	00141613          	slli	a2,s0,0x1
   126a4:	6f058593          	addi	a1,a1,1776 # 196f0 <__malloc_size_classes>
   126a8:	962e                	add	a2,a2,a1
   126aa:	00065603          	lhu	a2,0(a2) # fffffffffffff000 <__BSS_END__+0xfffffffffffe25e0>
   126ae:	02c8053b          	mulw	a0,a6,a2
   126b2:	00a75563          	bge	a4,a0,126bc <__malloc_allzerop+0xca>
   126b6:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   126ba:	9002                	ebreak
   126bc:	9e29                	addw	a2,a2,a0
   126be:	00c74f63          	blt	a4,a2,126dc <__malloc_allzerop+0xea>
   126c2:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   126c6:	9002                	ebreak
   126c8:	6605                	lui	a2,0x1
   126ca:	fc060613          	addi	a2,a2,-64 # fc0 <exit-0xf160>
   126ce:	00c7f5b3          	and	a1,a5,a2
   126d2:	00c58563          	beq	a1,a2,126dc <__malloc_allzerop+0xea>
   126d6:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   126da:	9002                	ebreak
   126dc:	767d                	lui	a2,0xfffff
   126de:	8e7d                	and	a2,a2,a5
   126e0:	ca09                	beqz	a2,126f2 <__malloc_allzerop+0x100>
   126e2:	83b1                	srli	a5,a5,0xc
   126e4:	07a2                	slli	a5,a5,0x8
   126e6:	17fd                	addi	a5,a5,-1
   126e8:	00e7f563          	bgeu	a5,a4,126f2 <__malloc_allzerop+0x100>
   126ec:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   126f0:	9002                	ebreak
   126f2:	02f00793          	li	a5,47
   126f6:	4505                	li	a0,1
   126f8:	0287e063          	bltu	a5,s0,12718 <__malloc_allzerop+0x126>
   126fc:	8536                	mv	a0,a3
   126fe:	c46ff0ef          	jal	ra,11b44 <get_stride>
   12702:	67e5                	lui	a5,0x19
   12704:	6f078793          	addi	a5,a5,1776 # 196f0 <__malloc_size_classes>
   12708:	0406                	slli	s0,s0,0x1
   1270a:	943e                	add	s0,s0,a5
   1270c:	00045783          	lhu	a5,0(s0)
   12710:	0047979b          	slliw	a5,a5,0x4
   12714:	00f53533          	sltu	a0,a0,a5
   12718:	60a2                	ld	ra,8(sp)
   1271a:	6402                	ld	s0,0(sp)
   1271c:	0141                	addi	sp,sp,16
   1271e:	8082                	ret

0000000000012720 <copysign>:
   12720:	22b50553          	fsgnj.d	fa0,fa0,fa1
   12724:	8082                	ret

0000000000012726 <copysignf>:
   12726:	20b50553          	fsgnj.s	fa0,fa0,fa1
   1272a:	8082                	ret

000000000001272c <scalbn>:
   1272c:	3ff00713          	li	a4,1023
   12730:	02a75b63          	bge	a4,a0,12766 <scalbn+0x3a>
   12734:	9081b787          	fld	fa5,-1784(gp) # 1c108 <__SDATA_BEGIN__>
   12738:	c015079b          	addiw	a5,a0,-1023
   1273c:	12f57553          	fmul.d	fa0,fa0,fa5
   12740:	00f75b63          	bge	a4,a5,12756 <scalbn+0x2a>
   12744:	8025069b          	addiw	a3,a0,-2046
   12748:	12f57553          	fmul.d	fa0,fa0,fa5
   1274c:	87b6                	mv	a5,a3
   1274e:	00d75463          	bge	a4,a3,12756 <scalbn+0x2a>
   12752:	3ff00793          	li	a5,1023
   12756:	3ff7879b          	addiw	a5,a5,1023
   1275a:	17d2                	slli	a5,a5,0x34
   1275c:	f20787d3          	fmv.d.x	fa5,a5
   12760:	12f57553          	fmul.d	fa0,fa0,fa5
   12764:	8082                	ret
   12766:	c0200713          	li	a4,-1022
   1276a:	87aa                	mv	a5,a0
   1276c:	fee555e3          	bge	a0,a4,12756 <scalbn+0x2a>
   12770:	9201b787          	fld	fa5,-1760(gp) # 1c120 <__SDATA_BEGIN__+0x18>
   12774:	3c95079b          	addiw	a5,a0,969
   12778:	12f57553          	fmul.d	fa0,fa0,fa5
   1277c:	fce7dde3          	bge	a5,a4,12756 <scalbn+0x2a>
   12780:	7925069b          	addiw	a3,a0,1938
   12784:	12f57553          	fmul.d	fa0,fa0,fa5
   12788:	87b6                	mv	a5,a3
   1278a:	fce6d6e3          	bge	a3,a4,12756 <scalbn+0x2a>
   1278e:	c0200793          	li	a5,-1022
   12792:	b7d1                	j	12756 <scalbn+0x2a>

0000000000012794 <__getopt_msg>:
   12794:	7139                	addi	sp,sp,-64
   12796:	e456                	sd	s5,8(sp)
   12798:	8aaa                	mv	s5,a0
   1279a:	852e                	mv	a0,a1
   1279c:	f822                	sd	s0,48(sp)
   1279e:	f426                	sd	s1,40(sp)
   127a0:	f04a                	sd	s2,32(sp)
   127a2:	ec4e                	sd	s3,24(sp)
   127a4:	e852                	sd	s4,16(sp)
   127a6:	fc06                	sd	ra,56(sp)
   127a8:	8a32                	mv	s4,a2
   127aa:	8936                	mv	s2,a3
   127ac:	6471                	lui	s0,0x1c
   127ae:	8ddfe0ef          	jal	ra,1108a <__lctrans_cur>
   127b2:	00040793          	mv	a5,s0
   127b6:	08c7a783          	lw	a5,140(a5)
   127ba:	89aa                	mv	s3,a0
   127bc:	4481                	li	s1,0
   127be:	02079713          	slli	a4,a5,0x20
   127c2:	00074763          	bltz	a4,127d0 <__getopt_msg+0x3c>
   127c6:	00040513          	mv	a0,s0
   127ca:	784000ef          	jal	ra,12f4e <__lockfile>
   127ce:	84aa                	mv	s1,a0
   127d0:	00040593          	mv	a1,s0
   127d4:	8556                	mv	a0,s5
   127d6:	05f000ef          	jal	ra,13034 <fputs>
   127da:	02054b63          	bltz	a0,12810 <__getopt_msg+0x7c>
   127de:	854e                	mv	a0,s3
   127e0:	20e010ef          	jal	ra,139ee <strlen>
   127e4:	85aa                	mv	a1,a0
   127e6:	00040693          	mv	a3,s0
   127ea:	4605                	li	a2,1
   127ec:	854e                	mv	a0,s3
   127ee:	117000ef          	jal	ra,13104 <fwrite>
   127f2:	cd19                	beqz	a0,12810 <__getopt_msg+0x7c>
   127f4:	00040693          	mv	a3,s0
   127f8:	864a                	mv	a2,s2
   127fa:	4585                	li	a1,1
   127fc:	8552                	mv	a0,s4
   127fe:	107000ef          	jal	ra,13104 <fwrite>
   12802:	01251763          	bne	a0,s2,12810 <__getopt_msg+0x7c>
   12806:	00040593          	mv	a1,s0
   1280a:	4529                	li	a0,10
   1280c:	211000ef          	jal	ra,1321c <putc>
   12810:	cc89                	beqz	s1,1282a <__getopt_msg+0x96>
   12812:	00040513          	mv	a0,s0
   12816:	7442                	ld	s0,48(sp)
   12818:	70e2                	ld	ra,56(sp)
   1281a:	74a2                	ld	s1,40(sp)
   1281c:	7902                	ld	s2,32(sp)
   1281e:	69e2                	ld	s3,24(sp)
   12820:	6a42                	ld	s4,16(sp)
   12822:	6aa2                	ld	s5,8(sp)
   12824:	6121                	addi	sp,sp,64
   12826:	7c40006f          	j	12fea <__unlockfile>
   1282a:	70e2                	ld	ra,56(sp)
   1282c:	7442                	ld	s0,48(sp)
   1282e:	74a2                	ld	s1,40(sp)
   12830:	7902                	ld	s2,32(sp)
   12832:	69e2                	ld	s3,24(sp)
   12834:	6a42                	ld	s4,16(sp)
   12836:	6aa2                	ld	s5,8(sp)
   12838:	6121                	addi	sp,sp,64
   1283a:	8082                	ret

000000000001283c <getopt>:
   1283c:	711d                	addi	sp,sp,-96
   1283e:	e8a2                	sd	s0,80(sp)
   12840:	9641a703          	lw	a4,-1692(gp) # 1c164 <optind>
   12844:	e4a6                	sd	s1,72(sp)
   12846:	f852                	sd	s4,48(sp)
   12848:	f456                	sd	s5,40(sp)
   1284a:	ec86                	sd	ra,88(sp)
   1284c:	e0ca                	sd	s2,64(sp)
   1284e:	fc4e                	sd	s3,56(sp)
   12850:	f05a                	sd	s6,32(sp)
   12852:	8aaa                	mv	s5,a0
   12854:	8a2e                	mv	s4,a1
   12856:	84b2                	mv	s1,a2
   12858:	96418413          	addi	s0,gp,-1692 # 1c164 <optind>
   1285c:	9b018793          	addi	a5,gp,-1616 # 1c1b0 <__optreset>
   12860:	c319                	beqz	a4,12866 <getopt+0x2a>
   12862:	4398                	lw	a4,0(a5)
   12864:	c719                	beqz	a4,12872 <getopt+0x36>
   12866:	0007a023          	sw	zero,0(a5)
   1286a:	2001a023          	sw	zero,512(gp) # 1ca00 <__optpos>
   1286e:	4785                	li	a5,1
   12870:	c01c                	sw	a5,0(s0)
   12872:	401c                	lw	a5,0(s0)
   12874:	557d                	li	a0,-1
   12876:	0357d663          	bge	a5,s5,128a2 <getopt+0x66>
   1287a:	00379713          	slli	a4,a5,0x3
   1287e:	9752                	add	a4,a4,s4
   12880:	6318                	ld	a4,0(a4)
   12882:	c305                	beqz	a4,128a2 <getopt+0x66>
   12884:	00074683          	lbu	a3,0(a4)
   12888:	02d00613          	li	a2,45
   1288c:	02c68563          	beq	a3,a2,128b6 <getopt+0x7a>
   12890:	0004c683          	lbu	a3,0(s1)
   12894:	00c69763          	bne	a3,a2,128a2 <getopt+0x66>
   12898:	2785                	addiw	a5,a5,1
   1289a:	c01c                	sw	a5,0(s0)
   1289c:	1ee1bc23          	sd	a4,504(gp) # 1c9f8 <optarg>
   128a0:	4505                	li	a0,1
   128a2:	60e6                	ld	ra,88(sp)
   128a4:	6446                	ld	s0,80(sp)
   128a6:	64a6                	ld	s1,72(sp)
   128a8:	6906                	ld	s2,64(sp)
   128aa:	79e2                	ld	s3,56(sp)
   128ac:	7a42                	ld	s4,48(sp)
   128ae:	7aa2                	ld	s5,40(sp)
   128b0:	7b02                	ld	s6,32(sp)
   128b2:	6125                	addi	sp,sp,96
   128b4:	8082                	ret
   128b6:	00174603          	lbu	a2,1(a4)
   128ba:	557d                	li	a0,-1
   128bc:	d27d                	beqz	a2,128a2 <getopt+0x66>
   128be:	00d61863          	bne	a2,a3,128ce <getopt+0x92>
   128c2:	00274683          	lbu	a3,2(a4)
   128c6:	e681                	bnez	a3,128ce <getopt+0x92>
   128c8:	2785                	addiw	a5,a5,1
   128ca:	c01c                	sw	a5,0(s0)
   128cc:	bfd9                	j	128a2 <getopt+0x66>
   128ce:	2001a783          	lw	a5,512(gp) # 1ca00 <__optpos>
   128d2:	e781                	bnez	a5,128da <getopt+0x9e>
   128d4:	4785                	li	a5,1
   128d6:	20f1a023          	sw	a5,512(gp) # 1ca00 <__optpos>
   128da:	2001a583          	lw	a1,512(gp) # 1ca00 <__optpos>
   128de:	4611                	li	a2,4
   128e0:	0828                	addi	a0,sp,24
   128e2:	95ba                	add	a1,a1,a4
   128e4:	2e6000ef          	jal	ra,12bca <mbtowc>
   128e8:	86aa                	mv	a3,a0
   128ea:	00055663          	bgez	a0,128f6 <getopt+0xba>
   128ee:	67c1                	lui	a5,0x10
   128f0:	37f5                	addiw	a5,a5,-3
   128f2:	cc3e                	sw	a5,24(sp)
   128f4:	4685                	li	a3,1
   128f6:	4018                	lw	a4,0(s0)
   128f8:	00371793          	slli	a5,a4,0x3
   128fc:	97d2                	add	a5,a5,s4
   128fe:	6390                	ld	a2,0(a5)
   12900:	2001a783          	lw	a5,512(gp) # 1ca00 <__optpos>
   12904:	00d785bb          	addw	a1,a5,a3
   12908:	00f60b33          	add	s6,a2,a5
   1290c:	20b1a023          	sw	a1,512(gp) # 1ca00 <__optpos>
   12910:	962e                	add	a2,a2,a1
   12912:	00064783          	lbu	a5,0(a2) # fffffffffffff000 <__BSS_END__+0xfffffffffffe25e0>
   12916:	e789                	bnez	a5,12920 <getopt+0xe4>
   12918:	2705                	addiw	a4,a4,1
   1291a:	c018                	sw	a4,0(s0)
   1291c:	2001a023          	sw	zero,512(gp) # 1ca00 <__optpos>
   12920:	0004c783          	lbu	a5,0(s1)
   12924:	fd57879b          	addiw	a5,a5,-43
   12928:	0fd7f793          	andi	a5,a5,253
   1292c:	e391                	bnez	a5,12930 <getopt+0xf4>
   1292e:	0485                	addi	s1,s1,1
   12930:	ce02                	sw	zero,28(sp)
   12932:	4981                	li	s3,0
   12934:	4611                	li	a2,4
   12936:	013485b3          	add	a1,s1,s3
   1293a:	0868                	addi	a0,sp,28
   1293c:	e436                	sd	a3,8(sp)
   1293e:	28c000ef          	jal	ra,12bca <mbtowc>
   12942:	47f2                	lw	a5,28(sp)
   12944:	66a2                	ld	a3,8(sp)
   12946:	00a05a63          	blez	a0,1295a <getopt+0x11e>
   1294a:	00a989bb          	addw	s3,s3,a0
   1294e:	4662                	lw	a2,24(sp)
   12950:	0007871b          	sext.w	a4,a5
   12954:	fec710e3          	bne	a4,a2,12934 <getopt+0xf8>
   12958:	a019                	j	1295e <getopt+0x122>
   1295a:	2985                	addiw	s3,s3,1
   1295c:	f96d                	bnez	a0,1294e <getopt+0x112>
   1295e:	4562                	lw	a0,24(sp)
   12960:	2781                	sext.w	a5,a5
   12962:	00a79663          	bne	a5,a0,1296e <getopt+0x132>
   12966:	03a00793          	li	a5,58
   1296a:	02f51963          	bne	a0,a5,1299c <getopt+0x160>
   1296e:	20a1a223          	sw	a0,516(gp) # 1ca04 <optopt>
   12972:	0004c703          	lbu	a4,0(s1)
   12976:	03a00793          	li	a5,58
   1297a:	00f71563          	bne	a4,a5,12984 <getopt+0x148>
   1297e:	03f00513          	li	a0,63
   12982:	b705                	j	128a2 <getopt+0x66>
   12984:	9601a783          	lw	a5,-1696(gp) # 1c160 <opterr>
   12988:	dbfd                	beqz	a5,1297e <getopt+0x142>
   1298a:	65e5                	lui	a1,0x19
   1298c:	865a                	mv	a2,s6
   1298e:	79058593          	addi	a1,a1,1936 # 19790 <small_cnt_tab+0x20>
   12992:	000a3503          	ld	a0,0(s4)
   12996:	dffff0ef          	jal	ra,12794 <__getopt_msg>
   1299a:	b7d5                	j	1297e <getopt+0x142>
   1299c:	99a6                	add	s3,s3,s1
   1299e:	0009c583          	lbu	a1,0(s3)
   129a2:	f0f590e3          	bne	a1,a5,128a2 <getopt+0x66>
   129a6:	1e01bc23          	sd	zero,504(gp) # 1c9f8 <optarg>
   129aa:	0019c803          	lbu	a6,1(s3)
   129ae:	2001a603          	lw	a2,512(gp) # 1ca00 <__optpos>
   129b2:	873e                	mv	a4,a5
   129b4:	00b81363          	bne	a6,a1,129ba <getopt+0x17e>
   129b8:	ce09                	beqz	a2,129d2 <getopt+0x196>
   129ba:	401c                	lw	a5,0(s0)
   129bc:	0017859b          	addiw	a1,a5,1
   129c0:	078e                	slli	a5,a5,0x3
   129c2:	97d2                	add	a5,a5,s4
   129c4:	639c                	ld	a5,0(a5)
   129c6:	c00c                	sw	a1,0(s0)
   129c8:	2001a023          	sw	zero,512(gp) # 1ca00 <__optpos>
   129cc:	97b2                	add	a5,a5,a2
   129ce:	1ef1bc23          	sd	a5,504(gp) # 1c9f8 <optarg>
   129d2:	401c                	lw	a5,0(s0)
   129d4:	ecfad7e3          	bge	s5,a5,128a2 <getopt+0x66>
   129d8:	20a1a223          	sw	a0,516(gp) # 1ca04 <optopt>
   129dc:	0004c703          	lbu	a4,0(s1)
   129e0:	03a00793          	li	a5,58
   129e4:	03a00513          	li	a0,58
   129e8:	eaf70de3          	beq	a4,a5,128a2 <getopt+0x66>
   129ec:	9601a783          	lw	a5,-1696(gp) # 1c160 <opterr>
   129f0:	d7d9                	beqz	a5,1297e <getopt+0x142>
   129f2:	65e5                	lui	a1,0x19
   129f4:	865a                	mv	a2,s6
   129f6:	7a858593          	addi	a1,a1,1960 # 197a8 <small_cnt_tab+0x38>
   129fa:	bf61                	j	12992 <getopt+0x156>

00000000000129fc <getrlimit>:
   129fc:	7179                	addi	sp,sp,-48
   129fe:	ec26                	sd	s1,24(sp)
   12a00:	e84a                	sd	s2,16(sp)
   12a02:	84ae                	mv	s1,a1
   12a04:	892a                	mv	s2,a0
   12a06:	f406                	sd	ra,40(sp)
   12a08:	f022                	sd	s0,32(sp)
   12a0a:	10500893          	li	a7,261
   12a0e:	4501                	li	a0,0
   12a10:	85ca                	mv	a1,s2
   12a12:	4601                	li	a2,0
   12a14:	86a6                	mv	a3,s1
   12a16:	00000073          	ecall
   12a1a:	e2efe0ef          	jal	ra,11048 <__syscall_ret>
   12a1e:	0005041b          	sext.w	s0,a0
   12a22:	e809                	bnez	s0,12a34 <getrlimit+0x38>
   12a24:	4401                	li	s0,0
   12a26:	70a2                	ld	ra,40(sp)
   12a28:	8522                	mv	a0,s0
   12a2a:	7402                	ld	s0,32(sp)
   12a2c:	64e2                	ld	s1,24(sp)
   12a2e:	6942                	ld	s2,16(sp)
   12a30:	6145                	addi	sp,sp,48
   12a32:	8082                	ret
   12a34:	d38fe0ef          	jal	ra,10f6c <__errno_location>
   12a38:	4118                	lw	a4,0(a0)
   12a3a:	02600793          	li	a5,38
   12a3e:	fef714e3          	bne	a4,a5,12a26 <getrlimit+0x2a>
   12a42:	0a300893          	li	a7,163
   12a46:	854a                	mv	a0,s2
   12a48:	858a                	mv	a1,sp
   12a4a:	00000073          	ecall
   12a4e:	dfafe0ef          	jal	ra,11048 <__syscall_ret>
   12a52:	547d                	li	s0,-1
   12a54:	fc0549e3          	bltz	a0,12a26 <getrlimit+0x2a>
   12a58:	6782                	ld	a5,0(sp)
   12a5a:	e09c                	sd	a5,0(s1)
   12a5c:	67a2                	ld	a5,8(sp)
   12a5e:	e49c                	sd	a5,8(s1)
   12a60:	b7d1                	j	12a24 <getrlimit+0x28>

0000000000012a62 <do_setrlimit>:
   12a62:	495c                	lw	a5,20(a0)
   12a64:	85aa                	mv	a1,a0
   12a66:	00f04a63          	bgtz	a5,12a7a <do_setrlimit+0x18>
   12a6a:	0a400893          	li	a7,164
   12a6e:	4908                	lw	a0,16(a0)
   12a70:	00000073          	ecall
   12a74:	40a0053b          	negw	a0,a0
   12a78:	c9c8                	sw	a0,20(a1)
   12a7a:	8082                	ret

0000000000012a7c <setrlimit>:
   12a7c:	7179                	addi	sp,sp,-48
   12a7e:	87aa                	mv	a5,a0
   12a80:	862e                	mv	a2,a1
   12a82:	f406                	sd	ra,40(sp)
   12a84:	f022                	sd	s0,32(sp)
   12a86:	10500893          	li	a7,261
   12a8a:	4501                	li	a0,0
   12a8c:	85be                	mv	a1,a5
   12a8e:	4681                	li	a3,0
   12a90:	00000073          	ecall
   12a94:	fda00713          	li	a4,-38
   12a98:	2501                	sext.w	a0,a0
   12a9a:	00e50b63          	beq	a0,a4,12ab0 <setrlimit+0x34>
   12a9e:	daafe0ef          	jal	ra,11048 <__syscall_ret>
   12aa2:	0005041b          	sext.w	s0,a0
   12aa6:	70a2                	ld	ra,40(sp)
   12aa8:	8522                	mv	a0,s0
   12aaa:	7402                	ld	s0,32(sp)
   12aac:	6145                	addi	sp,sp,48
   12aae:	8082                	ret
   12ab0:	6218                	ld	a4,0(a2)
   12ab2:	654d                	lui	a0,0x13
   12ab4:	cc3e                	sw	a5,24(sp)
   12ab6:	e43a                	sd	a4,8(sp)
   12ab8:	6618                	ld	a4,8(a2)
   12aba:	57fd                	li	a5,-1
   12abc:	002c                	addi	a1,sp,8
   12abe:	a6250513          	addi	a0,a0,-1438 # 12a62 <do_setrlimit>
   12ac2:	e83a                	sd	a4,16(sp)
   12ac4:	ce3e                	sw	a5,28(sp)
   12ac6:	380010ef          	jal	ra,13e46 <__synccall>
   12aca:	4472                	lw	s0,28(sp)
   12acc:	dc69                	beqz	s0,12aa6 <setrlimit+0x2a>
   12ace:	00805563          	blez	s0,12ad8 <setrlimit+0x5c>
   12ad2:	c9afe0ef          	jal	ra,10f6c <__errno_location>
   12ad6:	c100                	sw	s0,0(a0)
   12ad8:	547d                	li	s0,-1
   12ada:	b7f1                	j	12aa6 <setrlimit+0x2a>

0000000000012adc <__madvise>:
   12adc:	1141                	addi	sp,sp,-16
   12ade:	e406                	sd	ra,8(sp)
   12ae0:	0e900893          	li	a7,233
   12ae4:	00000073          	ecall
   12ae8:	d60fe0ef          	jal	ra,11048 <__syscall_ret>
   12aec:	60a2                	ld	ra,8(sp)
   12aee:	2501                	sext.w	a0,a0
   12af0:	0141                	addi	sp,sp,16
   12af2:	8082                	ret

0000000000012af4 <__vm_wait>:
   12af4:	8082                	ret

0000000000012af6 <__mmap>:
   12af6:	7139                	addi	sp,sp,-64
   12af8:	f822                	sd	s0,48(sp)
   12afa:	fc06                	sd	ra,56(sp)
   12afc:	8436                	mv	s0,a3
   12afe:	f426                	sd	s1,40(sp)
   12b00:	03479693          	slli	a3,a5,0x34
   12b04:	ca99                	beqz	a3,12b1a <__mmap+0x24>
   12b06:	c66fe0ef          	jal	ra,10f6c <__errno_location>
   12b0a:	47d9                	li	a5,22
   12b0c:	70e2                	ld	ra,56(sp)
   12b0e:	7442                	ld	s0,48(sp)
   12b10:	c11c                	sw	a5,0(a0)
   12b12:	74a2                	ld	s1,40(sp)
   12b14:	557d                	li	a0,-1
   12b16:	6121                	addi	sp,sp,64
   12b18:	8082                	ret
   12b1a:	56f5                	li	a3,-3
   12b1c:	8285                	srli	a3,a3,0x1
   12b1e:	00b6f663          	bgeu	a3,a1,12b2a <__mmap+0x34>
   12b22:	c4afe0ef          	jal	ra,10f6c <__errno_location>
   12b26:	47b1                	li	a5,12
   12b28:	b7d5                	j	12b0c <__mmap+0x16>
   12b2a:	01047693          	andi	a3,s0,16
   12b2e:	84aa                	mv	s1,a0
   12b30:	ca99                	beqz	a3,12b46 <__mmap+0x50>
   12b32:	ec3e                	sd	a5,24(sp)
   12b34:	e83a                	sd	a4,16(sp)
   12b36:	e432                	sd	a2,8(sp)
   12b38:	e02e                	sd	a1,0(sp)
   12b3a:	fbbff0ef          	jal	ra,12af4 <__vm_wait>
   12b3e:	67e2                	ld	a5,24(sp)
   12b40:	6742                	ld	a4,16(sp)
   12b42:	6622                	ld	a2,8(sp)
   12b44:	6582                	ld	a1,0(sp)
   12b46:	0de00893          	li	a7,222
   12b4a:	8526                	mv	a0,s1
   12b4c:	86a2                	mv	a3,s0
   12b4e:	00000073          	ecall
   12b52:	57fd                	li	a5,-1
   12b54:	00f51a63          	bne	a0,a5,12b68 <__mmap+0x72>
   12b58:	e881                	bnez	s1,12b68 <__mmap+0x72>
   12b5a:	03047413          	andi	s0,s0,48
   12b5e:	02000793          	li	a5,32
   12b62:	00f41363          	bne	s0,a5,12b68 <__mmap+0x72>
   12b66:	5551                	li	a0,-12
   12b68:	7442                	ld	s0,48(sp)
   12b6a:	70e2                	ld	ra,56(sp)
   12b6c:	74a2                	ld	s1,40(sp)
   12b6e:	6121                	addi	sp,sp,64
   12b70:	cd8fe06f          	j	11048 <__syscall_ret>

0000000000012b74 <__mprotect>:
   12b74:	1b01b703          	ld	a4,432(gp) # 1c9b0 <__libc+0x30>
   12b78:	87aa                	mv	a5,a0
   12b7a:	1141                	addi	sp,sp,-16
   12b7c:	40e006b3          	neg	a3,a4
   12b80:	177d                	addi	a4,a4,-1
   12b82:	972e                	add	a4,a4,a1
   12b84:	97ba                	add	a5,a5,a4
   12b86:	8d75                	and	a0,a0,a3
   12b88:	00d7f5b3          	and	a1,a5,a3
   12b8c:	e406                	sd	ra,8(sp)
   12b8e:	0e200893          	li	a7,226
   12b92:	8d89                	sub	a1,a1,a0
   12b94:	00000073          	ecall
   12b98:	cb0fe0ef          	jal	ra,11048 <__syscall_ret>
   12b9c:	60a2                	ld	ra,8(sp)
   12b9e:	2501                	sext.w	a0,a0
   12ba0:	0141                	addi	sp,sp,16
   12ba2:	8082                	ret

0000000000012ba4 <dummy>:
   12ba4:	8082                	ret

0000000000012ba6 <__munmap>:
   12ba6:	1101                	addi	sp,sp,-32
   12ba8:	ec06                	sd	ra,24(sp)
   12baa:	e42a                	sd	a0,8(sp)
   12bac:	e02e                	sd	a1,0(sp)
   12bae:	f47ff0ef          	jal	ra,12af4 <__vm_wait>
   12bb2:	6522                	ld	a0,8(sp)
   12bb4:	6582                	ld	a1,0(sp)
   12bb6:	0d700893          	li	a7,215
   12bba:	00000073          	ecall
   12bbe:	c8afe0ef          	jal	ra,11048 <__syscall_ret>
   12bc2:	60e2                	ld	ra,24(sp)
   12bc4:	2501                	sext.w	a0,a0
   12bc6:	6105                	addi	sp,sp,32
   12bc8:	8082                	ret

0000000000012bca <mbtowc>:
   12bca:	10058063          	beqz	a1,12cca <mbtowc+0x100>
   12bce:	1101                	addi	sp,sp,-32
   12bd0:	ec06                	sd	ra,24(sp)
   12bd2:	c66d                	beqz	a2,12cbc <mbtowc+0xf2>
   12bd4:	e111                	bnez	a0,12bd8 <mbtowc+0xe>
   12bd6:	0068                	addi	a0,sp,12
   12bd8:	0005c783          	lbu	a5,0(a1)
   12bdc:	0187971b          	slliw	a4,a5,0x18
   12be0:	4187571b          	sraiw	a4,a4,0x18
   12be4:	00074963          	bltz	a4,12bf6 <mbtowc+0x2c>
   12be8:	2781                	sext.w	a5,a5
   12bea:	c11c                	sw	a5,0(a0)
   12bec:	00f03533          	snez	a0,a5
   12bf0:	60e2                	ld	ra,24(sp)
   12bf2:	6105                	addi	sp,sp,32
   12bf4:	8082                	ret
   12bf6:	8692                	mv	a3,tp
   12bf8:	fd06b683          	ld	a3,-48(a3)
   12bfc:	6294                	ld	a3,0(a3)
   12bfe:	e699                	bnez	a3,12c0c <mbtowc+0x42>
   12c00:	67b9                	lui	a5,0xe
   12c02:	17fd                	addi	a5,a5,-1
   12c04:	8f7d                	and	a4,a4,a5
   12c06:	c118                	sw	a4,0(a0)
   12c08:	4505                	li	a0,1
   12c0a:	b7dd                	j	12bf0 <mbtowc+0x26>
   12c0c:	f3e7879b          	addiw	a5,a5,-194
   12c10:	0007869b          	sext.w	a3,a5
   12c14:	03200713          	li	a4,50
   12c18:	0ad76263          	bltu	a4,a3,12cbc <mbtowc+0xf2>
   12c1c:	6769                	lui	a4,0x1a
   12c1e:	1782                	slli	a5,a5,0x20
   12c20:	b3070713          	addi	a4,a4,-1232 # 19b30 <__fsmu8>
   12c24:	83f9                	srli	a5,a5,0x1e
   12c26:	97ba                	add	a5,a5,a4
   12c28:	4398                	lw	a4,0(a5)
   12c2a:	478d                	li	a5,3
   12c2c:	00c7ea63          	bltu	a5,a2,12c40 <mbtowc+0x76>
   12c30:	4799                	li	a5,6
   12c32:	02c787bb          	mulw	a5,a5,a2
   12c36:	37e9                	addiw	a5,a5,-6
   12c38:	00f717bb          	sllw	a5,a4,a5
   12c3c:	0807c063          	bltz	a5,12cbc <mbtowc+0xf2>
   12c40:	0015c683          	lbu	a3,1(a1)
   12c44:	41a7579b          	sraiw	a5,a4,0x1a
   12c48:	0036d61b          	srliw	a2,a3,0x3
   12c4c:	9fb1                	addw	a5,a5,a2
   12c4e:	3641                	addiw	a2,a2,-16
   12c50:	8fd1                	or	a5,a5,a2
   12c52:	9be1                	andi	a5,a5,-8
   12c54:	2781                	sext.w	a5,a5
   12c56:	e3bd                	bnez	a5,12cbc <mbtowc+0xf2>
   12c58:	0067179b          	slliw	a5,a4,0x6
   12c5c:	f806871b          	addiw	a4,a3,-128
   12c60:	8f5d                	or	a4,a4,a5
   12c62:	02071613          	slli	a2,a4,0x20
   12c66:	0007069b          	sext.w	a3,a4
   12c6a:	00064563          	bltz	a2,12c74 <mbtowc+0xaa>
   12c6e:	c114                	sw	a3,0(a0)
   12c70:	4509                	li	a0,2
   12c72:	bfbd                	j	12bf0 <mbtowc+0x26>
   12c74:	0025c783          	lbu	a5,2(a1)
   12c78:	03f00613          	li	a2,63
   12c7c:	f807879b          	addiw	a5,a5,-128
   12c80:	0007871b          	sext.w	a4,a5
   12c84:	02e66c63          	bltu	a2,a4,12cbc <mbtowc+0xf2>
   12c88:	0066971b          	slliw	a4,a3,0x6
   12c8c:	8fd9                	or	a5,a5,a4
   12c8e:	02079813          	slli	a6,a5,0x20
   12c92:	0007871b          	sext.w	a4,a5
   12c96:	00084563          	bltz	a6,12ca0 <mbtowc+0xd6>
   12c9a:	c118                	sw	a4,0(a0)
   12c9c:	450d                	li	a0,3
   12c9e:	bf89                	j	12bf0 <mbtowc+0x26>
   12ca0:	0035c783          	lbu	a5,3(a1)
   12ca4:	f807869b          	addiw	a3,a5,-128
   12ca8:	0006879b          	sext.w	a5,a3
   12cac:	00f66863          	bltu	a2,a5,12cbc <mbtowc+0xf2>
   12cb0:	0067179b          	slliw	a5,a4,0x6
   12cb4:	8fd5                	or	a5,a5,a3
   12cb6:	c11c                	sw	a5,0(a0)
   12cb8:	4511                	li	a0,4
   12cba:	bf1d                	j	12bf0 <mbtowc+0x26>
   12cbc:	ab0fe0ef          	jal	ra,10f6c <__errno_location>
   12cc0:	05400793          	li	a5,84
   12cc4:	c11c                	sw	a5,0(a0)
   12cc6:	557d                	li	a0,-1
   12cc8:	b725                	j	12bf0 <mbtowc+0x26>
   12cca:	4501                	li	a0,0
   12ccc:	8082                	ret

0000000000012cce <execv>:
   12cce:	9d81b603          	ld	a2,-1576(gp) # 1c1d8 <__environ>
   12cd2:	a009                	j	12cd4 <execve>

0000000000012cd4 <execve>:
   12cd4:	1141                	addi	sp,sp,-16
   12cd6:	e406                	sd	ra,8(sp)
   12cd8:	0dd00893          	li	a7,221
   12cdc:	00000073          	ecall
   12ce0:	b68fe0ef          	jal	ra,11048 <__syscall_ret>
   12ce4:	60a2                	ld	ra,8(sp)
   12ce6:	2501                	sext.w	a0,a0
   12ce8:	0141                	addi	sp,sp,16
   12cea:	8082                	ret

0000000000012cec <__fork_handler>:
   12cec:	8082                	ret

0000000000012cee <__tl_lock>:
   12cee:	8082                	ret

0000000000012cf0 <fork>:
   12cf0:	7131                	addi	sp,sp,-192
   12cf2:	557d                	li	a0,-1
   12cf4:	fd06                	sd	ra,184(sp)
   12cf6:	e952                	sd	s4,144(sp)
   12cf8:	f922                	sd	s0,176(sp)
   12cfa:	f526                	sd	s1,168(sp)
   12cfc:	f14a                	sd	s2,160(sp)
   12cfe:	ed4e                	sd	s3,152(sp)
   12d00:	e556                	sd	s5,136(sp)
   12d02:	febff0ef          	jal	ra,12cec <__fork_handler>
   12d06:	850a                	mv	a0,sp
   12d08:	124000ef          	jal	ra,12e2c <__block_app_sigs>
   12d0c:	18018793          	addi	a5,gp,384 # 1c980 <__libc>
   12d10:	0037ca03          	lbu	s4,3(a5) # e003 <exit-0x211d>
   12d14:	018a1a1b          	slliw	s4,s4,0x18
   12d18:	418a5a1b          	sraiw	s4,s4,0x18
   12d1c:	03405963          	blez	s4,12d4e <fork+0x5e>
   12d20:	6465                	lui	s0,0x19
   12d22:	557d                	li	a0,-1
   12d24:	7c840413          	addi	s0,s0,1992 # 197c8 <atfork_locks>
   12d28:	fc5ff0ef          	jal	ra,12cec <__fork_handler>
   12d2c:	05840493          	addi	s1,s0,88
   12d30:	6c3000ef          	jal	ra,13bf2 <__inhibit_ptc>
   12d34:	601c                	ld	a5,0(s0)
   12d36:	6388                	ld	a0,0(a5)
   12d38:	c119                	beqz	a0,12d3e <fork+0x4e>
   12d3a:	56d000ef          	jal	ra,13aa6 <__lock>
   12d3e:	0421                	addi	s0,s0,8
   12d40:	fe849ae3          	bne	s1,s0,12d34 <fork+0x44>
   12d44:	557d                	li	a0,-1
   12d46:	80eff0ef          	jal	ra,11d54 <__malloc_atfork>
   12d4a:	fa5ff0ef          	jal	ra,12cee <__tl_lock>
   12d4e:	8492                	mv	s1,tp
   12d50:	f484b983          	ld	s3,-184(s1)
   12d54:	6ee010ef          	jal	ra,14442 <_Fork>
   12d58:	842a                	mv	s0,a0
   12d5a:	a12fe0ef          	jal	ra,10f6c <__errno_location>
   12d5e:	f3848493          	addi	s1,s1,-200
   12d62:	00052a83          	lw	s5,0(a0)
   12d66:	892a                	mv	s2,a0
   12d68:	03405e63          	blez	s4,12da4 <fork+0xb4>
   12d6c:	57fd                	li	a5,-1
   12d6e:	c425                	beqz	s0,12dd6 <fork+0xe6>
   12d70:	f7fff0ef          	jal	ra,12cee <__tl_lock>
   12d74:	00143993          	seqz	s3,s0
   12d78:	64e5                	lui	s1,0x19
   12d7a:	854e                	mv	a0,s3
   12d7c:	7c848493          	addi	s1,s1,1992 # 197c8 <atfork_locks>
   12d80:	fd5fe0ef          	jal	ra,11d54 <__malloc_atfork>
   12d84:	05848a13          	addi	s4,s1,88
   12d88:	609c                	ld	a5,0(s1)
   12d8a:	6388                	ld	a0,0(a5)
   12d8c:	c501                	beqz	a0,12d94 <fork+0xa4>
   12d8e:	cc39                	beqz	s0,12dec <fork+0xfc>
   12d90:	5ef000ef          	jal	ra,13b7e <__unlock>
   12d94:	04a1                	addi	s1,s1,8
   12d96:	fe9a19e3          	bne	s4,s1,12d88 <fork+0x98>
   12d9a:	665000ef          	jal	ra,13bfe <__release_ptc>
   12d9e:	854e                	mv	a0,s3
   12da0:	f4dff0ef          	jal	ra,12cec <__fork_handler>
   12da4:	850a                	mv	a0,sp
   12da6:	09c000ef          	jal	ra,12e42 <__restore_sigs>
   12daa:	00143513          	seqz	a0,s0
   12dae:	f3fff0ef          	jal	ra,12cec <__fork_handler>
   12db2:	00045463          	bgez	s0,12dba <fork+0xca>
   12db6:	01592023          	sw	s5,0(s2)
   12dba:	70ea                	ld	ra,184(sp)
   12dbc:	8522                	mv	a0,s0
   12dbe:	744a                	ld	s0,176(sp)
   12dc0:	74aa                	ld	s1,168(sp)
   12dc2:	790a                	ld	s2,160(sp)
   12dc4:	69ea                	ld	s3,152(sp)
   12dc6:	6a4a                	ld	s4,144(sp)
   12dc8:	6aaa                	ld	s5,136(sp)
   12dca:	6129                	addi	sp,sp,192
   12dcc:	8082                	ret
   12dce:	02f9a023          	sw	a5,32(s3)
   12dd2:	0109b983          	ld	s3,16(s3)
   12dd6:	fe999ce3          	bne	s3,s1,12dce <fork+0xde>
   12dda:	67e9                	lui	a5,0x1a
   12ddc:	2187b783          	ld	a5,536(a5) # 1a218 <__at_quick_exit_lockptr>
   12de0:	dbc1                	beqz	a5,12d70 <fork+0x80>
   12de2:	0007a023          	sw	zero,0(a5)
   12de6:	0007a223          	sw	zero,4(a5)
   12dea:	b759                	j	12d70 <fork+0x80>
   12dec:	00052023          	sw	zero,0(a0)
   12df0:	b755                	j	12d94 <fork+0xa4>

0000000000012df2 <waitpid>:
   12df2:	1141                	addi	sp,sp,-16
   12df4:	86b2                	mv	a3,a2
   12df6:	4801                	li	a6,0
   12df8:	862e                	mv	a2,a1
   12dfa:	4781                	li	a5,0
   12dfc:	85aa                	mv	a1,a0
   12dfe:	4701                	li	a4,0
   12e00:	10400513          	li	a0,260
   12e04:	e406                	sd	ra,8(sp)
   12e06:	5eb000ef          	jal	ra,13bf0 <__syscall_cp>
   12e0a:	a3efe0ef          	jal	ra,11048 <__syscall_ret>
   12e0e:	60a2                	ld	ra,8(sp)
   12e10:	2501                	sext.w	a0,a0
   12e12:	0141                	addi	sp,sp,16
   12e14:	8082                	ret

0000000000012e16 <__block_all_sigs>:
   12e16:	65e9                	lui	a1,0x1a
   12e18:	862a                	mv	a2,a0
   12e1a:	08700893          	li	a7,135
   12e1e:	4501                	li	a0,0
   12e20:	22058593          	addi	a1,a1,544 # 1a220 <all_mask>
   12e24:	46a1                	li	a3,8
   12e26:	00000073          	ecall
   12e2a:	8082                	ret

0000000000012e2c <__block_app_sigs>:
   12e2c:	65e9                	lui	a1,0x1a
   12e2e:	862a                	mv	a2,a0
   12e30:	08700893          	li	a7,135
   12e34:	4501                	li	a0,0
   12e36:	22858593          	addi	a1,a1,552 # 1a228 <app_mask>
   12e3a:	46a1                	li	a3,8
   12e3c:	00000073          	ecall
   12e40:	8082                	ret

0000000000012e42 <__restore_sigs>:
   12e42:	85aa                	mv	a1,a0
   12e44:	08700893          	li	a7,135
   12e48:	4509                	li	a0,2
   12e4a:	4601                	li	a2,0
   12e4c:	46a1                	li	a3,8
   12e4e:	00000073          	ecall
   12e52:	8082                	ret

0000000000012e54 <kill>:
   12e54:	1141                	addi	sp,sp,-16
   12e56:	e406                	sd	ra,8(sp)
   12e58:	08100893          	li	a7,129
   12e5c:	00000073          	ecall
   12e60:	9e8fe0ef          	jal	ra,11048 <__syscall_ret>
   12e64:	60a2                	ld	ra,8(sp)
   12e66:	2501                	sext.w	a0,a0
   12e68:	0141                	addi	sp,sp,16
   12e6a:	8082                	ret

0000000000012e6c <sigaddset>:
   12e6c:	fff5871b          	addiw	a4,a1,-1
   12e70:	03f00793          	li	a5,63
   12e74:	00e7e663          	bltu	a5,a4,12e80 <sigaddset+0x14>
   12e78:	3581                	addiw	a1,a1,-32
   12e7a:	4789                	li	a5,2
   12e7c:	00b7ec63          	bltu	a5,a1,12e94 <sigaddset+0x28>
   12e80:	1141                	addi	sp,sp,-16
   12e82:	e406                	sd	ra,8(sp)
   12e84:	8e8fe0ef          	jal	ra,10f6c <__errno_location>
   12e88:	60a2                	ld	ra,8(sp)
   12e8a:	47d9                	li	a5,22
   12e8c:	c11c                	sw	a5,0(a0)
   12e8e:	557d                	li	a0,-1
   12e90:	0141                	addi	sp,sp,16
   12e92:	8082                	ret
   12e94:	4785                	li	a5,1
   12e96:	00e79733          	sll	a4,a5,a4
   12e9a:	611c                	ld	a5,0(a0)
   12e9c:	8fd9                	or	a5,a5,a4
   12e9e:	e11c                	sd	a5,0(a0)
   12ea0:	4501                	li	a0,0
   12ea2:	8082                	ret

0000000000012ea4 <sigemptyset>:
   12ea4:	00053023          	sd	zero,0(a0)
   12ea8:	4501                	li	a0,0
   12eaa:	8082                	ret

0000000000012eac <signal>:
   12eac:	714d                	addi	sp,sp,-336
   12eae:	e2a2                	sd	s0,320(sp)
   12eb0:	fe26                	sd	s1,312(sp)
   12eb2:	842a                	mv	s0,a0
   12eb4:	84ae                	mv	s1,a1
   12eb6:	09000613          	li	a2,144
   12eba:	4581                	li	a1,0
   12ebc:	1108                	addi	a0,sp,160
   12ebe:	e686                	sd	ra,328(sp)
   12ec0:	19d000ef          	jal	ra,1385c <memset>
   12ec4:	100007b7          	lui	a5,0x10000
   12ec8:	860a                	mv	a2,sp
   12eca:	092c                	addi	a1,sp,152
   12ecc:	8522                	mv	a0,s0
   12ece:	ed26                	sd	s1,152(sp)
   12ed0:	12f12023          	sw	a5,288(sp)
   12ed4:	742010ef          	jal	ra,14616 <__sigaction>
   12ed8:	00054863          	bltz	a0,12ee8 <signal+0x3c>
   12edc:	6502                	ld	a0,0(sp)
   12ede:	60b6                	ld	ra,328(sp)
   12ee0:	6416                	ld	s0,320(sp)
   12ee2:	74f2                	ld	s1,312(sp)
   12ee4:	6171                	addi	sp,sp,336
   12ee6:	8082                	ret
   12ee8:	557d                	li	a0,-1
   12eea:	bfd5                	j	12ede <signal+0x32>

0000000000012eec <sigprocmask>:
   12eec:	1141                	addi	sp,sp,-16
   12eee:	e022                	sd	s0,0(sp)
   12ef0:	e406                	sd	ra,8(sp)
   12ef2:	6bb000ef          	jal	ra,13dac <pthread_sigmask>
   12ef6:	842a                	mv	s0,a0
   12ef8:	c509                	beqz	a0,12f02 <sigprocmask+0x16>
   12efa:	872fe0ef          	jal	ra,10f6c <__errno_location>
   12efe:	c100                	sw	s0,0(a0)
   12f00:	547d                	li	s0,-1
   12f02:	60a2                	ld	ra,8(sp)
   12f04:	8522                	mv	a0,s0
   12f06:	6402                	ld	s0,0(sp)
   12f08:	0141                	addi	sp,sp,16
   12f0a:	8082                	ret

0000000000012f0c <sigtimedwait>:
   12f0c:	7179                	addi	sp,sp,-48
   12f0e:	f022                	sd	s0,32(sp)
   12f10:	ec26                	sd	s1,24(sp)
   12f12:	e84a                	sd	s2,16(sp)
   12f14:	e44e                	sd	s3,8(sp)
   12f16:	f406                	sd	ra,40(sp)
   12f18:	842a                	mv	s0,a0
   12f1a:	84ae                	mv	s1,a1
   12f1c:	8932                	mv	s2,a2
   12f1e:	59f1                	li	s3,-4
   12f20:	4801                	li	a6,0
   12f22:	4781                	li	a5,0
   12f24:	4721                	li	a4,8
   12f26:	86ca                	mv	a3,s2
   12f28:	8626                	mv	a2,s1
   12f2a:	85a2                	mv	a1,s0
   12f2c:	08900513          	li	a0,137
   12f30:	4c1000ef          	jal	ra,13bf0 <__syscall_cp>
   12f34:	2501                	sext.w	a0,a0
   12f36:	ff3505e3          	beq	a0,s3,12f20 <sigtimedwait+0x14>
   12f3a:	90efe0ef          	jal	ra,11048 <__syscall_ret>
   12f3e:	70a2                	ld	ra,40(sp)
   12f40:	7402                	ld	s0,32(sp)
   12f42:	64e2                	ld	s1,24(sp)
   12f44:	6942                	ld	s2,16(sp)
   12f46:	69a2                	ld	s3,8(sp)
   12f48:	2501                	sext.w	a0,a0
   12f4a:	6145                	addi	sp,sp,48
   12f4c:	8082                	ret

0000000000012f4e <__lockfile>:
   12f4e:	08c52783          	lw	a5,140(a0)
   12f52:	2781                	sext.w	a5,a5
   12f54:	8712                	mv	a4,tp
   12f56:	c00006b7          	lui	a3,0xc0000
   12f5a:	f5872703          	lw	a4,-168(a4)
   12f5e:	16fd                	addi	a3,a3,-1
   12f60:	8ff5                	and	a5,a5,a3
   12f62:	08e78263          	beq	a5,a4,12fe6 <__lockfile+0x98>
   12f66:	08c50793          	addi	a5,a0,140
   12f6a:	4601                	li	a2,0
   12f6c:	1607a6af          	lr.w.aqrl	a3,(a5)
   12f70:	00c69563          	bne	a3,a2,12f7a <__lockfile+0x2c>
   12f74:	1ee7a5af          	sc.w.aqrl	a1,a4,(a5)
   12f78:	f9f5                	bnez	a1,12f6c <__lockfile+0x1e>
   12f7a:	2681                	sext.w	a3,a3
   12f7c:	c295                	beqz	a3,12fa0 <__lockfile+0x52>
   12f7e:	40000837          	lui	a6,0x40000
   12f82:	01076733          	or	a4,a4,a6
   12f86:	4301                	li	t1,0
   12f88:	2701                	sext.w	a4,a4
   12f8a:	fda00e13          	li	t3,-38
   12f8e:	1607a6af          	lr.w.aqrl	a3,(a5)
   12f92:	00669563          	bne	a3,t1,12f9c <__lockfile+0x4e>
   12f96:	1ee7a62f          	sc.w.aqrl	a2,a4,(a5)
   12f9a:	fa75                	bnez	a2,12f8e <__lockfile+0x40>
   12f9c:	2681                	sext.w	a3,a3
   12f9e:	e299                	bnez	a3,12fa4 <__lockfile+0x56>
   12fa0:	4505                	li	a0,1
   12fa2:	8082                	ret
   12fa4:	0106f5b3          	and	a1,a3,a6
   12fa8:	0106e633          	or	a2,a3,a6
   12fac:	2581                	sext.w	a1,a1
   12fae:	2601                	sext.w	a2,a2
   12fb0:	c185                	beqz	a1,12fd0 <__lockfile+0x82>
   12fb2:	06200893          	li	a7,98
   12fb6:	853e                	mv	a0,a5
   12fb8:	08000593          	li	a1,128
   12fbc:	4681                	li	a3,0
   12fbe:	00000073          	ecall
   12fc2:	fdc516e3          	bne	a0,t3,12f8e <__lockfile+0x40>
   12fc6:	853e                	mv	a0,a5
   12fc8:	4581                	li	a1,0
   12fca:	00000073          	ecall
   12fce:	b7c1                	j	12f8e <__lockfile+0x40>
   12fd0:	1607a5af          	lr.w.aqrl	a1,(a5)
   12fd4:	00d59563          	bne	a1,a3,12fde <__lockfile+0x90>
   12fd8:	1ec7a52f          	sc.w.aqrl	a0,a2,(a5)
   12fdc:	f975                	bnez	a0,12fd0 <__lockfile+0x82>
   12fde:	2581                	sext.w	a1,a1
   12fe0:	fad597e3          	bne	a1,a3,12f8e <__lockfile+0x40>
   12fe4:	b7f9                	j	12fb2 <__lockfile+0x64>
   12fe6:	4501                	li	a0,0
   12fe8:	8082                	ret

0000000000012fea <__unlockfile>:
   12fea:	08c50713          	addi	a4,a0,140
   12fee:	4601                	li	a2,0
   12ff0:	08c52783          	lw	a5,140(a0)
   12ff4:	2781                	sext.w	a5,a5
   12ff6:	160726af          	lr.w.aqrl	a3,(a4)
   12ffa:	00f69563          	bne	a3,a5,13004 <__unlockfile+0x1a>
   12ffe:	1ec725af          	sc.w.aqrl	a1,a2,(a4)
   13002:	f9f5                	bnez	a1,12ff6 <__unlockfile+0xc>
   13004:	2681                	sext.w	a3,a3
   13006:	fed795e3          	bne	a5,a3,12ff0 <__unlockfile+0x6>
   1300a:	02179693          	slli	a3,a5,0x21
   1300e:	0206d263          	bgez	a3,13032 <__unlockfile+0x48>
   13012:	06200893          	li	a7,98
   13016:	853a                	mv	a0,a4
   13018:	08100593          	li	a1,129
   1301c:	4605                	li	a2,1
   1301e:	00000073          	ecall
   13022:	fda00793          	li	a5,-38
   13026:	00f51663          	bne	a0,a5,13032 <__unlockfile+0x48>
   1302a:	853a                	mv	a0,a4
   1302c:	4585                	li	a1,1
   1302e:	00000073          	ecall
   13032:	8082                	ret

0000000000013034 <fputs>:
   13034:	7179                	addi	sp,sp,-48
   13036:	f406                	sd	ra,40(sp)
   13038:	f022                	sd	s0,32(sp)
   1303a:	ec26                	sd	s1,24(sp)
   1303c:	e42e                	sd	a1,8(sp)
   1303e:	84aa                	mv	s1,a0
   13040:	1af000ef          	jal	ra,139ee <strlen>
   13044:	66a2                	ld	a3,8(sp)
   13046:	842a                	mv	s0,a0
   13048:	862a                	mv	a2,a0
   1304a:	4585                	li	a1,1
   1304c:	8526                	mv	a0,s1
   1304e:	0b6000ef          	jal	ra,13104 <fwrite>
   13052:	70a2                	ld	ra,40(sp)
   13054:	8d01                	sub	a0,a0,s0
   13056:	7402                	ld	s0,32(sp)
   13058:	00a03533          	snez	a0,a0
   1305c:	64e2                	ld	s1,24(sp)
   1305e:	40a00533          	neg	a0,a0
   13062:	6145                	addi	sp,sp,48
   13064:	8082                	ret

0000000000013066 <__fwritex>:
   13066:	721c                	ld	a5,32(a2)
   13068:	7179                	addi	sp,sp,-48
   1306a:	f022                	sd	s0,32(sp)
   1306c:	e84a                	sd	s2,16(sp)
   1306e:	e44e                	sd	s3,8(sp)
   13070:	f406                	sd	ra,40(sp)
   13072:	ec26                	sd	s1,24(sp)
   13074:	89aa                	mv	s3,a0
   13076:	892e                	mv	s2,a1
   13078:	8432                	mv	s0,a2
   1307a:	c395                	beqz	a5,1309e <__fwritex+0x38>
   1307c:	701c                	ld	a5,32(s0)
   1307e:	7418                	ld	a4,40(s0)
   13080:	8f99                	sub	a5,a5,a4
   13082:	0327f463          	bgeu	a5,s2,130aa <__fwritex+0x44>
   13086:	04843303          	ld	t1,72(s0)
   1308a:	8522                	mv	a0,s0
   1308c:	7402                	ld	s0,32(sp)
   1308e:	70a2                	ld	ra,40(sp)
   13090:	64e2                	ld	s1,24(sp)
   13092:	864a                	mv	a2,s2
   13094:	85ce                	mv	a1,s3
   13096:	6942                	ld	s2,16(sp)
   13098:	69a2                	ld	s3,8(sp)
   1309a:	6145                	addi	sp,sp,48
   1309c:	8302                	jr	t1
   1309e:	8532                	mv	a0,a2
   130a0:	712010ef          	jal	ra,147b2 <__towrite>
   130a4:	dd61                	beqz	a0,1307c <__fwritex+0x16>
   130a6:	4501                	li	a0,0
   130a8:	a005                	j	130c8 <__fwritex+0x62>
   130aa:	09042783          	lw	a5,144(s0)
   130ae:	84ca                	mv	s1,s2
   130b0:	4729                	li	a4,10
   130b2:	0207d363          	bgez	a5,130d8 <__fwritex+0x72>
   130b6:	7408                	ld	a0,40(s0)
   130b8:	8626                	mv	a2,s1
   130ba:	85ce                	mv	a1,s3
   130bc:	358000ef          	jal	ra,13414 <memcpy>
   130c0:	7410                	ld	a2,40(s0)
   130c2:	854a                	mv	a0,s2
   130c4:	94b2                	add	s1,s1,a2
   130c6:	f404                	sd	s1,40(s0)
   130c8:	70a2                	ld	ra,40(sp)
   130ca:	7402                	ld	s0,32(sp)
   130cc:	64e2                	ld	s1,24(sp)
   130ce:	6942                	ld	s2,16(sp)
   130d0:	69a2                	ld	s3,8(sp)
   130d2:	6145                	addi	sp,sp,48
   130d4:	8082                	ret
   130d6:	84be                	mv	s1,a5
   130d8:	c485                	beqz	s1,13100 <__fwritex+0x9a>
   130da:	fff48793          	addi	a5,s1,-1
   130de:	00f986b3          	add	a3,s3,a5
   130e2:	0006c683          	lbu	a3,0(a3) # ffffffffc0000000 <__BSS_END__+0xffffffffbffe35e0>
   130e6:	fee698e3          	bne	a3,a4,130d6 <__fwritex+0x70>
   130ea:	643c                	ld	a5,72(s0)
   130ec:	8626                	mv	a2,s1
   130ee:	85ce                	mv	a1,s3
   130f0:	8522                	mv	a0,s0
   130f2:	9782                	jalr	a5
   130f4:	fc956ae3          	bltu	a0,s1,130c8 <__fwritex+0x62>
   130f8:	99a6                	add	s3,s3,s1
   130fa:	409904b3          	sub	s1,s2,s1
   130fe:	bf65                	j	130b6 <__fwritex+0x50>
   13100:	84ca                	mv	s1,s2
   13102:	bf55                	j	130b6 <__fwritex+0x50>

0000000000013104 <fwrite>:
   13104:	7139                	addi	sp,sp,-64
   13106:	f822                	sd	s0,48(sp)
   13108:	f426                	sd	s1,40(sp)
   1310a:	f04a                	sd	s2,32(sp)
   1310c:	ec4e                	sd	s3,24(sp)
   1310e:	e456                	sd	s5,8(sp)
   13110:	fc06                	sd	ra,56(sp)
   13112:	e852                	sd	s4,16(sp)
   13114:	02c58ab3          	mul	s5,a1,a2
   13118:	89aa                	mv	s3,a0
   1311a:	892e                	mv	s2,a1
   1311c:	84b6                	mv	s1,a3
   1311e:	4401                	li	s0,0
   13120:	c191                	beqz	a1,13124 <fwrite+0x20>
   13122:	8432                	mv	s0,a2
   13124:	08c4a783          	lw	a5,140(s1)
   13128:	4a01                	li	s4,0
   1312a:	02079713          	slli	a4,a5,0x20
   1312e:	00074663          	bltz	a4,1313a <fwrite+0x36>
   13132:	8526                	mv	a0,s1
   13134:	e1bff0ef          	jal	ra,12f4e <__lockfile>
   13138:	8a2a                	mv	s4,a0
   1313a:	854e                	mv	a0,s3
   1313c:	8626                	mv	a2,s1
   1313e:	85d6                	mv	a1,s5
   13140:	f27ff0ef          	jal	ra,13066 <__fwritex>
   13144:	89aa                	mv	s3,a0
   13146:	000a0563          	beqz	s4,13150 <fwrite+0x4c>
   1314a:	8526                	mv	a0,s1
   1314c:	e9fff0ef          	jal	ra,12fea <__unlockfile>
   13150:	013a8463          	beq	s5,s3,13158 <fwrite+0x54>
   13154:	0329d433          	divu	s0,s3,s2
   13158:	70e2                	ld	ra,56(sp)
   1315a:	8522                	mv	a0,s0
   1315c:	7442                	ld	s0,48(sp)
   1315e:	74a2                	ld	s1,40(sp)
   13160:	7902                	ld	s2,32(sp)
   13162:	69e2                	ld	s3,24(sp)
   13164:	6a42                	ld	s4,16(sp)
   13166:	6aa2                	ld	s5,8(sp)
   13168:	6121                	addi	sp,sp,64
   1316a:	8082                	ret

000000000001316c <locking_putc>:
   1316c:	1101                	addi	sp,sp,-32
   1316e:	400007b7          	lui	a5,0x40000
   13172:	e822                	sd	s0,16(sp)
   13174:	e426                	sd	s1,8(sp)
   13176:	e04a                	sd	s2,0(sp)
   13178:	ec06                	sd	ra,24(sp)
   1317a:	892a                	mv	s2,a0
   1317c:	842e                	mv	s0,a1
   1317e:	08c58493          	addi	s1,a1,140
   13182:	4681                	li	a3,0
   13184:	17fd                	addi	a5,a5,-1
   13186:	1604a72f          	lr.w.aqrl	a4,(s1)
   1318a:	00d71563          	bne	a4,a3,13194 <locking_putc+0x28>
   1318e:	1ef4a62f          	sc.w.aqrl	a2,a5,(s1)
   13192:	fa75                	bnez	a2,13186 <locking_putc+0x1a>
   13194:	0007079b          	sext.w	a5,a4
   13198:	c781                	beqz	a5,131a0 <locking_putc+0x34>
   1319a:	852e                	mv	a0,a1
   1319c:	db3ff0ef          	jal	ra,12f4e <__lockfile>
   131a0:	09042703          	lw	a4,144(s0)
   131a4:	0ff97793          	andi	a5,s2,255
   131a8:	06f70463          	beq	a4,a5,13210 <locking_putc+0xa4>
   131ac:	7418                	ld	a4,40(s0)
   131ae:	7014                	ld	a3,32(s0)
   131b0:	06d70063          	beq	a4,a3,13210 <locking_putc+0xa4>
   131b4:	00170693          	addi	a3,a4,1
   131b8:	f414                	sd	a3,40(s0)
   131ba:	01270023          	sb	s2,0(a4)
   131be:	4601                	li	a2,0
   131c0:	08c42703          	lw	a4,140(s0)
   131c4:	2701                	sext.w	a4,a4
   131c6:	1604a6af          	lr.w.aqrl	a3,(s1)
   131ca:	00e69563          	bne	a3,a4,131d4 <locking_putc+0x68>
   131ce:	1ec4a5af          	sc.w.aqrl	a1,a2,(s1)
   131d2:	f9f5                	bnez	a1,131c6 <locking_putc+0x5a>
   131d4:	2681                	sext.w	a3,a3
   131d6:	fed715e3          	bne	a4,a3,131c0 <locking_putc+0x54>
   131da:	02171693          	slli	a3,a4,0x21
   131de:	0206d263          	bgez	a3,13202 <locking_putc+0x96>
   131e2:	06200893          	li	a7,98
   131e6:	8526                	mv	a0,s1
   131e8:	08100593          	li	a1,129
   131ec:	4605                	li	a2,1
   131ee:	00000073          	ecall
   131f2:	fda00713          	li	a4,-38
   131f6:	00e51663          	bne	a0,a4,13202 <locking_putc+0x96>
   131fa:	8526                	mv	a0,s1
   131fc:	4585                	li	a1,1
   131fe:	00000073          	ecall
   13202:	60e2                	ld	ra,24(sp)
   13204:	6442                	ld	s0,16(sp)
   13206:	64a2                	ld	s1,8(sp)
   13208:	6902                	ld	s2,0(sp)
   1320a:	853e                	mv	a0,a5
   1320c:	6105                	addi	sp,sp,32
   1320e:	8082                	ret
   13210:	85be                	mv	a1,a5
   13212:	8522                	mv	a0,s0
   13214:	474010ef          	jal	ra,14688 <__overflow>
   13218:	87aa                	mv	a5,a0
   1321a:	b755                	j	131be <locking_putc+0x52>

000000000001321c <putc>:
   1321c:	08c5a683          	lw	a3,140(a1)
   13220:	87ae                	mv	a5,a1
   13222:	02069613          	slli	a2,a3,0x20
   13226:	0006871b          	sext.w	a4,a3
   1322a:	00064c63          	bltz	a2,13242 <putc+0x26>
   1322e:	c71d                	beqz	a4,1325c <putc+0x40>
   13230:	8692                	mv	a3,tp
   13232:	f586a603          	lw	a2,-168(a3)
   13236:	c00006b7          	lui	a3,0xc0000
   1323a:	16fd                	addi	a3,a3,-1
   1323c:	8f75                	and	a4,a4,a3
   1323e:	00e61f63          	bne	a2,a4,1325c <putc+0x40>
   13242:	0907a703          	lw	a4,144(a5) # 40000090 <__BSS_END__+0x3ffe3670>
   13246:	0ff57593          	andi	a1,a0,255
   1324a:	00b70663          	beq	a4,a1,13256 <putc+0x3a>
   1324e:	7798                	ld	a4,40(a5)
   13250:	7394                	ld	a3,32(a5)
   13252:	00d71763          	bne	a4,a3,13260 <putc+0x44>
   13256:	853e                	mv	a0,a5
   13258:	4300106f          	j	14688 <__overflow>
   1325c:	85be                	mv	a1,a5
   1325e:	b739                	j	1316c <locking_putc>
   13260:	00170693          	addi	a3,a4,1
   13264:	f794                	sd	a3,40(a5)
   13266:	00a70023          	sb	a0,0(a4)
   1326a:	852e                	mv	a0,a1
   1326c:	8082                	ret

000000000001326e <snprintf>:
   1326e:	715d                	addi	sp,sp,-80
   13270:	f436                	sd	a3,40(sp)
   13272:	1034                	addi	a3,sp,40
   13274:	ec06                	sd	ra,24(sp)
   13276:	f83a                	sd	a4,48(sp)
   13278:	fc3e                	sd	a5,56(sp)
   1327a:	e0c2                	sd	a6,64(sp)
   1327c:	e4c6                	sd	a7,72(sp)
   1327e:	e436                	sd	a3,8(sp)
   13280:	0ae000ef          	jal	ra,1332e <vsnprintf>
   13284:	60e2                	ld	ra,24(sp)
   13286:	6161                	addi	sp,sp,80
   13288:	8082                	ret

000000000001328a <sprintf>:
   1328a:	715d                	addi	sp,sp,-80
   1328c:	f032                	sd	a2,32(sp)
   1328e:	1010                	addi	a2,sp,32
   13290:	ec06                	sd	ra,24(sp)
   13292:	f436                	sd	a3,40(sp)
   13294:	f83a                	sd	a4,48(sp)
   13296:	fc3e                	sd	a5,56(sp)
   13298:	e0c2                	sd	a6,64(sp)
   1329a:	e4c6                	sd	a7,72(sp)
   1329c:	e432                	sd	a2,8(sp)
   1329e:	10c000ef          	jal	ra,133aa <vsprintf>
   132a2:	60e2                	ld	ra,24(sp)
   132a4:	6161                	addi	sp,sp,80
   132a6:	8082                	ret

00000000000132a8 <sn_write>:
   132a8:	7179                	addi	sp,sp,-48
   132aa:	f022                	sd	s0,32(sp)
   132ac:	6d40                	ld	s0,152(a0)
   132ae:	e84a                	sd	s2,16(sp)
   132b0:	e052                	sd	s4,0(sp)
   132b2:	02853903          	ld	s2,40(a0)
   132b6:	8a2e                	mv	s4,a1
   132b8:	7d0c                	ld	a1,56(a0)
   132ba:	641c                	ld	a5,8(s0)
   132bc:	ec26                	sd	s1,24(sp)
   132be:	e44e                	sd	s3,8(sp)
   132c0:	f406                	sd	ra,40(sp)
   132c2:	40b90933          	sub	s2,s2,a1
   132c6:	84aa                	mv	s1,a0
   132c8:	89b2                	mv	s3,a2
   132ca:	0127f363          	bgeu	a5,s2,132d0 <sn_write+0x28>
   132ce:	893e                	mv	s2,a5
   132d0:	00090d63          	beqz	s2,132ea <sn_write+0x42>
   132d4:	6008                	ld	a0,0(s0)
   132d6:	864a                	mv	a2,s2
   132d8:	13c000ef          	jal	ra,13414 <memcpy>
   132dc:	601c                	ld	a5,0(s0)
   132de:	6410                	ld	a2,8(s0)
   132e0:	97ca                	add	a5,a5,s2
   132e2:	41260633          	sub	a2,a2,s2
   132e6:	e01c                	sd	a5,0(s0)
   132e8:	e410                	sd	a2,8(s0)
   132ea:	00843903          	ld	s2,8(s0)
   132ee:	0129f363          	bgeu	s3,s2,132f4 <sn_write+0x4c>
   132f2:	894e                	mv	s2,s3
   132f4:	00090e63          	beqz	s2,13310 <sn_write+0x68>
   132f8:	6008                	ld	a0,0(s0)
   132fa:	864a                	mv	a2,s2
   132fc:	85d2                	mv	a1,s4
   132fe:	116000ef          	jal	ra,13414 <memcpy>
   13302:	601c                	ld	a5,0(s0)
   13304:	6410                	ld	a2,8(s0)
   13306:	97ca                	add	a5,a5,s2
   13308:	41260633          	sub	a2,a2,s2
   1330c:	e01c                	sd	a5,0(s0)
   1330e:	e410                	sd	a2,8(s0)
   13310:	601c                	ld	a5,0(s0)
   13312:	854e                	mv	a0,s3
   13314:	00078023          	sb	zero,0(a5)
   13318:	6cbc                	ld	a5,88(s1)
   1331a:	70a2                	ld	ra,40(sp)
   1331c:	7402                	ld	s0,32(sp)
   1331e:	fc9c                	sd	a5,56(s1)
   13320:	f49c                	sd	a5,40(s1)
   13322:	6942                	ld	s2,16(sp)
   13324:	64e2                	ld	s1,24(sp)
   13326:	69a2                	ld	s3,8(sp)
   13328:	6a02                	ld	s4,0(sp)
   1332a:	6145                	addi	sp,sp,48
   1332c:	8082                	ret

000000000001332e <vsnprintf>:
   1332e:	7129                	addi	sp,sp,-320
   13330:	fa22                	sd	s0,304(sp)
   13332:	f626                	sd	s1,296(sp)
   13334:	f24a                	sd	s2,288(sp)
   13336:	ee4e                	sd	s3,280(sp)
   13338:	fe06                	sd	ra,312(sp)
   1333a:	842e                	mv	s0,a1
   1333c:	8932                	mv	s2,a2
   1333e:	89b6                	mv	s3,a3
   13340:	0804                	addi	s1,sp,16
   13342:	c191                	beqz	a1,13346 <vsnprintf+0x18>
   13344:	84aa                	mv	s1,a0
   13346:	ec26                	sd	s1,24(sp)
   13348:	4781                	li	a5,0
   1334a:	c019                	beqz	s0,13350 <vsnprintf+0x22>
   1334c:	fff40793          	addi	a5,s0,-1
   13350:	0e800613          	li	a2,232
   13354:	4581                	li	a1,0
   13356:	1028                	addi	a0,sp,40
   13358:	f03e                	sd	a5,32(sp)
   1335a:	502000ef          	jal	ra,1385c <memset>
   1335e:	67cd                	lui	a5,0x13
   13360:	2a878793          	addi	a5,a5,680 # 132a8 <sn_write>
   13364:	f8be                	sd	a5,112(sp)
   13366:	003c                	addi	a5,sp,8
   13368:	e13e                	sd	a5,128(sp)
   1336a:	57fd                	li	a5,-1
   1336c:	db3e                	sw	a5,180(sp)
   1336e:	dd3e                	sw	a5,184(sp)
   13370:	083c                	addi	a5,sp,24
   13372:	e1be                	sd	a5,192(sp)
   13374:	800007b7          	lui	a5,0x80000
   13378:	fff7c793          	not	a5,a5
   1337c:	0087ff63          	bgeu	a5,s0,1339a <vsnprintf+0x6c>
   13380:	bedfd0ef          	jal	ra,10f6c <__errno_location>
   13384:	04b00793          	li	a5,75
   13388:	c11c                	sw	a5,0(a0)
   1338a:	557d                	li	a0,-1
   1338c:	70f2                	ld	ra,312(sp)
   1338e:	7452                	ld	s0,304(sp)
   13390:	74b2                	ld	s1,296(sp)
   13392:	7912                	ld	s2,288(sp)
   13394:	69f2                	ld	s3,280(sp)
   13396:	6131                	addi	sp,sp,320
   13398:	8082                	ret
   1339a:	00048023          	sb	zero,0(s1)
   1339e:	864e                	mv	a2,s3
   133a0:	85ca                	mv	a1,s2
   133a2:	1028                	addi	a0,sp,40
   133a4:	007020ef          	jal	ra,15baa <vfprintf>
   133a8:	b7d5                	j	1338c <vsnprintf+0x5e>

00000000000133aa <vsprintf>:
   133aa:	86b2                	mv	a3,a2
   133ac:	862e                	mv	a2,a1
   133ae:	800005b7          	lui	a1,0x80000
   133b2:	fff5c593          	not	a1,a1
   133b6:	bfa5                	j	1332e <vsnprintf>

00000000000133b8 <atoi>:
   133b8:	87aa                	mv	a5,a0
   133ba:	02000593          	li	a1,32
   133be:	4691                	li	a3,4
   133c0:	0007c703          	lbu	a4,0(a5) # ffffffff80000000 <__BSS_END__+0xffffffff7ffe35e0>
   133c4:	02b70563          	beq	a4,a1,133ee <atoi+0x36>
   133c8:	ff77061b          	addiw	a2,a4,-9
   133cc:	02c6f163          	bgeu	a3,a2,133ee <atoi+0x36>
   133d0:	02b00693          	li	a3,43
   133d4:	00d70f63          	beq	a4,a3,133f2 <atoi+0x3a>
   133d8:	02d00613          	li	a2,45
   133dc:	4681                	li	a3,0
   133de:	00c71463          	bne	a4,a2,133e6 <atoi+0x2e>
   133e2:	4685                	li	a3,1
   133e4:	0785                	addi	a5,a5,1
   133e6:	4501                	li	a0,0
   133e8:	4625                	li	a2,9
   133ea:	4829                	li	a6,10
   133ec:	a809                	j	133fe <atoi+0x46>
   133ee:	0785                	addi	a5,a5,1
   133f0:	bfc1                	j	133c0 <atoi+0x8>
   133f2:	4681                	li	a3,0
   133f4:	bfc5                	j	133e4 <atoi+0x2c>
   133f6:	02a8053b          	mulw	a0,a6,a0
   133fa:	0785                	addi	a5,a5,1
   133fc:	9d19                	subw	a0,a0,a4
   133fe:	0007c703          	lbu	a4,0(a5)
   13402:	fd07059b          	addiw	a1,a4,-48
   13406:	872e                	mv	a4,a1
   13408:	feb677e3          	bgeu	a2,a1,133f6 <atoi+0x3e>
   1340c:	e299                	bnez	a3,13412 <atoi+0x5a>
   1340e:	40a0053b          	negw	a0,a0
   13412:	8082                	ret

0000000000013414 <memcpy>:
   13414:	0035f793          	andi	a5,a1,3
   13418:	3e078d63          	beqz	a5,13812 <memcpy+0x3fe>
   1341c:	22060063          	beqz	a2,1363c <memcpy+0x228>
   13420:	87aa                	mv	a5,a0
   13422:	a019                	j	13428 <memcpy+0x14>
   13424:	20060b63          	beqz	a2,1363a <memcpy+0x226>
   13428:	0005c683          	lbu	a3,0(a1) # ffffffff80000000 <__BSS_END__+0xffffffff7ffe35e0>
   1342c:	0585                	addi	a1,a1,1
   1342e:	0035f713          	andi	a4,a1,3
   13432:	00d78023          	sb	a3,0(a5)
   13436:	167d                	addi	a2,a2,-1
   13438:	0785                	addi	a5,a5,1
   1343a:	f76d                	bnez	a4,13424 <memcpy+0x10>
   1343c:	0037f813          	andi	a6,a5,3
   13440:	873e                	mv	a4,a5
   13442:	1e080e63          	beqz	a6,1363e <memcpy+0x22a>
   13446:	1141                	addi	sp,sp,-16
   13448:	e422                	sd	s0,8(sp)
   1344a:	e026                	sd	s1,0(sp)
   1344c:	477d                	li	a4,31
   1344e:	0ac77863          	bgeu	a4,a2,134fe <memcpy+0xea>
   13452:	4689                	li	a3,2
   13454:	0005c883          	lbu	a7,0(a1)
   13458:	4198                	lw	a4,0(a1)
   1345a:	30d80e63          	beq	a6,a3,13776 <memcpy+0x362>
   1345e:	468d                	li	a3,3
   13460:	28d80563          	beq	a6,a3,136ea <memcpy+0x2d6>
   13464:	0015c303          	lbu	t1,1(a1)
   13468:	0025c803          	lbu	a6,2(a1)
   1346c:	fec60693          	addi	a3,a2,-20
   13470:	ff06f393          	andi	t2,a3,-16
   13474:	00358413          	addi	s0,a1,3
   13478:	00378493          	addi	s1,a5,3
   1347c:	03cd                	addi	t2,t2,19
   1347e:	006780a3          	sb	t1,1(a5)
   13482:	01178023          	sb	a7,0(a5)
   13486:	01078123          	sb	a6,2(a5)
   1348a:	93be                	add	t2,t2,a5
   1348c:	0046d593          	srli	a1,a3,0x4
   13490:	8322                	mv	t1,s0
   13492:	87a6                	mv	a5,s1
   13494:	00132883          	lw	a7,1(t1)
   13498:	00532803          	lw	a6,5(t1)
   1349c:	00932683          	lw	a3,9(t1)
   134a0:	01875e1b          	srliw	t3,a4,0x18
   134a4:	00d32703          	lw	a4,13(t1)
   134a8:	0088929b          	slliw	t0,a7,0x8
   134ac:	00881f9b          	slliw	t6,a6,0x8
   134b0:	00869f1b          	slliw	t5,a3,0x8
   134b4:	0188d89b          	srliw	a7,a7,0x18
   134b8:	0188581b          	srliw	a6,a6,0x18
   134bc:	0186d69b          	srliw	a3,a3,0x18
   134c0:	00871e9b          	slliw	t4,a4,0x8
   134c4:	005e6e33          	or	t3,t3,t0
   134c8:	01f8e8b3          	or	a7,a7,t6
   134cc:	01e86833          	or	a6,a6,t5
   134d0:	01d6e6b3          	or	a3,a3,t4
   134d4:	01c7a023          	sw	t3,0(a5)
   134d8:	0117a223          	sw	a7,4(a5)
   134dc:	0107a423          	sw	a6,8(a5)
   134e0:	c7d4                	sw	a3,12(a5)
   134e2:	07c1                	addi	a5,a5,16
   134e4:	0341                	addi	t1,t1,16
   134e6:	fa7797e3          	bne	a5,t2,13494 <memcpy+0x80>
   134ea:	00158713          	addi	a4,a1,1
   134ee:	0712                	slli	a4,a4,0x4
   134f0:	0592                	slli	a1,a1,0x4
   134f2:	1635                	addi	a2,a2,-19
   134f4:	8e0d                	sub	a2,a2,a1
   134f6:	00e487b3          	add	a5,s1,a4
   134fa:	00e405b3          	add	a1,s0,a4
   134fe:	01067713          	andi	a4,a2,16
   13502:	00867893          	andi	a7,a2,8
   13506:	00467813          	andi	a6,a2,4
   1350a:	00267693          	andi	a3,a2,2
   1350e:	8a05                	andi	a2,a2,1
   13510:	34070263          	beqz	a4,13854 <memcpy+0x440>
   13514:	0005c403          	lbu	s0,0(a1)
   13518:	00e5c703          	lbu	a4,14(a1)
   1351c:	0085c383          	lbu	t2,8(a1)
   13520:	00878023          	sb	s0,0(a5)
   13524:	0015c403          	lbu	s0,1(a1)
   13528:	0095c283          	lbu	t0,9(a1)
   1352c:	00a5cf83          	lbu	t6,10(a1)
   13530:	008780a3          	sb	s0,1(a5)
   13534:	0025c403          	lbu	s0,2(a1)
   13538:	00b5cf03          	lbu	t5,11(a1)
   1353c:	00c5ce83          	lbu	t4,12(a1)
   13540:	00878123          	sb	s0,2(a5)
   13544:	0035c403          	lbu	s0,3(a1)
   13548:	00d5ce03          	lbu	t3,13(a1)
   1354c:	00f5c303          	lbu	t1,15(a1)
   13550:	008781a3          	sb	s0,3(a5)
   13554:	0045c403          	lbu	s0,4(a1)
   13558:	00e78723          	sb	a4,14(a5)
   1355c:	00778423          	sb	t2,8(a5)
   13560:	00878223          	sb	s0,4(a5)
   13564:	0055c403          	lbu	s0,5(a1)
   13568:	005784a3          	sb	t0,9(a5)
   1356c:	01f78523          	sb	t6,10(a5)
   13570:	008782a3          	sb	s0,5(a5)
   13574:	0065c403          	lbu	s0,6(a1)
   13578:	01e785a3          	sb	t5,11(a5)
   1357c:	01d78623          	sb	t4,12(a5)
   13580:	00878323          	sb	s0,6(a5)
   13584:	0075c403          	lbu	s0,7(a1)
   13588:	01c786a3          	sb	t3,13(a5)
   1358c:	05c1                	addi	a1,a1,16
   1358e:	008783a3          	sb	s0,7(a5)
   13592:	01078713          	addi	a4,a5,16
   13596:	006787a3          	sb	t1,15(a5)
   1359a:	26088a63          	beqz	a7,1380e <memcpy+0x3fa>
   1359e:	0065c783          	lbu	a5,6(a1)
   135a2:	0005c283          	lbu	t0,0(a1)
   135a6:	0015cf83          	lbu	t6,1(a1)
   135aa:	0025cf03          	lbu	t5,2(a1)
   135ae:	0035ce83          	lbu	t4,3(a1)
   135b2:	0045ce03          	lbu	t3,4(a1)
   135b6:	0055c303          	lbu	t1,5(a1)
   135ba:	0075c883          	lbu	a7,7(a1)
   135be:	00f70323          	sb	a5,6(a4)
   135c2:	00570023          	sb	t0,0(a4)
   135c6:	01f700a3          	sb	t6,1(a4)
   135ca:	01e70123          	sb	t5,2(a4)
   135ce:	01d701a3          	sb	t4,3(a4)
   135d2:	01c70223          	sb	t3,4(a4)
   135d6:	006702a3          	sb	t1,5(a4)
   135da:	05a1                	addi	a1,a1,8
   135dc:	00870793          	addi	a5,a4,8
   135e0:	011703a3          	sb	a7,7(a4)
   135e4:	26080663          	beqz	a6,13850 <memcpy+0x43c>
   135e8:	0025c703          	lbu	a4,2(a1)
   135ec:	0005c303          	lbu	t1,0(a1)
   135f0:	0015c883          	lbu	a7,1(a1)
   135f4:	0035c803          	lbu	a6,3(a1)
   135f8:	00e78123          	sb	a4,2(a5)
   135fc:	00678023          	sb	t1,0(a5)
   13600:	011780a3          	sb	a7,1(a5)
   13604:	0591                	addi	a1,a1,4
   13606:	00478713          	addi	a4,a5,4
   1360a:	010781a3          	sb	a6,3(a5)
   1360e:	1e068e63          	beqz	a3,1380a <memcpy+0x3f6>
   13612:	0005c783          	lbu	a5,0(a1)
   13616:	0015c683          	lbu	a3,1(a1)
   1361a:	0589                	addi	a1,a1,2
   1361c:	00f70023          	sb	a5,0(a4)
   13620:	00d700a3          	sb	a3,1(a4)
   13624:	00270793          	addi	a5,a4,2
   13628:	c609                	beqz	a2,13632 <memcpy+0x21e>
   1362a:	0005c703          	lbu	a4,0(a1)
   1362e:	00e78023          	sb	a4,0(a5)
   13632:	6422                	ld	s0,8(sp)
   13634:	6482                	ld	s1,0(sp)
   13636:	0141                	addi	sp,sp,16
   13638:	8082                	ret
   1363a:	8082                	ret
   1363c:	8082                	ret
   1363e:	46bd                	li	a3,15
   13640:	04c6f963          	bgeu	a3,a2,13692 <memcpy+0x27e>
   13644:	00b7e6b3          	or	a3,a5,a1
   13648:	8a9d                	andi	a3,a3,7
   1364a:	ff060e93          	addi	t4,a2,-16
   1364e:	1c069463          	bnez	a3,13816 <memcpy+0x402>
   13652:	020eb693          	sltiu	a3,t4,32
   13656:	0016c693          	xori	a3,a3,1
   1365a:	0ff6f693          	andi	a3,a3,255
   1365e:	1a068c63          	beqz	a3,13816 <memcpy+0x402>
   13662:	004ede93          	srli	t4,t4,0x4
   13666:	001e8e13          	addi	t3,t4,1
   1366a:	86ae                	mv	a3,a1
   1366c:	0086b303          	ld	t1,8(a3) # ffffffffc0000008 <__BSS_END__+0xffffffffbffe35e8>
   13670:	0006b883          	ld	a7,0(a3)
   13674:	0805                	addi	a6,a6,1
   13676:	00673423          	sd	t1,8(a4)
   1367a:	01173023          	sd	a7,0(a4)
   1367e:	06c1                	addi	a3,a3,16
   13680:	0741                	addi	a4,a4,16
   13682:	ffc865e3          	bltu	a6,t3,1366c <memcpy+0x258>
   13686:	001e8713          	addi	a4,t4,1
   1368a:	0712                	slli	a4,a4,0x4
   1368c:	8a3d                	andi	a2,a2,15
   1368e:	97ba                	add	a5,a5,a4
   13690:	95ba                	add	a1,a1,a4
   13692:	00867813          	andi	a6,a2,8
   13696:	00467693          	andi	a3,a2,4
   1369a:	00267713          	andi	a4,a2,2
   1369e:	8a05                	andi	a2,a2,1
   136a0:	00080c63          	beqz	a6,136b8 <memcpy+0x2a4>
   136a4:	0005a883          	lw	a7,0(a1)
   136a8:	0045a803          	lw	a6,4(a1)
   136ac:	07a1                	addi	a5,a5,8
   136ae:	05a1                	addi	a1,a1,8
   136b0:	ff17ac23          	sw	a7,-8(a5)
   136b4:	ff07ae23          	sw	a6,-4(a5)
   136b8:	c691                	beqz	a3,136c4 <memcpy+0x2b0>
   136ba:	4194                	lw	a3,0(a1)
   136bc:	0791                	addi	a5,a5,4
   136be:	0591                	addi	a1,a1,4
   136c0:	fed7ae23          	sw	a3,-4(a5)
   136c4:	18070a63          	beqz	a4,13858 <memcpy+0x444>
   136c8:	0005c703          	lbu	a4,0(a1)
   136cc:	0015c683          	lbu	a3,1(a1)
   136d0:	0589                	addi	a1,a1,2
   136d2:	00e78023          	sb	a4,0(a5)
   136d6:	00d780a3          	sb	a3,1(a5)
   136da:	00278713          	addi	a4,a5,2
   136de:	de39                	beqz	a2,1363c <memcpy+0x228>
   136e0:	0005c783          	lbu	a5,0(a1)
   136e4:	00f70023          	sb	a5,0(a4)
   136e8:	8082                	ret
   136ea:	fec60693          	addi	a3,a2,-20
   136ee:	ff06f393          	andi	t2,a3,-16
   136f2:	00158413          	addi	s0,a1,1
   136f6:	00178493          	addi	s1,a5,1
   136fa:	03c5                	addi	t2,t2,17
   136fc:	01178023          	sb	a7,0(a5)
   13700:	93be                	add	t2,t2,a5
   13702:	0046d593          	srli	a1,a3,0x4
   13706:	8322                	mv	t1,s0
   13708:	87a6                	mv	a5,s1
   1370a:	00332883          	lw	a7,3(t1)
   1370e:	00732803          	lw	a6,7(t1)
   13712:	00b32683          	lw	a3,11(t1)
   13716:	00875e1b          	srliw	t3,a4,0x8
   1371a:	00f32703          	lw	a4,15(t1)
   1371e:	0188929b          	slliw	t0,a7,0x18
   13722:	01881f9b          	slliw	t6,a6,0x18
   13726:	01869f1b          	slliw	t5,a3,0x18
   1372a:	0088d89b          	srliw	a7,a7,0x8
   1372e:	0088581b          	srliw	a6,a6,0x8
   13732:	0086d69b          	srliw	a3,a3,0x8
   13736:	01871e9b          	slliw	t4,a4,0x18
   1373a:	005e6e33          	or	t3,t3,t0
   1373e:	01f8e8b3          	or	a7,a7,t6
   13742:	01e86833          	or	a6,a6,t5
   13746:	01d6e6b3          	or	a3,a3,t4
   1374a:	01c7a023          	sw	t3,0(a5)
   1374e:	0117a223          	sw	a7,4(a5)
   13752:	0107a423          	sw	a6,8(a5)
   13756:	c7d4                	sw	a3,12(a5)
   13758:	07c1                	addi	a5,a5,16
   1375a:	0341                	addi	t1,t1,16
   1375c:	fa7797e3          	bne	a5,t2,1370a <memcpy+0x2f6>
   13760:	00158713          	addi	a4,a1,1
   13764:	0712                	slli	a4,a4,0x4
   13766:	0592                	slli	a1,a1,0x4
   13768:	163d                	addi	a2,a2,-17
   1376a:	8e0d                	sub	a2,a2,a1
   1376c:	00e487b3          	add	a5,s1,a4
   13770:	00e405b3          	add	a1,s0,a4
   13774:	b369                	j	134fe <memcpy+0xea>
   13776:	0015c803          	lbu	a6,1(a1)
   1377a:	fec60693          	addi	a3,a2,-20
   1377e:	ff06f393          	andi	t2,a3,-16
   13782:	00258413          	addi	s0,a1,2
   13786:	00278493          	addi	s1,a5,2
   1378a:	03c9                	addi	t2,t2,18
   1378c:	01178023          	sb	a7,0(a5)
   13790:	010780a3          	sb	a6,1(a5)
   13794:	93be                	add	t2,t2,a5
   13796:	0046d593          	srli	a1,a3,0x4
   1379a:	8322                	mv	t1,s0
   1379c:	87a6                	mv	a5,s1
   1379e:	00232883          	lw	a7,2(t1)
   137a2:	00632803          	lw	a6,6(t1)
   137a6:	00a32683          	lw	a3,10(t1)
   137aa:	01075e1b          	srliw	t3,a4,0x10
   137ae:	00e32703          	lw	a4,14(t1)
   137b2:	0108929b          	slliw	t0,a7,0x10
   137b6:	01081f9b          	slliw	t6,a6,0x10
   137ba:	01069f1b          	slliw	t5,a3,0x10
   137be:	0108d89b          	srliw	a7,a7,0x10
   137c2:	0108581b          	srliw	a6,a6,0x10
   137c6:	0106d69b          	srliw	a3,a3,0x10
   137ca:	01071e9b          	slliw	t4,a4,0x10
   137ce:	005e6e33          	or	t3,t3,t0
   137d2:	01f8e8b3          	or	a7,a7,t6
   137d6:	01e86833          	or	a6,a6,t5
   137da:	01d6e6b3          	or	a3,a3,t4
   137de:	01c7a023          	sw	t3,0(a5)
   137e2:	0117a223          	sw	a7,4(a5)
   137e6:	0107a423          	sw	a6,8(a5)
   137ea:	c7d4                	sw	a3,12(a5)
   137ec:	07c1                	addi	a5,a5,16
   137ee:	0341                	addi	t1,t1,16
   137f0:	fa7797e3          	bne	a5,t2,1379e <memcpy+0x38a>
   137f4:	00158713          	addi	a4,a1,1
   137f8:	0712                	slli	a4,a4,0x4
   137fa:	0592                	slli	a1,a1,0x4
   137fc:	1639                	addi	a2,a2,-18
   137fe:	8e0d                	sub	a2,a2,a1
   13800:	00e487b3          	add	a5,s1,a4
   13804:	00e405b3          	add	a1,s0,a4
   13808:	b9dd                	j	134fe <memcpy+0xea>
   1380a:	87ba                	mv	a5,a4
   1380c:	bd31                	j	13628 <memcpy+0x214>
   1380e:	87ba                	mv	a5,a4
   13810:	bbd1                	j	135e4 <memcpy+0x1d0>
   13812:	87aa                	mv	a5,a0
   13814:	b125                	j	1343c <memcpy+0x28>
   13816:	004ede93          	srli	t4,t4,0x4
   1381a:	001e8f13          	addi	t5,t4,1
   1381e:	0f12                	slli	t5,t5,0x4
   13820:	9f2e                	add	t5,t5,a1
   13822:	872e                	mv	a4,a1
   13824:	86be                	mv	a3,a5
   13826:	00072e03          	lw	t3,0(a4)
   1382a:	00472303          	lw	t1,4(a4)
   1382e:	00872883          	lw	a7,8(a4)
   13832:	00c72803          	lw	a6,12(a4)
   13836:	01c6a023          	sw	t3,0(a3)
   1383a:	0066a223          	sw	t1,4(a3)
   1383e:	0116a423          	sw	a7,8(a3)
   13842:	0106a623          	sw	a6,12(a3)
   13846:	0741                	addi	a4,a4,16
   13848:	06c1                	addi	a3,a3,16
   1384a:	fcef1ee3          	bne	t5,a4,13826 <memcpy+0x412>
   1384e:	bd25                	j	13686 <memcpy+0x272>
   13850:	873e                	mv	a4,a5
   13852:	bb75                	j	1360e <memcpy+0x1fa>
   13854:	873e                	mv	a4,a5
   13856:	b391                	j	1359a <memcpy+0x186>
   13858:	873e                	mv	a4,a5
   1385a:	b551                	j	136de <memcpy+0x2ca>

000000000001385c <memset>:
   1385c:	c679                	beqz	a2,1392a <memset+0xce>
   1385e:	0ff5f793          	andi	a5,a1,255
   13862:	00f50023          	sb	a5,0(a0)
   13866:	00c50733          	add	a4,a0,a2
   1386a:	fef70fa3          	sb	a5,-1(a4)
   1386e:	4689                	li	a3,2
   13870:	0ac6fd63          	bgeu	a3,a2,1392a <memset+0xce>
   13874:	00f500a3          	sb	a5,1(a0)
   13878:	00f50123          	sb	a5,2(a0)
   1387c:	fef70f23          	sb	a5,-2(a4)
   13880:	fef70ea3          	sb	a5,-3(a4)
   13884:	4699                	li	a3,6
   13886:	0ac6f263          	bgeu	a3,a2,1392a <memset+0xce>
   1388a:	00f501a3          	sb	a5,3(a0)
   1388e:	fef70e23          	sb	a5,-4(a4)
   13892:	46a1                	li	a3,8
   13894:	08c6fb63          	bgeu	a3,a2,1392a <memset+0xce>
   13898:	0ff5f593          	andi	a1,a1,255
   1389c:	0085979b          	slliw	a5,a1,0x8
   138a0:	40a00733          	neg	a4,a0
   138a4:	8b0d                	andi	a4,a4,3
   138a6:	9dbd                	addw	a1,a1,a5
   138a8:	8e19                	sub	a2,a2,a4
   138aa:	0105979b          	slliw	a5,a1,0x10
   138ae:	9dbd                	addw	a1,a1,a5
   138b0:	972a                	add	a4,a4,a0
   138b2:	9a71                	andi	a2,a2,-4
   138b4:	c30c                	sw	a1,0(a4)
   138b6:	00c707b3          	add	a5,a4,a2
   138ba:	feb7ae23          	sw	a1,-4(a5)
   138be:	06c6f663          	bgeu	a3,a2,1392a <memset+0xce>
   138c2:	c34c                	sw	a1,4(a4)
   138c4:	c70c                	sw	a1,8(a4)
   138c6:	feb7aa23          	sw	a1,-12(a5)
   138ca:	feb7ac23          	sw	a1,-8(a5)
   138ce:	46e1                	li	a3,24
   138d0:	04c6fd63          	bgeu	a3,a2,1392a <memset+0xce>
   138d4:	00477813          	andi	a6,a4,4
   138d8:	c74c                	sw	a1,12(a4)
   138da:	cb0c                	sw	a1,16(a4)
   138dc:	cb4c                	sw	a1,20(a4)
   138de:	cf0c                	sw	a1,24(a4)
   138e0:	0861                	addi	a6,a6,24
   138e2:	02059893          	slli	a7,a1,0x20
   138e6:	0208d893          	srli	a7,a7,0x20
   138ea:	02059693          	slli	a3,a1,0x20
   138ee:	feb7a223          	sw	a1,-28(a5)
   138f2:	feb7a423          	sw	a1,-24(a5)
   138f6:	feb7a623          	sw	a1,-20(a5)
   138fa:	feb7a823          	sw	a1,-16(a5)
   138fe:	41060633          	sub	a2,a2,a6
   13902:	47fd                	li	a5,31
   13904:	9742                	add	a4,a4,a6
   13906:	0116e6b3          	or	a3,a3,a7
   1390a:	02c7f063          	bgeu	a5,a2,1392a <memset+0xce>
   1390e:	1601                	addi	a2,a2,-32
   13910:	fe067793          	andi	a5,a2,-32
   13914:	02078793          	addi	a5,a5,32
   13918:	97ba                	add	a5,a5,a4
   1391a:	e314                	sd	a3,0(a4)
   1391c:	e714                	sd	a3,8(a4)
   1391e:	eb14                	sd	a3,16(a4)
   13920:	ef14                	sd	a3,24(a4)
   13922:	02070713          	addi	a4,a4,32
   13926:	fef71ae3          	bne	a4,a5,1391a <memset+0xbe>
   1392a:	8082                	ret

000000000001392c <__strchrnul>:
   1392c:	1141                	addi	sp,sp,-16
   1392e:	e022                	sd	s0,0(sp)
   13930:	e406                	sd	ra,8(sp)
   13932:	0ff5f593          	andi	a1,a1,255
   13936:	842a                	mv	s0,a0
   13938:	e981                	bnez	a1,13948 <__strchrnul+0x1c>
   1393a:	a059                	j	139c0 <__strchrnul+0x94>
   1393c:	00044783          	lbu	a5,0(s0)
   13940:	cbbd                	beqz	a5,139b6 <__strchrnul+0x8a>
   13942:	06b78a63          	beq	a5,a1,139b6 <__strchrnul+0x8a>
   13946:	0405                	addi	s0,s0,1
   13948:	00747793          	andi	a5,s0,7
   1394c:	fbe5                	bnez	a5,1393c <__strchrnul+0x10>
   1394e:	9281b683          	ld	a3,-1752(gp) # 1c128 <__SDATA_BEGIN__+0x20>
   13952:	601c                	ld	a5,0(s0)
   13954:	02d588b3          	mul	a7,a1,a3
   13958:	9381b803          	ld	a6,-1736(gp) # 1c138 <__SDATA_BEGIN__+0x30>
   1395c:	fff7c613          	not	a2,a5
   13960:	40d78733          	sub	a4,a5,a3
   13964:	8f71                	and	a4,a4,a2
   13966:	00f8c7b3          	xor	a5,a7,a5
   1396a:	40d786b3          	sub	a3,a5,a3
   1396e:	fff7c793          	not	a5,a5
   13972:	8ff5                	and	a5,a5,a3
   13974:	8fd9                	or	a5,a5,a4
   13976:	0107f7b3          	and	a5,a5,a6
   1397a:	e78d                	bnez	a5,139a4 <__strchrnul+0x78>
   1397c:	9301b503          	ld	a0,-1744(gp) # 1c130 <__SDATA_BEGIN__+0x28>
   13980:	6418                	ld	a4,8(s0)
   13982:	0421                	addi	s0,s0,8
   13984:	011746b3          	xor	a3,a4,a7
   13988:	00a707b3          	add	a5,a4,a0
   1398c:	00a68633          	add	a2,a3,a0
   13990:	fff74713          	not	a4,a4
   13994:	fff6c693          	not	a3,a3
   13998:	8ff9                	and	a5,a5,a4
   1399a:	8ef1                	and	a3,a3,a2
   1399c:	8fd5                	or	a5,a5,a3
   1399e:	0107f7b3          	and	a5,a5,a6
   139a2:	dff9                	beqz	a5,13980 <__strchrnul+0x54>
   139a4:	00044783          	lbu	a5,0(s0)
   139a8:	c799                	beqz	a5,139b6 <__strchrnul+0x8a>
   139aa:	00b78663          	beq	a5,a1,139b6 <__strchrnul+0x8a>
   139ae:	00144783          	lbu	a5,1(s0)
   139b2:	0405                	addi	s0,s0,1
   139b4:	fbfd                	bnez	a5,139aa <__strchrnul+0x7e>
   139b6:	60a2                	ld	ra,8(sp)
   139b8:	8522                	mv	a0,s0
   139ba:	6402                	ld	s0,0(sp)
   139bc:	0141                	addi	sp,sp,16
   139be:	8082                	ret
   139c0:	02e000ef          	jal	ra,139ee <strlen>
   139c4:	942a                	add	s0,s0,a0
   139c6:	60a2                	ld	ra,8(sp)
   139c8:	8522                	mv	a0,s0
   139ca:	6402                	ld	s0,0(sp)
   139cc:	0141                	addi	sp,sp,16
   139ce:	8082                	ret

00000000000139d0 <strcmp>:
   139d0:	00054783          	lbu	a5,0(a0)
   139d4:	0005c703          	lbu	a4,0(a1)
   139d8:	00e79863          	bne	a5,a4,139e8 <strcmp+0x18>
   139dc:	0505                	addi	a0,a0,1
   139de:	0585                	addi	a1,a1,1
   139e0:	fbe5                	bnez	a5,139d0 <strcmp>
   139e2:	4501                	li	a0,0
   139e4:	9d19                	subw	a0,a0,a4
   139e6:	8082                	ret
   139e8:	0007851b          	sext.w	a0,a5
   139ec:	bfe5                	j	139e4 <strcmp+0x14>

00000000000139ee <strlen>:
   139ee:	00757793          	andi	a5,a0,7
   139f2:	cf89                	beqz	a5,13a0c <strlen+0x1e>
   139f4:	87aa                	mv	a5,a0
   139f6:	a029                	j	13a00 <strlen+0x12>
   139f8:	0785                	addi	a5,a5,1
   139fa:	0077f713          	andi	a4,a5,7
   139fe:	cb01                	beqz	a4,13a0e <strlen+0x20>
   13a00:	0007c703          	lbu	a4,0(a5)
   13a04:	fb75                	bnez	a4,139f8 <strlen+0xa>
   13a06:	40a78533          	sub	a0,a5,a0
   13a0a:	8082                	ret
   13a0c:	87aa                	mv	a5,a0
   13a0e:	9301b583          	ld	a1,-1744(gp) # 1c130 <__SDATA_BEGIN__+0x28>
   13a12:	6394                	ld	a3,0(a5)
   13a14:	9381b603          	ld	a2,-1736(gp) # 1c138 <__SDATA_BEGIN__+0x30>
   13a18:	a019                	j	13a1e <strlen+0x30>
   13a1a:	6794                	ld	a3,8(a5)
   13a1c:	07a1                	addi	a5,a5,8
   13a1e:	00b68733          	add	a4,a3,a1
   13a22:	fff6c693          	not	a3,a3
   13a26:	8f75                	and	a4,a4,a3
   13a28:	8f71                	and	a4,a4,a2
   13a2a:	db65                	beqz	a4,13a1a <strlen+0x2c>
   13a2c:	0007c703          	lbu	a4,0(a5)
   13a30:	db79                	beqz	a4,13a06 <strlen+0x18>
   13a32:	0017c703          	lbu	a4,1(a5)
   13a36:	0785                	addi	a5,a5,1
   13a38:	d779                	beqz	a4,13a06 <strlen+0x18>
   13a3a:	0017c703          	lbu	a4,1(a5)
   13a3e:	0785                	addi	a5,a5,1
   13a40:	fb6d                	bnez	a4,13a32 <strlen+0x44>
   13a42:	b7d1                	j	13a06 <strlen+0x18>

0000000000013a44 <strrchr>:
   13a44:	1101                	addi	sp,sp,-32
   13a46:	e822                	sd	s0,16(sp)
   13a48:	e426                	sd	s1,8(sp)
   13a4a:	ec06                	sd	ra,24(sp)
   13a4c:	842a                	mv	s0,a0
   13a4e:	84ae                	mv	s1,a1
   13a50:	f9fff0ef          	jal	ra,139ee <strlen>
   13a54:	00150613          	addi	a2,a0,1
   13a58:	8522                	mv	a0,s0
   13a5a:	6442                	ld	s0,16(sp)
   13a5c:	60e2                	ld	ra,24(sp)
   13a5e:	85a6                	mv	a1,s1
   13a60:	64a2                	ld	s1,8(sp)
   13a62:	6105                	addi	sp,sp,32
   13a64:	2400206f          	j	15ca4 <__memrchr>

0000000000013a68 <strsignal>:
   13a68:	fff5069b          	addiw	a3,a0,-1
   13a6c:	03f00793          	li	a5,63
   13a70:	02d7e663          	bltu	a5,a3,13a9c <strsignal+0x34>
   13a74:	67e9                	lui	a5,0x1a
   13a76:	05500713          	li	a4,85
   13a7a:	82078793          	addi	a5,a5,-2016 # 19820 <strings>
   13a7e:	567d                	li	a2,-1
   13a80:	c709                	beqz	a4,13a8a <strsignal+0x22>
   13a82:	0017c703          	lbu	a4,1(a5)
   13a86:	0785                	addi	a5,a5,1
   13a88:	ff6d                	bnez	a4,13a82 <strsignal+0x1a>
   13a8a:	36fd                	addiw	a3,a3,-1
   13a8c:	00178513          	addi	a0,a5,1
   13a90:	00c68963          	beq	a3,a2,13aa2 <strsignal+0x3a>
   13a94:	0017c703          	lbu	a4,1(a5)
   13a98:	87aa                	mv	a5,a0
   13a9a:	b7dd                	j	13a80 <strsignal+0x18>
   13a9c:	6569                	lui	a0,0x1a
   13a9e:	82050513          	addi	a0,a0,-2016 # 19820 <strings>
   13aa2:	de8fd06f          	j	1108a <__lctrans_cur>

0000000000013aa6 <__lock>:
   13aa6:	18018693          	addi	a3,gp,384 # 1c980 <__libc>
   13aaa:	0036c603          	lbu	a2,3(a3)
   13aae:	872a                	mv	a4,a0
   13ab0:	0186161b          	slliw	a2,a2,0x18
   13ab4:	4186561b          	sraiw	a2,a2,0x18
   13ab8:	c271                	beqz	a2,13b7c <__lock+0xd6>
   13aba:	800005b7          	lui	a1,0x80000
   13abe:	4501                	li	a0,0
   13ac0:	0585                	addi	a1,a1,1
   13ac2:	160727af          	lr.w.aqrl	a5,(a4)
   13ac6:	00a79663          	bne	a5,a0,13ad2 <__lock+0x2c>
   13aca:	1eb7282f          	sc.w.aqrl	a6,a1,(a4)
   13ace:	fe081ae3          	bnez	a6,13ac2 <__lock+0x1c>
   13ad2:	2781                	sext.w	a5,a5
   13ad4:	00065463          	bgez	a2,13adc <__lock+0x36>
   13ad8:	000681a3          	sb	zero,3(a3)
   13adc:	c3c5                	beqz	a5,13b7c <__lock+0xd6>
   13ade:	800006b7          	lui	a3,0x80000
   13ae2:	45a9                	li	a1,10
   13ae4:	fff6c813          	not	a6,a3
   13ae8:	0016861b          	addiw	a2,a3,1
   13aec:	86be                	mv	a3,a5
   13aee:	0007d463          	bgez	a5,13af6 <__lock+0x50>
   13af2:	00f806bb          	addw	a3,a6,a5
   13af6:	00d6053b          	addw	a0,a2,a3
   13afa:	160727af          	lr.w.aqrl	a5,(a4)
   13afe:	00d79663          	bne	a5,a3,13b0a <__lock+0x64>
   13b02:	1ea728af          	sc.w.aqrl	a7,a0,(a4)
   13b06:	fe089ae3          	bnez	a7,13afa <__lock+0x54>
   13b0a:	2781                	sext.w	a5,a5
   13b0c:	06f68863          	beq	a3,a5,13b7c <__lock+0xd6>
   13b10:	35fd                	addiw	a1,a1,-1
   13b12:	fde9                	bnez	a1,13aec <__lock+0x46>
   13b14:	431c                	lw	a5,0(a4)
   13b16:	0007861b          	sext.w	a2,a5
   13b1a:	2785                	addiw	a5,a5,1
   13b1c:	160726af          	lr.w.aqrl	a3,(a4)
   13b20:	00c69563          	bne	a3,a2,13b2a <__lock+0x84>
   13b24:	1ef725af          	sc.w.aqrl	a1,a5,(a4)
   13b28:	f9f5                	bnez	a1,13b1c <__lock+0x76>
   13b2a:	2681                	sext.w	a3,a3
   13b2c:	fed614e3          	bne	a2,a3,13b14 <__lock+0x6e>
   13b30:	80000837          	lui	a6,0x80000
   13b34:	fda00313          	li	t1,-38
   13b38:	fff84e13          	not	t3,a6
   13b3c:	86be                	mv	a3,a5
   13b3e:	0207d363          	bgez	a5,13b64 <__lock+0xbe>
   13b42:	06200893          	li	a7,98
   13b46:	853a                	mv	a0,a4
   13b48:	08000593          	li	a1,128
   13b4c:	863e                	mv	a2,a5
   13b4e:	4681                	li	a3,0
   13b50:	00000073          	ecall
   13b54:	00651663          	bne	a0,t1,13b60 <__lock+0xba>
   13b58:	853a                	mv	a0,a4
   13b5a:	4581                	li	a1,0
   13b5c:	00000073          	ecall
   13b60:	00fe06bb          	addw	a3,t3,a5
   13b64:	00d8063b          	addw	a2,a6,a3
   13b68:	160727af          	lr.w.aqrl	a5,(a4)
   13b6c:	00d79563          	bne	a5,a3,13b76 <__lock+0xd0>
   13b70:	1ec725af          	sc.w.aqrl	a1,a2,(a4)
   13b74:	f9f5                	bnez	a1,13b68 <__lock+0xc2>
   13b76:	2781                	sext.w	a5,a5
   13b78:	fcf692e3          	bne	a3,a5,13b3c <__lock+0x96>
   13b7c:	8082                	ret

0000000000013b7e <__unlock>:
   13b7e:	4118                	lw	a4,0(a0)
   13b80:	87aa                	mv	a5,a0
   13b82:	02071693          	slli	a3,a4,0x20
   13b86:	0406da63          	bgez	a3,13bda <__unlock+0x5c>
   13b8a:	800005b7          	lui	a1,0x80000
   13b8e:	fff5c593          	not	a1,a1
   13b92:	4398                	lw	a4,0(a5)
   13b94:	0007069b          	sext.w	a3,a4
   13b98:	9f2d                	addw	a4,a4,a1
   13b9a:	1607a62f          	lr.w.aqrl	a2,(a5)
   13b9e:	00d61563          	bne	a2,a3,13ba8 <__unlock+0x2a>
   13ba2:	1ee7a52f          	sc.w.aqrl	a0,a4,(a5)
   13ba6:	f975                	bnez	a0,13b9a <__unlock+0x1c>
   13ba8:	0006071b          	sext.w	a4,a2
   13bac:	fee693e3          	bne	a3,a4,13b92 <__unlock+0x14>
   13bb0:	80000737          	lui	a4,0x80000
   13bb4:	0705                	addi	a4,a4,1
   13bb6:	02e68263          	beq	a3,a4,13bda <__unlock+0x5c>
   13bba:	06200893          	li	a7,98
   13bbe:	853e                	mv	a0,a5
   13bc0:	08100593          	li	a1,129
   13bc4:	4605                	li	a2,1
   13bc6:	00000073          	ecall
   13bca:	fda00713          	li	a4,-38
   13bce:	00e51663          	bne	a0,a4,13bda <__unlock+0x5c>
   13bd2:	853e                	mv	a0,a5
   13bd4:	4585                	li	a1,1
   13bd6:	00000073          	ecall
   13bda:	8082                	ret

0000000000013bdc <__syscall_cp_c>:
   13bdc:	88aa                	mv	a7,a0
   13bde:	852e                	mv	a0,a1
   13be0:	85b2                	mv	a1,a2
   13be2:	8636                	mv	a2,a3
   13be4:	86ba                	mv	a3,a4
   13be6:	873e                	mv	a4,a5
   13be8:	87c2                	mv	a5,a6
   13bea:	00000073          	ecall
   13bee:	8082                	ret

0000000000013bf0 <__syscall_cp>:
   13bf0:	b7f5                	j	13bdc <__syscall_cp_c>

0000000000013bf2 <__inhibit_ptc>:
   13bf2:	f7818513          	addi	a0,gp,-136 # 1c778 <lock>
   13bf6:	aa45                	j	13da6 <__pthread_rwlock_wrlock>

0000000000013bf8 <__acquire_ptc>:
   13bf8:	f7818513          	addi	a0,gp,-136 # 1c778 <lock>
   13bfc:	a021                	j	13c04 <__pthread_rwlock_rdlock>

0000000000013bfe <__release_ptc>:
   13bfe:	f7818513          	addi	a0,gp,-136 # 1c778 <lock>
   13c02:	a23d                	j	13d30 <__pthread_rwlock_unlock>

0000000000013c04 <__pthread_rwlock_rdlock>:
   13c04:	4581                	li	a1,0
   13c06:	a015                	j	13c2a <__pthread_rwlock_timedrdlock>

0000000000013c08 <a_fetch_add>:
   13c08:	87aa                	mv	a5,a0
   13c0a:	4398                	lw	a4,0(a5)
   13c0c:	0007051b          	sext.w	a0,a4
   13c10:	9f2d                	addw	a4,a4,a1
   13c12:	1607a6af          	lr.w.aqrl	a3,(a5)
   13c16:	00a69563          	bne	a3,a0,13c20 <a_fetch_add+0x18>
   13c1a:	1ee7a62f          	sc.w.aqrl	a2,a4,(a5)
   13c1e:	fa75                	bnez	a2,13c12 <a_fetch_add+0xa>
   13c20:	0006871b          	sext.w	a4,a3
   13c24:	fee513e3          	bne	a0,a4,13c0a <a_fetch_add+0x2>
   13c28:	8082                	ret

0000000000013c2a <__pthread_rwlock_timedrdlock>:
   13c2a:	715d                	addi	sp,sp,-80
   13c2c:	e0a2                	sd	s0,64(sp)
   13c2e:	fc26                	sd	s1,56(sp)
   13c30:	f052                	sd	s4,32(sp)
   13c32:	e486                	sd	ra,72(sp)
   13c34:	f84a                	sd	s2,48(sp)
   13c36:	f44e                	sd	s3,40(sp)
   13c38:	ec56                	sd	s5,24(sp)
   13c3a:	e85a                	sd	s6,16(sp)
   13c3c:	e45e                	sd	s7,8(sp)
   13c3e:	84aa                	mv	s1,a0
   13c40:	8a2e                	mv	s4,a1
   13c42:	0ac000ef          	jal	ra,13cee <__pthread_rwlock_tryrdlock>
   13c46:	47c1                	li	a5,16
   13c48:	842a                	mv	s0,a0
   13c4a:	06500713          	li	a4,101
   13c4e:	02f50063          	beq	a0,a5,13c6e <__pthread_rwlock_timedrdlock+0x44>
   13c52:	60a6                	ld	ra,72(sp)
   13c54:	8522                	mv	a0,s0
   13c56:	6406                	ld	s0,64(sp)
   13c58:	74e2                	ld	s1,56(sp)
   13c5a:	7942                	ld	s2,48(sp)
   13c5c:	79a2                	ld	s3,40(sp)
   13c5e:	7a02                	ld	s4,32(sp)
   13c60:	6ae2                	ld	s5,24(sp)
   13c62:	6b42                	ld	s6,16(sp)
   13c64:	6ba2                	ld	s7,8(sp)
   13c66:	6161                	addi	sp,sp,80
   13c68:	8082                	ret
   13c6a:	0330000f          	fence	rw,rw
   13c6e:	377d                	addiw	a4,a4,-1
   13c70:	c719                	beqz	a4,13c7e <__pthread_rwlock_timedrdlock+0x54>
   13c72:	409c                	lw	a5,0(s1)
   13c74:	2781                	sext.w	a5,a5
   13c76:	c781                	beqz	a5,13c7e <__pthread_rwlock_timedrdlock+0x54>
   13c78:	40dc                	lw	a5,4(s1)
   13c7a:	2781                	sext.w	a5,a5
   13c7c:	d7fd                	beqz	a5,13c6a <__pthread_rwlock_timedrdlock+0x40>
   13c7e:	800009b7          	lui	s3,0x80000
   13c82:	4b41                	li	s6,16
   13c84:	fff9c993          	not	s3,s3
   13c88:	80000bb7          	lui	s7,0x80000
   13c8c:	00448a93          	addi	s5,s1,4
   13c90:	a881                	j	13ce0 <__pthread_rwlock_timedrdlock+0xb6>
   13c92:	4080                	lw	s0,0(s1)
   13c94:	2401                	sext.w	s0,s0
   13c96:	c429                	beqz	s0,13ce0 <__pthread_rwlock_timedrdlock+0xb6>
   13c98:	013477b3          	and	a5,s0,s3
   13c9c:	05379263          	bne	a5,s3,13ce0 <__pthread_rwlock_timedrdlock+0xb6>
   13ca0:	01746933          	or	s2,s0,s7
   13ca4:	4585                	li	a1,1
   13ca6:	8556                	mv	a0,s5
   13ca8:	f61ff0ef          	jal	ra,13c08 <a_fetch_add>
   13cac:	2901                	sext.w	s2,s2
   13cae:	1604a7af          	lr.w.aqrl	a5,(s1)
   13cb2:	00879563          	bne	a5,s0,13cbc <__pthread_rwlock_timedrdlock+0x92>
   13cb6:	1f24a72f          	sc.w.aqrl	a4,s2,(s1)
   13cba:	fb75                	bnez	a4,13cae <__pthread_rwlock_timedrdlock+0x84>
   13cbc:	4498                	lw	a4,8(s1)
   13cbe:	86d2                	mv	a3,s4
   13cc0:	4601                	li	a2,0
   13cc2:	08074713          	xori	a4,a4,128
   13cc6:	85ca                	mv	a1,s2
   13cc8:	8526                	mv	a0,s1
   13cca:	100020ef          	jal	ra,15dca <__timedwait>
   13cce:	842a                	mv	s0,a0
   13cd0:	55fd                	li	a1,-1
   13cd2:	8556                	mv	a0,s5
   13cd4:	f35ff0ef          	jal	ra,13c08 <a_fetch_add>
   13cd8:	ffb47793          	andi	a5,s0,-5
   13cdc:	2781                	sext.w	a5,a5
   13cde:	fbb5                	bnez	a5,13c52 <__pthread_rwlock_timedrdlock+0x28>
   13ce0:	8526                	mv	a0,s1
   13ce2:	00c000ef          	jal	ra,13cee <__pthread_rwlock_tryrdlock>
   13ce6:	842a                	mv	s0,a0
   13ce8:	fb6505e3          	beq	a0,s6,13c92 <__pthread_rwlock_timedrdlock+0x68>
   13cec:	b79d                	j	13c52 <__pthread_rwlock_timedrdlock+0x28>

0000000000013cee <__pthread_rwlock_tryrdlock>:
   13cee:	800007b7          	lui	a5,0x80000
   13cf2:	fff7c593          	not	a1,a5
   13cf6:	ffe7c713          	xori	a4,a5,-2
   13cfa:	411c                	lw	a5,0(a0)
   13cfc:	2781                	sext.w	a5,a5
   13cfe:	00b7f6b3          	and	a3,a5,a1
   13d02:	02b68363          	beq	a3,a1,13d28 <__pthread_rwlock_tryrdlock+0x3a>
   13d06:	02e68363          	beq	a3,a4,13d2c <__pthread_rwlock_tryrdlock+0x3e>
   13d0a:	0017861b          	addiw	a2,a5,1
   13d0e:	160526af          	lr.w.aqrl	a3,(a0)
   13d12:	00f69663          	bne	a3,a5,13d1e <__pthread_rwlock_tryrdlock+0x30>
   13d16:	1ec5282f          	sc.w.aqrl	a6,a2,(a0)
   13d1a:	fe081ae3          	bnez	a6,13d0e <__pthread_rwlock_tryrdlock+0x20>
   13d1e:	2681                	sext.w	a3,a3
   13d20:	fcd79de3          	bne	a5,a3,13cfa <__pthread_rwlock_tryrdlock+0xc>
   13d24:	4501                	li	a0,0
   13d26:	8082                	ret
   13d28:	4541                	li	a0,16
   13d2a:	8082                	ret
   13d2c:	452d                	li	a0,11
   13d2e:	8082                	ret

0000000000013d30 <__pthread_rwlock_unlock>:
   13d30:	00852883          	lw	a7,8(a0)
   13d34:	800006b7          	lui	a3,0x80000
   13d38:	87aa                	mv	a5,a0
   13d3a:	fff6c693          	not	a3,a3
   13d3e:	4305                	li	t1,1
   13d40:	4398                	lw	a4,0(a5)
   13d42:	43c8                	lw	a0,4(a5)
   13d44:	4581                	li	a1,0
   13d46:	2701                	sext.w	a4,a4
   13d48:	00d77633          	and	a2,a4,a3
   13d4c:	2501                	sext.w	a0,a0
   13d4e:	00d60663          	beq	a2,a3,13d5a <__pthread_rwlock_unlock+0x2a>
   13d52:	00660463          	beq	a2,t1,13d5a <__pthread_rwlock_unlock+0x2a>
   13d56:	fff7059b          	addiw	a1,a4,-1
   13d5a:	1607a82f          	lr.w.aqrl	a6,(a5)
   13d5e:	00e81663          	bne	a6,a4,13d6a <__pthread_rwlock_unlock+0x3a>
   13d62:	1eb7ae2f          	sc.w.aqrl	t3,a1,(a5)
   13d66:	fe0e1ae3          	bnez	t3,13d5a <__pthread_rwlock_unlock+0x2a>
   13d6a:	2801                	sext.w	a6,a6
   13d6c:	fd071ae3          	bne	a4,a6,13d40 <__pthread_rwlock_unlock+0x10>
   13d70:	e98d                	bnez	a1,13da2 <__pthread_rwlock_unlock+0x72>
   13d72:	e119                	bnez	a0,13d78 <__pthread_rwlock_unlock+0x48>
   13d74:	02075763          	bgez	a4,13da2 <__pthread_rwlock_unlock+0x72>
   13d78:	08000713          	li	a4,128
   13d7c:	00e88463          	beq	a7,a4,13d84 <__pthread_rwlock_unlock+0x54>
   13d80:	08000593          	li	a1,128
   13d84:	06200893          	li	a7,98
   13d88:	853e                	mv	a0,a5
   13d8a:	0015e593          	ori	a1,a1,1
   13d8e:	00000073          	ecall
   13d92:	fda00713          	li	a4,-38
   13d96:	00e51663          	bne	a0,a4,13da2 <__pthread_rwlock_unlock+0x72>
   13d9a:	853e                	mv	a0,a5
   13d9c:	4585                	li	a1,1
   13d9e:	00000073          	ecall
   13da2:	4501                	li	a0,0
   13da4:	8082                	ret

0000000000013da6 <__pthread_rwlock_wrlock>:
   13da6:	4581                	li	a1,0
   13da8:	0820206f          	j	15e2a <__pthread_rwlock_timedwrlock>

0000000000013dac <pthread_sigmask>:
   13dac:	c589                	beqz	a1,13db6 <pthread_sigmask+0xa>
   13dae:	4709                	li	a4,2
   13db0:	47d9                	li	a5,22
   13db2:	02a76163          	bltu	a4,a0,13dd4 <pthread_sigmask+0x28>
   13db6:	08700893          	li	a7,135
   13dba:	46a1                	li	a3,8
   13dbc:	00000073          	ecall
   13dc0:	40a007bb          	negw	a5,a0
   13dc4:	eb81                	bnez	a5,13dd4 <pthread_sigmask+0x28>
   13dc6:	c619                	beqz	a2,13dd4 <pthread_sigmask+0x28>
   13dc8:	6214                	ld	a3,0(a2)
   13dca:	5765                	li	a4,-7
   13dcc:	077e                	slli	a4,a4,0x1f
   13dce:	177d                	addi	a4,a4,-1
   13dd0:	8f75                	and	a4,a4,a3
   13dd2:	e218                	sd	a4,0(a2)
   13dd4:	853e                	mv	a0,a5
   13dd6:	8082                	ret

0000000000013dd8 <dummy>:
   13dd8:	8082                	ret

0000000000013dda <handler>:
   13dda:	8792                	mv	a5,tp
   13ddc:	f587a703          	lw	a4,-168(a5) # ffffffff7fffff58 <__BSS_END__+0xffffffff7ffe3538>
   13de0:	9d01a783          	lw	a5,-1584(gp) # 1c1d0 <target_tid>
   13de4:	04f71f63          	bne	a4,a5,13e42 <handler+0x68>
   13de8:	7179                	addi	sp,sp,-48
   13dea:	f406                	sd	ra,40(sp)
   13dec:	f022                	sd	s0,32(sp)
   13dee:	ec26                	sd	s1,24(sp)
   13df0:	e84a                	sd	s2,16(sp)
   13df2:	e44e                	sd	s3,8(sp)
   13df4:	978fd0ef          	jal	ra,10f6c <__errno_location>
   13df8:	00052983          	lw	s3,0(a0)
   13dfc:	842a                	mv	s0,a0
   13dfe:	fb018513          	addi	a0,gp,-80 # 1c7b0 <caller_sem>
   13e02:	156020ef          	jal	ra,15f58 <sem_post>
   13e06:	fd018513          	addi	a0,gp,-48 # 1c7d0 <target_sem>
   13e0a:	1d2020ef          	jal	ra,15fdc <sem_wait>
   13e0e:	9c81b503          	ld	a0,-1592(gp) # 1c1c8 <context>
   13e12:	9c01b783          	ld	a5,-1600(gp) # 1c1c0 <callback>
   13e16:	9782                	jalr	a5
   13e18:	fb018513          	addi	a0,gp,-80 # 1c7b0 <caller_sem>
   13e1c:	13c020ef          	jal	ra,15f58 <sem_post>
   13e20:	fd018513          	addi	a0,gp,-48 # 1c7d0 <target_sem>
   13e24:	1b8020ef          	jal	ra,15fdc <sem_wait>
   13e28:	fb018513          	addi	a0,gp,-80 # 1c7b0 <caller_sem>
   13e2c:	12c020ef          	jal	ra,15f58 <sem_post>
   13e30:	70a2                	ld	ra,40(sp)
   13e32:	01342023          	sw	s3,0(s0)
   13e36:	7402                	ld	s0,32(sp)
   13e38:	64e2                	ld	s1,24(sp)
   13e3a:	6942                	ld	s2,16(sp)
   13e3c:	69a2                	ld	s3,8(sp)
   13e3e:	6145                	addi	sp,sp,48
   13e40:	8082                	ret
   13e42:	8082                	ret

0000000000013e44 <dummy_0>:
   13e44:	8082                	ret

0000000000013e46 <__synccall>:
   13e46:	7109                	addi	sp,sp,-384
   13e48:	faa2                	sd	s0,368(sp)
   13e4a:	e2da                	sd	s6,320(sp)
   13e4c:	842a                	mv	s0,a0
   13e4e:	8b2e                	mv	s6,a1
   13e50:	09000613          	li	a2,144
   13e54:	4581                	li	a1,0
   13e56:	0908                	addi	a0,sp,144
   13e58:	fe86                	sd	ra,376(sp)
   13e5a:	f6a6                	sd	s1,360(sp)
   13e5c:	f2ca                	sd	s2,352(sp)
   13e5e:	eece                	sd	s3,344(sp)
   13e60:	ead2                	sd	s4,336(sp)
   13e62:	e6d6                	sd	s5,328(sp)
   13e64:	fe5e                	sd	s7,312(sp)
   13e66:	fa62                	sd	s8,304(sp)
   13e68:	f666                	sd	s9,296(sp)
   13e6a:	9f3ff0ef          	jal	ra,1385c <memset>
   13e6e:	67d1                	lui	a5,0x14
   13e70:	dda78793          	addi	a5,a5,-550 # 13dda <handler>
   13e74:	e53e                	sd	a5,136(sp)
   13e76:	100007b7          	lui	a5,0x10000
   13e7a:	10f12823          	sw	a5,272(sp)
   13e7e:	8a12                	mv	s4,tp
   13e80:	0028                	addi	a0,sp,8
   13e82:	fabfe0ef          	jal	ra,12e2c <__block_app_sigs>
   13e86:	e69fe0ef          	jal	ra,12cee <__tl_lock>
   13e8a:	4501                	li	a0,0
   13e8c:	f8bfe0ef          	jal	ra,12e16 <__block_all_sigs>
   13e90:	004c                	addi	a1,sp,4
   13e92:	4505                	li	a0,1
   13e94:	068020ef          	jal	ra,15efc <__pthread_setcancelstate>
   13e98:	4601                	li	a2,0
   13e9a:	4581                	li	a1,0
   13e9c:	fd018513          	addi	a0,gp,-48 # 1c7d0 <target_sem>
   13ea0:	08e020ef          	jal	ra,15f2e <sem_init>
   13ea4:	4601                	li	a2,0
   13ea6:	4581                	li	a1,0
   13ea8:	fb018513          	addi	a0,gp,-80 # 1c7b0 <caller_sem>
   13eac:	082020ef          	jal	ra,15f2e <sem_init>
   13eb0:	1841a783          	lw	a5,388(gp) # 1c984 <__libc+0x4>
   13eb4:	eba1                	bnez	a5,13f04 <__synccall+0xbe>
   13eb6:	4981                	li	s3,0
   13eb8:	855a                	mv	a0,s6
   13eba:	9402                	jalr	s0
   13ebc:	4401                	li	s0,0
   13ebe:	0f341e63          	bne	s0,s3,13fba <__synccall+0x174>
   13ec2:	4401                	li	s0,0
   13ec4:	fb018513          	addi	a0,gp,-80 # 1c7b0 <caller_sem>
   13ec8:	0f341f63          	bne	s0,s3,13fc6 <__synccall+0x180>
   13ecc:	05e020ef          	jal	ra,15f2a <sem_destroy>
   13ed0:	fd018513          	addi	a0,gp,-48 # 1c7d0 <target_sem>
   13ed4:	056020ef          	jal	ra,15f2a <sem_destroy>
   13ed8:	4512                	lw	a0,4(sp)
   13eda:	4581                	li	a1,0
   13edc:	020020ef          	jal	ra,15efc <__pthread_setcancelstate>
   13ee0:	e0ffe0ef          	jal	ra,12cee <__tl_lock>
   13ee4:	0028                	addi	a0,sp,8
   13ee6:	f5dfe0ef          	jal	ra,12e42 <__restore_sigs>
   13eea:	70f6                	ld	ra,376(sp)
   13eec:	7456                	ld	s0,368(sp)
   13eee:	74b6                	ld	s1,360(sp)
   13ef0:	7916                	ld	s2,352(sp)
   13ef2:	69f6                	ld	s3,344(sp)
   13ef4:	6a56                	ld	s4,336(sp)
   13ef6:	6ab6                	ld	s5,328(sp)
   13ef8:	6b16                	ld	s6,320(sp)
   13efa:	7bf2                	ld	s7,312(sp)
   13efc:	7c52                	ld	s8,304(sp)
   13efe:	7cb2                	ld	s9,296(sp)
   13f00:	6119                	addi	sp,sp,384
   13f02:	8082                	ret
   13f04:	f38a0a13          	addi	s4,s4,-200
   13f08:	0b200893          	li	a7,178
   13f0c:	00000073          	ecall
   13f10:	020a2783          	lw	a5,32(s4)
   13f14:	faa791e3          	bne	a5,a0,13eb6 <__synccall+0x70>
   13f18:	08000613          	li	a2,128
   13f1c:	55fd                	li	a1,-1
   13f1e:	0908                	addi	a0,sp,144
   13f20:	9c81b023          	sd	s0,-1600(gp) # 1c1c0 <callback>
   13f24:	9d61b423          	sd	s6,-1592(gp) # 1c1c8 <context>
   13f28:	935ff0ef          	jal	ra,1385c <memset>
   13f2c:	4601                	li	a2,0
   13f2e:	012c                	addi	a1,sp,136
   13f30:	02200513          	li	a0,34
   13f34:	5cc000ef          	jal	ra,14500 <__libc_sigaction>
   13f38:	010a3a83          	ld	s5,16(s4)
   13f3c:	4981                	li	s3,0
   13f3e:	9c018b93          	addi	s7,gp,-1600 # 1c1c0 <callback>
   13f42:	4cad                	li	s9,11
   13f44:	9d018793          	addi	a5,gp,-1584 # 1c1d0 <target_tid>
   13f48:	034a8963          	beq	s5,s4,13f7a <__synccall+0x134>
   13f4c:	020aa703          	lw	a4,32(s5)
   13f50:	c398                	sw	a4,0(a5)
   13f52:	020aa503          	lw	a0,32(s5)
   13f56:	08200893          	li	a7,130
   13f5a:	02200593          	li	a1,34
   13f5e:	00000073          	ecall
   13f62:	40a0053b          	negw	a0,a0
   13f66:	ff9506e3          	beq	a0,s9,13f52 <__synccall+0x10c>
   13f6a:	c515                	beqz	a0,13f96 <__synccall+0x150>
   13f6c:	6451                	lui	s0,0x14
   13f6e:	dd840713          	addi	a4,s0,-552 # 13dd8 <dummy>
   13f72:	00ebb023          	sd	a4,0(s7) # ffffffff80000000 <__BSS_END__+0xffffffff7ffe35e0>
   13f76:	dd840413          	addi	s0,s0,-552
   13f7a:	0007a023          	sw	zero,0(a5) # 10000000 <__BSS_END__+0xffe35e0>
   13f7e:	4a01                	li	s4,0
   13f80:	033a1363          	bne	s4,s3,13fa6 <__synccall+0x160>
   13f84:	4785                	li	a5,1
   13f86:	4601                	li	a2,0
   13f88:	012c                	addi	a1,sp,136
   13f8a:	02200513          	li	a0,34
   13f8e:	e53e                	sd	a5,136(sp)
   13f90:	570000ef          	jal	ra,14500 <__libc_sigaction>
   13f94:	b715                	j	13eb8 <__synccall+0x72>
   13f96:	fb018513          	addi	a0,gp,-80 # 1c7b0 <caller_sem>
   13f9a:	042020ef          	jal	ra,15fdc <sem_wait>
   13f9e:	2985                	addiw	s3,s3,1
   13fa0:	010aba83          	ld	s5,16(s5)
   13fa4:	b745                	j	13f44 <__synccall+0xfe>
   13fa6:	fd018513          	addi	a0,gp,-48 # 1c7d0 <target_sem>
   13faa:	7af010ef          	jal	ra,15f58 <sem_post>
   13fae:	fb018513          	addi	a0,gp,-80 # 1c7b0 <caller_sem>
   13fb2:	02a020ef          	jal	ra,15fdc <sem_wait>
   13fb6:	2a05                	addiw	s4,s4,1
   13fb8:	b7e1                	j	13f80 <__synccall+0x13a>
   13fba:	fd018513          	addi	a0,gp,-48 # 1c7d0 <target_sem>
   13fbe:	79b010ef          	jal	ra,15f58 <sem_post>
   13fc2:	2405                	addiw	s0,s0,1
   13fc4:	bded                	j	13ebe <__synccall+0x78>
   13fc6:	016020ef          	jal	ra,15fdc <sem_wait>
   13fca:	2405                	addiw	s0,s0,1
   13fcc:	bde5                	j	13ec4 <__synccall+0x7e>

0000000000013fce <dup>:
   13fce:	1141                	addi	sp,sp,-16
   13fd0:	e406                	sd	ra,8(sp)
   13fd2:	48dd                	li	a7,23
   13fd4:	00000073          	ecall
   13fd8:	870fd0ef          	jal	ra,11048 <__syscall_ret>
   13fdc:	60a2                	ld	ra,8(sp)
   13fde:	2501                	sext.w	a0,a0
   13fe0:	0141                	addi	sp,sp,16
   13fe2:	8082                	ret

0000000000013fe4 <write>:
   13fe4:	1141                	addi	sp,sp,-16
   13fe6:	86b2                	mv	a3,a2
   13fe8:	4801                	li	a6,0
   13fea:	862e                	mv	a2,a1
   13fec:	4781                	li	a5,0
   13fee:	85aa                	mv	a1,a0
   13ff0:	4701                	li	a4,0
   13ff2:	04000513          	li	a0,64
   13ff6:	e406                	sd	ra,8(sp)
   13ff8:	bf9ff0ef          	jal	ra,13bf0 <__syscall_cp>
   13ffc:	60a2                	ld	ra,8(sp)
   13ffe:	0141                	addi	sp,sp,16
   14000:	848fd06f          	j	11048 <__syscall_ret>

0000000000014004 <__init_tp>:
   14004:	1141                	addi	sp,sp,-16
   14006:	e022                	sd	s0,0(sp)
   14008:	e406                	sd	ra,8(sp)
   1400a:	842a                	mv	s0,a0
   1400c:	e108                	sd	a0,0(a0)
   1400e:	0c850513          	addi	a0,a0,200
   14012:	713010ef          	jal	ra,15f24 <__set_thread_area>
   14016:	02054f63          	bltz	a0,14054 <__init_tp+0x50>
   1401a:	e501                	bnez	a0,14022 <__init_tp+0x1e>
   1401c:	4705                	li	a4,1
   1401e:	18e18023          	sb	a4,384(gp) # 1c980 <__libc>
   14022:	4789                	li	a5,2
   14024:	d41c                	sw	a5,40(s0)
   14026:	06000893          	li	a7,96
   1402a:	20818513          	addi	a0,gp,520 # 1ca08 <__thread_list_lock>
   1402e:	00000073          	ecall
   14032:	1b818793          	addi	a5,gp,440 # 1c9b8 <__libc+0x38>
   14036:	ec5c                	sd	a5,152(s0)
   14038:	07840793          	addi	a5,s0,120
   1403c:	fc3c                	sd	a5,120(s0)
   1403e:	1701b783          	ld	a5,368(gp) # 1c970 <__sysinfo>
   14042:	d008                	sw	a0,32(s0)
   14044:	e400                	sd	s0,8(s0)
   14046:	ec1c                	sd	a5,24(s0)
   14048:	e800                	sd	s0,16(s0)
   1404a:	4501                	li	a0,0
   1404c:	60a2                	ld	ra,8(sp)
   1404e:	6402                	ld	s0,0(sp)
   14050:	0141                	addi	sp,sp,16
   14052:	8082                	ret
   14054:	557d                	li	a0,-1
   14056:	bfdd                	j	1404c <__init_tp+0x48>

0000000000014058 <__copy_tls>:
   14058:	7139                	addi	sp,sp,-64
   1405a:	18018713          	addi	a4,gp,384 # 1c980 <__libc>
   1405e:	7714                	ld	a3,40(a4)
   14060:	f426                	sd	s1,40(sp)
   14062:	6f04                	ld	s1,24(a4)
   14064:	f822                	sd	s0,48(sp)
   14066:	7300                	ld	s0,32(a4)
   14068:	068e                	slli	a3,a3,0x3
   1406a:	14e1                	addi	s1,s1,-8
   1406c:	8c95                	sub	s1,s1,a3
   1406e:	f3800693          	li	a3,-200
   14072:	147d                	addi	s0,s0,-1
   14074:	8e89                	sub	a3,a3,a0
   14076:	f04a                	sd	s2,32(sp)
   14078:	8c75                	and	s0,s0,a3
   1407a:	01073903          	ld	s2,16(a4) # ffffffff80000010 <__BSS_END__+0xffffffff7ffe35f0>
   1407e:	e456                	sd	s5,8(sp)
   14080:	94aa                	add	s1,s1,a0
   14082:	942a                	add	s0,s0,a0
   14084:	6a85                	lui	s5,0x1
   14086:	ec4e                	sd	s3,24(sp)
   14088:	e852                	sd	s4,16(sp)
   1408a:	e05a                	sd	s6,0(sp)
   1408c:	fc06                	sd	ra,56(sp)
   1408e:	0c840b13          	addi	s6,s0,200
   14092:	00848a13          	addi	s4,s1,8
   14096:	18018993          	addi	s3,gp,384 # 1c980 <__libc>
   1409a:	800a8a93          	addi	s5,s5,-2048 # 800 <exit-0xf920>
   1409e:	02091163          	bnez	s2,140c0 <__copy_tls+0x68>
   140a2:	0289b783          	ld	a5,40(s3) # ffffffff80000028 <__BSS_END__+0xffffffff7ffe3608>
   140a6:	70e2                	ld	ra,56(sp)
   140a8:	8522                	mv	a0,s0
   140aa:	e09c                	sd	a5,0(s1)
   140ac:	e064                	sd	s1,192(s0)
   140ae:	7442                	ld	s0,48(sp)
   140b0:	74a2                	ld	s1,40(sp)
   140b2:	7902                	ld	s2,32(sp)
   140b4:	69e2                	ld	s3,24(sp)
   140b6:	6a42                	ld	s4,16(sp)
   140b8:	6aa2                	ld	s5,8(sp)
   140ba:	6b02                	ld	s6,0(sp)
   140bc:	6121                	addi	sp,sp,64
   140be:	8082                	ret
   140c0:	02893783          	ld	a5,40(s2)
   140c4:	00893583          	ld	a1,8(s2)
   140c8:	0a21                	addi	s4,s4,8
   140ca:	97da                	add	a5,a5,s6
   140cc:	97d6                	add	a5,a5,s5
   140ce:	fefa3c23          	sd	a5,-8(s4)
   140d2:	02893503          	ld	a0,40(s2)
   140d6:	01093603          	ld	a2,16(s2)
   140da:	955a                	add	a0,a0,s6
   140dc:	b38ff0ef          	jal	ra,13414 <memcpy>
   140e0:	00093903          	ld	s2,0(s2)
   140e4:	bf6d                	j	1409e <__copy_tls+0x46>

00000000000140e6 <__init_tls>:
   140e6:	01853f83          	ld	t6,24(a0)
   140ea:	02853e03          	ld	t3,40(a0)
   140ee:	9741a303          	lw	t1,-1676(gp) # 1c174 <__default_stacksize>
   140f2:	1141                	addi	sp,sp,-16
   140f4:	00000837          	lui	a6,0x0
   140f8:	6474eeb7          	lui	t4,0x6474e
   140fc:	e406                	sd	ra,8(sp)
   140fe:	e022                	sd	s0,0(sp)
   14100:	87fe                	mv	a5,t6
   14102:	4f01                	li	t5,0
   14104:	4701                	li	a4,0
   14106:	4601                	li	a2,0
   14108:	4299                	li	t0,6
   1410a:	4389                	li	t2,2
   1410c:	00080813          	mv	a6,a6
   14110:	409d                	li	ra,7
   14112:	551e8e93          	addi	t4,t4,1361 # 6474e551 <__BSS_END__+0x64731b31>
   14116:	00800437          	lui	s0,0x800
   1411a:	080e1a63          	bnez	t3,141ae <__init_tls+0xc8>
   1411e:	000f0463          	beqz	t5,14126 <__init_tls+0x40>
   14122:	9661aa23          	sw	t1,-1676(gp) # 1c174 <__default_stacksize>
   14126:	14018793          	addi	a5,gp,320 # 1c940 <main_tls>
   1412a:	18018693          	addi	a3,gp,384 # 1c980 <__libc>
   1412e:	ce09                	beqz	a2,14148 <__init_tls+0x62>
   14130:	6a0c                	ld	a1,16(a2)
   14132:	ea9c                	sd	a5,16(a3)
   14134:	972e                	add	a4,a4,a1
   14136:	e798                	sd	a4,8(a5)
   14138:	7218                	ld	a4,32(a2)
   1413a:	eb98                	sd	a4,16(a5)
   1413c:	7618                	ld	a4,40(a2)
   1413e:	ef98                	sd	a4,24(a5)
   14140:	7a18                	ld	a4,48(a2)
   14142:	f398                	sd	a4,32(a5)
   14144:	4705                	li	a4,1
   14146:	f698                	sd	a4,40(a3)
   14148:	0187b803          	ld	a6,24(a5)
   1414c:	678c                	ld	a1,8(a5)
   1414e:	7388                	ld	a0,32(a5)
   14150:	00b80733          	add	a4,a6,a1
   14154:	fff50613          	addi	a2,a0,-1
   14158:	40e00733          	neg	a4,a4
   1415c:	8f71                	and	a4,a4,a2
   1415e:	9742                	add	a4,a4,a6
   14160:	8e6d                	and	a2,a2,a1
   14162:	ef98                	sd	a4,24(a5)
   14164:	f790                	sd	a2,40(a5)
   14166:	459d                	li	a1,7
   14168:	00a5e463          	bltu	a1,a0,14170 <__init_tls+0x8a>
   1416c:	45a1                	li	a1,8
   1416e:	f38c                	sd	a1,32(a5)
   14170:	739c                	ld	a5,32(a5)
   14172:	0df78593          	addi	a1,a5,223
   14176:	95b2                	add	a1,a1,a2
   14178:	95ba                	add	a1,a1,a4
   1417a:	99e1                	andi	a1,a1,-8
   1417c:	f29c                	sd	a5,32(a3)
   1417e:	ee8c                	sd	a1,24(a3)
   14180:	15000793          	li	a5,336
   14184:	06b7fd63          	bgeu	a5,a1,141fe <__init_tls+0x118>
   14188:	0de00893          	li	a7,222
   1418c:	4501                	li	a0,0
   1418e:	460d                	li	a2,3
   14190:	02200693          	li	a3,34
   14194:	577d                	li	a4,-1
   14196:	4781                	li	a5,0
   14198:	00000073          	ecall
   1419c:	ebdff0ef          	jal	ra,14058 <__copy_tls>
   141a0:	e65ff0ef          	jal	ra,14004 <__init_tp>
   141a4:	06055063          	bgez	a0,14204 <__init_tls+0x11e>
   141a8:	00000023          	sb	zero,0(zero) # 0 <exit-0x10120>
   141ac:	9002                	ebreak
   141ae:	0007a883          	lw	a7,0(a5)
   141b2:	00589963          	bne	a7,t0,141c4 <__init_tls+0xde>
   141b6:	6b98                	ld	a4,16(a5)
   141b8:	40ef8733          	sub	a4,t6,a4
   141bc:	7114                	ld	a3,32(a0)
   141be:	1e7d                	addi	t3,t3,-1
   141c0:	97b6                	add	a5,a5,a3
   141c2:	bfa1                	j	1411a <__init_tls+0x34>
   141c4:	02788563          	beq	a7,t2,141ee <__init_tls+0x108>
   141c8:	02188963          	beq	a7,ra,141fa <__init_tls+0x114>
   141cc:	ffd898e3          	bne	a7,t4,141bc <__init_tls+0xd6>
   141d0:	7794                	ld	a3,40(a5)
   141d2:	02031893          	slli	a7,t1,0x20
   141d6:	0208d893          	srli	a7,a7,0x20
   141da:	fed8f1e3          	bgeu	a7,a3,141bc <__init_tls+0xd6>
   141de:	00d47463          	bgeu	s0,a3,141e6 <__init_tls+0x100>
   141e2:	008006b7          	lui	a3,0x800
   141e6:	0006831b          	sext.w	t1,a3
   141ea:	4f05                	li	t5,1
   141ec:	bfc1                	j	141bc <__init_tls+0xd6>
   141ee:	fc0807e3          	beqz	a6,141bc <__init_tls+0xd6>
   141f2:	6b98                	ld	a4,16(a5)
   141f4:	40e80733          	sub	a4,a6,a4
   141f8:	b7d1                	j	141bc <__init_tls+0xd6>
   141fa:	863e                	mv	a2,a5
   141fc:	b7c1                	j	141bc <__init_tls+0xd6>
   141fe:	ff018513          	addi	a0,gp,-16 # 1c7f0 <builtin_tls>
   14202:	bf69                	j	1419c <__init_tls+0xb6>
   14204:	60a2                	ld	ra,8(sp)
   14206:	6402                	ld	s0,0(sp)
   14208:	0141                	addi	sp,sp,16
   1420a:	8082                	ret

000000000001420c <_Exit>:
   1420c:	87aa                	mv	a5,a0
   1420e:	05e00893          	li	a7,94
   14212:	00000073          	ecall
   14216:	05d00893          	li	a7,93
   1421a:	853e                	mv	a0,a5
   1421c:	bfdd                	j	14212 <_Exit+0x6>

000000000001421e <__lctrans_impl>:
   1421e:	1141                	addi	sp,sp,-16
   14220:	e022                	sd	s0,0(sp)
   14222:	e406                	sd	ra,8(sp)
   14224:	842a                	mv	s0,a0
   14226:	e591                	bnez	a1,14232 <__lctrans_impl+0x14>
   14228:	8522                	mv	a0,s0
   1422a:	60a2                	ld	ra,8(sp)
   1422c:	6402                	ld	s0,0(sp)
   1422e:	0141                	addi	sp,sp,16
   14230:	8082                	ret
   14232:	87ae                	mv	a5,a1
   14234:	862a                	mv	a2,a0
   14236:	658c                	ld	a1,8(a1)
   14238:	6388                	ld	a0,0(a5)
   1423a:	705010ef          	jal	ra,1613e <__mo_lookup>
   1423e:	f575                	bnez	a0,1422a <__lctrans_impl+0xc>
   14240:	b7e5                	j	14228 <__lctrans_impl+0xa>

0000000000014242 <__get_locale>:
   14242:	7149                	addi	sp,sp,-368
   14244:	f2a2                	sd	s0,352(sp)
   14246:	fe56                	sd	s5,312(sp)
   14248:	f686                	sd	ra,360(sp)
   1424a:	eea6                	sd	s1,344(sp)
   1424c:	eaca                	sd	s2,336(sp)
   1424e:	e6ce                	sd	s3,328(sp)
   14250:	e2d2                	sd	s4,320(sp)
   14252:	fa5a                	sd	s6,304(sp)
   14254:	f65e                	sd	s7,296(sp)
   14256:	f262                	sd	s8,288(sp)
   14258:	ee66                	sd	s9,280(sp)
   1425a:	0005c783          	lbu	a5,0(a1) # ffffffff80000000 <__BSS_END__+0xffffffff7ffe35e0>
   1425e:	8aaa                	mv	s5,a0
   14260:	842e                	mv	s0,a1
   14262:	e3b9                	bnez	a5,142a8 <__get_locale+0x66>
   14264:	6569                	lui	a0,0x1a
   14266:	ac050513          	addi	a0,a0,-1344 # 19ac0 <strings+0x2a0>
   1426a:	647010ef          	jal	ra,160b0 <getenv>
   1426e:	842a                	mv	s0,a0
   14270:	c501                	beqz	a0,14278 <__get_locale+0x36>
   14272:	00054783          	lbu	a5,0(a0)
   14276:	eb8d                	bnez	a5,142a8 <__get_locale+0x66>
   14278:	47b1                	li	a5,12
   1427a:	02fa87b3          	mul	a5,s5,a5
   1427e:	6569                	lui	a0,0x1a
   14280:	ae850513          	addi	a0,a0,-1304 # 19ae8 <envvars>
   14284:	953e                	add	a0,a0,a5
   14286:	62b010ef          	jal	ra,160b0 <getenv>
   1428a:	842a                	mv	s0,a0
   1428c:	c501                	beqz	a0,14294 <__get_locale+0x52>
   1428e:	00054783          	lbu	a5,0(a0)
   14292:	eb99                	bnez	a5,142a8 <__get_locale+0x66>
   14294:	6569                	lui	a0,0x1a
   14296:	ac850513          	addi	a0,a0,-1336 # 19ac8 <strings+0x2a8>
   1429a:	617010ef          	jal	ra,160b0 <getenv>
   1429e:	842a                	mv	s0,a0
   142a0:	cd25                	beqz	a0,14318 <__get_locale+0xd6>
   142a2:	00054783          	lbu	a5,0(a0)
   142a6:	cbad                	beqz	a5,14318 <__get_locale+0xd6>
   142a8:	4901                	li	s2,0
   142aa:	02f00713          	li	a4,47
   142ae:	46dd                	li	a3,23
   142b0:	012407b3          	add	a5,s0,s2
   142b4:	0007c783          	lbu	a5,0(a5)
   142b8:	c791                	beqz	a5,142c4 <__get_locale+0x82>
   142ba:	00e78563          	beq	a5,a4,142c4 <__get_locale+0x82>
   142be:	0905                	addi	s2,s2,1
   142c0:	fed918e3          	bne	s2,a3,142b0 <__get_locale+0x6e>
   142c4:	00044783          	lbu	a5,0(s0) # 800000 <__BSS_END__+0x7e35e0>
   142c8:	02e00713          	li	a4,46
   142cc:	04e78a63          	beq	a5,a4,14320 <__get_locale+0xde>
   142d0:	01240733          	add	a4,s0,s2
   142d4:	00074703          	lbu	a4,0(a4)
   142d8:	e721                	bnez	a4,14320 <__get_locale+0xde>
   142da:	04300713          	li	a4,67
   142de:	00e79563          	bne	a5,a4,142e8 <__get_locale+0xa6>
   142e2:	00144783          	lbu	a5,1(s0)
   142e6:	cb91                	beqz	a5,142fa <__get_locale+0xb8>
   142e8:	65e5                	lui	a1,0x19
   142ea:	a2858593          	addi	a1,a1,-1496 # 18a28 <CSWTCH.18+0x98>
   142ee:	8522                	mv	a0,s0
   142f0:	ee0ff0ef          	jal	ra,139d0 <strcmp>
   142f4:	e915                	bnez	a0,14328 <__get_locale+0xe6>
   142f6:	080a8c63          	beqz	s5,1438e <__get_locale+0x14c>
   142fa:	4481                	li	s1,0
   142fc:	70b6                	ld	ra,360(sp)
   142fe:	7416                	ld	s0,352(sp)
   14300:	6956                	ld	s2,336(sp)
   14302:	69b6                	ld	s3,328(sp)
   14304:	6a16                	ld	s4,320(sp)
   14306:	7af2                	ld	s5,312(sp)
   14308:	7b52                	ld	s6,304(sp)
   1430a:	7bb2                	ld	s7,296(sp)
   1430c:	7c12                	ld	s8,288(sp)
   1430e:	6cf2                	ld	s9,280(sp)
   14310:	8526                	mv	a0,s1
   14312:	64f6                	ld	s1,344(sp)
   14314:	6175                	addi	sp,sp,368
   14316:	8082                	ret
   14318:	6465                	lui	s0,0x19
   1431a:	a2840413          	addi	s0,s0,-1496 # 18a28 <CSWTCH.18+0x98>
   1431e:	b769                	j	142a8 <__get_locale+0x66>
   14320:	6465                	lui	s0,0x19
   14322:	a2840413          	addi	s0,s0,-1496 # 18a28 <CSWTCH.18+0x98>
   14326:	bf75                	j	142e2 <__get_locale+0xa0>
   14328:	65e9                	lui	a1,0x1a
   1432a:	ad058593          	addi	a1,a1,-1328 # 19ad0 <strings+0x2b0>
   1432e:	8522                	mv	a0,s0
   14330:	ea0ff0ef          	jal	ra,139d0 <strcmp>
   14334:	d169                	beqz	a0,142f6 <__get_locale+0xb4>
   14336:	9e018793          	addi	a5,gp,-1568 # 1c1e0 <loc_head.2929>
   1433a:	6384                	ld	s1,0(a5)
   1433c:	9e018993          	addi	s3,gp,-1568 # 1c1e0 <loc_head.2929>
   14340:	e0ad                	bnez	s1,143a2 <__get_locale+0x160>
   14342:	1821c783          	lbu	a5,386(gp) # 1c982 <__libc+0x2>
   14346:	ebd5                	bnez	a5,143fa <__get_locale+0x1b8>
   14348:	6569                	lui	a0,0x1a
   1434a:	ad850513          	addi	a0,a0,-1320 # 19ad8 <strings+0x2b8>
   1434e:	563010ef          	jal	ra,160b0 <getenv>
   14352:	84aa                	mv	s1,a0
   14354:	c15d                	beqz	a0,143fa <__get_locale+0x1b8>
   14356:	0fe00b13          	li	s6,254
   1435a:	0a1c                	addi	a5,sp,272
   1435c:	412b0b33          	sub	s6,s6,s2
   14360:	02f00b93          	li	s7,47
   14364:	01278c33          	add	s8,a5,s2
   14368:	0004c783          	lbu	a5,0(s1)
   1436c:	c7d9                	beqz	a5,143fa <__get_locale+0x1b8>
   1436e:	03a00593          	li	a1,58
   14372:	8526                	mv	a0,s1
   14374:	db8ff0ef          	jal	ra,1392c <__strchrnul>
   14378:	40950a33          	sub	s4,a0,s1
   1437c:	8caa                	mv	s9,a0
   1437e:	036a6a63          	bltu	s4,s6,143b2 <__get_locale+0x170>
   14382:	000cc483          	lbu	s1,0(s9)
   14386:	009034b3          	snez	s1,s1
   1438a:	94e6                	add	s1,s1,s9
   1438c:	bff1                	j	14368 <__get_locale+0x126>
   1438e:	00144703          	lbu	a4,1(s0)
   14392:	02e00793          	li	a5,46
   14396:	f6f712e3          	bne	a4,a5,142fa <__get_locale+0xb8>
   1439a:	64e9                	lui	s1,0x1a
   1439c:	00048493          	mv	s1,s1
   143a0:	bfb1                	j	142fc <__get_locale+0xba>
   143a2:	01048593          	addi	a1,s1,16 # 1a010 <__c_dot_utf8+0x10>
   143a6:	8522                	mv	a0,s0
   143a8:	e28ff0ef          	jal	ra,139d0 <strcmp>
   143ac:	d921                	beqz	a0,142fc <__get_locale+0xba>
   143ae:	7484                	ld	s1,40(s1)
   143b0:	bf41                	j	14340 <__get_locale+0xfe>
   143b2:	8652                	mv	a2,s4
   143b4:	85a6                	mv	a1,s1
   143b6:	0808                	addi	a0,sp,16
   143b8:	85cff0ef          	jal	ra,13414 <memcpy>
   143bc:	0a1c                	addi	a5,sp,272
   143be:	97d2                	add	a5,a5,s4
   143c0:	f1778023          	sb	s7,-256(a5)
   143c4:	001a0513          	addi	a0,s4,1
   143c8:	081c                	addi	a5,sp,16
   143ca:	864a                	mv	a2,s2
   143cc:	85a2                	mv	a1,s0
   143ce:	953e                	add	a0,a0,a5
   143d0:	844ff0ef          	jal	ra,13414 <memcpy>
   143d4:	9a62                	add	s4,s4,s8
   143d6:	002c                	addi	a1,sp,8
   143d8:	0808                	addi	a0,sp,16
   143da:	f00a00a3          	sb	zero,-255(s4)
   143de:	403010ef          	jal	ra,15fe0 <__map_file>
   143e2:	8a2a                	mv	s4,a0
   143e4:	dd59                	beqz	a0,14382 <__get_locale+0x140>
   143e6:	03000513          	li	a0,48
   143ea:	9ccfd0ef          	jal	ra,115b6 <__libc_malloc>
   143ee:	65a2                	ld	a1,8(sp)
   143f0:	84aa                	mv	s1,a0
   143f2:	e115                	bnez	a0,14416 <__get_locale+0x1d4>
   143f4:	8552                	mv	a0,s4
   143f6:	fb0fe0ef          	jal	ra,12ba6 <__munmap>
   143fa:	03000513          	li	a0,48
   143fe:	9b8fd0ef          	jal	ra,115b6 <__libc_malloc>
   14402:	84aa                	mv	s1,a0
   14404:	c91d                	beqz	a0,1443a <__get_locale+0x1f8>
   14406:	67e9                	lui	a5,0x1a
   14408:	00078793          	mv	a5,a5
   1440c:	6398                	ld	a4,0(a5)
   1440e:	679c                	ld	a5,8(a5)
   14410:	e118                	sd	a4,0(a0)
   14412:	e51c                	sd	a5,8(a0)
   14414:	a021                	j	1441c <__get_locale+0x1da>
   14416:	01453023          	sd	s4,0(a0)
   1441a:	e50c                	sd	a1,8(a0)
   1441c:	864a                	mv	a2,s2
   1441e:	85a2                	mv	a1,s0
   14420:	01048513          	addi	a0,s1,16
   14424:	9926                	add	s2,s2,s1
   14426:	feffe0ef          	jal	ra,13414 <memcpy>
   1442a:	00090823          	sb	zero,16(s2)
   1442e:	0009b783          	ld	a5,0(s3)
   14432:	f49c                	sd	a5,40(s1)
   14434:	0099b023          	sd	s1,0(s3)
   14438:	b5d1                	j	142fc <__get_locale+0xba>
   1443a:	f60a80e3          	beqz	s5,1439a <__get_locale+0x158>
   1443e:	bd75                	j	142fa <__get_locale+0xb8>

0000000000014440 <__aio_atfork>:
   14440:	8082                	ret

0000000000014442 <_Fork>:
   14442:	7135                	addi	sp,sp,-160
   14444:	850a                	mv	a0,sp
   14446:	ed06                	sd	ra,152(sp)
   14448:	e526                	sd	s1,136(sp)
   1444a:	e922                	sd	s0,144(sp)
   1444c:	9cbfe0ef          	jal	ra,12e16 <__block_all_sigs>
   14450:	557d                	li	a0,-1
   14452:	fefff0ef          	jal	ra,14440 <__aio_atfork>
   14456:	21818513          	addi	a0,gp,536 # 1ca18 <__abort_lock>
   1445a:	e4cff0ef          	jal	ra,13aa6 <__lock>
   1445e:	0dc00893          	li	a7,220
   14462:	4545                	li	a0,17
   14464:	4581                	li	a1,0
   14466:	00000073          	ecall
   1446a:	0005041b          	sext.w	s0,a0
   1446e:	ec1d                	bnez	s0,144ac <_Fork+0x6a>
   14470:	8792                	mv	a5,tp
   14472:	0b200893          	li	a7,178
   14476:	f3878793          	addi	a5,a5,-200 # 19f38 <states+0x118>
   1447a:	00000073          	ecall
   1447e:	0807b423          	sd	zero,136(a5)
   14482:	d388                	sw	a0,32(a5)
   14484:	0807b023          	sd	zero,128(a5)
   14488:	e79c                	sd	a5,8(a5)
   1448a:	eb9c                	sd	a5,16(a5)
   1448c:	2001a423          	sw	zero,520(gp) # 1ca08 <__thread_list_lock>
   14490:	18018793          	addi	a5,gp,384 # 1c980 <__libc>
   14494:	0037c703          	lbu	a4,3(a5)
   14498:	0007a223          	sw	zero,4(a5)
   1449c:	0187171b          	slliw	a4,a4,0x18
   144a0:	4187571b          	sraiw	a4,a4,0x18
   144a4:	c701                	beqz	a4,144ac <_Fork+0x6a>
   144a6:	577d                	li	a4,-1
   144a8:	00e781a3          	sb	a4,3(a5)
   144ac:	21818513          	addi	a0,gp,536 # 1ca18 <__abort_lock>
   144b0:	eceff0ef          	jal	ra,13b7e <__unlock>
   144b4:	00143513          	seqz	a0,s0
   144b8:	f89ff0ef          	jal	ra,14440 <__aio_atfork>
   144bc:	850a                	mv	a0,sp
   144be:	985fe0ef          	jal	ra,12e42 <__restore_sigs>
   144c2:	8522                	mv	a0,s0
   144c4:	b85fc0ef          	jal	ra,11048 <__syscall_ret>
   144c8:	60ea                	ld	ra,152(sp)
   144ca:	644a                	ld	s0,144(sp)
   144cc:	64aa                	ld	s1,136(sp)
   144ce:	2501                	sext.w	a0,a0
   144d0:	610d                	addi	sp,sp,160
   144d2:	8082                	ret

00000000000144d4 <a_fetch_or>:
   144d4:	872a                	mv	a4,a0
   144d6:	431c                	lw	a5,0(a4)
   144d8:	0007851b          	sext.w	a0,a5
   144dc:	00b566b3          	or	a3,a0,a1
   144e0:	160727af          	lr.w.aqrl	a5,(a4)
   144e4:	00a79563          	bne	a5,a0,144ee <a_fetch_or+0x1a>
   144e8:	1ed7262f          	sc.w.aqrl	a2,a3,(a4)
   144ec:	fa75                	bnez	a2,144e0 <a_fetch_or+0xc>
   144ee:	2781                	sext.w	a5,a5
   144f0:	fef513e3          	bne	a0,a5,144d6 <a_fetch_or+0x2>
   144f4:	8082                	ret

00000000000144f6 <__get_handler_set>:
   144f6:	4621                	li	a2,8
   144f8:	9e818593          	addi	a1,gp,-1560 # 1c1e8 <handler_set>
   144fc:	f19fe06f          	j	13414 <memcpy>

0000000000014500 <__libc_sigaction>:
   14500:	7159                	addi	sp,sp,-112
   14502:	e8ca                	sd	s2,80(sp)
   14504:	e0d2                	sd	s4,64(sp)
   14506:	f486                	sd	ra,104(sp)
   14508:	f0a2                	sd	s0,96(sp)
   1450a:	eca6                	sd	s1,88(sp)
   1450c:	e4ce                	sd	s3,72(sp)
   1450e:	8a2a                	mv	s4,a0
   14510:	8932                	mv	s2,a2
   14512:	c5d5                	beqz	a1,145be <__libc_sigaction+0xbe>
   14514:	619c                	ld	a5,0(a1)
   14516:	4485                	li	s1,1
   14518:	842e                	mv	s0,a1
   1451a:	06f4fc63          	bgeu	s1,a5,14592 <__libc_sigaction+0x92>
   1451e:	fff5099b          	addiw	s3,a0,-1
   14522:	85ce                	mv	a1,s3
   14524:	0069d993          	srli	s3,s3,0x6
   14528:	00399793          	slli	a5,s3,0x3
   1452c:	00b494b3          	sll	s1,s1,a1
   14530:	9e818993          	addi	s3,gp,-1560 # 1c1e8 <handler_set>
   14534:	0004859b          	sext.w	a1,s1
   14538:	99be                	add	s3,s3,a5
   1453a:	c581                	beqz	a1,14542 <__libc_sigaction+0x42>
   1453c:	854e                	mv	a0,s3
   1453e:	f97ff0ef          	jal	ra,144d4 <a_fetch_or>
   14542:	4204d593          	srai	a1,s1,0x20
   14546:	c589                	beqz	a1,14550 <__libc_sigaction+0x50>
   14548:	00498513          	addi	a0,s3,4
   1454c:	f89ff0ef          	jal	ra,144d4 <a_fetch_or>
   14550:	1811c783          	lbu	a5,385(gp) # 1c981 <__libc+0x1>
   14554:	e395                	bnez	a5,14578 <__libc_sigaction+0x78>
   14556:	9f018793          	addi	a5,gp,-1552 # 1c1f0 <unmask_done>
   1455a:	4398                	lw	a4,0(a5)
   1455c:	ef11                	bnez	a4,14578 <__libc_sigaction+0x78>
   1455e:	470d                	li	a4,3
   14560:	1702                	slli	a4,a4,0x20
   14562:	f03a                	sd	a4,32(sp)
   14564:	08700893          	li	a7,135
   14568:	4505                	li	a0,1
   1456a:	100c                	addi	a1,sp,32
   1456c:	4601                	li	a2,0
   1456e:	46a1                	li	a3,8
   14570:	00000073          	ecall
   14574:	4705                	li	a4,1
   14576:	c398                	sw	a4,0(a5)
   14578:	08842783          	lw	a5,136(s0)
   1457c:	02379713          	slli	a4,a5,0x23
   14580:	00074963          	bltz	a4,14592 <__libc_sigaction+0x92>
   14584:	0330000f          	fence	rw,rw
   14588:	4705                	li	a4,1
   1458a:	20e1aa23          	sw	a4,532(gp) # 1ca14 <__eintr_valid_flag>
   1458e:	0330000f          	fence	rw,rw
   14592:	601c                	ld	a5,0(s0)
   14594:	04000737          	lui	a4,0x4000
   14598:	e03e                	sd	a5,0(sp)
   1459a:	08842783          	lw	a5,136(s0)
   1459e:	8f5d                	or	a4,a4,a5
   145a0:	e43a                	sd	a4,8(sp)
   145a2:	8b91                	andi	a5,a5,4
   145a4:	e7ad                	bnez	a5,1460e <__libc_sigaction+0x10e>
   145a6:	67d9                	lui	a5,0x16
   145a8:	3c678793          	addi	a5,a5,966 # 163c6 <__restore>
   145ac:	00840593          	addi	a1,s0,8
   145b0:	4621                	li	a2,8
   145b2:	0828                	addi	a0,sp,24
   145b4:	e83e                	sd	a5,16(sp)
   145b6:	e5ffe0ef          	jal	ra,13414 <memcpy>
   145ba:	858a                	mv	a1,sp
   145bc:	e011                	bnez	s0,145c0 <__libc_sigaction+0xc0>
   145be:	4581                	li	a1,0
   145c0:	4601                	li	a2,0
   145c2:	00090363          	beqz	s2,145c8 <__libc_sigaction+0xc8>
   145c6:	1010                	addi	a2,sp,32
   145c8:	08600893          	li	a7,134
   145cc:	8552                	mv	a0,s4
   145ce:	46a1                	li	a3,8
   145d0:	00000073          	ecall
   145d4:	0005041b          	sext.w	s0,a0
   145d8:	00090f63          	beqz	s2,145f6 <__libc_sigaction+0xf6>
   145dc:	ec09                	bnez	s0,145f6 <__libc_sigaction+0xf6>
   145de:	7782                	ld	a5,32(sp)
   145e0:	4621                	li	a2,8
   145e2:	182c                	addi	a1,sp,56
   145e4:	00f93023          	sd	a5,0(s2)
   145e8:	77a2                	ld	a5,40(sp)
   145ea:	00890513          	addi	a0,s2,8
   145ee:	08f92423          	sw	a5,136(s2)
   145f2:	e23fe0ef          	jal	ra,13414 <memcpy>
   145f6:	8522                	mv	a0,s0
   145f8:	a51fc0ef          	jal	ra,11048 <__syscall_ret>
   145fc:	70a6                	ld	ra,104(sp)
   145fe:	7406                	ld	s0,96(sp)
   14600:	64e6                	ld	s1,88(sp)
   14602:	6946                	ld	s2,80(sp)
   14604:	69a6                	ld	s3,72(sp)
   14606:	6a06                	ld	s4,64(sp)
   14608:	2501                	sext.w	a0,a0
   1460a:	6165                	addi	sp,sp,112
   1460c:	8082                	ret
   1460e:	67d9                	lui	a5,0x16
   14610:	3c678793          	addi	a5,a5,966 # 163c6 <__restore>
   14614:	bf61                	j	145ac <__libc_sigaction+0xac>

0000000000014616 <__sigaction>:
   14616:	7139                	addi	sp,sp,-64
   14618:	fc06                	sd	ra,56(sp)
   1461a:	f822                	sd	s0,48(sp)
   1461c:	f426                	sd	s1,40(sp)
   1461e:	fe05069b          	addiw	a3,a0,-32
   14622:	4709                	li	a4,2
   14624:	00d77963          	bgeu	a4,a3,14636 <__sigaction+0x20>
   14628:	0005079b          	sext.w	a5,a0
   1462c:	37fd                	addiw	a5,a5,-1
   1462e:	03f00713          	li	a4,63
   14632:	00f77d63          	bgeu	a4,a5,1464c <__sigaction+0x36>
   14636:	937fc0ef          	jal	ra,10f6c <__errno_location>
   1463a:	47d9                	li	a5,22
   1463c:	c11c                	sw	a5,0(a0)
   1463e:	54fd                	li	s1,-1
   14640:	70e2                	ld	ra,56(sp)
   14642:	7442                	ld	s0,48(sp)
   14644:	8526                	mv	a0,s1
   14646:	74a2                	ld	s1,40(sp)
   14648:	6121                	addi	sp,sp,64
   1464a:	8082                	ret
   1464c:	4799                	li	a5,6
   1464e:	842a                	mv	s0,a0
   14650:	00f51d63          	bne	a0,a5,1466a <__sigaction+0x54>
   14654:	0828                	addi	a0,sp,24
   14656:	e432                	sd	a2,8(sp)
   14658:	e02e                	sd	a1,0(sp)
   1465a:	fbcfe0ef          	jal	ra,12e16 <__block_all_sigs>
   1465e:	21818513          	addi	a0,gp,536 # 1ca18 <__abort_lock>
   14662:	c44ff0ef          	jal	ra,13aa6 <__lock>
   14666:	6622                	ld	a2,8(sp)
   14668:	6582                	ld	a1,0(sp)
   1466a:	8522                	mv	a0,s0
   1466c:	e95ff0ef          	jal	ra,14500 <__libc_sigaction>
   14670:	4799                	li	a5,6
   14672:	84aa                	mv	s1,a0
   14674:	fcf416e3          	bne	s0,a5,14640 <__sigaction+0x2a>
   14678:	21818513          	addi	a0,gp,536 # 1ca18 <__abort_lock>
   1467c:	d02ff0ef          	jal	ra,13b7e <__unlock>
   14680:	0828                	addi	a0,sp,24
   14682:	fc0fe0ef          	jal	ra,12e42 <__restore_sigs>
   14686:	bf6d                	j	14640 <__sigaction+0x2a>

0000000000014688 <__overflow>:
   14688:	711c                	ld	a5,32(a0)
   1468a:	1101                	addi	sp,sp,-32
   1468c:	e822                	sd	s0,16(sp)
   1468e:	ec06                	sd	ra,24(sp)
   14690:	00b107a3          	sb	a1,15(sp)
   14694:	842a                	mv	s0,a0
   14696:	c38d                	beqz	a5,146b8 <__overflow+0x30>
   14698:	741c                	ld	a5,40(s0)
   1469a:	7018                	ld	a4,32(s0)
   1469c:	02e79663          	bne	a5,a4,146c8 <__overflow+0x40>
   146a0:	643c                	ld	a5,72(s0)
   146a2:	4605                	li	a2,1
   146a4:	00f10593          	addi	a1,sp,15
   146a8:	8522                	mv	a0,s0
   146aa:	9782                	jalr	a5
   146ac:	4785                	li	a5,1
   146ae:	00f51863          	bne	a0,a5,146be <__overflow+0x36>
   146b2:	00f14503          	lbu	a0,15(sp)
   146b6:	a029                	j	146c0 <__overflow+0x38>
   146b8:	0fa000ef          	jal	ra,147b2 <__towrite>
   146bc:	dd71                	beqz	a0,14698 <__overflow+0x10>
   146be:	557d                	li	a0,-1
   146c0:	60e2                	ld	ra,24(sp)
   146c2:	6442                	ld	s0,16(sp)
   146c4:	6105                	addi	sp,sp,32
   146c6:	8082                	ret
   146c8:	00f14703          	lbu	a4,15(sp)
   146cc:	09042683          	lw	a3,144(s0)
   146d0:	0007051b          	sext.w	a0,a4
   146d4:	fce686e3          	beq	a3,a4,146a0 <__overflow+0x18>
   146d8:	00178693          	addi	a3,a5,1
   146dc:	f414                	sd	a3,40(s0)
   146de:	00e78023          	sb	a4,0(a5)
   146e2:	bff9                	j	146c0 <__overflow+0x38>

00000000000146e4 <__aio_close>:
   146e4:	8082                	ret

00000000000146e6 <__stdio_close>:
   146e6:	5d28                	lw	a0,120(a0)
   146e8:	1141                	addi	sp,sp,-16
   146ea:	e406                	sd	ra,8(sp)
   146ec:	ff9ff0ef          	jal	ra,146e4 <__aio_close>
   146f0:	03900893          	li	a7,57
   146f4:	00000073          	ecall
   146f8:	951fc0ef          	jal	ra,11048 <__syscall_ret>
   146fc:	60a2                	ld	ra,8(sp)
   146fe:	2501                	sext.w	a0,a0
   14700:	0141                	addi	sp,sp,16
   14702:	8082                	ret

0000000000014704 <__stdio_seek>:
   14704:	5d28                	lw	a0,120(a0)
   14706:	19f0106f          	j	160a4 <__lseek>

000000000001470a <__stdio_write>:
   1470a:	715d                	addi	sp,sp,-80
   1470c:	7d1c                	ld	a5,56(a0)
   1470e:	f44e                	sd	s3,40(sp)
   14710:	02853983          	ld	s3,40(a0)
   14714:	e0a2                	sd	s0,64(sp)
   14716:	fc26                	sd	s1,56(sp)
   14718:	40f989b3          	sub	s3,s3,a5
   1471c:	f84a                	sd	s2,48(sp)
   1471e:	f052                	sd	s4,32(sp)
   14720:	e44e                	sd	s3,8(sp)
   14722:	e486                	sd	ra,72(sp)
   14724:	842a                	mv	s0,a0
   14726:	8932                	mv	s2,a2
   14728:	e03e                	sd	a5,0(sp)
   1472a:	e82e                	sd	a1,16(sp)
   1472c:	ec32                	sd	a2,24(sp)
   1472e:	99b2                	add	s3,s3,a2
   14730:	4a09                	li	s4,2
   14732:	848a                	mv	s1,sp
   14734:	5c28                	lw	a0,120(s0)
   14736:	04200893          	li	a7,66
   1473a:	85a6                	mv	a1,s1
   1473c:	8652                	mv	a2,s4
   1473e:	00000073          	ecall
   14742:	907fc0ef          	jal	ra,11048 <__syscall_ret>
   14746:	02a99163          	bne	s3,a0,14768 <__stdio_write+0x5e>
   1474a:	6c3c                	ld	a5,88(s0)
   1474c:	7038                	ld	a4,96(s0)
   1474e:	fc1c                	sd	a5,56(s0)
   14750:	973e                	add	a4,a4,a5
   14752:	f018                	sd	a4,32(s0)
   14754:	f41c                	sd	a5,40(s0)
   14756:	60a6                	ld	ra,72(sp)
   14758:	6406                	ld	s0,64(sp)
   1475a:	74e2                	ld	s1,56(sp)
   1475c:	79a2                	ld	s3,40(sp)
   1475e:	7a02                	ld	s4,32(sp)
   14760:	854a                	mv	a0,s2
   14762:	7942                	ld	s2,48(sp)
   14764:	6161                	addi	sp,sp,80
   14766:	8082                	ret
   14768:	02055363          	bgez	a0,1478e <__stdio_write+0x84>
   1476c:	401c                	lw	a5,0(s0)
   1476e:	02043023          	sd	zero,32(s0)
   14772:	02043c23          	sd	zero,56(s0)
   14776:	0207e793          	ori	a5,a5,32
   1477a:	c01c                	sw	a5,0(s0)
   1477c:	02043423          	sd	zero,40(s0)
   14780:	4789                	li	a5,2
   14782:	02fa0663          	beq	s4,a5,147ae <__stdio_write+0xa4>
   14786:	649c                	ld	a5,8(s1)
   14788:	40f90933          	sub	s2,s2,a5
   1478c:	b7e9                	j	14756 <__stdio_write+0x4c>
   1478e:	649c                	ld	a5,8(s1)
   14790:	40a989b3          	sub	s3,s3,a0
   14794:	00a7f563          	bgeu	a5,a0,1479e <__stdio_write+0x94>
   14798:	8d1d                	sub	a0,a0,a5
   1479a:	04c1                	addi	s1,s1,16
   1479c:	3a7d                	addiw	s4,s4,-1
   1479e:	609c                	ld	a5,0(s1)
   147a0:	97aa                	add	a5,a5,a0
   147a2:	e09c                	sd	a5,0(s1)
   147a4:	649c                	ld	a5,8(s1)
   147a6:	40a78533          	sub	a0,a5,a0
   147aa:	e488                	sd	a0,8(s1)
   147ac:	b761                	j	14734 <__stdio_write+0x2a>
   147ae:	4901                	li	s2,0
   147b0:	b75d                	j	14756 <__stdio_write+0x4c>

00000000000147b2 <__towrite>:
   147b2:	08852783          	lw	a5,136(a0)
   147b6:	fff7871b          	addiw	a4,a5,-1
   147ba:	8fd9                	or	a5,a5,a4
   147bc:	08f52423          	sw	a5,136(a0)
   147c0:	411c                	lw	a5,0(a0)
   147c2:	0087f713          	andi	a4,a5,8
   147c6:	c711                	beqz	a4,147d2 <__towrite+0x20>
   147c8:	0207e793          	ori	a5,a5,32
   147cc:	c11c                	sw	a5,0(a0)
   147ce:	557d                	li	a0,-1
   147d0:	8082                	ret
   147d2:	6d3c                	ld	a5,88(a0)
   147d4:	7138                	ld	a4,96(a0)
   147d6:	00053823          	sd	zero,16(a0)
   147da:	fd1c                	sd	a5,56(a0)
   147dc:	f51c                	sd	a5,40(a0)
   147de:	97ba                	add	a5,a5,a4
   147e0:	00053423          	sd	zero,8(a0)
   147e4:	f11c                	sd	a5,32(a0)
   147e6:	4501                	li	a0,0
   147e8:	8082                	ret

00000000000147ea <__towrite_needs_stdio_exit>:
   147ea:	4350106f          	j	1641e <__stdio_exit>

00000000000147ee <pop_arg>:
   147ee:	35dd                	addiw	a1,a1,-9
   147f0:	0005871b          	sext.w	a4,a1
   147f4:	47c5                	li	a5,17
   147f6:	08e7ee63          	bltu	a5,a4,14892 <pop_arg+0xa4>
   147fa:	1582                	slli	a1,a1,0x20
   147fc:	67e9                	lui	a5,0x1a
   147fe:	bfc78793          	addi	a5,a5,-1028 # 19bfc <__fsmu8+0xcc>
   14802:	81f9                	srli	a1,a1,0x1e
   14804:	95be                	add	a1,a1,a5
   14806:	4198                	lw	a4,0(a1)
   14808:	1141                	addi	sp,sp,-16
   1480a:	e022                	sd	s0,0(sp)
   1480c:	621c                	ld	a5,0(a2)
   1480e:	e406                	sd	ra,8(sp)
   14810:	842a                	mv	s0,a0
   14812:	8702                	jr	a4
   14814:	00878713          	addi	a4,a5,8
   14818:	439c                	lw	a5,0(a5)
   1481a:	e218                	sd	a4,0(a2)
   1481c:	e01c                	sd	a5,0(s0)
   1481e:	60a2                	ld	ra,8(sp)
   14820:	6402                	ld	s0,0(sp)
   14822:	0141                	addi	sp,sp,16
   14824:	8082                	ret
   14826:	00878713          	addi	a4,a5,8
   1482a:	e218                	sd	a4,0(a2)
   1482c:	0007e783          	lwu	a5,0(a5)
   14830:	b7f5                	j	1481c <pop_arg+0x2e>
   14832:	00878713          	addi	a4,a5,8
   14836:	e218                	sd	a4,0(a2)
   14838:	00079783          	lh	a5,0(a5)
   1483c:	b7c5                	j	1481c <pop_arg+0x2e>
   1483e:	00878713          	addi	a4,a5,8
   14842:	e218                	sd	a4,0(a2)
   14844:	0007d783          	lhu	a5,0(a5)
   14848:	bfd1                	j	1481c <pop_arg+0x2e>
   1484a:	00878713          	addi	a4,a5,8
   1484e:	e218                	sd	a4,0(a2)
   14850:	00078783          	lb	a5,0(a5)
   14854:	b7e1                	j	1481c <pop_arg+0x2e>
   14856:	00878713          	addi	a4,a5,8
   1485a:	e218                	sd	a4,0(a2)
   1485c:	0007c783          	lbu	a5,0(a5)
   14860:	bf75                	j	1481c <pop_arg+0x2e>
   14862:	00878713          	addi	a4,a5,8
   14866:	e218                	sd	a4,0(a2)
   14868:	639c                	ld	a5,0(a5)
   1486a:	bf4d                	j	1481c <pop_arg+0x2e>
   1486c:	2388                	fld	fa0,0(a5)
   1486e:	00878713          	addi	a4,a5,8
   14872:	e218                	sd	a4,0(a2)
   14874:	49d030ef          	jal	ra,18510 <__extenddftf2>
   14878:	e008                	sd	a0,0(s0)
   1487a:	e40c                	sd	a1,8(s0)
   1487c:	b74d                	j	1481e <pop_arg+0x30>
   1487e:	07bd                	addi	a5,a5,15
   14880:	9bc1                	andi	a5,a5,-16
   14882:	01078713          	addi	a4,a5,16
   14886:	e218                	sd	a4,0(a2)
   14888:	6398                	ld	a4,0(a5)
   1488a:	679c                	ld	a5,8(a5)
   1488c:	e118                	sd	a4,0(a0)
   1488e:	e51c                	sd	a5,8(a0)
   14890:	b779                	j	1481e <pop_arg+0x30>
   14892:	8082                	ret

0000000000014894 <fmt_u>:
   14894:	87aa                	mv	a5,a0
   14896:	46a9                	li	a3,10
   14898:	852e                	mv	a0,a1
   1489a:	e391                	bnez	a5,1489e <fmt_u+0xa>
   1489c:	8082                	ret
   1489e:	02d7f733          	remu	a4,a5,a3
   148a2:	157d                	addi	a0,a0,-1
   148a4:	0307071b          	addiw	a4,a4,48
   148a8:	02d7d7b3          	divu	a5,a5,a3
   148ac:	00e50023          	sb	a4,0(a0)
   148b0:	b7ed                	j	1489a <fmt_u+0x6>

00000000000148b2 <getint>:
   148b2:	0cccd5b7          	lui	a1,0xcccd
   148b6:	80000837          	lui	a6,0x80000
   148ba:	862a                	mv	a2,a0
   148bc:	4325                	li	t1,9
   148be:	4501                	li	a0,0
   148c0:	ccc58593          	addi	a1,a1,-820 # ccccccc <__BSS_END__+0xccb02ac>
   148c4:	5e59                	li	t3,-10
   148c6:	fff84813          	not	a6,a6
   148ca:	4ea9                	li	t4,10
   148cc:	6214                	ld	a3,0(a2)
   148ce:	0006c703          	lbu	a4,0(a3) # 800000 <__BSS_END__+0x7e35e0>
   148d2:	fd07089b          	addiw	a7,a4,-48
   148d6:	8746                	mv	a4,a7
   148d8:	01137363          	bgeu	t1,a7,148de <getint+0x2c>
   148dc:	8082                	ret
   148de:	00a5ef63          	bltu	a1,a0,148fc <getint+0x4a>
   148e2:	02ae07bb          	mulw	a5,t3,a0
   148e6:	010787bb          	addw	a5,a5,a6
   148ea:	0117c963          	blt	a5,a7,148fc <getint+0x4a>
   148ee:	02ae87bb          	mulw	a5,t4,a0
   148f2:	00f7053b          	addw	a0,a4,a5
   148f6:	0685                	addi	a3,a3,1
   148f8:	e214                	sd	a3,0(a2)
   148fa:	bfc9                	j	148cc <getint+0x1a>
   148fc:	557d                	li	a0,-1
   148fe:	bfe5                	j	148f6 <getint+0x44>

0000000000014900 <out>:
   14900:	87aa                	mv	a5,a0
   14902:	4398                	lw	a4,0(a5)
   14904:	852e                	mv	a0,a1
   14906:	85b2                	mv	a1,a2
   14908:	02077713          	andi	a4,a4,32
   1490c:	e701                	bnez	a4,14914 <out+0x14>
   1490e:	863e                	mv	a2,a5
   14910:	f56fe06f          	j	13066 <__fwritex>
   14914:	8082                	ret

0000000000014916 <pad>:
   14916:	67c9                	lui	a5,0x12
   14918:	8f7d                	and	a4,a4,a5
   1491a:	e73d                	bnez	a4,14988 <pad+0x72>
   1491c:	06c6d663          	bge	a3,a2,14988 <pad+0x72>
   14920:	7169                	addi	sp,sp,-304
   14922:	f222                	sd	s0,288(sp)
   14924:	ee26                	sd	s1,280(sp)
   14926:	ea4a                	sd	s2,272(sp)
   14928:	40d604bb          	subw	s1,a2,a3
   1492c:	f606                	sd	ra,296(sp)
   1492e:	e64e                	sd	s3,264(sp)
   14930:	10000793          	li	a5,256
   14934:	892a                	mv	s2,a0
   14936:	8426                	mv	s0,s1
   14938:	0004861b          	sext.w	a2,s1
   1493c:	0097d463          	bge	a5,s1,14944 <pad+0x2e>
   14940:	10000613          	li	a2,256
   14944:	850a                	mv	a0,sp
   14946:	f17fe0ef          	jal	ra,1385c <memset>
   1494a:	0ff00993          	li	s3,255
   1494e:	0299c463          	blt	s3,s1,14976 <pad+0x60>
   14952:	0084561b          	srliw	a2,s0,0x8
   14956:	f0000793          	li	a5,-256
   1495a:	02f6063b          	mulw	a2,a2,a5
   1495e:	858a                	mv	a1,sp
   14960:	854a                	mv	a0,s2
   14962:	9e21                	addw	a2,a2,s0
   14964:	f9dff0ef          	jal	ra,14900 <out>
   14968:	70b2                	ld	ra,296(sp)
   1496a:	7412                	ld	s0,288(sp)
   1496c:	64f2                	ld	s1,280(sp)
   1496e:	6952                	ld	s2,272(sp)
   14970:	69b2                	ld	s3,264(sp)
   14972:	6155                	addi	sp,sp,304
   14974:	8082                	ret
   14976:	10000613          	li	a2,256
   1497a:	858a                	mv	a1,sp
   1497c:	854a                	mv	a0,s2
   1497e:	f83ff0ef          	jal	ra,14900 <out>
   14982:	f004849b          	addiw	s1,s1,-256
   14986:	b7e1                	j	1494e <pad+0x38>
   14988:	8082                	ret

000000000001498a <fmt_fp>:
   1498a:	72f9                	lui	t0,0xffffe
   1498c:	7119                	addi	sp,sp,-128
   1498e:	2c028293          	addi	t0,t0,704 # ffffffffffffe2c0 <__BSS_END__+0xfffffffffffe18a0>
   14992:	f8a2                	sd	s0,112(sp)
   14994:	f4a6                	sd	s1,104(sp)
   14996:	ecce                	sd	s3,88(sp)
   14998:	e8d2                	sd	s4,80(sp)
   1499a:	e4d6                	sd	s5,72(sp)
   1499c:	fc5e                	sd	s7,56(sp)
   1499e:	fc86                	sd	ra,120(sp)
   149a0:	f0ca                	sd	s2,96(sp)
   149a2:	e0da                	sd	s6,64(sp)
   149a4:	f862                	sd	s8,48(sp)
   149a6:	f466                	sd	s9,40(sp)
   149a8:	f06a                	sd	s10,32(sp)
   149aa:	ec6e                	sd	s11,24(sp)
   149ac:	843a                	mv	s0,a4
   149ae:	9116                	add	sp,sp,t0
   149b0:	6709                	lui	a4,0x2
   149b2:	e036                	sd	a3,0(sp)
   149b4:	d1070713          	addi	a4,a4,-752 # 1d10 <exit-0xe410>
   149b8:	0094                	addi	a3,sp,64
   149ba:	8bbe                	mv	s7,a5
   149bc:	9736                	add	a4,a4,a3
   149be:	77f9                	lui	a5,0xffffe
   149c0:	97ba                	add	a5,a5,a4
   149c2:	84aa                	mv	s1,a0
   149c4:	8aae                	mv	s5,a1
   149c6:	852e                	mv	a0,a1
   149c8:	85b2                	mv	a1,a2
   149ca:	8a32                	mv	s4,a2
   149cc:	89c2                	mv	s3,a6
   149ce:	e83e                	sd	a5,16(sp)
   149d0:	2e07ae23          	sw	zero,764(a5) # ffffffffffffe2fc <__BSS_END__+0xfffffffffffe18dc>
   149d4:	14b010ef          	jal	ra,1631e <__signbitl>
   149d8:	cd49                	beqz	a0,14a72 <fmt_fp+0xe8>
   149da:	57fd                	li	a5,-1
   149dc:	17fe                	slli	a5,a5,0x3f
   149de:	6969                	lui	s2,0x1a
   149e0:	00fa4a33          	xor	s4,s4,a5
   149e4:	4c05                	li	s8,1
   149e6:	c4890913          	addi	s2,s2,-952 # 19c48 <__fsmu8+0x118>
   149ea:	8556                	mv	a0,s5
   149ec:	85d2                	mv	a1,s4
   149ee:	107010ef          	jal	ra,162f4 <__fpclassifyl>
   149f2:	4785                	li	a5,1
   149f4:	0aa7ce63          	blt	a5,a0,14ab0 <fmt_fp+0x126>
   149f8:	0209f993          	andi	s3,s3,32
   149fc:	00098c9b          	sext.w	s9,s3
   14a00:	0a099063          	bnez	s3,14aa0 <fmt_fp+0x116>
   14a04:	6469                	lui	s0,0x1a
   14a06:	c6840413          	addi	s0,s0,-920 # 19c68 <__fsmu8+0x138>
   14a0a:	8656                	mv	a2,s5
   14a0c:	86d2                	mv	a3,s4
   14a0e:	8556                	mv	a0,s5
   14a10:	85d2                	mv	a1,s4
   14a12:	776020ef          	jal	ra,17188 <__eqtf2>
   14a16:	c511                	beqz	a0,14a22 <fmt_fp+0x98>
   14a18:	080c9863          	bnez	s9,14aa8 <fmt_fp+0x11e>
   14a1c:	6469                	lui	s0,0x1a
   14a1e:	c7840413          	addi	s0,s0,-904 # 19c78 <__fsmu8+0x148>
   14a22:	6602                	ld	a2,0(sp)
   14a24:	7741                	lui	a4,0xffff0
   14a26:	003c0a1b          	addiw	s4,s8,3
   14a2a:	177d                	addi	a4,a4,-1
   14a2c:	00ebf733          	and	a4,s7,a4
   14a30:	86d2                	mv	a3,s4
   14a32:	02000593          	li	a1,32
   14a36:	8526                	mv	a0,s1
   14a38:	edfff0ef          	jal	ra,14916 <pad>
   14a3c:	8662                	mv	a2,s8
   14a3e:	85ca                	mv	a1,s2
   14a40:	8526                	mv	a0,s1
   14a42:	ebfff0ef          	jal	ra,14900 <out>
   14a46:	460d                	li	a2,3
   14a48:	85a2                	mv	a1,s0
   14a4a:	8526                	mv	a0,s1
   14a4c:	eb5ff0ef          	jal	ra,14900 <out>
   14a50:	6602                	ld	a2,0(sp)
   14a52:	6709                	lui	a4,0x2
   14a54:	00ebc733          	xor	a4,s7,a4
   14a58:	86d2                	mv	a3,s4
   14a5a:	02000593          	li	a1,32
   14a5e:	8526                	mv	a0,s1
   14a60:	eb7ff0ef          	jal	ra,14916 <pad>
   14a64:	6782                	ld	a5,0(sp)
   14a66:	89d2                	mv	s3,s4
   14a68:	1cfa55e3          	bge	s4,a5,15432 <fmt_fp+0xaa8>
   14a6c:	4982                	lw	s3,0(sp)
   14a6e:	1c50006f          	j	15432 <fmt_fp+0xaa8>
   14a72:	034b9713          	slli	a4,s7,0x34
   14a76:	000b879b          	sext.w	a5,s7
   14a7a:	00074963          	bltz	a4,14a8c <fmt_fp+0x102>
   14a7e:	8b85                	andi	a5,a5,1
   14a80:	eb99                	bnez	a5,14a96 <fmt_fp+0x10c>
   14a82:	6969                	lui	s2,0x1a
   14a84:	8c2a                	mv	s8,a0
   14a86:	c4990913          	addi	s2,s2,-951 # 19c49 <__fsmu8+0x119>
   14a8a:	b785                	j	149ea <fmt_fp+0x60>
   14a8c:	6969                	lui	s2,0x1a
   14a8e:	4c05                	li	s8,1
   14a90:	c4b90913          	addi	s2,s2,-949 # 19c4b <__fsmu8+0x11b>
   14a94:	bf99                	j	149ea <fmt_fp+0x60>
   14a96:	6969                	lui	s2,0x1a
   14a98:	4c05                	li	s8,1
   14a9a:	c4e90913          	addi	s2,s2,-946 # 19c4e <__fsmu8+0x11e>
   14a9e:	b7b1                	j	149ea <fmt_fp+0x60>
   14aa0:	6469                	lui	s0,0x1a
   14aa2:	c6040413          	addi	s0,s0,-928 # 19c60 <__fsmu8+0x130>
   14aa6:	b795                	j	14a0a <fmt_fp+0x80>
   14aa8:	6469                	lui	s0,0x1a
   14aaa:	c7040413          	addi	s0,s0,-912 # 19c70 <__fsmu8+0x140>
   14aae:	bf95                	j	14a22 <fmt_fp+0x98>
   14ab0:	6c89                	lui	s9,0x2
   14ab2:	0098                	addi	a4,sp,64
   14ab4:	7d79                	lui	s10,0xffffe
   14ab6:	d10c8793          	addi	a5,s9,-752 # 1d10 <exit-0xe410>
   14aba:	97ba                	add	a5,a5,a4
   14abc:	2fcd0613          	addi	a2,s10,764 # ffffffffffffe2fc <__BSS_END__+0xfffffffffffe18dc>
   14ac0:	963e                	add	a2,a2,a5
   14ac2:	8556                	mv	a0,s5
   14ac4:	85d2                	mv	a1,s4
   14ac6:	05f010ef          	jal	ra,16324 <frexpl>
   14aca:	862a                	mv	a2,a0
   14acc:	86ae                	mv	a3,a1
   14ace:	4bd010ef          	jal	ra,1678a <__addtf3>
   14ad2:	4601                	li	a2,0
   14ad4:	4681                	li	a3,0
   14ad6:	8daa                	mv	s11,a0
   14ad8:	8b2e                	mv	s6,a1
   14ada:	8aaa                	mv	s5,a0
   14adc:	8a2e                	mv	s4,a1
   14ade:	6aa020ef          	jal	ra,17188 <__eqtf2>
   14ae2:	cd01                	beqz	a0,14afa <fmt_fp+0x170>
   14ae4:	d10c8793          	addi	a5,s9,-752
   14ae8:	0098                	addi	a4,sp,64
   14aea:	97ba                	add	a5,a5,a4
   14aec:	97ea                	add	a5,a5,s10
   14aee:	2fc7a683          	lw	a3,764(a5)
   14af2:	e83e                	sd	a5,16(sp)
   14af4:	36fd                	addiw	a3,a3,-1
   14af6:	2ed7ae23          	sw	a3,764(a5)
   14afa:	0209ec93          	ori	s9,s3,32
   14afe:	06100693          	li	a3,97
   14b02:	2cdc9b63          	bne	s9,a3,14dd8 <fmt_fp+0x44e>
   14b06:	0209fd13          	andi	s10,s3,32
   14b0a:	000d0363          	beqz	s10,14b10 <fmt_fp+0x186>
   14b0e:	0925                	addi	s2,s2,9
   14b10:	002c079b          	addiw	a5,s8,2
   14b14:	c83e                	sw	a5,16(sp)
   14b16:	e43e                	sd	a5,8(sp)
   14b18:	0004079b          	sext.w	a5,s0
   14b1c:	f03e                	sd	a5,32(sp)
   14b1e:	46e9                	li	a3,26
   14b20:	6ce9                	lui	s9,0x1a
   14b22:	0486e463          	bltu	a3,s0,14b6a <fmt_fp+0x1e0>
   14b26:	da8cba83          	ld	s5,-600(s9) # 19da8 <__fsmu8+0x278>
   14b2a:	4a6d                	li	s4,27
   14b2c:	408a0a3b          	subw	s4,s4,s0
   14b30:	4c01                	li	s8,0
   14b32:	587d                	li	a6,-1
   14b34:	86d6                	mv	a3,s5
   14b36:	3a7d                	addiw	s4,s4,-1
   14b38:	1b0a1063          	bne	s4,a6,14cd8 <fmt_fp+0x34e>
   14b3c:	00094603          	lbu	a2,0(s2)
   14b40:	02d00693          	li	a3,45
   14b44:	1ad61763          	bne	a2,a3,14cf2 <fmt_fp+0x368>
   14b48:	1a7e                	slli	s4,s4,0x3f
   14b4a:	86d6                	mv	a3,s5
   14b4c:	8662                	mv	a2,s8
   14b4e:	014b45b3          	xor	a1,s6,s4
   14b52:	856e                	mv	a0,s11
   14b54:	56d020ef          	jal	ra,178c0 <__subtf3>
   14b58:	862a                	mv	a2,a0
   14b5a:	86ae                	mv	a3,a1
   14b5c:	8562                	mv	a0,s8
   14b5e:	85d6                	mv	a1,s5
   14b60:	42b010ef          	jal	ra,1678a <__addtf3>
   14b64:	8aaa                	mv	s5,a0
   14b66:	0145ca33          	xor	s4,a1,s4
   14b6a:	6689                	lui	a3,0x2
   14b6c:	77f9                	lui	a5,0xffffe
   14b6e:	0090                	addi	a2,sp,64
   14b70:	d1068713          	addi	a4,a3,-752 # 1d10 <exit-0xe410>
   14b74:	30078c13          	addi	s8,a5,768 # ffffffffffffe300 <__BSS_END__+0xfffffffffffe18e0>
   14b78:	9732                	add	a4,a4,a2
   14b7a:	9c3a                	add	s8,s8,a4
   14b7c:	d1068713          	addi	a4,a3,-752
   14b80:	9732                	add	a4,a4,a2
   14b82:	97ba                	add	a5,a5,a4
   14b84:	ec3e                	sd	a5,24(sp)
   14b86:	2fc7a783          	lw	a5,764(a5)
   14b8a:	00cc0b13          	addi	s6,s8,12
   14b8e:	85da                	mv	a1,s6
   14b90:	41f7d51b          	sraiw	a0,a5,0x1f
   14b94:	8fa9                	xor	a5,a5,a0
   14b96:	40a7853b          	subw	a0,a5,a0
   14b9a:	cfbff0ef          	jal	ra,14894 <fmt_u>
   14b9e:	01651963          	bne	a0,s6,14bb0 <fmt_fp+0x226>
   14ba2:	6762                	ld	a4,24(sp)
   14ba4:	03000793          	li	a5,48
   14ba8:	00bc0513          	addi	a0,s8,11
   14bac:	30f705a3          	sb	a5,779(a4) # 230b <exit-0xde15>
   14bb0:	6709                	lui	a4,0x2
   14bb2:	d1070713          	addi	a4,a4,-752 # 1d10 <exit-0xe410>
   14bb6:	0094                	addi	a3,sp,64
   14bb8:	9736                	add	a4,a4,a3
   14bba:	77f9                	lui	a5,0xffffe
   14bbc:	97ba                	add	a5,a5,a4
   14bbe:	2fc7a703          	lw	a4,764(a5) # ffffffffffffe2fc <__BSS_END__+0xfffffffffffe18dc>
   14bc2:	ec3e                	sd	a5,24(sp)
   14bc4:	02d00793          	li	a5,45
   14bc8:	00074463          	bltz	a4,14bd0 <fmt_fp+0x246>
   14bcc:	02b00793          	li	a5,43
   14bd0:	6709                	lui	a4,0x2
   14bd2:	fef50fa3          	sb	a5,-1(a0)
   14bd6:	d1070713          	addi	a4,a4,-752 # 1d10 <exit-0xe410>
   14bda:	77f9                	lui	a5,0xffffe
   14bdc:	0094                	addi	a3,sp,64
   14bde:	31078793          	addi	a5,a5,784 # ffffffffffffe310 <__BSS_END__+0xfffffffffffe18f0>
   14be2:	9736                	add	a4,a4,a3
   14be4:	00f70b33          	add	s6,a4,a5
   14be8:	6769                	lui	a4,0x1a
   14bea:	ff070793          	addi	a5,a4,-16 # 19ff0 <xdigits>
   14bee:	ec3e                	sd	a5,24(sp)
   14bf0:	da8cb783          	ld	a5,-600(s9)
   14bf4:	29bd                	addiw	s3,s3,15
   14bf6:	008bfd93          	andi	s11,s7,8
   14bfa:	ffe50c13          	addi	s8,a0,-2
   14bfe:	ff350f23          	sb	s3,-2(a0)
   14c02:	f83e                	sd	a5,48(sp)
   14c04:	f45a                	sd	s6,40(sp)
   14c06:	2d81                	sext.w	s11,s11
   14c08:	85d2                	mv	a1,s4
   14c0a:	8556                	mv	a0,s5
   14c0c:	72e030ef          	jal	ra,1833a <__fixtfsi>
   14c10:	67e2                	ld	a5,24(sp)
   14c12:	2501                	sext.w	a0,a0
   14c14:	001b0993          	addi	s3,s6,1
   14c18:	00a78733          	add	a4,a5,a0
   14c1c:	00074703          	lbu	a4,0(a4)
   14c20:	00ed6733          	or	a4,s10,a4
   14c24:	00eb0023          	sb	a4,0(s6)
   14c28:	035030ef          	jal	ra,1845c <__floatsitf>
   14c2c:	862a                	mv	a2,a0
   14c2e:	86ae                	mv	a3,a1
   14c30:	8556                	mv	a0,s5
   14c32:	85d2                	mv	a1,s4
   14c34:	48d020ef          	jal	ra,178c0 <__subtf3>
   14c38:	76c2                	ld	a3,48(sp)
   14c3a:	4601                	li	a2,0
   14c3c:	5de020ef          	jal	ra,1721a <__multf3>
   14c40:	77a2                	ld	a5,40(sp)
   14c42:	882a                	mv	a6,a0
   14c44:	8cae                	mv	s9,a1
   14c46:	40f986b3          	sub	a3,s3,a5
   14c4a:	4785                	li	a5,1
   14c4c:	8aaa                	mv	s5,a0
   14c4e:	8a2e                	mv	s4,a1
   14c50:	02f69363          	bne	a3,a5,14c76 <fmt_fp+0x2ec>
   14c54:	4601                	li	a2,0
   14c56:	4681                	li	a3,0
   14c58:	fc2a                	sd	a0,56(sp)
   14c5a:	52e020ef          	jal	ra,17188 <__eqtf2>
   14c5e:	7862                	ld	a6,56(sp)
   14c60:	e509                	bnez	a0,14c6a <fmt_fp+0x2e0>
   14c62:	00804463          	bgtz	s0,14c6a <fmt_fp+0x2e0>
   14c66:	000d8f63          	beqz	s11,14c84 <fmt_fp+0x2fa>
   14c6a:	02e00793          	li	a5,46
   14c6e:	002b0993          	addi	s3,s6,2
   14c72:	00fb00a3          	sb	a5,1(s6)
   14c76:	4601                	li	a2,0
   14c78:	4681                	li	a3,0
   14c7a:	8542                	mv	a0,a6
   14c7c:	85e6                	mv	a1,s9
   14c7e:	50a020ef          	jal	ra,17188 <__eqtf2>
   14c82:	e549                	bnez	a0,14d0c <fmt_fp+0x382>
   14c84:	6789                	lui	a5,0x2
   14c86:	0094                	addi	a3,sp,64
   14c88:	d1078793          	addi	a5,a5,-752 # 1d10 <exit-0xe410>
   14c8c:	97b6                	add	a5,a5,a3
   14c8e:	7779                	lui	a4,0xffffe
   14c90:	00e78ab3          	add	s5,a5,a4
   14c94:	66a2                	ld	a3,8(sp)
   14c96:	30ca8a93          	addi	s5,s5,780
   14c9a:	800007b7          	lui	a5,0x80000
   14c9e:	418a8ab3          	sub	s5,s5,s8
   14ca2:	ffd7c793          	xori	a5,a5,-3
   14ca6:	415787b3          	sub	a5,a5,s5
   14caa:	8f95                	sub	a5,a5,a3
   14cac:	0687d263          	bge	a5,s0,14d10 <fmt_fp+0x386>
   14cb0:	557d                	li	a0,-1
   14cb2:	6289                	lui	t0,0x2
   14cb4:	d4028293          	addi	t0,t0,-704 # 1d40 <exit-0xe3e0>
   14cb8:	9116                	add	sp,sp,t0
   14cba:	70e6                	ld	ra,120(sp)
   14cbc:	7446                	ld	s0,112(sp)
   14cbe:	74a6                	ld	s1,104(sp)
   14cc0:	7906                	ld	s2,96(sp)
   14cc2:	69e6                	ld	s3,88(sp)
   14cc4:	6a46                	ld	s4,80(sp)
   14cc6:	6aa6                	ld	s5,72(sp)
   14cc8:	6b06                	ld	s6,64(sp)
   14cca:	7be2                	ld	s7,56(sp)
   14ccc:	7c42                	ld	s8,48(sp)
   14cce:	7ca2                	ld	s9,40(sp)
   14cd0:	7d02                	ld	s10,32(sp)
   14cd2:	6de2                	ld	s11,24(sp)
   14cd4:	6109                	addi	sp,sp,128
   14cd6:	8082                	ret
   14cd8:	8562                	mv	a0,s8
   14cda:	85d6                	mv	a1,s5
   14cdc:	4601                	li	a2,0
   14cde:	53c020ef          	jal	ra,1721a <__multf3>
   14ce2:	040037b7          	lui	a5,0x4003
   14ce6:	8c2a                	mv	s8,a0
   14ce8:	8aae                	mv	s5,a1
   14cea:	587d                	li	a6,-1
   14cec:	02479693          	slli	a3,a5,0x24
   14cf0:	b599                	j	14b36 <fmt_fp+0x1ac>
   14cf2:	866e                	mv	a2,s11
   14cf4:	86da                	mv	a3,s6
   14cf6:	85d6                	mv	a1,s5
   14cf8:	8562                	mv	a0,s8
   14cfa:	291010ef          	jal	ra,1678a <__addtf3>
   14cfe:	86d6                	mv	a3,s5
   14d00:	8662                	mv	a2,s8
   14d02:	3bf020ef          	jal	ra,178c0 <__subtf3>
   14d06:	8aaa                	mv	s5,a0
   14d08:	8a2e                	mv	s4,a1
   14d0a:	b585                	j	14b6a <fmt_fp+0x1e0>
   14d0c:	8b4e                	mv	s6,s3
   14d0e:	bded                	j	14c08 <fmt_fp+0x27e>
   14d10:	6789                	lui	a5,0x2
   14d12:	d1078793          	addi	a5,a5,-752 # 1d10 <exit-0xe410>
   14d16:	0094                	addi	a3,sp,64
   14d18:	97b6                	add	a5,a5,a3
   14d1a:	31070713          	addi	a4,a4,784 # ffffffffffffe310 <__BSS_END__+0xfffffffffffe18f0>
   14d1e:	973e                	add	a4,a4,a5
   14d20:	40e989b3          	sub	s3,s3,a4
   14d24:	000a879b          	sext.w	a5,s5
   14d28:	013a8dbb          	addw	s11,s5,s3
   14d2c:	c05d                	beqz	s0,14dd2 <fmt_fp+0x448>
   14d2e:	fff98713          	addi	a4,s3,-1
   14d32:	0ae44063          	blt	s0,a4,14dd2 <fmt_fp+0x448>
   14d36:	7702                	ld	a4,32(sp)
   14d38:	00270a1b          	addiw	s4,a4,2
   14d3c:	00fa0a3b          	addw	s4,s4,a5
   14d40:	47c2                	lw	a5,16(sp)
   14d42:	6602                	ld	a2,0(sp)
   14d44:	875e                	mv	a4,s7
   14d46:	00fa0d3b          	addw	s10,s4,a5
   14d4a:	86ea                	mv	a3,s10
   14d4c:	02000593          	li	a1,32
   14d50:	8526                	mv	a0,s1
   14d52:	bc5ff0ef          	jal	ra,14916 <pad>
   14d56:	6622                	ld	a2,8(sp)
   14d58:	85ca                	mv	a1,s2
   14d5a:	8526                	mv	a0,s1
   14d5c:	ba5ff0ef          	jal	ra,14900 <out>
   14d60:	6602                	ld	a2,0(sp)
   14d62:	6741                	lui	a4,0x10
   14d64:	86ea                	mv	a3,s10
   14d66:	00ebc733          	xor	a4,s7,a4
   14d6a:	03000593          	li	a1,48
   14d6e:	8526                	mv	a0,s1
   14d70:	ba7ff0ef          	jal	ra,14916 <pad>
   14d74:	6789                	lui	a5,0x2
   14d76:	0098                	addi	a4,sp,64
   14d78:	75f9                	lui	a1,0xffffe
   14d7a:	d1078793          	addi	a5,a5,-752 # 1d10 <exit-0xe410>
   14d7e:	97ba                	add	a5,a5,a4
   14d80:	31058593          	addi	a1,a1,784 # ffffffffffffe310 <__BSS_END__+0xfffffffffffe18f0>
   14d84:	95be                	add	a1,a1,a5
   14d86:	864e                	mv	a2,s3
   14d88:	8526                	mv	a0,s1
   14d8a:	b77ff0ef          	jal	ra,14900 <out>
   14d8e:	4701                	li	a4,0
   14d90:	4681                	li	a3,0
   14d92:	41ba063b          	subw	a2,s4,s11
   14d96:	03000593          	li	a1,48
   14d9a:	8526                	mv	a0,s1
   14d9c:	b7bff0ef          	jal	ra,14916 <pad>
   14da0:	8656                	mv	a2,s5
   14da2:	85e2                	mv	a1,s8
   14da4:	8526                	mv	a0,s1
   14da6:	b5bff0ef          	jal	ra,14900 <out>
   14daa:	6602                	ld	a2,0(sp)
   14dac:	000b891b          	sext.w	s2,s7
   14db0:	6709                	lui	a4,0x2
   14db2:	00e94733          	xor	a4,s2,a4
   14db6:	86ea                	mv	a3,s10
   14db8:	02000593          	li	a1,32
   14dbc:	8526                	mv	a0,s1
   14dbe:	b59ff0ef          	jal	ra,14916 <pad>
   14dc2:	6782                	ld	a5,0(sp)
   14dc4:	846a                	mv	s0,s10
   14dc6:	00fd5363          	bge	s10,a5,14dcc <fmt_fp+0x442>
   14dca:	4402                	lw	s0,0(sp)
   14dcc:	0004051b          	sext.w	a0,s0
   14dd0:	b5cd                	j	14cb2 <fmt_fp+0x328>
   14dd2:	000d8a1b          	sext.w	s4,s11
   14dd6:	b7ad                	j	14d40 <fmt_fp+0x3b6>
   14dd8:	00045363          	bgez	s0,14dde <fmt_fp+0x454>
   14ddc:	4419                	li	s0,6
   14dde:	4601                	li	a2,0
   14de0:	4681                	li	a3,0
   14de2:	856e                	mv	a0,s11
   14de4:	85da                	mv	a1,s6
   14de6:	3a2020ef          	jal	ra,17188 <__eqtf2>
   14dea:	c90d                	beqz	a0,14e1c <fmt_fp+0x492>
   14dec:	66e9                	lui	a3,0x1a
   14dee:	e086b683          	ld	a3,-504(a3) # 19e08 <__fsmu8+0x2d8>
   14df2:	4601                	li	a2,0
   14df4:	856e                	mv	a0,s11
   14df6:	85da                	mv	a1,s6
   14df8:	422020ef          	jal	ra,1721a <__multf3>
   14dfc:	6709                	lui	a4,0x2
   14dfe:	d1070713          	addi	a4,a4,-752 # 1d10 <exit-0xe410>
   14e02:	0094                	addi	a3,sp,64
   14e04:	9736                	add	a4,a4,a3
   14e06:	77f9                	lui	a5,0xffffe
   14e08:	97ba                	add	a5,a5,a4
   14e0a:	e83e                	sd	a5,16(sp)
   14e0c:	2fc7a783          	lw	a5,764(a5) # ffffffffffffe2fc <__BSS_END__+0xfffffffffffe18dc>
   14e10:	6742                	ld	a4,16(sp)
   14e12:	8aaa                	mv	s5,a0
   14e14:	3791                	addiw	a5,a5,-28
   14e16:	8a2e                	mv	s4,a1
   14e18:	2ef72e23          	sw	a5,764(a4)
   14e1c:	4336                	lw	t1,76(sp)
   14e1e:	6709                	lui	a4,0x2
   14e20:	b4870693          	addi	a3,a4,-1208 # 1b48 <exit-0xe5d8>
   14e24:	0090                	addi	a2,sp,64
   14e26:	77f9                	lui	a5,0xffffe
   14e28:	00d608b3          	add	a7,a2,a3
   14e2c:	00035863          	bgez	t1,14e3c <fmt_fp+0x4b2>
   14e30:	33878893          	addi	a7,a5,824 # ffffffffffffe338 <__BSS_END__+0xfffffffffffe1918>
   14e34:	d1070793          	addi	a5,a4,-752
   14e38:	97b2                	add	a5,a5,a2
   14e3a:	98be                	add	a7,a7,a5
   14e3c:	6769                	lui	a4,0x1a
   14e3e:	e1873d83          	ld	s11,-488(a4) # 19e18 <__fsmu8+0x2e8>
   14e42:	8b46                	mv	s6,a7
   14e44:	85d2                	mv	a1,s4
   14e46:	8556                	mv	a0,s5
   14e48:	ec46                	sd	a7,24(sp)
   14e4a:	e81a                	sd	t1,16(sp)
   14e4c:	592030ef          	jal	ra,183de <__fixunstfsi>
   14e50:	00ab2023          	sw	a0,0(s6)
   14e54:	2501                	sext.w	a0,a0
   14e56:	66a030ef          	jal	ra,184c0 <__floatunsitf>
   14e5a:	862a                	mv	a2,a0
   14e5c:	86ae                	mv	a3,a1
   14e5e:	8556                	mv	a0,s5
   14e60:	85d2                	mv	a1,s4
   14e62:	25f020ef          	jal	ra,178c0 <__subtf3>
   14e66:	4601                	li	a2,0
   14e68:	86ee                	mv	a3,s11
   14e6a:	3b0020ef          	jal	ra,1721a <__multf3>
   14e6e:	4601                	li	a2,0
   14e70:	4681                	li	a3,0
   14e72:	8aaa                	mv	s5,a0
   14e74:	8a2e                	mv	s4,a1
   14e76:	312020ef          	jal	ra,17188 <__eqtf2>
   14e7a:	6342                	ld	t1,16(sp)
   14e7c:	68e2                	ld	a7,24(sp)
   14e7e:	0b11                	addi	s6,s6,4
   14e80:	f171                	bnez	a0,14e44 <fmt_fp+0x4ba>
   14e82:	3b9ad537          	lui	a0,0x3b9ad
   14e86:	8d46                	mv	s10,a7
   14e88:	4701                	li	a4,0
   14e8a:	4e75                	li	t3,29
   14e8c:	a0050513          	addi	a0,a0,-1536 # 3b9aca00 <__BSS_END__+0x3b98ffe0>
   14e90:	14604163          	bgtz	t1,14fd2 <fmt_fp+0x648>
   14e94:	cb19                	beqz	a4,14eaa <fmt_fp+0x520>
   14e96:	6789                	lui	a5,0x2
   14e98:	d1078793          	addi	a5,a5,-752 # 1d10 <exit-0xe410>
   14e9c:	0094                	addi	a3,sp,64
   14e9e:	7779                	lui	a4,0xffffe
   14ea0:	97b6                	add	a5,a5,a3
   14ea2:	97ba                	add	a5,a5,a4
   14ea4:	e83e                	sd	a5,16(sp)
   14ea6:	2e67ae23          	sw	t1,764(a5)
   14eaa:	46a5                	li	a3,9
   14eac:	02d4071b          	addiw	a4,s0,45
   14eb0:	02d7573b          	divuw	a4,a4,a3
   14eb4:	6789                	lui	a5,0x2
   14eb6:	0090                	addi	a2,sp,64
   14eb8:	d1078793          	addi	a5,a5,-752 # 1d10 <exit-0xe410>
   14ebc:	76f9                	lui	a3,0xffffe
   14ebe:	97b2                	add	a5,a5,a2
   14ec0:	97b6                	add	a5,a5,a3
   14ec2:	2fc7a683          	lw	a3,764(a5)
   14ec6:	3b9ade37          	lui	t3,0x3b9ad
   14eca:	e83e                	sd	a5,16(sp)
   14ecc:	4601                	li	a2,0
   14ece:	5f5d                	li	t5,-9
   14ed0:	4f85                	li	t6,1
   14ed2:	a00e0e1b          	addiw	t3,t3,-1536
   14ed6:	06600293          	li	t0,102
   14eda:	2705                	addiw	a4,a4,1
   14edc:	1702                	slli	a4,a4,0x20
   14ede:	9301                	srli	a4,a4,0x20
   14ee0:	00271e93          	slli	t4,a4,0x2
   14ee4:	1206cf63          	bltz	a3,15022 <fmt_fp+0x698>
   14ee8:	ca19                	beqz	a2,14efe <fmt_fp+0x574>
   14eea:	6789                	lui	a5,0x2
   14eec:	d1078793          	addi	a5,a5,-752 # 1d10 <exit-0xe410>
   14ef0:	0090                	addi	a2,sp,64
   14ef2:	7779                	lui	a4,0xffffe
   14ef4:	97b2                	add	a5,a5,a2
   14ef6:	97ba                	add	a5,a5,a4
   14ef8:	e83e                	sd	a5,16(sp)
   14efa:	2ed7ae23          	sw	a3,764(a5)
   14efe:	4d81                	li	s11,0
   14f00:	016d7f63          	bgeu	s10,s6,14f1e <fmt_fp+0x594>
   14f04:	41a88db3          	sub	s11,a7,s10
   14f08:	000d2683          	lw	a3,0(s10)
   14f0c:	402dd713          	srai	a4,s11,0x2
   14f10:	4da5                	li	s11,9
   14f12:	02ed8dbb          	mulw	s11,s11,a4
   14f16:	4629                	li	a2,10
   14f18:	4729                	li	a4,10
   14f1a:	16e6f663          	bgeu	a3,a4,15086 <fmt_fp+0x6fc>
   14f1e:	f9ac8713          	addi	a4,s9,-102
   14f22:	00e03733          	snez	a4,a4
   14f26:	06700613          	li	a2,103
   14f2a:	4681                	li	a3,0
   14f2c:	03b7073b          	mulw	a4,a4,s11
   14f30:	40e4073b          	subw	a4,s0,a4
   14f34:	00cc9463          	bne	s9,a2,14f3c <fmt_fp+0x5b2>
   14f38:	008036b3          	snez	a3,s0
   14f3c:	40d7063b          	subw	a2,a4,a3
   14f40:	411b0733          	sub	a4,s6,a7
   14f44:	8709                	srai	a4,a4,0x2
   14f46:	177d                	addi	a4,a4,-1
   14f48:	45a5                	li	a1,9
   14f4a:	02b70733          	mul	a4,a4,a1
   14f4e:	1ae65363          	bge	a2,a4,150f4 <fmt_fp+0x76a>
   14f52:	00024737          	lui	a4,0x24
   14f56:	9f31                	addw	a4,a4,a2
   14f58:	4625                	li	a2,9
   14f5a:	02c74a3b          	divw	s4,a4,a2
   14f5e:	76c1                	lui	a3,0xffff0
   14f60:	0691                	addi	a3,a3,4
   14f62:	45a9                	li	a1,10
   14f64:	02c7673b          	remw	a4,a4,a2
   14f68:	0a0a                	slli	s4,s4,0x2
   14f6a:	9a36                	add	s4,s4,a3
   14f6c:	9a46                	add	s4,s4,a7
   14f6e:	46a9                	li	a3,10
   14f70:	4625                	li	a2,9
   14f72:	2705                	addiw	a4,a4,1
   14f74:	10c71d63          	bne	a4,a2,1508e <fmt_fp+0x704>
   14f78:	000a2703          	lw	a4,0(s4)
   14f7c:	00068e1b          	sext.w	t3,a3
   14f80:	02d7763b          	remuw	a2,a4,a3
   14f84:	8532                	mv	a0,a2
   14f86:	e609                	bnez	a2,14f90 <fmt_fp+0x606>
   14f88:	004a0593          	addi	a1,s4,4
   14f8c:	16bb0063          	beq	s6,a1,150ec <fmt_fp+0x762>
   14f90:	03c755bb          	divuw	a1,a4,t3
   14f94:	8985                	andi	a1,a1,1
   14f96:	ed89                	bnez	a1,14fb0 <fmt_fp+0x626>
   14f98:	3b9ad5b7          	lui	a1,0x3b9ad
   14f9c:	a0058593          	addi	a1,a1,-1536 # 3b9aca00 <__BSS_END__+0x3b98ffe0>
   14fa0:	0eb69b63          	bne	a3,a1,15096 <fmt_fp+0x70c>
   14fa4:	0f4d7963          	bgeu	s10,s4,15096 <fmt_fp+0x70c>
   14fa8:	ffca2583          	lw	a1,-4(s4)
   14fac:	8985                	andi	a1,a1,1
   14fae:	c5e5                	beqz	a1,15096 <fmt_fp+0x70c>
   14fb0:	65e9                	lui	a1,0x1a
   14fb2:	dc85ba83          	ld	s5,-568(a1) # 19dc8 <__fsmu8+0x298>
   14fb6:	4305                	li	t1,1
   14fb8:	8685                	srai	a3,a3,0x1
   14fba:	0ed66363          	bltu	a2,a3,150a0 <fmt_fp+0x716>
   14fbe:	00c69663          	bne	a3,a2,14fca <fmt_fp+0x640>
   14fc2:	004a0693          	addi	a3,s4,4
   14fc6:	2adb0a63          	beq	s6,a3,1527a <fmt_fp+0x8f0>
   14fca:	66e9                	lui	a3,0x1a
   14fcc:	dd86b683          	ld	a3,-552(a3) # 19dd8 <__fsmu8+0x2a8>
   14fd0:	a8d9                	j	150a6 <fmt_fp+0x71c>
   14fd2:	0003061b          	sext.w	a2,t1
   14fd6:	006e5363          	bge	t3,t1,14fdc <fmt_fp+0x652>
   14fda:	4675                	li	a2,29
   14fdc:	ffcb0593          	addi	a1,s6,-4
   14fe0:	4701                	li	a4,0
   14fe2:	01a5ff63          	bgeu	a1,s10,15000 <fmt_fp+0x676>
   14fe6:	c701                	beqz	a4,14fee <fmt_fp+0x664>
   14fe8:	feed2e23          	sw	a4,-4(s10)
   14fec:	1d71                	addi	s10,s10,-4
   14fee:	016d7563          	bgeu	s10,s6,14ff8 <fmt_fp+0x66e>
   14ff2:	ffcb2703          	lw	a4,-4(s6)
   14ff6:	c705                	beqz	a4,1501e <fmt_fp+0x694>
   14ff8:	40c3033b          	subw	t1,t1,a2
   14ffc:	4705                	li	a4,1
   14ffe:	bd49                	j	14e90 <fmt_fp+0x506>
   15000:	0005e683          	lwu	a3,0(a1)
   15004:	1702                	slli	a4,a4,0x20
   15006:	9301                	srli	a4,a4,0x20
   15008:	00c696b3          	sll	a3,a3,a2
   1500c:	9736                	add	a4,a4,a3
   1500e:	02a776b3          	remu	a3,a4,a0
   15012:	15f1                	addi	a1,a1,-4
   15014:	02a75733          	divu	a4,a4,a0
   15018:	c1d4                	sw	a3,4(a1)
   1501a:	2701                	sext.w	a4,a4
   1501c:	b7d9                	j	14fe2 <fmt_fp+0x658>
   1501e:	1b71                	addi	s6,s6,-4
   15020:	b7f9                	j	14fee <fmt_fp+0x664>
   15022:	45a5                	li	a1,9
   15024:	01e6c463          	blt	a3,t5,1502c <fmt_fp+0x6a2>
   15028:	40d005bb          	negw	a1,a3
   1502c:	00bf93bb          	sllw	t2,t6,a1
   15030:	33fd                	addiw	t2,t2,-1
   15032:	40be5a3b          	sraw	s4,t3,a1
   15036:	866a                	mv	a2,s10
   15038:	4501                	li	a0,0
   1503a:	03666863          	bltu	a2,s6,1506a <fmt_fp+0x6e0>
   1503e:	000d2603          	lw	a2,0(s10)
   15042:	e211                	bnez	a2,15046 <fmt_fp+0x6bc>
   15044:	0d11                	addi	s10,s10,4
   15046:	c501                	beqz	a0,1504e <fmt_fp+0x6c4>
   15048:	00ab2023          	sw	a0,0(s6)
   1504c:	0b11                	addi	s6,s6,4
   1504e:	8646                	mv	a2,a7
   15050:	005c8363          	beq	s9,t0,15056 <fmt_fp+0x6cc>
   15054:	866a                	mv	a2,s10
   15056:	40cb0533          	sub	a0,s6,a2
   1505a:	8509                	srai	a0,a0,0x2
   1505c:	00a75463          	bge	a4,a0,15064 <fmt_fp+0x6da>
   15060:	01d60b33          	add	s6,a2,t4
   15064:	9ead                	addw	a3,a3,a1
   15066:	4605                	li	a2,1
   15068:	bdb5                	j	14ee4 <fmt_fp+0x55a>
   1506a:	00062303          	lw	t1,0(a2)
   1506e:	0611                	addi	a2,a2,4
   15070:	00b35abb          	srlw	s5,t1,a1
   15074:	00aa853b          	addw	a0,s5,a0
   15078:	fea62e23          	sw	a0,-4(a2)
   1507c:	00737533          	and	a0,t1,t2
   15080:	0345053b          	mulw	a0,a0,s4
   15084:	bf5d                	j	1503a <fmt_fp+0x6b0>
   15086:	02e6073b          	mulw	a4,a2,a4
   1508a:	2d85                	addiw	s11,s11,1
   1508c:	b579                	j	14f1a <fmt_fp+0x590>
   1508e:	02d586bb          	mulw	a3,a1,a3
   15092:	2705                	addiw	a4,a4,1
   15094:	b5c5                	j	14f74 <fmt_fp+0x5ea>
   15096:	65e9                	lui	a1,0x1a
   15098:	db85ba83          	ld	s5,-584(a1) # 19db8 <__fsmu8+0x288>
   1509c:	4301                	li	t1,0
   1509e:	bf29                	j	14fb8 <fmt_fp+0x62e>
   150a0:	66e9                	lui	a3,0x1a
   150a2:	df86b683          	ld	a3,-520(a3) # 19df8 <__fsmu8+0x2c8>
   150a6:	000c0d63          	beqz	s8,150c0 <fmt_fp+0x736>
   150aa:	00094583          	lbu	a1,0(s2)
   150ae:	02d00613          	li	a2,45
   150b2:	00c59763          	bne	a1,a2,150c0 <fmt_fp+0x736>
   150b6:	567d                	li	a2,-1
   150b8:	167e                	slli	a2,a2,0x3f
   150ba:	00cacab3          	xor	s5,s5,a2
   150be:	8eb1                	xor	a3,a3,a2
   150c0:	9f09                	subw	a4,a4,a0
   150c2:	4601                	li	a2,0
   150c4:	851a                	mv	a0,t1
   150c6:	85d6                	mv	a1,s5
   150c8:	f446                	sd	a7,40(sp)
   150ca:	f072                	sd	t3,32(sp)
   150cc:	ec3a                	sd	a4,24(sp)
   150ce:	e81a                	sd	t1,16(sp)
   150d0:	6ba010ef          	jal	ra,1678a <__addtf3>
   150d4:	6342                	ld	t1,16(sp)
   150d6:	86d6                	mv	a3,s5
   150d8:	861a                	mv	a2,t1
   150da:	0ae020ef          	jal	ra,17188 <__eqtf2>
   150de:	6762                	ld	a4,24(sp)
   150e0:	7e02                	ld	t3,32(sp)
   150e2:	78a2                	ld	a7,40(sp)
   150e4:	18051f63          	bnez	a0,15282 <fmt_fp+0x8f8>
   150e8:	00ea2023          	sw	a4,0(s4)
   150ec:	0a11                	addi	s4,s4,4
   150ee:	016a7363          	bgeu	s4,s6,150f4 <fmt_fp+0x76a>
   150f2:	8b52                	mv	s6,s4
   150f4:	016d7663          	bgeu	s10,s6,15100 <fmt_fp+0x776>
   150f8:	ffcb2703          	lw	a4,-4(s6)
   150fc:	1c070e63          	beqz	a4,152d8 <fmt_fp+0x94e>
   15100:	06700713          	li	a4,103
   15104:	04ec9d63          	bne	s9,a4,1515e <fmt_fp+0x7d4>
   15108:	e011                	bnez	s0,1510c <fmt_fp+0x782>
   1510a:	4405                	li	s0,1
   1510c:	1c8dd863          	bge	s11,s0,152dc <fmt_fp+0x952>
   15110:	5771                	li	a4,-4
   15112:	1cedc563          	blt	s11,a4,152dc <fmt_fp+0x952>
   15116:	001d871b          	addiw	a4,s11,1
   1511a:	39fd                	addiw	s3,s3,-1
   1511c:	9c19                	subw	s0,s0,a4
   1511e:	008bf713          	andi	a4,s7,8
   15122:	ef15                	bnez	a4,1515e <fmt_fp+0x7d4>
   15124:	46a5                	li	a3,9
   15126:	016d7663          	bgeu	s10,s6,15132 <fmt_fp+0x7a8>
   1512a:	ffcb2603          	lw	a2,-4(s6)
   1512e:	1c061163          	bnez	a2,152f0 <fmt_fp+0x966>
   15132:	411b0733          	sub	a4,s6,a7
   15136:	8709                	srai	a4,a4,0x2
   15138:	4625                	li	a2,9
   1513a:	177d                	addi	a4,a4,-1
   1513c:	02c70733          	mul	a4,a4,a2
   15140:	06600593          	li	a1,102
   15144:	0209e613          	ori	a2,s3,32
   15148:	1ab61863          	bne	a2,a1,152f8 <fmt_fp+0x96e>
   1514c:	8f15                	sub	a4,a4,a3
   1514e:	00075363          	bgez	a4,15154 <fmt_fp+0x7ca>
   15152:	4701                	li	a4,0
   15154:	00e45363          	bge	s0,a4,1515a <fmt_fp+0x7d0>
   15158:	8722                	mv	a4,s0
   1515a:	0007041b          	sext.w	s0,a4
   1515e:	80000737          	lui	a4,0x80000
   15162:	ffd74713          	xori	a4,a4,-3
   15166:	b48745e3          	blt	a4,s0,14cb0 <fmt_fp+0x326>
   1516a:	000b879b          	sext.w	a5,s7
   1516e:	008bfa13          	andi	s4,s7,8
   15172:	e83e                	sd	a5,16(sp)
   15174:	000a079b          	sext.w	a5,s4
   15178:	01446a33          	or	s4,s0,s4
   1517c:	014036b3          	snez	a3,s4
   15180:	0014071b          	addiw	a4,s0,1
   15184:	9f35                	addw	a4,a4,a3
   15186:	800006b7          	lui	a3,0x80000
   1518a:	fff6c693          	not	a3,a3
   1518e:	ec3e                	sd	a5,24(sp)
   15190:	0209ea93          	ori	s5,s3,32
   15194:	06600613          	li	a2,102
   15198:	0007051b          	sext.w	a0,a4
   1519c:	9e99                	subw	a3,a3,a4
   1519e:	14ca9f63          	bne	s5,a2,152fc <fmt_fp+0x972>
   151a2:	b1b6c7e3          	blt	a3,s11,14cb0 <fmt_fp+0x326>
   151a6:	01b05463          	blez	s11,151ae <fmt_fp+0x824>
   151aa:	01b7053b          	addw	a0,a4,s11
   151ae:	80000737          	lui	a4,0x80000
   151b2:	1c0c1563          	bnez	s8,1537c <fmt_fp+0x9f2>
   151b6:	fff74713          	not	a4,a4
   151ba:	f046                	sd	a7,32(sp)
   151bc:	2701                	sext.w	a4,a4
   151be:	aea749e3          	blt	a4,a0,14cb0 <fmt_fp+0x326>
   151c2:	6602                	ld	a2,0(sp)
   151c4:	01850dbb          	addw	s11,a0,s8
   151c8:	875e                	mv	a4,s7
   151ca:	86ee                	mv	a3,s11
   151cc:	02000593          	li	a1,32
   151d0:	8526                	mv	a0,s1
   151d2:	f44ff0ef          	jal	ra,14916 <pad>
   151d6:	8662                	mv	a2,s8
   151d8:	85ca                	mv	a1,s2
   151da:	8526                	mv	a0,s1
   151dc:	f24ff0ef          	jal	ra,14900 <out>
   151e0:	67c2                	ld	a5,16(sp)
   151e2:	6602                	ld	a2,0(sp)
   151e4:	6741                	lui	a4,0x10
   151e6:	8f3d                	xor	a4,a4,a5
   151e8:	86ee                	mv	a3,s11
   151ea:	03000593          	li	a1,48
   151ee:	8526                	mv	a0,s1
   151f0:	f26ff0ef          	jal	ra,14916 <pad>
   151f4:	06600713          	li	a4,102
   151f8:	7882                	ld	a7,32(sp)
   151fa:	89ee                	mv	s3,s11
   151fc:	24ea9563          	bne	s5,a4,15446 <fmt_fp+0xabc>
   15200:	8946                	mv	s2,a7
   15202:	011d7363          	bgeu	s10,a7,15208 <fmt_fp+0x87e>
   15206:	896a                	mv	s2,s10
   15208:	6709                	lui	a4,0x2
   1520a:	7bf9                	lui	s7,0xffffe
   1520c:	0094                	addi	a3,sp,64
   1520e:	d1070793          	addi	a5,a4,-752 # 1d10 <exit-0xe410>
   15212:	310b8c93          	addi	s9,s7,784 # ffffffffffffe310 <__BSS_END__+0xfffffffffffe18f0>
   15216:	97b6                	add	a5,a5,a3
   15218:	9cbe                	add	s9,s9,a5
   1521a:	d1070793          	addi	a5,a4,-752
   1521e:	97b6                	add	a5,a5,a3
   15220:	8d4a                	mv	s10,s2
   15222:	009c8a93          	addi	s5,s9,9
   15226:	03000c13          	li	s8,48
   1522a:	9bbe                	add	s7,s7,a5
   1522c:	15a8fb63          	bgeu	a7,s10,15382 <fmt_fp+0x9f8>
   15230:	00188693          	addi	a3,a7,1
   15234:	ffd90713          	addi	a4,s2,-3
   15238:	4601                	li	a2,0
   1523a:	00e6e763          	bltu	a3,a4,15248 <fmt_fp+0x8be>
   1523e:	0891                	addi	a7,a7,4
   15240:	412888b3          	sub	a7,a7,s2
   15244:	ffc8f613          	andi	a2,a7,-4
   15248:	9932                	add	s2,s2,a2
   1524a:	1e0a0c63          	beqz	s4,15442 <fmt_fp+0xab8>
   1524e:	65e9                	lui	a1,0x1a
   15250:	4605                	li	a2,1
   15252:	c8058593          	addi	a1,a1,-896 # 19c80 <__fsmu8+0x150>
   15256:	8526                	mv	a0,s1
   15258:	ea8ff0ef          	jal	ra,14900 <out>
   1525c:	6789                	lui	a5,0x2
   1525e:	7a79                	lui	s4,0xffffe
   15260:	d1078793          	addi	a5,a5,-752 # 1d10 <exit-0xe410>
   15264:	0098                	addi	a4,sp,64
   15266:	310a0a13          	addi	s4,s4,784 # ffffffffffffe310 <__BSS_END__+0xfffffffffffe18f0>
   1526a:	97ba                	add	a5,a5,a4
   1526c:	9a3e                	add	s4,s4,a5
   1526e:	009a0b93          	addi	s7,s4,9
   15272:	03000c13          	li	s8,48
   15276:	4ca5                	li	s9,9
   15278:	a259                	j	153fe <fmt_fp+0xa74>
   1527a:	66e9                	lui	a3,0x1a
   1527c:	de86b683          	ld	a3,-536(a3) # 19de8 <__fsmu8+0x2b8>
   15280:	b51d                	j	150a6 <fmt_fp+0x71c>
   15282:	00ee073b          	addw	a4,t3,a4
   15286:	00ea2023          	sw	a4,0(s4)
   1528a:	3b9ad737          	lui	a4,0x3b9ad
   1528e:	9ff70713          	addi	a4,a4,-1537 # 3b9ac9ff <__BSS_END__+0x3b98ffdf>
   15292:	000a2683          	lw	a3,0(s4)
   15296:	02d76363          	bltu	a4,a3,152bc <fmt_fp+0x932>
   1529a:	41a88db3          	sub	s11,a7,s10
   1529e:	000d2683          	lw	a3,0(s10)
   152a2:	402dd713          	srai	a4,s11,0x2
   152a6:	4da5                	li	s11,9
   152a8:	02ed8dbb          	mulw	s11,s11,a4
   152ac:	4629                	li	a2,10
   152ae:	4729                	li	a4,10
   152b0:	e2e6eee3          	bltu	a3,a4,150ec <fmt_fp+0x762>
   152b4:	2d85                	addiw	s11,s11,1
   152b6:	02e6073b          	mulw	a4,a2,a4
   152ba:	bfdd                	j	152b0 <fmt_fp+0x926>
   152bc:	1a71                	addi	s4,s4,-4
   152be:	000a2223          	sw	zero,4(s4)
   152c2:	01aa7563          	bgeu	s4,s10,152cc <fmt_fp+0x942>
   152c6:	fe0d2e23          	sw	zero,-4(s10)
   152ca:	1d71                	addi	s10,s10,-4
   152cc:	000a2683          	lw	a3,0(s4)
   152d0:	2685                	addiw	a3,a3,1
   152d2:	00da2023          	sw	a3,0(s4)
   152d6:	bf75                	j	15292 <fmt_fp+0x908>
   152d8:	1b71                	addi	s6,s6,-4
   152da:	bd29                	j	150f4 <fmt_fp+0x76a>
   152dc:	39f9                	addiw	s3,s3,-2
   152de:	347d                	addiw	s0,s0,-1
   152e0:	bd3d                	j	1511e <fmt_fp+0x794>
   152e2:	02e5073b          	mulw	a4,a0,a4
   152e6:	2685                	addiw	a3,a3,1
   152e8:	02e675bb          	remuw	a1,a2,a4
   152ec:	d9fd                	beqz	a1,152e2 <fmt_fp+0x958>
   152ee:	b591                	j	15132 <fmt_fp+0x7a8>
   152f0:	4681                	li	a3,0
   152f2:	4729                	li	a4,10
   152f4:	4529                	li	a0,10
   152f6:	bfcd                	j	152e8 <fmt_fp+0x95e>
   152f8:	976e                	add	a4,a4,s11
   152fa:	bd89                	j	1514c <fmt_fp+0x7c2>
   152fc:	6789                	lui	a5,0x2
   152fe:	f43a                	sd	a4,40(sp)
   15300:	d1078793          	addi	a5,a5,-752 # 1d10 <exit-0xe410>
   15304:	0098                	addi	a4,sp,64
   15306:	97ba                	add	a5,a5,a4
   15308:	7cf9                	lui	s9,0xffffe
   1530a:	41fdd61b          	sraiw	a2,s11,0x1f
   1530e:	9cbe                	add	s9,s9,a5
   15310:	00cdc533          	xor	a0,s11,a2
   15314:	30cc8c93          	addi	s9,s9,780 # ffffffffffffe30c <__BSS_END__+0xfffffffffffe18ec>
   15318:	9d11                	subw	a0,a0,a2
   1531a:	85e6                	mv	a1,s9
   1531c:	f036                	sd	a3,32(sp)
   1531e:	e446                	sd	a7,8(sp)
   15320:	d74ff0ef          	jal	ra,14894 <fmt_u>
   15324:	68a2                	ld	a7,8(sp)
   15326:	7682                	ld	a3,32(sp)
   15328:	7722                	ld	a4,40(sp)
   1532a:	4605                	li	a2,1
   1532c:	03000313          	li	t1,48
   15330:	40ac85b3          	sub	a1,s9,a0
   15334:	04b65063          	bge	a2,a1,15374 <fmt_fp+0x9ea>
   15338:	02d00613          	li	a2,45
   1533c:	000dc463          	bltz	s11,15344 <fmt_fp+0x9ba>
   15340:	02b00613          	li	a2,43
   15344:	ffe50793          	addi	a5,a0,-2
   15348:	e43e                	sd	a5,8(sp)
   1534a:	6789                	lui	a5,0x2
   1534c:	fec50fa3          	sb	a2,-1(a0)
   15350:	d1078793          	addi	a5,a5,-752 # 1d10 <exit-0xe410>
   15354:	0090                	addi	a2,sp,64
   15356:	97b2                	add	a5,a5,a2
   15358:	ff350f23          	sb	s3,-2(a0)
   1535c:	79f9                	lui	s3,0xffffe
   1535e:	99be                	add	s3,s3,a5
   15360:	67a2                	ld	a5,8(sp)
   15362:	30c98993          	addi	s3,s3,780 # ffffffffffffe30c <__BSS_END__+0xfffffffffffe18ec>
   15366:	40f989b3          	sub	s3,s3,a5
   1536a:	9536c3e3          	blt	a3,s3,14cb0 <fmt_fp+0x326>
   1536e:	0137053b          	addw	a0,a4,s3
   15372:	bd35                	j	151ae <fmt_fp+0x824>
   15374:	157d                	addi	a0,a0,-1
   15376:	00650023          	sb	t1,0(a0)
   1537a:	bf5d                	j	15330 <fmt_fp+0x9a6>
   1537c:	ffe74713          	xori	a4,a4,-2
   15380:	bd2d                	j	151ba <fmt_fp+0x830>
   15382:	000d6503          	lwu	a0,0(s10)
   15386:	85d6                	mv	a1,s5
   15388:	e446                	sd	a7,8(sp)
   1538a:	d0aff0ef          	jal	ra,14894 <fmt_u>
   1538e:	68a2                	ld	a7,8(sp)
   15390:	85aa                	mv	a1,a0
   15392:	872a                	mv	a4,a0
   15394:	012d1c63          	bne	s10,s2,153ac <fmt_fp+0xa22>
   15398:	03551263          	bne	a0,s5,153bc <fmt_fp+0xa32>
   1539c:	318b8c23          	sb	s8,792(s7)
   153a0:	008c8593          	addi	a1,s9,8
   153a4:	a821                	j	153bc <fmt_fp+0xa32>
   153a6:	177d                	addi	a4,a4,-1
   153a8:	01870023          	sb	s8,0(a4)
   153ac:	feecede3          	bltu	s9,a4,153a6 <fmt_fp+0xa1c>
   153b0:	4701                	li	a4,0
   153b2:	0195e463          	bltu	a1,s9,153ba <fmt_fp+0xa30>
   153b6:	40bc8733          	sub	a4,s9,a1
   153ba:	95ba                	add	a1,a1,a4
   153bc:	40ba8633          	sub	a2,s5,a1
   153c0:	8526                	mv	a0,s1
   153c2:	e446                	sd	a7,8(sp)
   153c4:	d3cff0ef          	jal	ra,14900 <out>
   153c8:	68a2                	ld	a7,8(sp)
   153ca:	0d11                	addi	s10,s10,4
   153cc:	b585                	j	1522c <fmt_fp+0x8a2>
   153ce:	00096503          	lwu	a0,0(s2)
   153d2:	85de                	mv	a1,s7
   153d4:	cc0ff0ef          	jal	ra,14894 <fmt_u>
   153d8:	872a                	mv	a4,a0
   153da:	06ea6063          	bltu	s4,a4,1543a <fmt_fp+0xab0>
   153de:	4581                	li	a1,0
   153e0:	01456463          	bltu	a0,s4,153e8 <fmt_fp+0xa5e>
   153e4:	40aa05b3          	sub	a1,s4,a0
   153e8:	0004061b          	sext.w	a2,s0
   153ec:	008cd363          	bge	s9,s0,153f2 <fmt_fp+0xa68>
   153f0:	4625                	li	a2,9
   153f2:	95aa                	add	a1,a1,a0
   153f4:	8526                	mv	a0,s1
   153f6:	d0aff0ef          	jal	ra,14900 <out>
   153fa:	0911                	addi	s2,s2,4
   153fc:	345d                	addiw	s0,s0,-9
   153fe:	01697463          	bgeu	s2,s6,15406 <fmt_fp+0xa7c>
   15402:	fc8046e3          	bgtz	s0,153ce <fmt_fp+0xa44>
   15406:	4701                	li	a4,0
   15408:	46a5                	li	a3,9
   1540a:	0094061b          	addiw	a2,s0,9
   1540e:	03000593          	li	a1,48
   15412:	8526                	mv	a0,s1
   15414:	d02ff0ef          	jal	ra,14916 <pad>
   15418:	67c2                	ld	a5,16(sp)
   1541a:	6602                	ld	a2,0(sp)
   1541c:	6709                	lui	a4,0x2
   1541e:	8f3d                	xor	a4,a4,a5
   15420:	86ee                	mv	a3,s11
   15422:	02000593          	li	a1,32
   15426:	8526                	mv	a0,s1
   15428:	ceeff0ef          	jal	ra,14916 <pad>
   1542c:	6782                	ld	a5,0(sp)
   1542e:	e2fdcf63          	blt	s11,a5,14a6c <fmt_fp+0xe2>
   15432:	0009851b          	sext.w	a0,s3
   15436:	87dff06f          	j	14cb2 <fmt_fp+0x328>
   1543a:	177d                	addi	a4,a4,-1
   1543c:	01870023          	sb	s8,0(a4) # 2000 <exit-0xe120>
   15440:	bf69                	j	153da <fmt_fp+0xa50>
   15442:	4401                	li	s0,0
   15444:	bd21                	j	1525c <fmt_fp+0x8d2>
   15446:	016d6463          	bltu	s10,s6,1544e <fmt_fp+0xac4>
   1544a:	004d0b13          	addi	s6,s10,4
   1544e:	6689                	lui	a3,0x2
   15450:	7779                	lui	a4,0xffffe
   15452:	0090                	addi	a2,sp,64
   15454:	d1068793          	addi	a5,a3,-752 # 1d10 <exit-0xe410>
   15458:	97b2                	add	a5,a5,a2
   1545a:	31070913          	addi	s2,a4,784 # ffffffffffffe310 <__BSS_END__+0xfffffffffffe18f0>
   1545e:	993e                	add	s2,s2,a5
   15460:	d1068793          	addi	a5,a3,-752
   15464:	97b2                	add	a5,a5,a2
   15466:	97ba                	add	a5,a5,a4
   15468:	f03e                	sd	a5,32(sp)
   1546a:	00890793          	addi	a5,s2,8
   1546e:	8a6a                	mv	s4,s10
   15470:	00990b93          	addi	s7,s2,9
   15474:	03000c13          	li	s8,48
   15478:	f43e                	sd	a5,40(sp)
   1547a:	016a7463          	bgeu	s4,s6,15482 <fmt_fp+0xaf8>
   1547e:	02045b63          	bgez	s0,154b4 <fmt_fp+0xb2a>
   15482:	0124061b          	addiw	a2,s0,18
   15486:	8526                	mv	a0,s1
   15488:	4701                	li	a4,0
   1548a:	46c9                	li	a3,18
   1548c:	03000593          	li	a1,48
   15490:	c86ff0ef          	jal	ra,14916 <pad>
   15494:	6789                	lui	a5,0x2
   15496:	0098                	addi	a4,sp,64
   15498:	d1078793          	addi	a5,a5,-752 # 1d10 <exit-0xe410>
   1549c:	97ba                	add	a5,a5,a4
   1549e:	7679                	lui	a2,0xffffe
   154a0:	963e                	add	a2,a2,a5
   154a2:	67a2                	ld	a5,8(sp)
   154a4:	30c60613          	addi	a2,a2,780 # ffffffffffffe30c <__BSS_END__+0xfffffffffffe18ec>
   154a8:	8526                	mv	a0,s1
   154aa:	8e1d                	sub	a2,a2,a5
   154ac:	85be                	mv	a1,a5
   154ae:	c52ff0ef          	jal	ra,14900 <out>
   154b2:	b79d                	j	15418 <fmt_fp+0xa8e>
   154b4:	000a6503          	lwu	a0,0(s4)
   154b8:	85de                	mv	a1,s7
   154ba:	bdaff0ef          	jal	ra,14894 <fmt_u>
   154be:	85aa                	mv	a1,a0
   154c0:	01751663          	bne	a0,s7,154cc <fmt_fp+0xb42>
   154c4:	7782                	ld	a5,32(sp)
   154c6:	75a2                	ld	a1,40(sp)
   154c8:	31878c23          	sb	s8,792(a5)
   154cc:	00040c9b          	sext.w	s9,s0
   154d0:	872e                	mv	a4,a1
   154d2:	03aa1763          	bne	s4,s10,15500 <fmt_fp+0xb76>
   154d6:	4605                	li	a2,1
   154d8:	8526                	mv	a0,s1
   154da:	00158a93          	addi	s5,a1,1
   154de:	c22ff0ef          	jal	ra,14900 <out>
   154e2:	67e2                	ld	a5,24(sp)
   154e4:	0197e733          	or	a4,a5,s9
   154e8:	c70d                	beqz	a4,15512 <fmt_fp+0xb88>
   154ea:	67e9                	lui	a5,0x1a
   154ec:	4605                	li	a2,1
   154ee:	c8078593          	addi	a1,a5,-896 # 19c80 <__fsmu8+0x150>
   154f2:	8526                	mv	a0,s1
   154f4:	c0cff0ef          	jal	ra,14900 <out>
   154f8:	a829                	j	15512 <fmt_fp+0xb88>
   154fa:	177d                	addi	a4,a4,-1
   154fc:	01870023          	sb	s8,0(a4)
   15500:	fee96de3          	bltu	s2,a4,154fa <fmt_fp+0xb70>
   15504:	4681                	li	a3,0
   15506:	0125e463          	bltu	a1,s2,1550e <fmt_fp+0xb84>
   1550a:	40b906b3          	sub	a3,s2,a1
   1550e:	00d58ab3          	add	s5,a1,a3
   15512:	415b88b3          	sub	a7,s7,s5
   15516:	8622                	mv	a2,s0
   15518:	0088d363          	bge	a7,s0,1551e <fmt_fp+0xb94>
   1551c:	8646                	mv	a2,a7
   1551e:	85d6                	mv	a1,s5
   15520:	8526                	mv	a0,s1
   15522:	f846                	sd	a7,48(sp)
   15524:	bdcff0ef          	jal	ra,14900 <out>
   15528:	78c2                	ld	a7,48(sp)
   1552a:	0a11                	addi	s4,s4,4
   1552c:	411c843b          	subw	s0,s9,a7
   15530:	b7a9                	j	1547a <fmt_fp+0xaf0>

0000000000015532 <printf_core>:
   15532:	716d                	addi	sp,sp,-272
   15534:	67e9                	lui	a5,0x1a
   15536:	e2078793          	addi	a5,a5,-480 # 19e20 <states>
   1553a:	f96a                	sd	s10,176(sp)
   1553c:	6d69                	lui	s10,0x1a
   1553e:	f5ce                	sd	s3,232(sp)
   15540:	e83e                	sd	a5,16(sp)
   15542:	69e9                	lui	s3,0x1a
   15544:	ca0d0793          	addi	a5,s10,-864 # 19ca0 <__fsmu8+0x170>
   15548:	e222                	sd	s0,256(sp)
   1554a:	f9ca                	sd	s2,240(sp)
   1554c:	f1d2                	sd	s4,224(sp)
   1554e:	e9da                	sd	s6,208(sp)
   15550:	e1e2                	sd	s8,192(sp)
   15552:	fd66                	sd	s9,184(sp)
   15554:	e606                	sd	ra,264(sp)
   15556:	fda6                	sd	s1,248(sp)
   15558:	edd6                	sd	s5,216(sp)
   1555a:	e5de                	sd	s7,200(sp)
   1555c:	f56e                	sd	s11,168(sp)
   1555e:	842a                	mv	s0,a0
   15560:	8a32                	mv	s4,a2
   15562:	e036                	sd	a3,0(sp)
   15564:	8b3a                	mv	s6,a4
   15566:	e0ae                	sd	a1,64(sp)
   15568:	4c81                	li	s9,0
   1556a:	4901                	li	s2,0
   1556c:	4c01                	li	s8,0
   1556e:	c8898993          	addi	s3,s3,-888 # 19c88 <__fsmu8+0x158>
   15572:	ec3e                	sd	a5,24(sp)
   15574:	6b86                	ld	s7,64(sp)
   15576:	0199093b          	addw	s2,s2,s9
   1557a:	8dca                	mv	s11,s2
   1557c:	000bc783          	lbu	a5,0(s7)
   15580:	5e078363          	beqz	a5,15b66 <printf_core+0x634>
   15584:	02500713          	li	a4,37
   15588:	6306                	ld	t1,64(sp)
   1558a:	00034783          	lbu	a5,0(t1)
   1558e:	c399                	beqz	a5,15594 <printf_core+0x62>
   15590:	00e79563          	bne	a5,a4,1559a <printf_core+0x68>
   15594:	02500713          	li	a4,37
   15598:	a039                	j	155a6 <printf_core+0x74>
   1559a:	0305                	addi	t1,t1,1
   1559c:	e09a                	sd	t1,64(sp)
   1559e:	b7ed                	j	15588 <printf_core+0x56>
   155a0:	0789                	addi	a5,a5,2
   155a2:	0305                	addi	t1,t1,1
   155a4:	e0be                	sd	a5,64(sp)
   155a6:	6786                	ld	a5,64(sp)
   155a8:	0007c683          	lbu	a3,0(a5)
   155ac:	00e69663          	bne	a3,a4,155b8 <printf_core+0x86>
   155b0:	0017c683          	lbu	a3,1(a5)
   155b4:	fee686e3          	beq	a3,a4,155a0 <printf_core+0x6e>
   155b8:	80000ab7          	lui	s5,0x80000
   155bc:	fffaca93          	not	s5,s5
   155c0:	41ba87bb          	subw	a5,s5,s11
   155c4:	41730333          	sub	t1,t1,s7
   155c8:	e43e                	sd	a5,8(sp)
   155ca:	5467c163          	blt	a5,t1,15b0c <printf_core+0x5da>
   155ce:	00030c9b          	sext.w	s9,t1
   155d2:	c411                	beqz	s0,155de <printf_core+0xac>
   155d4:	8666                	mv	a2,s9
   155d6:	85de                	mv	a1,s7
   155d8:	8522                	mv	a0,s0
   155da:	b26ff0ef          	jal	ra,14900 <out>
   155de:	520c9463          	bnez	s9,15b06 <printf_core+0x5d4>
   155e2:	6786                	ld	a5,64(sp)
   155e4:	4725                	li	a4,9
   155e6:	0017c683          	lbu	a3,1(a5)
   155ea:	fd06869b          	addiw	a3,a3,-48
   155ee:	12d76c63          	bltu	a4,a3,15726 <printf_core+0x1f4>
   155f2:	0027c603          	lbu	a2,2(a5)
   155f6:	02400713          	li	a4,36
   155fa:	12e61663          	bne	a2,a4,15726 <printf_core+0x1f4>
   155fe:	078d                	addi	a5,a5,3
   15600:	e0be                	sd	a5,64(sp)
   15602:	4c05                	li	s8,1
   15604:	654d                	lui	a0,0x13
   15606:	4d01                	li	s10,0
   15608:	48fd                	li	a7,31
   1560a:	8895051b          	addiw	a0,a0,-1911
   1560e:	4e85                	li	t4,1
   15610:	6706                	ld	a4,64(sp)
   15612:	00074803          	lbu	a6,0(a4)
   15616:	fe08061b          	addiw	a2,a6,-32
   1561a:	00c8e763          	bltu	a7,a2,15628 <printf_core+0xf6>
   1561e:	00c555bb          	srlw	a1,a0,a2
   15622:	8985                	andi	a1,a1,1
   15624:	10059563          	bnez	a1,1572e <printf_core+0x1fc>
   15628:	02a00613          	li	a2,42
   1562c:	12c81e63          	bne	a6,a2,15768 <printf_core+0x236>
   15630:	00174603          	lbu	a2,1(a4)
   15634:	45a5                	li	a1,9
   15636:	fd06051b          	addiw	a0,a2,-48
   1563a:	10a5e363          	bltu	a1,a0,15740 <printf_core+0x20e>
   1563e:	00274503          	lbu	a0,2(a4)
   15642:	02400593          	li	a1,36
   15646:	0eb51d63          	bne	a0,a1,15740 <printf_core+0x20e>
   1564a:	060a                	slli	a2,a2,0x2
   1564c:	965a                	add	a2,a2,s6
   1564e:	45a9                	li	a1,10
   15650:	f4b62023          	sw	a1,-192(a2)
   15654:	00174603          	lbu	a2,1(a4)
   15658:	6782                	ld	a5,0(sp)
   1565a:	070d                	addi	a4,a4,3
   1565c:	0612                	slli	a2,a2,0x4
   1565e:	963e                	add	a2,a2,a5
   15660:	d0062483          	lw	s1,-768(a2)
   15664:	e0ba                	sd	a4,64(sp)
   15666:	4c05                	li	s8,1
   15668:	0004d763          	bgez	s1,15676 <printf_core+0x144>
   1566c:	6709                	lui	a4,0x2
   1566e:	00ed6d33          	or	s10,s10,a4
   15672:	409004bb          	negw	s1,s1
   15676:	6706                	ld	a4,64(sp)
   15678:	02e00613          	li	a2,46
   1567c:	00074583          	lbu	a1,0(a4) # 2000 <exit-0xe120>
   15680:	12c59563          	bne	a1,a2,157aa <printf_core+0x278>
   15684:	00174583          	lbu	a1,1(a4)
   15688:	02a00613          	li	a2,42
   1568c:	10c59563          	bne	a1,a2,15796 <printf_core+0x264>
   15690:	00274603          	lbu	a2,2(a4)
   15694:	45a5                	li	a1,9
   15696:	fd06051b          	addiw	a0,a2,-48
   1569a:	0ea5e063          	bltu	a1,a0,1577a <printf_core+0x248>
   1569e:	00374503          	lbu	a0,3(a4)
   156a2:	02400593          	li	a1,36
   156a6:	0cb51a63          	bne	a0,a1,1577a <printf_core+0x248>
   156aa:	060a                	slli	a2,a2,0x2
   156ac:	965a                	add	a2,a2,s6
   156ae:	45a9                	li	a1,10
   156b0:	f4b62023          	sw	a1,-192(a2)
   156b4:	00274603          	lbu	a2,2(a4)
   156b8:	6782                	ld	a5,0(sp)
   156ba:	0711                	addi	a4,a4,4
   156bc:	0612                	slli	a2,a2,0x4
   156be:	963e                	add	a2,a2,a5
   156c0:	d0062a83          	lw	s5,-768(a2)
   156c4:	ffface93          	not	t4,s5
   156c8:	e0ba                	sd	a4,64(sp)
   156ca:	01fede9b          	srliw	t4,t4,0x1f
   156ce:	4701                	li	a4,0
   156d0:	03900f13          	li	t5,57
   156d4:	03a00813          	li	a6,58
   156d8:	451d                	li	a0,7
   156da:	6606                	ld	a2,64(sp)
   156dc:	00064583          	lbu	a1,0(a2)
   156e0:	fbf5859b          	addiw	a1,a1,-65
   156e4:	06bf6063          	bltu	t5,a1,15744 <printf_core+0x212>
   156e8:	00160593          	addi	a1,a2,1
   156ec:	e0ae                	sd	a1,64(sp)
   156ee:	00064603          	lbu	a2,0(a2)
   156f2:	67c2                	ld	a5,16(sp)
   156f4:	fbf6059b          	addiw	a1,a2,-65
   156f8:	02071613          	slli	a2,a4,0x20
   156fc:	9201                	srli	a2,a2,0x20
   156fe:	03060633          	mul	a2,a2,a6
   15702:	963e                	add	a2,a2,a5
   15704:	962e                	add	a2,a2,a1
   15706:	00064f83          	lbu	t6,0(a2)
   1570a:	ffff861b          	addiw	a2,t6,-1
   1570e:	000f859b          	sext.w	a1,t6
   15712:	08c57f63          	bgeu	a0,a2,157b0 <printf_core+0x27e>
   15716:	c59d                	beqz	a1,15744 <printf_core+0x212>
   15718:	466d                	li	a2,27
   1571a:	08c59d63          	bne	a1,a2,157b4 <printf_core+0x282>
   1571e:	0206d363          	bgez	a3,15744 <printf_core+0x212>
   15722:	e075                	bnez	s0,15806 <printf_core+0x2d4>
   15724:	bd81                	j	15574 <printf_core+0x42>
   15726:	0785                	addi	a5,a5,1
   15728:	e0be                	sd	a5,64(sp)
   1572a:	56fd                	li	a3,-1
   1572c:	bde1                	j	15604 <printf_core+0xd2>
   1572e:	00ce963b          	sllw	a2,t4,a2
   15732:	00cd67b3          	or	a5,s10,a2
   15736:	0705                	addi	a4,a4,1
   15738:	00078d1b          	sext.w	s10,a5
   1573c:	e0ba                	sd	a4,64(sp)
   1573e:	bdc9                	j	15610 <printf_core+0xde>
   15740:	000c0863          	beqz	s8,15750 <printf_core+0x21e>
   15744:	829fb0ef          	jal	ra,10f6c <__errno_location>
   15748:	47d9                	li	a5,22
   1574a:	c11c                	sw	a5,0(a0)
   1574c:	597d                	li	s2,-1
   1574e:	a061                	j	157d6 <printf_core+0x2a4>
   15750:	4481                	li	s1,0
   15752:	c801                	beqz	s0,15762 <printf_core+0x230>
   15754:	000a3603          	ld	a2,0(s4)
   15758:	4204                	lw	s1,0(a2)
   1575a:	00860593          	addi	a1,a2,8
   1575e:	00ba3023          	sd	a1,0(s4)
   15762:	0705                	addi	a4,a4,1
   15764:	e0ba                	sd	a4,64(sp)
   15766:	b709                	j	15668 <printf_core+0x136>
   15768:	0088                	addi	a0,sp,64
   1576a:	f036                	sd	a3,32(sp)
   1576c:	946ff0ef          	jal	ra,148b2 <getint>
   15770:	7682                	ld	a3,32(sp)
   15772:	84aa                	mv	s1,a0
   15774:	f00551e3          	bgez	a0,15676 <printf_core+0x144>
   15778:	ae51                	j	15b0c <printf_core+0x5da>
   1577a:	fc0c15e3          	bnez	s8,15744 <printf_core+0x212>
   1577e:	4a81                	li	s5,0
   15780:	c809                	beqz	s0,15792 <printf_core+0x260>
   15782:	000a3603          	ld	a2,0(s4)
   15786:	00062a83          	lw	s5,0(a2)
   1578a:	00860593          	addi	a1,a2,8
   1578e:	00ba3023          	sd	a1,0(s4)
   15792:	0709                	addi	a4,a4,2
   15794:	bf05                	j	156c4 <printf_core+0x192>
   15796:	0705                	addi	a4,a4,1
   15798:	0088                	addi	a0,sp,64
   1579a:	f036                	sd	a3,32(sp)
   1579c:	e0ba                	sd	a4,64(sp)
   1579e:	914ff0ef          	jal	ra,148b2 <getint>
   157a2:	7682                	ld	a3,32(sp)
   157a4:	8aaa                	mv	s5,a0
   157a6:	4e85                	li	t4,1
   157a8:	b71d                	j	156ce <printf_core+0x19c>
   157aa:	4e81                	li	t4,0
   157ac:	5afd                	li	s5,-1
   157ae:	b705                	j	156ce <printf_core+0x19c>
   157b0:	872e                	mv	a4,a1
   157b2:	b725                	j	156da <printf_core+0x1a8>
   157b4:	0006cf63          	bltz	a3,157d2 <printf_core+0x2a0>
   157b8:	6782                	ld	a5,0(sp)
   157ba:	00269613          	slli	a2,a3,0x2
   157be:	0692                	slli	a3,a3,0x4
   157c0:	965a                	add	a2,a2,s6
   157c2:	96be                	add	a3,a3,a5
   157c4:	01f62023          	sw	t6,0(a2)
   157c8:	6290                	ld	a2,0(a3)
   157ca:	6694                	ld	a3,8(a3)
   157cc:	e8b2                	sd	a2,80(sp)
   157ce:	ecb6                	sd	a3,88(sp)
   157d0:	bf89                	j	15722 <printf_core+0x1f0>
   157d2:	e015                	bnez	s0,157f6 <printf_core+0x2c4>
   157d4:	4901                	li	s2,0
   157d6:	60b2                	ld	ra,264(sp)
   157d8:	6412                	ld	s0,256(sp)
   157da:	74ee                	ld	s1,248(sp)
   157dc:	79ae                	ld	s3,232(sp)
   157de:	7a0e                	ld	s4,224(sp)
   157e0:	6aee                	ld	s5,216(sp)
   157e2:	6b4e                	ld	s6,208(sp)
   157e4:	6bae                	ld	s7,200(sp)
   157e6:	6c0e                	ld	s8,192(sp)
   157e8:	7cea                	ld	s9,184(sp)
   157ea:	7d4a                	ld	s10,176(sp)
   157ec:	7daa                	ld	s11,168(sp)
   157ee:	854a                	mv	a0,s2
   157f0:	794e                	ld	s2,240(sp)
   157f2:	6151                	addi	sp,sp,272
   157f4:	8082                	ret
   157f6:	8652                	mv	a2,s4
   157f8:	0888                	addi	a0,sp,80
   157fa:	f43a                	sd	a4,40(sp)
   157fc:	f076                	sd	t4,32(sp)
   157fe:	ff1fe0ef          	jal	ra,147ee <pop_arg>
   15802:	7e82                	ld	t4,32(sp)
   15804:	7722                	ld	a4,40(sp)
   15806:	6686                	ld	a3,64(sp)
   15808:	fff6c683          	lbu	a3,-1(a3)
   1580c:	0006881b          	sext.w	a6,a3
   15810:	c719                	beqz	a4,1581e <printf_core+0x2ec>
   15812:	8abd                	andi	a3,a3,15
   15814:	460d                	li	a2,3
   15816:	00c69463          	bne	a3,a2,1581e <printf_core+0x2ec>
   1581a:	0df87813          	andi	a6,a6,223
   1581e:	032d1793          	slli	a5,s10,0x32
   15822:	0007d663          	bgez	a5,1582e <printf_core+0x2fc>
   15826:	76c1                	lui	a3,0xffff0
   15828:	16fd                	addi	a3,a3,-1
   1582a:	00dd7d33          	and	s10,s10,a3
   1582e:	fbf8069b          	addiw	a3,a6,-65
   15832:	0006859b          	sext.w	a1,a3
   15836:	03700613          	li	a2,55
   1583a:	32b66063          	bltu	a2,a1,15b5a <printf_core+0x628>
   1583e:	67e2                	ld	a5,24(sp)
   15840:	1682                	slli	a3,a3,0x20
   15842:	82f9                	srli	a3,a3,0x1e
   15844:	96be                	add	a3,a3,a5
   15846:	4294                	lw	a3,0(a3)
   15848:	8682                	jr	a3
   1584a:	479d                	li	a5,7
   1584c:	2ae7ed63          	bltu	a5,a4,15b06 <printf_core+0x5d4>
   15850:	00271793          	slli	a5,a4,0x2
   15854:	6769                	lui	a4,0x1a
   15856:	d8070713          	addi	a4,a4,-640 # 19d80 <__fsmu8+0x250>
   1585a:	97ba                	add	a5,a5,a4
   1585c:	439c                	lw	a5,0(a5)
   1585e:	8782                	jr	a5
   15860:	67c6                	ld	a5,80(sp)
   15862:	01b7a023          	sw	s11,0(a5)
   15866:	b339                	j	15574 <printf_core+0x42>
   15868:	67c6                	ld	a5,80(sp)
   1586a:	01279023          	sh	s2,0(a5)
   1586e:	b319                	j	15574 <printf_core+0x42>
   15870:	67c6                	ld	a5,80(sp)
   15872:	01278023          	sb	s2,0(a5)
   15876:	b9fd                	j	15574 <printf_core+0x42>
   15878:	67c6                	ld	a5,80(sp)
   1587a:	0127b023          	sd	s2,0(a5)
   1587e:	b9dd                	j	15574 <printf_core+0x42>
   15880:	000a889b          	sext.w	a7,s5
   15884:	46c1                	li	a3,16
   15886:	8756                	mv	a4,s5
   15888:	00d8f363          	bgeu	a7,a3,1588e <printf_core+0x35c>
   1588c:	4741                	li	a4,16
   1588e:	00070a9b          	sext.w	s5,a4
   15892:	008d6d13          	ori	s10,s10,8
   15896:	07800813          	li	a6,120
   1589a:	6546                	ld	a0,80(sp)
   1589c:	02087693          	andi	a3,a6,32
   158a0:	6669                	lui	a2,0x1a
   158a2:	2681                	sext.w	a3,a3
   158a4:	872a                	mv	a4,a0
   158a6:	09f10b93          	addi	s7,sp,159
   158aa:	ff060613          	addi	a2,a2,-16 # 19ff0 <xdigits>
   158ae:	0e071663          	bnez	a4,1599a <printf_core+0x468>
   158b2:	8dce                	mv	s11,s3
   158b4:	c909                	beqz	a0,158c6 <printf_core+0x394>
   158b6:	008d7713          	andi	a4,s10,8
   158ba:	c711                	beqz	a4,158c6 <printf_core+0x394>
   158bc:	00485813          	srli	a6,a6,0x4
   158c0:	01098db3          	add	s11,s3,a6
   158c4:	4c89                	li	s9,2
   158c6:	000e8863          	beqz	t4,158d6 <printf_core+0x3a4>
   158ca:	240ac163          	bltz	s5,15b0c <printf_core+0x5da>
   158ce:	7741                	lui	a4,0xffff0
   158d0:	177d                	addi	a4,a4,-1
   158d2:	00ed7d33          	and	s10,s10,a4
   158d6:	66c6                	ld	a3,80(sp)
   158d8:	09f10613          	addi	a2,sp,159
   158dc:	e299                	bnez	a3,158e2 <printf_core+0x3b0>
   158de:	280a8263          	beqz	s5,15b62 <printf_core+0x630>
   158e2:	41760733          	sub	a4,a2,s7
   158e6:	0016b693          	seqz	a3,a3
   158ea:	9736                	add	a4,a4,a3
   158ec:	01575363          	bge	a4,s5,158f2 <printf_core+0x3c0>
   158f0:	8756                	mv	a4,s5
   158f2:	00070a9b          	sext.w	s5,a4
   158f6:	41760833          	sub	a6,a2,s7
   158fa:	010ad463          	bge	s5,a6,15902 <printf_core+0x3d0>
   158fe:	00080a9b          	sext.w	s5,a6
   15902:	80000737          	lui	a4,0x80000
   15906:	fff74713          	not	a4,a4
   1590a:	4197073b          	subw	a4,a4,s9
   1590e:	1f574f63          	blt	a4,s5,15b0c <printf_core+0x5da>
   15912:	019a8ebb          	addw	t4,s5,s9
   15916:	8776                	mv	a4,t4
   15918:	009ed363          	bge	t4,s1,1591e <printf_core+0x3ec>
   1591c:	8726                	mv	a4,s1
   1591e:	67a2                	ld	a5,8(sp)
   15920:	f442                	sd	a6,40(sp)
   15922:	0007049b          	sext.w	s1,a4
   15926:	1e97c363          	blt	a5,s1,15b0c <printf_core+0x5da>
   1592a:	86f6                	mv	a3,t4
   1592c:	876a                	mv	a4,s10
   1592e:	8626                	mv	a2,s1
   15930:	02000593          	li	a1,32
   15934:	8522                	mv	a0,s0
   15936:	e476                	sd	t4,8(sp)
   15938:	fdffe0ef          	jal	ra,14916 <pad>
   1593c:	8666                	mv	a2,s9
   1593e:	85ee                	mv	a1,s11
   15940:	8522                	mv	a0,s0
   15942:	fbffe0ef          	jal	ra,14900 <out>
   15946:	6ea2                	ld	t4,8(sp)
   15948:	6741                	lui	a4,0x10
   1594a:	00ed4733          	xor	a4,s10,a4
   1594e:	86f6                	mv	a3,t4
   15950:	8626                	mv	a2,s1
   15952:	03000593          	li	a1,48
   15956:	8522                	mv	a0,s0
   15958:	f076                	sd	t4,32(sp)
   1595a:	fbdfe0ef          	jal	ra,14916 <pad>
   1595e:	7822                	ld	a6,40(sp)
   15960:	4701                	li	a4,0
   15962:	8656                	mv	a2,s5
   15964:	0008069b          	sext.w	a3,a6
   15968:	03000593          	li	a1,48
   1596c:	8522                	mv	a0,s0
   1596e:	e442                	sd	a6,8(sp)
   15970:	fa7fe0ef          	jal	ra,14916 <pad>
   15974:	6822                	ld	a6,8(sp)
   15976:	85de                	mv	a1,s7
   15978:	8522                	mv	a0,s0
   1597a:	8642                	mv	a2,a6
   1597c:	f85fe0ef          	jal	ra,14900 <out>
   15980:	7e82                	ld	t4,32(sp)
   15982:	6709                	lui	a4,0x2
   15984:	00ed4733          	xor	a4,s10,a4
   15988:	86f6                	mv	a3,t4
   1598a:	8626                	mv	a2,s1
   1598c:	02000593          	li	a1,32
   15990:	8522                	mv	a0,s0
   15992:	f85fe0ef          	jal	ra,14916 <pad>
   15996:	8ca6                	mv	s9,s1
   15998:	bef1                	j	15574 <printf_core+0x42>
   1599a:	00f77593          	andi	a1,a4,15
   1599e:	95b2                	add	a1,a1,a2
   159a0:	0005c583          	lbu	a1,0(a1)
   159a4:	1bfd                	addi	s7,s7,-1
   159a6:	8311                	srli	a4,a4,0x4
   159a8:	8dd5                	or	a1,a1,a3
   159aa:	00bb8023          	sb	a1,0(s7)
   159ae:	b701                	j	158ae <printf_core+0x37c>
   159b0:	66c6                	ld	a3,80(sp)
   159b2:	09f10b93          	addi	s7,sp,159
   159b6:	875e                	mv	a4,s7
   159b8:	ee89                	bnez	a3,159d2 <printf_core+0x4a0>
   159ba:	008d7693          	andi	a3,s10,8
   159be:	8dce                	mv	s11,s3
   159c0:	f00683e3          	beqz	a3,158c6 <printf_core+0x394>
   159c4:	41770733          	sub	a4,a4,s7
   159c8:	ef574fe3          	blt	a4,s5,158c6 <printf_core+0x394>
   159cc:	00170a9b          	addiw	s5,a4,1
   159d0:	bddd                	j	158c6 <printf_core+0x394>
   159d2:	0076f613          	andi	a2,a3,7
   159d6:	1bfd                	addi	s7,s7,-1
   159d8:	0306061b          	addiw	a2,a2,48
   159dc:	00cb8023          	sb	a2,0(s7)
   159e0:	828d                	srli	a3,a3,0x3
   159e2:	bfd9                	j	159b8 <printf_core+0x486>
   159e4:	6746                	ld	a4,80(sp)
   159e6:	02075063          	bgez	a4,15a06 <printf_core+0x4d4>
   159ea:	40e00733          	neg	a4,a4
   159ee:	e8ba                	sd	a4,80(sp)
   159f0:	4c85                	li	s9,1
   159f2:	8dce                	mv	s11,s3
   159f4:	6546                	ld	a0,80(sp)
   159f6:	09f10593          	addi	a1,sp,159
   159fa:	f076                	sd	t4,32(sp)
   159fc:	e99fe0ef          	jal	ra,14894 <fmt_u>
   15a00:	7e82                	ld	t4,32(sp)
   15a02:	8baa                	mv	s7,a0
   15a04:	b5c9                	j	158c6 <printf_core+0x394>
   15a06:	034d1793          	slli	a5,s10,0x34
   15a0a:	0007ca63          	bltz	a5,15a1e <printf_core+0x4ec>
   15a0e:	001d7713          	andi	a4,s10,1
   15a12:	d365                	beqz	a4,159f2 <printf_core+0x4c0>
   15a14:	6de9                	lui	s11,0x1a
   15a16:	4c85                	li	s9,1
   15a18:	c8ad8d93          	addi	s11,s11,-886 # 19c8a <__fsmu8+0x15a>
   15a1c:	bfe1                	j	159f4 <printf_core+0x4c2>
   15a1e:	6de9                	lui	s11,0x1a
   15a20:	4c85                	li	s9,1
   15a22:	c89d8d93          	addi	s11,s11,-887 # 19c89 <__fsmu8+0x159>
   15a26:	b7f9                	j	159f4 <printf_core+0x4c2>
   15a28:	6746                	ld	a4,80(sp)
   15a2a:	8dce                	mv	s11,s3
   15a2c:	4a85                	li	s5,1
   15a2e:	08e10f23          	sb	a4,158(sp)
   15a32:	7741                	lui	a4,0xffff0
   15a34:	177d                	addi	a4,a4,-1
   15a36:	00ed7d33          	and	s10,s10,a4
   15a3a:	09f10613          	addi	a2,sp,159
   15a3e:	09e10b93          	addi	s7,sp,158
   15a42:	bd55                	j	158f6 <printf_core+0x3c4>
   15a44:	d28fb0ef          	jal	ra,10f6c <__errno_location>
   15a48:	4108                	lw	a0,0(a0)
   15a4a:	d4efb0ef          	jal	ra,10f98 <strerror>
   15a4e:	8baa                	mv	s7,a0
   15a50:	85d6                	mv	a1,s5
   15a52:	000ad663          	bgez	s5,15a5e <printf_core+0x52c>
   15a56:	800005b7          	lui	a1,0x80000
   15a5a:	fff5c593          	not	a1,a1
   15a5e:	855e                	mv	a0,s7
   15a60:	266000ef          	jal	ra,15cc6 <strnlen>
   15a64:	00ab8633          	add	a2,s7,a0
   15a68:	000ad563          	bgez	s5,15a72 <printf_core+0x540>
   15a6c:	00064703          	lbu	a4,0(a2)
   15a70:	ef51                	bnez	a4,15b0c <printf_core+0x5da>
   15a72:	7741                	lui	a4,0xffff0
   15a74:	177d                	addi	a4,a4,-1
   15a76:	00050a9b          	sext.w	s5,a0
   15a7a:	00ed7d33          	and	s10,s10,a4
   15a7e:	8dce                	mv	s11,s3
   15a80:	bd9d                	j	158f6 <printf_core+0x3c4>
   15a82:	6bc6                	ld	s7,80(sp)
   15a84:	fc0b96e3          	bnez	s7,15a50 <printf_core+0x51e>
   15a88:	6e69                	lui	t3,0x1a
   15a8a:	c98e0b93          	addi	s7,t3,-872 # 19c98 <__fsmu8+0x168>
   15a8e:	b7c9                	j	15a50 <printf_core+0x51e>
   15a90:	6746                	ld	a4,80(sp)
   15a92:	c682                	sw	zero,76(sp)
   15a94:	5afd                	li	s5,-1
   15a96:	c4ba                	sw	a4,72(sp)
   15a98:	00b8                	addi	a4,sp,72
   15a9a:	e8ba                	sd	a4,80(sp)
   15a9c:	6dc6                	ld	s11,80(sp)
   15a9e:	4c81                	li	s9,0
   15aa0:	015cff63          	bgeu	s9,s5,15abe <printf_core+0x58c>
   15aa4:	000da583          	lw	a1,0(s11)
   15aa8:	c999                	beqz	a1,15abe <printf_core+0x58c>
   15aaa:	1828                	addi	a0,sp,56
   15aac:	0d91                	addi	s11,s11,4
   15aae:	101000ef          	jal	ra,163ae <wctomb>
   15ab2:	c8054de3          	bltz	a0,1574c <printf_core+0x21a>
   15ab6:	419a8733          	sub	a4,s5,s9
   15aba:	04a77e63          	bgeu	a4,a0,15b16 <printf_core+0x5e4>
   15abe:	80000737          	lui	a4,0x80000
   15ac2:	fff74713          	not	a4,a4
   15ac6:	05976363          	bltu	a4,s9,15b0c <printf_core+0x5da>
   15aca:	000c8d9b          	sext.w	s11,s9
   15ace:	876a                	mv	a4,s10
   15ad0:	86ee                	mv	a3,s11
   15ad2:	8626                	mv	a2,s1
   15ad4:	02000593          	li	a1,32
   15ad8:	8522                	mv	a0,s0
   15ada:	e3dfe0ef          	jal	ra,14916 <pad>
   15ade:	6bc6                	ld	s7,80(sp)
   15ae0:	4a81                	li	s5,0
   15ae2:	039aec63          	bltu	s5,s9,15b1a <printf_core+0x5e8>
   15ae6:	6709                	lui	a4,0x2
   15ae8:	00ed4733          	xor	a4,s10,a4
   15aec:	86ee                	mv	a3,s11
   15aee:	8626                	mv	a2,s1
   15af0:	02000593          	li	a1,32
   15af4:	8522                	mv	a0,s0
   15af6:	e21fe0ef          	jal	ra,14916 <pad>
   15afa:	8326                	mv	t1,s1
   15afc:	01b4d363          	bge	s1,s11,15b02 <printf_core+0x5d0>
   15b00:	836e                	mv	t1,s11
   15b02:	00030c9b          	sext.w	s9,t1
   15b06:	67a2                	ld	a5,8(sp)
   15b08:	a797d6e3          	bge	a5,s9,15574 <printf_core+0x42>
   15b0c:	c60fb0ef          	jal	ra,10f6c <__errno_location>
   15b10:	04b00793          	li	a5,75
   15b14:	b91d                	j	1574a <printf_core+0x218>
   15b16:	9caa                	add	s9,s9,a0
   15b18:	b761                	j	15aa0 <printf_core+0x56e>
   15b1a:	000ba583          	lw	a1,0(s7)
   15b1e:	d5e1                	beqz	a1,15ae6 <printf_core+0x5b4>
   15b20:	1828                	addi	a0,sp,56
   15b22:	08d000ef          	jal	ra,163ae <wctomb>
   15b26:	9aaa                	add	s5,s5,a0
   15b28:	0b91                	addi	s7,s7,4
   15b2a:	862a                	mv	a2,a0
   15b2c:	fb5cede3          	bltu	s9,s5,15ae6 <printf_core+0x5b4>
   15b30:	182c                	addi	a1,sp,56
   15b32:	8522                	mv	a0,s0
   15b34:	dcdfe0ef          	jal	ra,14900 <out>
   15b38:	b76d                	j	15ae2 <printf_core+0x5b0>
   15b3a:	000e8463          	beqz	t4,15b42 <printf_core+0x610>
   15b3e:	fc0ac7e3          	bltz	s5,15b0c <printf_core+0x5da>
   15b42:	65c6                	ld	a1,80(sp)
   15b44:	6666                	ld	a2,88(sp)
   15b46:	87ea                	mv	a5,s10
   15b48:	8756                	mv	a4,s5
   15b4a:	86a6                	mv	a3,s1
   15b4c:	8522                	mv	a0,s0
   15b4e:	e3dfe0ef          	jal	ra,1498a <fmt_fp>
   15b52:	8caa                	mv	s9,a0
   15b54:	fa0559e3          	bgez	a0,15b06 <printf_core+0x5d4>
   15b58:	bf55                	j	15b0c <printf_core+0x5da>
   15b5a:	8dce                	mv	s11,s3
   15b5c:	09f10613          	addi	a2,sp,159
   15b60:	bb59                	j	158f6 <printf_core+0x3c4>
   15b62:	8bb2                	mv	s7,a2
   15b64:	bb49                	j	158f6 <printf_core+0x3c4>
   15b66:	c60418e3          	bnez	s0,157d6 <printf_core+0x2a4>
   15b6a:	c60c05e3          	beqz	s8,157d4 <printf_core+0x2a2>
   15b6e:	4405                	li	s0,1
   15b70:	44a9                	li	s1,10
   15b72:	00241793          	slli	a5,s0,0x2
   15b76:	97da                	add	a5,a5,s6
   15b78:	438c                	lw	a1,0(a5)
   15b7a:	ed81                	bnez	a1,15b92 <printf_core+0x660>
   15b7c:	4729                	li	a4,10
   15b7e:	00241793          	slli	a5,s0,0x2
   15b82:	97da                	add	a5,a5,s6
   15b84:	439c                	lw	a5,0(a5)
   15b86:	ba079fe3          	bnez	a5,15744 <printf_core+0x212>
   15b8a:	0405                	addi	s0,s0,1
   15b8c:	fee419e3          	bne	s0,a4,15b7e <printf_core+0x64c>
   15b90:	a819                	j	15ba6 <printf_core+0x674>
   15b92:	6782                	ld	a5,0(sp)
   15b94:	00441513          	slli	a0,s0,0x4
   15b98:	8652                	mv	a2,s4
   15b9a:	953e                	add	a0,a0,a5
   15b9c:	0405                	addi	s0,s0,1
   15b9e:	c51fe0ef          	jal	ra,147ee <pop_arg>
   15ba2:	fc9418e3          	bne	s0,s1,15b72 <printf_core+0x640>
   15ba6:	4905                	li	s2,1
   15ba8:	b13d                	j	157d6 <printf_core+0x2a4>

0000000000015baa <vfprintf>:
   15baa:	710d                	addi	sp,sp,-352
   15bac:	eaa2                	sd	s0,336(sp)
   15bae:	e032                	sd	a2,0(sp)
   15bb0:	842a                	mv	s0,a0
   15bb2:	0038                	addi	a4,sp,8
   15bb4:	0114                	addi	a3,sp,128
   15bb6:	860a                	mv	a2,sp
   15bb8:	4501                	li	a0,0
   15bba:	e6a6                	sd	s1,328(sp)
   15bbc:	f656                	sd	s5,296(sp)
   15bbe:	ee86                	sd	ra,344(sp)
   15bc0:	e2ca                	sd	s2,320(sp)
   15bc2:	fe4e                	sd	s3,312(sp)
   15bc4:	fa52                	sd	s4,304(sp)
   15bc6:	8aae                	mv	s5,a1
   15bc8:	e402                	sd	zero,8(sp)
   15bca:	e802                	sd	zero,16(sp)
   15bcc:	ec02                	sd	zero,24(sp)
   15bce:	f002                	sd	zero,32(sp)
   15bd0:	f402                	sd	zero,40(sp)
   15bd2:	961ff0ef          	jal	ra,15532 <printf_core>
   15bd6:	54fd                	li	s1,-1
   15bd8:	0a054663          	bltz	a0,15c84 <vfprintf+0xda>
   15bdc:	08c42783          	lw	a5,140(s0)
   15be0:	4a01                	li	s4,0
   15be2:	02079713          	slli	a4,a5,0x20
   15be6:	00074663          	bltz	a4,15bf2 <vfprintf+0x48>
   15bea:	8522                	mv	a0,s0
   15bec:	b62fd0ef          	jal	ra,12f4e <__lockfile>
   15bf0:	8a2a                	mv	s4,a0
   15bf2:	401c                	lw	a5,0(s0)
   15bf4:	08842703          	lw	a4,136(s0)
   15bf8:	0207f913          	andi	s2,a5,32
   15bfc:	2901                	sext.w	s2,s2
   15bfe:	00e04563          	bgtz	a4,15c08 <vfprintf+0x5e>
   15c02:	fdf7f793          	andi	a5,a5,-33
   15c06:	c01c                	sw	a5,0(s0)
   15c08:	703c                	ld	a5,96(s0)
   15c0a:	e7d9                	bnez	a5,15c98 <vfprintf+0xee>
   15c0c:	181c                	addi	a5,sp,48
   15c0e:	05843983          	ld	s3,88(s0)
   15c12:	ec3c                	sd	a5,88(s0)
   15c14:	05000793          	li	a5,80
   15c18:	f03c                	sd	a5,96(s0)
   15c1a:	02043023          	sd	zero,32(s0)
   15c1e:	02043c23          	sd	zero,56(s0)
   15c22:	02043423          	sd	zero,40(s0)
   15c26:	8522                	mv	a0,s0
   15c28:	b8bfe0ef          	jal	ra,147b2 <__towrite>
   15c2c:	54fd                	li	s1,-1
   15c2e:	e909                	bnez	a0,15c40 <vfprintf+0x96>
   15c30:	0038                	addi	a4,sp,8
   15c32:	0114                	addi	a3,sp,128
   15c34:	860a                	mv	a2,sp
   15c36:	85d6                	mv	a1,s5
   15c38:	8522                	mv	a0,s0
   15c3a:	8f9ff0ef          	jal	ra,15532 <printf_core>
   15c3e:	84aa                	mv	s1,a0
   15c40:	02098463          	beqz	s3,15c68 <vfprintf+0xbe>
   15c44:	643c                	ld	a5,72(s0)
   15c46:	4601                	li	a2,0
   15c48:	4581                	li	a1,0
   15c4a:	8522                	mv	a0,s0
   15c4c:	9782                	jalr	a5
   15c4e:	741c                	ld	a5,40(s0)
   15c50:	e391                	bnez	a5,15c54 <vfprintf+0xaa>
   15c52:	54fd                	li	s1,-1
   15c54:	05343c23          	sd	s3,88(s0)
   15c58:	06043023          	sd	zero,96(s0)
   15c5c:	02043023          	sd	zero,32(s0)
   15c60:	02043c23          	sd	zero,56(s0)
   15c64:	02043423          	sd	zero,40(s0)
   15c68:	401c                	lw	a5,0(s0)
   15c6a:	0207f713          	andi	a4,a5,32
   15c6e:	c311                	beqz	a4,15c72 <vfprintf+0xc8>
   15c70:	54fd                	li	s1,-1
   15c72:	00f96933          	or	s2,s2,a5
   15c76:	01242023          	sw	s2,0(s0)
   15c7a:	000a0563          	beqz	s4,15c84 <vfprintf+0xda>
   15c7e:	8522                	mv	a0,s0
   15c80:	b6afd0ef          	jal	ra,12fea <__unlockfile>
   15c84:	60f6                	ld	ra,344(sp)
   15c86:	6456                	ld	s0,336(sp)
   15c88:	6916                	ld	s2,320(sp)
   15c8a:	79f2                	ld	s3,312(sp)
   15c8c:	7a52                	ld	s4,304(sp)
   15c8e:	7ab2                	ld	s5,296(sp)
   15c90:	8526                	mv	a0,s1
   15c92:	64b6                	ld	s1,328(sp)
   15c94:	6135                	addi	sp,sp,352
   15c96:	8082                	ret
   15c98:	02043983          	ld	s3,32(s0)
   15c9c:	f80985e3          	beqz	s3,15c26 <vfprintf+0x7c>
   15ca0:	4981                	li	s3,0
   15ca2:	b779                	j	15c30 <vfprintf+0x86>

0000000000015ca4 <__memrchr>:
   15ca4:	167d                	addi	a2,a2,-1
   15ca6:	0ff5f593          	andi	a1,a1,255
   15caa:	962a                	add	a2,a2,a0
   15cac:	fff50713          	addi	a4,a0,-1 # 12fff <__unlockfile+0x15>
   15cb0:	a031                	j	15cbc <__memrchr+0x18>
   15cb2:	00054783          	lbu	a5,0(a0)
   15cb6:	167d                	addi	a2,a2,-1
   15cb8:	00b78663          	beq	a5,a1,15cc4 <__memrchr+0x20>
   15cbc:	8532                	mv	a0,a2
   15cbe:	fee61ae3          	bne	a2,a4,15cb2 <__memrchr+0xe>
   15cc2:	4501                	li	a0,0
   15cc4:	8082                	ret

0000000000015cc6 <strnlen>:
   15cc6:	1101                	addi	sp,sp,-32
   15cc8:	e822                	sd	s0,16(sp)
   15cca:	862e                	mv	a2,a1
   15ccc:	842e                	mv	s0,a1
   15cce:	4581                	li	a1,0
   15cd0:	e426                	sd	s1,8(sp)
   15cd2:	ec06                	sd	ra,24(sp)
   15cd4:	84aa                	mv	s1,a0
   15cd6:	79c000ef          	jal	ra,16472 <memchr>
   15cda:	c519                	beqz	a0,15ce8 <strnlen+0x22>
   15cdc:	60e2                	ld	ra,24(sp)
   15cde:	6442                	ld	s0,16(sp)
   15ce0:	8d05                	sub	a0,a0,s1
   15ce2:	64a2                	ld	s1,8(sp)
   15ce4:	6105                	addi	sp,sp,32
   15ce6:	8082                	ret
   15ce8:	60e2                	ld	ra,24(sp)
   15cea:	8522                	mv	a0,s0
   15cec:	6442                	ld	s0,16(sp)
   15cee:	64a2                	ld	s1,8(sp)
   15cf0:	6105                	addi	sp,sp,32
   15cf2:	8082                	ret

0000000000015cf4 <__timedwait_cp>:
   15cf4:	7139                	addi	sp,sp,-64
   15cf6:	f822                	sd	s0,48(sp)
   15cf8:	f426                	sd	s1,40(sp)
   15cfa:	f04a                	sd	s2,32(sp)
   15cfc:	ec4e                	sd	s3,24(sp)
   15cfe:	fc06                	sd	ra,56(sp)
   15d00:	e852                	sd	s4,16(sp)
   15d02:	84aa                	mv	s1,a0
   15d04:	892e                	mv	s2,a1
   15d06:	8532                	mv	a0,a2
   15d08:	8436                	mv	s0,a3
   15d0a:	08000993          	li	s3,128
   15d0e:	e311                	bnez	a4,15d12 <__timedwait_cp+0x1e>
   15d10:	89ba                	mv	s3,a4
   15d12:	c839                	beqz	s0,15d68 <__timedwait_cp+0x74>
   15d14:	6418                	ld	a4,8(s0)
   15d16:	3b9ada37          	lui	s4,0x3b9ad
   15d1a:	9ffa0793          	addi	a5,s4,-1537 # 3b9ac9ff <__BSS_END__+0x3b98ffdf>
   15d1e:	00e7fb63          	bgeu	a5,a4,15d34 <__timedwait_cp+0x40>
   15d22:	4559                	li	a0,22
   15d24:	70e2                	ld	ra,56(sp)
   15d26:	7442                	ld	s0,48(sp)
   15d28:	74a2                	ld	s1,40(sp)
   15d2a:	7902                	ld	s2,32(sp)
   15d2c:	69e2                	ld	s3,24(sp)
   15d2e:	6a42                	ld	s4,16(sp)
   15d30:	6121                	addi	sp,sp,64
   15d32:	8082                	ret
   15d34:	858a                	mv	a1,sp
   15d36:	324000ef          	jal	ra,1605a <__clock_gettime>
   15d3a:	f565                	bnez	a0,15d22 <__timedwait_cp+0x2e>
   15d3c:	6782                	ld	a5,0(sp)
   15d3e:	6014                	ld	a3,0(s0)
   15d40:	6722                	ld	a4,8(sp)
   15d42:	8e9d                	sub	a3,a3,a5
   15d44:	641c                	ld	a5,8(s0)
   15d46:	e036                	sd	a3,0(sp)
   15d48:	8f99                	sub	a5,a5,a4
   15d4a:	e43e                	sd	a5,8(sp)
   15d4c:	0007d863          	bgez	a5,15d5c <__timedwait_cp+0x68>
   15d50:	a00a0a13          	addi	s4,s4,-1536
   15d54:	16fd                	addi	a3,a3,-1
   15d56:	97d2                	add	a5,a5,s4
   15d58:	e036                	sd	a3,0(sp)
   15d5a:	e43e                	sd	a5,8(sp)
   15d5c:	6782                	ld	a5,0(sp)
   15d5e:	06e00513          	li	a0,110
   15d62:	fc07c1e3          	bltz	a5,15d24 <__timedwait_cp+0x30>
   15d66:	840a                	mv	s0,sp
   15d68:	4781                	li	a5,0
   15d6a:	8722                	mv	a4,s0
   15d6c:	4801                	li	a6,0
   15d6e:	86ca                	mv	a3,s2
   15d70:	864e                	mv	a2,s3
   15d72:	85a6                	mv	a1,s1
   15d74:	06200513          	li	a0,98
   15d78:	e79fd0ef          	jal	ra,13bf0 <__syscall_cp>
   15d7c:	0005079b          	sext.w	a5,a0
   15d80:	fda00713          	li	a4,-38
   15d84:	00e79e63          	bne	a5,a4,15da0 <__timedwait_cp+0xac>
   15d88:	4781                	li	a5,0
   15d8a:	4801                	li	a6,0
   15d8c:	8722                	mv	a4,s0
   15d8e:	86ca                	mv	a3,s2
   15d90:	4601                	li	a2,0
   15d92:	85a6                	mv	a1,s1
   15d94:	06200513          	li	a0,98
   15d98:	e59fd0ef          	jal	ra,13bf0 <__syscall_cp>
   15d9c:	0005079b          	sext.w	a5,a0
   15da0:	5771                	li	a4,-4
   15da2:	00e78e63          	beq	a5,a4,15dbe <__timedwait_cp+0xca>
   15da6:	f9200713          	li	a4,-110
   15daa:	00e78763          	beq	a5,a4,15db8 <__timedwait_cp+0xc4>
   15dae:	f8300713          	li	a4,-125
   15db2:	4501                	li	a0,0
   15db4:	f6e798e3          	bne	a5,a4,15d24 <__timedwait_cp+0x30>
   15db8:	40f0053b          	negw	a0,a5
   15dbc:	b7a5                	j	15d24 <__timedwait_cp+0x30>
   15dbe:	2141a503          	lw	a0,532(gp) # 1ca14 <__eintr_valid_flag>
   15dc2:	2501                	sext.w	a0,a0
   15dc4:	d125                	beqz	a0,15d24 <__timedwait_cp+0x30>
   15dc6:	4511                	li	a0,4
   15dc8:	bfb1                	j	15d24 <__timedwait_cp+0x30>

0000000000015dca <__timedwait>:
   15dca:	715d                	addi	sp,sp,-80
   15dcc:	e0a2                	sd	s0,64(sp)
   15dce:	fc26                	sd	s1,56(sp)
   15dd0:	842a                	mv	s0,a0
   15dd2:	84ae                	mv	s1,a1
   15dd4:	4505                	li	a0,1
   15dd6:	106c                	addi	a1,sp,44
   15dd8:	e486                	sd	ra,72(sp)
   15dda:	ec32                	sd	a2,24(sp)
   15ddc:	e836                	sd	a3,16(sp)
   15dde:	e43a                	sd	a4,8(sp)
   15de0:	11c000ef          	jal	ra,15efc <__pthread_setcancelstate>
   15de4:	6722                	ld	a4,8(sp)
   15de6:	66c2                	ld	a3,16(sp)
   15de8:	6662                	ld	a2,24(sp)
   15dea:	85a6                	mv	a1,s1
   15dec:	8522                	mv	a0,s0
   15dee:	f07ff0ef          	jal	ra,15cf4 <__timedwait_cp>
   15df2:	842a                	mv	s0,a0
   15df4:	5532                	lw	a0,44(sp)
   15df6:	4581                	li	a1,0
   15df8:	104000ef          	jal	ra,15efc <__pthread_setcancelstate>
   15dfc:	60a6                	ld	ra,72(sp)
   15dfe:	8522                	mv	a0,s0
   15e00:	6406                	ld	s0,64(sp)
   15e02:	74e2                	ld	s1,56(sp)
   15e04:	6161                	addi	sp,sp,80
   15e06:	8082                	ret

0000000000015e08 <a_fetch_add>:
   15e08:	87aa                	mv	a5,a0
   15e0a:	4398                	lw	a4,0(a5)
   15e0c:	0007051b          	sext.w	a0,a4
   15e10:	9f2d                	addw	a4,a4,a1
   15e12:	1607a6af          	lr.w.aqrl	a3,(a5)
   15e16:	00a69563          	bne	a3,a0,15e20 <a_fetch_add+0x18>
   15e1a:	1ee7a62f          	sc.w.aqrl	a2,a4,(a5)
   15e1e:	fa75                	bnez	a2,15e12 <a_fetch_add+0xa>
   15e20:	0006871b          	sext.w	a4,a3
   15e24:	fee513e3          	bne	a0,a4,15e0a <a_fetch_add+0x2>
   15e28:	8082                	ret

0000000000015e2a <__pthread_rwlock_timedwrlock>:
   15e2a:	7139                	addi	sp,sp,-64
   15e2c:	f822                	sd	s0,48(sp)
   15e2e:	f426                	sd	s1,40(sp)
   15e30:	ec4e                	sd	s3,24(sp)
   15e32:	fc06                	sd	ra,56(sp)
   15e34:	f04a                	sd	s2,32(sp)
   15e36:	e852                	sd	s4,16(sp)
   15e38:	e456                	sd	s5,8(sp)
   15e3a:	e05a                	sd	s6,0(sp)
   15e3c:	84aa                	mv	s1,a0
   15e3e:	89ae                	mv	s3,a1
   15e40:	09a000ef          	jal	ra,15eda <__pthread_rwlock_trywrlock>
   15e44:	47c1                	li	a5,16
   15e46:	842a                	mv	s0,a0
   15e48:	06500713          	li	a4,101
   15e4c:	00f50f63          	beq	a0,a5,15e6a <__pthread_rwlock_timedwrlock+0x40>
   15e50:	70e2                	ld	ra,56(sp)
   15e52:	8522                	mv	a0,s0
   15e54:	7442                	ld	s0,48(sp)
   15e56:	74a2                	ld	s1,40(sp)
   15e58:	7902                	ld	s2,32(sp)
   15e5a:	69e2                	ld	s3,24(sp)
   15e5c:	6a42                	ld	s4,16(sp)
   15e5e:	6aa2                	ld	s5,8(sp)
   15e60:	6b02                	ld	s6,0(sp)
   15e62:	6121                	addi	sp,sp,64
   15e64:	8082                	ret
   15e66:	0330000f          	fence	rw,rw
   15e6a:	377d                	addiw	a4,a4,-1
   15e6c:	c719                	beqz	a4,15e7a <__pthread_rwlock_timedwrlock+0x50>
   15e6e:	409c                	lw	a5,0(s1)
   15e70:	2781                	sext.w	a5,a5
   15e72:	c781                	beqz	a5,15e7a <__pthread_rwlock_timedwrlock+0x50>
   15e74:	40dc                	lw	a5,4(s1)
   15e76:	2781                	sext.w	a5,a5
   15e78:	d7fd                	beqz	a5,15e66 <__pthread_rwlock_timedwrlock+0x3c>
   15e7a:	4ac1                	li	s5,16
   15e7c:	80000b37          	lui	s6,0x80000
   15e80:	00448a13          	addi	s4,s1,4
   15e84:	a0a1                	j	15ecc <__pthread_rwlock_timedwrlock+0xa2>
   15e86:	4080                	lw	s0,0(s1)
   15e88:	2401                	sext.w	s0,s0
   15e8a:	c029                	beqz	s0,15ecc <__pthread_rwlock_timedwrlock+0xa2>
   15e8c:	01646933          	or	s2,s0,s6
   15e90:	4585                	li	a1,1
   15e92:	8552                	mv	a0,s4
   15e94:	f75ff0ef          	jal	ra,15e08 <a_fetch_add>
   15e98:	2901                	sext.w	s2,s2
   15e9a:	1604a7af          	lr.w.aqrl	a5,(s1)
   15e9e:	00879563          	bne	a5,s0,15ea8 <__pthread_rwlock_timedwrlock+0x7e>
   15ea2:	1f24a72f          	sc.w.aqrl	a4,s2,(s1)
   15ea6:	fb75                	bnez	a4,15e9a <__pthread_rwlock_timedwrlock+0x70>
   15ea8:	4498                	lw	a4,8(s1)
   15eaa:	86ce                	mv	a3,s3
   15eac:	4601                	li	a2,0
   15eae:	08074713          	xori	a4,a4,128
   15eb2:	85ca                	mv	a1,s2
   15eb4:	8526                	mv	a0,s1
   15eb6:	f15ff0ef          	jal	ra,15dca <__timedwait>
   15eba:	842a                	mv	s0,a0
   15ebc:	55fd                	li	a1,-1
   15ebe:	8552                	mv	a0,s4
   15ec0:	f49ff0ef          	jal	ra,15e08 <a_fetch_add>
   15ec4:	ffb47793          	andi	a5,s0,-5
   15ec8:	2781                	sext.w	a5,a5
   15eca:	f3d9                	bnez	a5,15e50 <__pthread_rwlock_timedwrlock+0x26>
   15ecc:	8526                	mv	a0,s1
   15ece:	00c000ef          	jal	ra,15eda <__pthread_rwlock_trywrlock>
   15ed2:	842a                	mv	s0,a0
   15ed4:	fb5509e3          	beq	a0,s5,15e86 <__pthread_rwlock_timedwrlock+0x5c>
   15ed8:	bfa5                	j	15e50 <__pthread_rwlock_timedwrlock+0x26>

0000000000015eda <__pthread_rwlock_trywrlock>:
   15eda:	80000737          	lui	a4,0x80000
   15ede:	4681                	li	a3,0
   15ee0:	fff74713          	not	a4,a4
   15ee4:	160527af          	lr.w.aqrl	a5,(a0)
   15ee8:	00d79563          	bne	a5,a3,15ef2 <__pthread_rwlock_trywrlock+0x18>
   15eec:	1ee5262f          	sc.w.aqrl	a2,a4,(a0)
   15ef0:	fa75                	bnez	a2,15ee4 <__pthread_rwlock_trywrlock+0xa>
   15ef2:	0007851b          	sext.w	a0,a5
   15ef6:	c111                	beqz	a0,15efa <__pthread_rwlock_trywrlock+0x20>
   15ef8:	4541                	li	a0,16
   15efa:	8082                	ret

0000000000015efc <__pthread_setcancelstate>:
   15efc:	4789                	li	a5,2
   15efe:	02a7e163          	bltu	a5,a0,15f20 <__pthread_setcancelstate+0x24>
   15f02:	8792                	mv	a5,tp
   15f04:	f3878793          	addi	a5,a5,-200
   15f08:	c591                	beqz	a1,15f14 <__pthread_setcancelstate+0x18>
   15f0a:	0307c703          	lbu	a4,48(a5)
   15f0e:	0ff77713          	andi	a4,a4,255
   15f12:	c198                	sw	a4,0(a1)
   15f14:	0ff57513          	andi	a0,a0,255
   15f18:	02a78823          	sb	a0,48(a5)
   15f1c:	4501                	li	a0,0
   15f1e:	8082                	ret
   15f20:	4559                	li	a0,22
   15f22:	8082                	ret

0000000000015f24 <__set_thread_area>:
   15f24:	822a                	mv	tp,a0
   15f26:	4501                	li	a0,0
   15f28:	8082                	ret

0000000000015f2a <sem_destroy>:
   15f2a:	4501                	li	a0,0
   15f2c:	8082                	ret

0000000000015f2e <sem_init>:
   15f2e:	00065c63          	bgez	a2,15f46 <sem_init+0x18>
   15f32:	1141                	addi	sp,sp,-16
   15f34:	e406                	sd	ra,8(sp)
   15f36:	836fb0ef          	jal	ra,10f6c <__errno_location>
   15f3a:	60a2                	ld	ra,8(sp)
   15f3c:	47d9                	li	a5,22
   15f3e:	c11c                	sw	a5,0(a0)
   15f40:	557d                	li	a0,-1
   15f42:	0141                	addi	sp,sp,16
   15f44:	8082                	ret
   15f46:	c110                	sw	a2,0(a0)
   15f48:	0015b593          	seqz	a1,a1
   15f4c:	00052223          	sw	zero,4(a0)
   15f50:	059e                	slli	a1,a1,0x7
   15f52:	c50c                	sw	a1,8(a0)
   15f54:	4501                	li	a0,0
   15f56:	8082                	ret

0000000000015f58 <sem_post>:
   15f58:	450c                	lw	a1,8(a0)
   15f5a:	80000837          	lui	a6,0x80000
   15f5e:	87aa                	mv	a5,a0
   15f60:	2581                	sext.w	a1,a1
   15f62:	fff84813          	not	a6,a6
   15f66:	4398                	lw	a4,0(a5)
   15f68:	43d0                	lw	a2,4(a5)
   15f6a:	2701                	sext.w	a4,a4
   15f6c:	2601                	sext.w	a2,a2
   15f6e:	01071d63          	bne	a4,a6,15f88 <sem_post+0x30>
   15f72:	1141                	addi	sp,sp,-16
   15f74:	e406                	sd	ra,8(sp)
   15f76:	ff7fa0ef          	jal	ra,10f6c <__errno_location>
   15f7a:	60a2                	ld	ra,8(sp)
   15f7c:	04b00793          	li	a5,75
   15f80:	c11c                	sw	a5,0(a0)
   15f82:	557d                	li	a0,-1
   15f84:	0141                	addi	sp,sp,16
   15f86:	8082                	ret
   15f88:	0017069b          	addiw	a3,a4,1
   15f8c:	01f7551b          	srliw	a0,a4,0x1f
   15f90:	9ea9                	addw	a3,a3,a0
   15f92:	1607a52f          	lr.w.aqrl	a0,(a5)
   15f96:	00e51663          	bne	a0,a4,15fa2 <sem_post+0x4a>
   15f9a:	1ed7a8af          	sc.w.aqrl	a7,a3,(a5)
   15f9e:	fe089ae3          	bnez	a7,15f92 <sem_post+0x3a>
   15fa2:	0005069b          	sext.w	a3,a0
   15fa6:	fcd710e3          	bne	a4,a3,15f66 <sem_post+0xe>
   15faa:	00074563          	bltz	a4,15fb4 <sem_post+0x5c>
   15fae:	e219                	bnez	a2,15fb4 <sem_post+0x5c>
   15fb0:	4501                	li	a0,0
   15fb2:	8082                	ret
   15fb4:	c199                	beqz	a1,15fba <sem_post+0x62>
   15fb6:	08000593          	li	a1,128
   15fba:	06200893          	li	a7,98
   15fbe:	853e                	mv	a0,a5
   15fc0:	0015e593          	ori	a1,a1,1
   15fc4:	4605                	li	a2,1
   15fc6:	00000073          	ecall
   15fca:	fda00713          	li	a4,-38
   15fce:	fee511e3          	bne	a0,a4,15fb0 <sem_post+0x58>
   15fd2:	853e                	mv	a0,a5
   15fd4:	4585                	li	a1,1
   15fd6:	00000073          	ecall
   15fda:	bfd9                	j	15fb0 <sem_post+0x58>

0000000000015fdc <sem_wait>:
   15fdc:	4581                	li	a1,0
   15fde:	a341                	j	1655e <sem_timedwait>

0000000000015fe0 <__map_file>:
   15fe0:	7135                	addi	sp,sp,-160
   15fe2:	87aa                	mv	a5,a0
   15fe4:	00089637          	lui	a2,0x89
   15fe8:	e526                	sd	s1,136(sp)
   15fea:	ed06                	sd	ra,152(sp)
   15fec:	84ae                	mv	s1,a1
   15fee:	e922                	sd	s0,144(sp)
   15ff0:	03800893          	li	a7,56
   15ff4:	f9c00513          	li	a0,-100
   15ff8:	85be                	mv	a1,a5
   15ffa:	80060613          	addi	a2,a2,-2048 # 88800 <__BSS_END__+0x6bde0>
   15ffe:	00000073          	ecall
   16002:	846fb0ef          	jal	ra,11048 <__syscall_ret>
   16006:	02051793          	slli	a5,a0,0x20
   1600a:	0007d963          	bgez	a5,1601c <__map_file+0x3c>
   1600e:	4781                	li	a5,0
   16010:	60ea                	ld	ra,152(sp)
   16012:	644a                	ld	s0,144(sp)
   16014:	64aa                	ld	s1,136(sp)
   16016:	853e                	mv	a0,a5
   16018:	610d                	addi	sp,sp,160
   1601a:	8082                	ret
   1601c:	0005041b          	sext.w	s0,a0
   16020:	05000893          	li	a7,80
   16024:	8522                	mv	a0,s0
   16026:	858a                	mv	a1,sp
   16028:	00000073          	ecall
   1602c:	81cfb0ef          	jal	ra,11048 <__syscall_ret>
   16030:	57fd                	li	a5,-1
   16032:	e919                	bnez	a0,16048 <__map_file+0x68>
   16034:	75c2                	ld	a1,48(sp)
   16036:	4781                	li	a5,0
   16038:	8722                	mv	a4,s0
   1603a:	4685                	li	a3,1
   1603c:	4605                	li	a2,1
   1603e:	ab9fc0ef          	jal	ra,12af6 <__mmap>
   16042:	7742                	ld	a4,48(sp)
   16044:	87aa                	mv	a5,a0
   16046:	e098                	sd	a4,0(s1)
   16048:	03900893          	li	a7,57
   1604c:	8522                	mv	a0,s0
   1604e:	00000073          	ecall
   16052:	577d                	li	a4,-1
   16054:	fae79ee3          	bne	a5,a4,16010 <__map_file+0x30>
   16058:	bf5d                	j	1600e <__map_file+0x2e>

000000000001605a <__clock_gettime>:
   1605a:	87aa                	mv	a5,a0
   1605c:	07100893          	li	a7,113
   16060:	00000073          	ecall
   16064:	fda00693          	li	a3,-38
   16068:	2501                	sext.w	a0,a0
   1606a:	02d51363          	bne	a0,a3,16090 <__clock_gettime+0x36>
   1606e:	5529                	li	a0,-22
   16070:	e385                	bnez	a5,16090 <__clock_gettime+0x36>
   16072:	872e                	mv	a4,a1
   16074:	852e                	mv	a0,a1
   16076:	0a900893          	li	a7,169
   1607a:	4581                	li	a1,0
   1607c:	00000073          	ecall
   16080:	6710                	ld	a2,8(a4)
   16082:	3e800693          	li	a3,1000
   16086:	853e                	mv	a0,a5
   16088:	02c686bb          	mulw	a3,a3,a2
   1608c:	e714                	sd	a3,8(a4)
   1608e:	8082                	ret
   16090:	1141                	addi	sp,sp,-16
   16092:	e406                	sd	ra,8(sp)
   16094:	fb5fa0ef          	jal	ra,11048 <__syscall_ret>
   16098:	60a2                	ld	ra,8(sp)
   1609a:	0005079b          	sext.w	a5,a0
   1609e:	853e                	mv	a0,a5
   160a0:	0141                	addi	sp,sp,16
   160a2:	8082                	ret

00000000000160a4 <__lseek>:
   160a4:	03e00893          	li	a7,62
   160a8:	00000073          	ecall
   160ac:	f9dfa06f          	j	11048 <__syscall_ret>

00000000000160b0 <getenv>:
   160b0:	7179                	addi	sp,sp,-48
   160b2:	03d00593          	li	a1,61
   160b6:	ec26                	sd	s1,24(sp)
   160b8:	e84a                	sd	s2,16(sp)
   160ba:	f406                	sd	ra,40(sp)
   160bc:	892a                	mv	s2,a0
   160be:	f022                	sd	s0,32(sp)
   160c0:	e44e                	sd	s3,8(sp)
   160c2:	86bfd0ef          	jal	ra,1392c <__strchrnul>
   160c6:	412504b3          	sub	s1,a0,s2
   160ca:	01251a63          	bne	a0,s2,160de <getenv+0x2e>
   160ce:	4501                	li	a0,0
   160d0:	70a2                	ld	ra,40(sp)
   160d2:	7402                	ld	s0,32(sp)
   160d4:	64e2                	ld	s1,24(sp)
   160d6:	6942                	ld	s2,16(sp)
   160d8:	69a2                	ld	s3,8(sp)
   160da:	6145                	addi	sp,sp,48
   160dc:	8082                	ret
   160de:	00054783          	lbu	a5,0(a0)
   160e2:	f7f5                	bnez	a5,160ce <getenv+0x1e>
   160e4:	9d81b403          	ld	s0,-1576(gp) # 1c1d8 <__environ>
   160e8:	03d00993          	li	s3,61
   160ec:	d06d                	beqz	s0,160ce <getenv+0x1e>
   160ee:	600c                	ld	a1,0(s0)
   160f0:	ddf9                	beqz	a1,160ce <getenv+0x1e>
   160f2:	8626                	mv	a2,s1
   160f4:	854a                	mv	a0,s2
   160f6:	3fe000ef          	jal	ra,164f4 <strncmp>
   160fa:	ed01                	bnez	a0,16112 <getenv+0x62>
   160fc:	601c                	ld	a5,0(s0)
   160fe:	00978733          	add	a4,a5,s1
   16102:	00074703          	lbu	a4,0(a4) # ffffffff80000000 <__BSS_END__+0xffffffff7ffe35e0>
   16106:	01371663          	bne	a4,s3,16112 <getenv+0x62>
   1610a:	00148513          	addi	a0,s1,1
   1610e:	953e                	add	a0,a0,a5
   16110:	b7c1                	j	160d0 <getenv+0x20>
   16112:	0421                	addi	s0,s0,8
   16114:	bfe9                	j	160ee <getenv+0x3e>

0000000000016116 <swapc.part.0>:
   16116:	0185171b          	slliw	a4,a0,0x18
   1611a:	0185579b          	srliw	a5,a0,0x18
   1611e:	66c1                	lui	a3,0x10
   16120:	8fd9                	or	a5,a5,a4
   16122:	f0068693          	addi	a3,a3,-256 # ff00 <exit-0x220>
   16126:	0085571b          	srliw	a4,a0,0x8
   1612a:	8f75                	and	a4,a4,a3
   1612c:	8fd9                	or	a5,a5,a4
   1612e:	0085151b          	slliw	a0,a0,0x8
   16132:	00ff0737          	lui	a4,0xff0
   16136:	8d79                	and	a0,a0,a4
   16138:	8d5d                	or	a0,a0,a5
   1613a:	2501                	sext.w	a0,a0
   1613c:	8082                	ret

000000000001613e <__mo_lookup>:
   1613e:	7119                	addi	sp,sp,-128
   16140:	ecce                	sd	s3,88(sp)
   16142:	00052983          	lw	s3,0(a0)
   16146:	6afbf7b7          	lui	a5,0x6afbf
   1614a:	d227879b          	addiw	a5,a5,-734
   1614e:	f4a6                	sd	s1,104(sp)
   16150:	f0ca                	sd	s2,96(sp)
   16152:	e0da                	sd	s6,64(sp)
   16154:	fc86                	sd	ra,120(sp)
   16156:	f8a2                	sd	s0,112(sp)
   16158:	e8d2                	sd	s4,80(sp)
   1615a:	e4d6                	sd	s5,72(sp)
   1615c:	fc5e                	sd	s7,56(sp)
   1615e:	f862                	sd	s8,48(sp)
   16160:	f466                	sd	s9,40(sp)
   16162:	f06a                	sd	s10,32(sp)
   16164:	ec6e                	sd	s11,24(sp)
   16166:	e032                	sd	a2,0(sp)
   16168:	00f989bb          	addw	s3,s3,a5
   1616c:	00852b03          	lw	s6,8(a0)
   16170:	84aa                	mv	s1,a0
   16172:	892e                	mv	s2,a1
   16174:	00098763          	beqz	s3,16182 <__mo_lookup+0x44>
   16178:	855a                	mv	a0,s6
   1617a:	f9dff0ef          	jal	ra,16116 <swapc.part.0>
   1617e:	00050b1b          	sext.w	s6,a0
   16182:	00c4aa03          	lw	s4,12(s1)
   16186:	00098763          	beqz	s3,16194 <__mo_lookup+0x56>
   1618a:	8552                	mv	a0,s4
   1618c:	f8bff0ef          	jal	ra,16116 <swapc.part.0>
   16190:	00050a1b          	sext.w	s4,a0
   16194:	4880                	lw	s0,16(s1)
   16196:	00098763          	beqz	s3,161a4 <__mo_lookup+0x66>
   1619a:	8522                	mv	a0,s0
   1619c:	f7bff0ef          	jal	ra,16116 <swapc.part.0>
   161a0:	0005041b          	sext.w	s0,a0
   161a4:	020b1793          	slli	a5,s6,0x20
   161a8:	9381                	srli	a5,a5,0x20
   161aa:	00295693          	srli	a3,s2,0x2
   161ae:	10d7fa63          	bgeu	a5,a3,162c2 <__mo_lookup+0x184>
   161b2:	002b179b          	slliw	a5,s6,0x2
   161b6:	1782                	slli	a5,a5,0x20
   161b8:	9381                	srli	a5,a5,0x20
   161ba:	020a1693          	slli	a3,s4,0x20
   161be:	40f907b3          	sub	a5,s2,a5
   161c2:	9281                	srli	a3,a3,0x20
   161c4:	0ef6ff63          	bgeu	a3,a5,162c2 <__mo_lookup+0x184>
   161c8:	02041693          	slli	a3,s0,0x20
   161cc:	9281                	srli	a3,a3,0x20
   161ce:	0ef6fa63          	bgeu	a3,a5,162c2 <__mo_lookup+0x184>
   161d2:	01446ab3          	or	s5,s0,s4
   161d6:	003afa93          	andi	s5,s5,3
   161da:	0e0a9463          	bnez	s5,162c2 <__mo_lookup+0x184>
   161de:	002a5a1b          	srliw	s4,s4,0x2
   161e2:	4d05                	li	s10,1
   161e4:	001b5c1b          	srliw	s8,s6,0x1
   161e8:	015c0cbb          	addw	s9,s8,s5
   161ec:	001c9d9b          	slliw	s11,s9,0x1
   161f0:	014d87bb          	addw	a5,s11,s4
   161f4:	02079693          	slli	a3,a5,0x20
   161f8:	82f9                	srli	a3,a3,0x1e
   161fa:	96a6                	add	a3,a3,s1
   161fc:	4294                	lw	a3,0(a3)
   161fe:	001b5b9b          	srliw	s7,s6,0x1
   16202:	00098963          	beqz	s3,16214 <__mo_lookup+0xd6>
   16206:	8536                	mv	a0,a3
   16208:	e43e                	sd	a5,8(sp)
   1620a:	f0dff0ef          	jal	ra,16116 <swapc.part.0>
   1620e:	67a2                	ld	a5,8(sp)
   16210:	0005069b          	sext.w	a3,a0
   16214:	2785                	addiw	a5,a5,1
   16216:	1782                	slli	a5,a5,0x20
   16218:	83f9                	srli	a5,a5,0x1e
   1621a:	97a6                	add	a5,a5,s1
   1621c:	4388                	lw	a0,0(a5)
   1621e:	00098763          	beqz	s3,1622c <__mo_lookup+0xee>
   16222:	e436                	sd	a3,8(sp)
   16224:	ef3ff0ef          	jal	ra,16116 <swapc.part.0>
   16228:	66a2                	ld	a3,8(sp)
   1622a:	2501                	sext.w	a0,a0
   1622c:	02051593          	slli	a1,a0,0x20
   16230:	9181                	srli	a1,a1,0x20
   16232:	0925f863          	bgeu	a1,s2,162c2 <__mo_lookup+0x184>
   16236:	02069793          	slli	a5,a3,0x20
   1623a:	9381                	srli	a5,a5,0x20
   1623c:	40b90633          	sub	a2,s2,a1
   16240:	08c7f163          	bgeu	a5,a2,162c2 <__mo_lookup+0x184>
   16244:	00d507bb          	addw	a5,a0,a3
   16248:	1782                	slli	a5,a5,0x20
   1624a:	9381                	srli	a5,a5,0x20
   1624c:	97a6                	add	a5,a5,s1
   1624e:	0007c783          	lbu	a5,0(a5) # 6afbf000 <__BSS_END__+0x6afa25e0>
   16252:	eba5                	bnez	a5,162c2 <__mo_lookup+0x184>
   16254:	6502                	ld	a0,0(sp)
   16256:	95a6                	add	a1,a1,s1
   16258:	f78fd0ef          	jal	ra,139d0 <strcmp>
   1625c:	e159                	bnez	a0,162e2 <__mo_lookup+0x1a4>
   1625e:	0024541b          	srliw	s0,s0,0x2
   16262:	01b40dbb          	addw	s11,s0,s11
   16266:	020d9793          	slli	a5,s11,0x20
   1626a:	83f9                	srli	a5,a5,0x1e
   1626c:	97a6                	add	a5,a5,s1
   1626e:	4380                	lw	s0,0(a5)
   16270:	00098763          	beqz	s3,1627e <__mo_lookup+0x140>
   16274:	8522                	mv	a0,s0
   16276:	ea1ff0ef          	jal	ra,16116 <swapc.part.0>
   1627a:	0005041b          	sext.w	s0,a0
   1627e:	2d85                	addiw	s11,s11,1
   16280:	1d82                	slli	s11,s11,0x20
   16282:	01eddd93          	srli	s11,s11,0x1e
   16286:	9da6                	add	s11,s11,s1
   16288:	000da503          	lw	a0,0(s11)
   1628c:	00098563          	beqz	s3,16296 <__mo_lookup+0x158>
   16290:	e87ff0ef          	jal	ra,16116 <swapc.part.0>
   16294:	2501                	sext.w	a0,a0
   16296:	02051713          	slli	a4,a0,0x20
   1629a:	9301                	srli	a4,a4,0x20
   1629c:	03277363          	bgeu	a4,s2,162c2 <__mo_lookup+0x184>
   162a0:	02041793          	slli	a5,s0,0x20
   162a4:	9381                	srli	a5,a5,0x20
   162a6:	40e90933          	sub	s2,s2,a4
   162aa:	0127fc63          	bgeu	a5,s2,162c2 <__mo_lookup+0x184>
   162ae:	008507bb          	addw	a5,a0,s0
   162b2:	1782                	slli	a5,a5,0x20
   162b4:	9381                	srli	a5,a5,0x20
   162b6:	97a6                	add	a5,a5,s1
   162b8:	0007c783          	lbu	a5,0(a5)
   162bc:	00e48533          	add	a0,s1,a4
   162c0:	c391                	beqz	a5,162c4 <__mo_lookup+0x186>
   162c2:	4501                	li	a0,0
   162c4:	70e6                	ld	ra,120(sp)
   162c6:	7446                	ld	s0,112(sp)
   162c8:	74a6                	ld	s1,104(sp)
   162ca:	7906                	ld	s2,96(sp)
   162cc:	69e6                	ld	s3,88(sp)
   162ce:	6a46                	ld	s4,80(sp)
   162d0:	6aa6                	ld	s5,72(sp)
   162d2:	6b06                	ld	s6,64(sp)
   162d4:	7be2                	ld	s7,56(sp)
   162d6:	7c42                	ld	s8,48(sp)
   162d8:	7ca2                	ld	s9,40(sp)
   162da:	7d02                	ld	s10,32(sp)
   162dc:	6de2                	ld	s11,24(sp)
   162de:	6109                	addi	sp,sp,128
   162e0:	8082                	ret
   162e2:	ffab00e3          	beq	s6,s10,162c2 <__mo_lookup+0x184>
   162e6:	00054563          	bltz	a0,162f0 <__mo_lookup+0x1b2>
   162ea:	418b0bbb          	subw	s7,s6,s8
   162ee:	8ae6                	mv	s5,s9
   162f0:	8b5e                	mv	s6,s7
   162f2:	bdcd                	j	161e4 <__mo_lookup+0xa6>

00000000000162f4 <__fpclassifyl>:
   162f4:	6721                	lui	a4,0x8
   162f6:	0305d693          	srli	a3,a1,0x30
   162fa:	177d                	addi	a4,a4,-1
   162fc:	05c2                	slli	a1,a1,0x10
   162fe:	8ef9                	and	a3,a3,a4
   16300:	87aa                	mv	a5,a0
   16302:	81c1                	srli	a1,a1,0x10
   16304:	e691                	bnez	a3,16310 <__fpclassifyl+0x1c>
   16306:	8dc9                	or	a1,a1,a0
   16308:	00b03533          	snez	a0,a1
   1630c:	0509                	addi	a0,a0,2
   1630e:	8082                	ret
   16310:	4511                	li	a0,4
   16312:	00e69563          	bne	a3,a4,1631c <__fpclassifyl+0x28>
   16316:	8ddd                	or	a1,a1,a5
   16318:	0015b513          	seqz	a0,a1
   1631c:	8082                	ret

000000000001631e <__signbitl>:
   1631e:	03f5d513          	srli	a0,a1,0x3f
   16322:	8082                	ret

0000000000016324 <frexpl>:
   16324:	6721                	lui	a4,0x8
   16326:	1101                	addi	sp,sp,-32
   16328:	0305d793          	srli	a5,a1,0x30
   1632c:	177d                	addi	a4,a4,-1
   1632e:	e822                	sd	s0,16(sp)
   16330:	e426                	sd	s1,8(sp)
   16332:	e04a                	sd	s2,0(sp)
   16334:	ec06                	sd	ra,24(sp)
   16336:	8932                	mv	s2,a2
   16338:	00e7f633          	and	a2,a5,a4
   1633c:	84aa                	mv	s1,a0
   1633e:	842e                	mv	s0,a1
   16340:	e239                	bnez	a2,16386 <frexpl+0x62>
   16342:	4681                	li	a3,0
   16344:	645000ef          	jal	ra,17188 <__eqtf2>
   16348:	cd05                	beqz	a0,16380 <frexpl+0x5c>
   1634a:	67e9                	lui	a5,0x1a
   1634c:	0b87b683          	ld	a3,184(a5) # 1a0b8 <empty_mo+0x28>
   16350:	4601                	li	a2,0
   16352:	8526                	mv	a0,s1
   16354:	85a2                	mv	a1,s0
   16356:	6c5000ef          	jal	ra,1721a <__multf3>
   1635a:	864a                	mv	a2,s2
   1635c:	fc9ff0ef          	jal	ra,16324 <frexpl>
   16360:	00092783          	lw	a5,0(s2)
   16364:	84aa                	mv	s1,a0
   16366:	842e                	mv	s0,a1
   16368:	f887879b          	addiw	a5,a5,-120
   1636c:	00f92023          	sw	a5,0(s2)
   16370:	60e2                	ld	ra,24(sp)
   16372:	85a2                	mv	a1,s0
   16374:	6442                	ld	s0,16(sp)
   16376:	6902                	ld	s2,0(sp)
   16378:	8526                	mv	a0,s1
   1637a:	64a2                	ld	s1,8(sp)
   1637c:	6105                	addi	sp,sp,32
   1637e:	8082                	ret
   16380:	00092023          	sw	zero,0(s2)
   16384:	b7f5                	j	16370 <frexpl+0x4c>
   16386:	0006069b          	sext.w	a3,a2
   1638a:	fee603e3          	beq	a2,a4,16370 <frexpl+0x4c>
   1638e:	7771                	lui	a4,0xffffc
   16390:	2709                	addiw	a4,a4,2
   16392:	9f35                	addw	a4,a4,a3
   16394:	00e92023          	sw	a4,0(s2)
   16398:	7761                	lui	a4,0xffff8
   1639a:	8ff9                	and	a5,a5,a4
   1639c:	6711                	lui	a4,0x4
   1639e:	1779                	addi	a4,a4,-2
   163a0:	8fd9                	or	a5,a5,a4
   163a2:	01059413          	slli	s0,a1,0x10
   163a6:	17c2                	slli	a5,a5,0x30
   163a8:	8041                	srli	s0,s0,0x10
   163aa:	8c5d                	or	s0,s0,a5
   163ac:	b7d1                	j	16370 <frexpl+0x4c>

00000000000163ae <wctomb>:
   163ae:	c911                	beqz	a0,163c2 <wctomb+0x14>
   163b0:	1141                	addi	sp,sp,-16
   163b2:	4601                	li	a2,0
   163b4:	e406                	sd	ra,8(sp)
   163b6:	2a2000ef          	jal	ra,16658 <wcrtomb>
   163ba:	60a2                	ld	ra,8(sp)
   163bc:	2501                	sext.w	a0,a0
   163be:	0141                	addi	sp,sp,16
   163c0:	8082                	ret
   163c2:	4501                	li	a0,0
   163c4:	8082                	ret

00000000000163c6 <__restore>:
   163c6:	08b00893          	li	a7,139
   163ca:	00000073          	ecall

00000000000163ce <close_file>:
   163ce:	c539                	beqz	a0,1641c <close_file+0x4e>
   163d0:	08c52783          	lw	a5,140(a0)
   163d4:	1141                	addi	sp,sp,-16
   163d6:	e022                	sd	s0,0(sp)
   163d8:	e406                	sd	ra,8(sp)
   163da:	02079713          	slli	a4,a5,0x20
   163de:	842a                	mv	s0,a0
   163e0:	00074463          	bltz	a4,163e8 <close_file+0x1a>
   163e4:	b6bfc0ef          	jal	ra,12f4e <__lockfile>
   163e8:	7418                	ld	a4,40(s0)
   163ea:	7c1c                	ld	a5,56(s0)
   163ec:	00f70763          	beq	a4,a5,163fa <close_file+0x2c>
   163f0:	643c                	ld	a5,72(s0)
   163f2:	4601                	li	a2,0
   163f4:	4581                	li	a1,0
   163f6:	8522                	mv	a0,s0
   163f8:	9782                	jalr	a5
   163fa:	640c                	ld	a1,8(s0)
   163fc:	681c                	ld	a5,16(s0)
   163fe:	00f58b63          	beq	a1,a5,16414 <close_file+0x46>
   16402:	05043303          	ld	t1,80(s0)
   16406:	8522                	mv	a0,s0
   16408:	6402                	ld	s0,0(sp)
   1640a:	60a2                	ld	ra,8(sp)
   1640c:	4605                	li	a2,1
   1640e:	8d9d                	sub	a1,a1,a5
   16410:	0141                	addi	sp,sp,16
   16412:	8302                	jr	t1
   16414:	60a2                	ld	ra,8(sp)
   16416:	6402                	ld	s0,0(sp)
   16418:	0141                	addi	sp,sp,16
   1641a:	8082                	ret
   1641c:	8082                	ret

000000000001641e <__stdio_exit>:
   1641e:	1141                	addi	sp,sp,-16
   16420:	e022                	sd	s0,0(sp)
   16422:	e406                	sd	ra,8(sp)
   16424:	030000ef          	jal	ra,16454 <__ofl_lock>
   16428:	6100                	ld	s0,0(a0)
   1642a:	e005                	bnez	s0,1644a <__stdio_exit+0x2c>
   1642c:	9f81b503          	ld	a0,-1544(gp) # 1c1f8 <__stdin_used>
   16430:	f9fff0ef          	jal	ra,163ce <close_file>
   16434:	9f81b503          	ld	a0,-1544(gp) # 1c1f8 <__stdin_used>
   16438:	f97ff0ef          	jal	ra,163ce <close_file>
   1643c:	6402                	ld	s0,0(sp)
   1643e:	60a2                	ld	ra,8(sp)
   16440:	9681b503          	ld	a0,-1688(gp) # 1c168 <__stderr_used>
   16444:	0141                	addi	sp,sp,16
   16446:	f89ff06f          	j	163ce <close_file>
   1644a:	8522                	mv	a0,s0
   1644c:	f83ff0ef          	jal	ra,163ce <close_file>
   16450:	7820                	ld	s0,112(s0)
   16452:	bfe1                	j	1642a <__stdio_exit+0xc>

0000000000016454 <__ofl_lock>:
   16454:	1141                	addi	sp,sp,-16
   16456:	a0818513          	addi	a0,gp,-1528 # 1c208 <ofl_lock>
   1645a:	e406                	sd	ra,8(sp)
   1645c:	e4afd0ef          	jal	ra,13aa6 <__lock>
   16460:	60a2                	ld	ra,8(sp)
   16462:	a0018513          	addi	a0,gp,-1536 # 1c200 <ofl_head>
   16466:	0141                	addi	sp,sp,16
   16468:	8082                	ret

000000000001646a <__ofl_unlock>:
   1646a:	a0818513          	addi	a0,gp,-1528 # 1c208 <ofl_lock>
   1646e:	f10fd06f          	j	13b7e <__unlock>

0000000000016472 <memchr>:
   16472:	00757713          	andi	a4,a0,7
   16476:	87aa                	mv	a5,a0
   16478:	0ff5f593          	andi	a1,a1,255
   1647c:	cb19                	beqz	a4,16492 <memchr+0x20>
   1647e:	c635                	beqz	a2,164ea <memchr+0x78>
   16480:	0007c703          	lbu	a4,0(a5)
   16484:	04b70863          	beq	a4,a1,164d4 <memchr+0x62>
   16488:	0785                	addi	a5,a5,1
   1648a:	0077f713          	andi	a4,a5,7
   1648e:	167d                	addi	a2,a2,-1
   16490:	f77d                	bnez	a4,1647e <memchr+0xc>
   16492:	4501                	li	a0,0
   16494:	ce21                	beqz	a2,164ec <memchr+0x7a>
   16496:	0007c703          	lbu	a4,0(a5)
   1649a:	02b70d63          	beq	a4,a1,164d4 <memchr+0x62>
   1649e:	9281b503          	ld	a0,-1752(gp) # 1c128 <__SDATA_BEGIN__+0x20>
   164a2:	471d                	li	a4,7
   164a4:	02a58533          	mul	a0,a1,a0
   164a8:	02c77663          	bgeu	a4,a2,164d4 <memchr+0x62>
   164ac:	9301b883          	ld	a7,-1744(gp) # 1c130 <__SDATA_BEGIN__+0x28>
   164b0:	9381b803          	ld	a6,-1736(gp) # 1c138 <__SDATA_BEGIN__+0x30>
   164b4:	431d                	li	t1,7
   164b6:	a029                	j	164c0 <memchr+0x4e>
   164b8:	1661                	addi	a2,a2,-8
   164ba:	07a1                	addi	a5,a5,8
   164bc:	02c37963          	bgeu	t1,a2,164ee <memchr+0x7c>
   164c0:	6398                	ld	a4,0(a5)
   164c2:	8f29                	xor	a4,a4,a0
   164c4:	011706b3          	add	a3,a4,a7
   164c8:	fff74713          	not	a4,a4
   164cc:	8f75                	and	a4,a4,a3
   164ce:	01077733          	and	a4,a4,a6
   164d2:	d37d                	beqz	a4,164b8 <memchr+0x46>
   164d4:	853e                	mv	a0,a5
   164d6:	97b2                	add	a5,a5,a2
   164d8:	a021                	j	164e0 <memchr+0x6e>
   164da:	0505                	addi	a0,a0,1
   164dc:	00f50763          	beq	a0,a5,164ea <memchr+0x78>
   164e0:	00054703          	lbu	a4,0(a0)
   164e4:	feb71be3          	bne	a4,a1,164da <memchr+0x68>
   164e8:	8082                	ret
   164ea:	4501                	li	a0,0
   164ec:	8082                	ret
   164ee:	4501                	li	a0,0
   164f0:	f275                	bnez	a2,164d4 <memchr+0x62>
   164f2:	8082                	ret

00000000000164f4 <strncmp>:
   164f4:	ce05                	beqz	a2,1652c <strncmp+0x38>
   164f6:	00054703          	lbu	a4,0(a0)
   164fa:	0005c783          	lbu	a5,0(a1) # ffffffff80000000 <__BSS_END__+0xffffffff7ffe35e0>
   164fe:	cb0d                	beqz	a4,16530 <strncmp+0x3c>
   16500:	167d                	addi	a2,a2,-1
   16502:	00c506b3          	add	a3,a0,a2
   16506:	a819                	j	1651c <strncmp+0x28>
   16508:	00a68e63          	beq	a3,a0,16524 <strncmp+0x30>
   1650c:	0505                	addi	a0,a0,1
   1650e:	00e79b63          	bne	a5,a4,16524 <strncmp+0x30>
   16512:	00054703          	lbu	a4,0(a0)
   16516:	0005c783          	lbu	a5,0(a1)
   1651a:	cb19                	beqz	a4,16530 <strncmp+0x3c>
   1651c:	0005c783          	lbu	a5,0(a1)
   16520:	0585                	addi	a1,a1,1
   16522:	f3fd                	bnez	a5,16508 <strncmp+0x14>
   16524:	0007051b          	sext.w	a0,a4
   16528:	9d1d                	subw	a0,a0,a5
   1652a:	8082                	ret
   1652c:	4501                	li	a0,0
   1652e:	8082                	ret
   16530:	4501                	li	a0,0
   16532:	9d1d                	subw	a0,a0,a5
   16534:	8082                	ret

0000000000016536 <a_fetch_add>:
   16536:	87aa                	mv	a5,a0
   16538:	4398                	lw	a4,0(a5)
   1653a:	0007051b          	sext.w	a0,a4
   1653e:	9f2d                	addw	a4,a4,a1
   16540:	1607a6af          	lr.w.aqrl	a3,(a5)
   16544:	00a69563          	bne	a3,a0,1654e <a_fetch_add+0x18>
   16548:	1ee7a62f          	sc.w.aqrl	a2,a4,(a5)
   1654c:	fa75                	bnez	a2,16540 <a_fetch_add+0xa>
   1654e:	0006871b          	sext.w	a4,a3
   16552:	fee513e3          	bne	a0,a4,16538 <a_fetch_add+0x2>
   16556:	8082                	ret

0000000000016558 <cleanup>:
   16558:	55fd                	li	a1,-1
   1655a:	fddff06f          	j	16536 <a_fetch_add>

000000000001655e <sem_timedwait>:
   1655e:	711d                	addi	sp,sp,-96
   16560:	ec86                	sd	ra,88(sp)
   16562:	e8a2                	sd	s0,80(sp)
   16564:	e0ca                	sd	s2,64(sp)
   16566:	842a                	mv	s0,a0
   16568:	892e                	mv	s2,a1
   1656a:	e4a6                	sd	s1,72(sp)
   1656c:	fc4e                	sd	s3,56(sp)
   1656e:	f852                	sd	s4,48(sp)
   16570:	f456                	sd	s5,40(sp)
   16572:	f05a                	sd	s6,32(sp)
   16574:	212000ef          	jal	ra,16786 <__pthread_testcancel>
   16578:	8522                	mv	a0,s0
   1657a:	090000ef          	jal	ra,1660a <sem_trywait>
   1657e:	06500713          	li	a4,101
   16582:	ed11                	bnez	a0,1659e <sem_timedwait+0x40>
   16584:	4501                	li	a0,0
   16586:	60e6                	ld	ra,88(sp)
   16588:	6446                	ld	s0,80(sp)
   1658a:	64a6                	ld	s1,72(sp)
   1658c:	6906                	ld	s2,64(sp)
   1658e:	79e2                	ld	s3,56(sp)
   16590:	7a42                	ld	s4,48(sp)
   16592:	7aa2                	ld	s5,40(sp)
   16594:	7b02                	ld	s6,32(sp)
   16596:	6125                	addi	sp,sp,96
   16598:	8082                	ret
   1659a:	0330000f          	fence	rw,rw
   1659e:	377d                	addiw	a4,a4,-1
   165a0:	cb01                	beqz	a4,165b0 <sem_timedwait+0x52>
   165a2:	401c                	lw	a5,0(s0)
   165a4:	2781                	sext.w	a5,a5
   165a6:	00f04563          	bgtz	a5,165b0 <sem_timedwait+0x52>
   165aa:	405c                	lw	a5,4(s0)
   165ac:	2781                	sext.w	a5,a5
   165ae:	d7f5                	beqz	a5,1659a <sem_timedwait+0x3c>
   165b0:	00440993          	addi	s3,s0,4
   165b4:	4a01                	li	s4,0
   165b6:	5afd                	li	s5,-1
   165b8:	6b59                	lui	s6,0x16
   165ba:	8522                	mv	a0,s0
   165bc:	04e000ef          	jal	ra,1660a <sem_trywait>
   165c0:	d171                	beqz	a0,16584 <sem_timedwait+0x26>
   165c2:	4585                	li	a1,1
   165c4:	854e                	mv	a0,s3
   165c6:	f71ff0ef          	jal	ra,16536 <a_fetch_add>
   165ca:	160427af          	lr.w.aqrl	a5,(s0)
   165ce:	01479563          	bne	a5,s4,165d8 <sem_timedwait+0x7a>
   165d2:	1f54272f          	sc.w.aqrl	a4,s5,(s0)
   165d6:	fb75                	bnez	a4,165ca <sem_timedwait+0x6c>
   165d8:	864e                	mv	a2,s3
   165da:	558b0593          	addi	a1,s6,1368 # 16558 <cleanup>
   165de:	0028                	addi	a0,sp,8
   165e0:	170000ef          	jal	ra,16750 <_pthread_cleanup_push>
   165e4:	4418                	lw	a4,8(s0)
   165e6:	55fd                	li	a1,-1
   165e8:	86ca                	mv	a3,s2
   165ea:	2701                	sext.w	a4,a4
   165ec:	4601                	li	a2,0
   165ee:	8522                	mv	a0,s0
   165f0:	f04ff0ef          	jal	ra,15cf4 <__timedwait_cp>
   165f4:	84aa                	mv	s1,a0
   165f6:	4585                	li	a1,1
   165f8:	0028                	addi	a0,sp,8
   165fa:	15e000ef          	jal	ra,16758 <_pthread_cleanup_pop>
   165fe:	dcd5                	beqz	s1,165ba <sem_timedwait+0x5c>
   16600:	96dfa0ef          	jal	ra,10f6c <__errno_location>
   16604:	c104                	sw	s1,0(a0)
   16606:	557d                	li	a0,-1
   16608:	bfbd                	j	16586 <sem_timedwait+0x28>

000000000001660a <sem_trywait>:
   1660a:	4605                	li	a2,1
   1660c:	411c                	lw	a5,0(a0)
   1660e:	2781                	sext.w	a5,a5
   16610:	00f04c63          	bgtz	a5,16628 <sem_trywait+0x1e>
   16614:	1141                	addi	sp,sp,-16
   16616:	e406                	sd	ra,8(sp)
   16618:	955fa0ef          	jal	ra,10f6c <__errno_location>
   1661c:	60a2                	ld	ra,8(sp)
   1661e:	47ad                	li	a5,11
   16620:	c11c                	sw	a5,0(a0)
   16622:	557d                	li	a0,-1
   16624:	0141                	addi	sp,sp,16
   16626:	8082                	ret
   16628:	fff7869b          	addiw	a3,a5,-1
   1662c:	4701                	li	a4,0
   1662e:	00c79663          	bne	a5,a2,1663a <sem_trywait+0x30>
   16632:	4158                	lw	a4,4(a0)
   16634:	2701                	sext.w	a4,a4
   16636:	00e03733          	snez	a4,a4
   1663a:	40e6873b          	subw	a4,a3,a4
   1663e:	160526af          	lr.w.aqrl	a3,(a0)
   16642:	00f69563          	bne	a3,a5,1664c <sem_trywait+0x42>
   16646:	1ee525af          	sc.w.aqrl	a1,a4,(a0)
   1664a:	f9f5                	bnez	a1,1663e <sem_trywait+0x34>
   1664c:	0006871b          	sext.w	a4,a3
   16650:	fae79ee3          	bne	a5,a4,1660c <sem_trywait+0x2>
   16654:	4501                	li	a0,0
   16656:	8082                	ret

0000000000016658 <wcrtomb>:
   16658:	c909                	beqz	a0,1666a <wcrtomb+0x12>
   1665a:	07f00693          	li	a3,127
   1665e:	0005879b          	sext.w	a5,a1
   16662:	00b6ec63          	bltu	a3,a1,1667a <wcrtomb+0x22>
   16666:	00b50023          	sb	a1,0(a0)
   1666a:	4505                	li	a0,1
   1666c:	8082                	ret
   1666e:	00b50023          	sb	a1,0(a0)
   16672:	4505                	li	a0,1
   16674:	60a2                	ld	ra,8(sp)
   16676:	0141                	addi	sp,sp,16
   16678:	8082                	ret
   1667a:	1141                	addi	sp,sp,-16
   1667c:	e406                	sd	ra,8(sp)
   1667e:	8712                	mv	a4,tp
   16680:	fd073703          	ld	a4,-48(a4) # 3fd0 <exit-0xc150>
   16684:	6318                	ld	a4,0(a4)
   16686:	ef11                	bnez	a4,166a2 <wcrtomb+0x4a>
   16688:	7749                	lui	a4,0xffff2
   1668a:	0807071b          	addiw	a4,a4,128
   1668e:	9fb9                	addw	a5,a5,a4
   16690:	fcf6ffe3          	bgeu	a3,a5,1666e <wcrtomb+0x16>
   16694:	8d9fa0ef          	jal	ra,10f6c <__errno_location>
   16698:	05400793          	li	a5,84
   1669c:	c11c                	sw	a5,0(a0)
   1669e:	557d                	li	a0,-1
   166a0:	bfd1                	j	16674 <wcrtomb+0x1c>
   166a2:	7ff00713          	li	a4,2047
   166a6:	02f76063          	bltu	a4,a5,166c6 <wcrtomb+0x6e>
   166aa:	4065d79b          	sraiw	a5,a1,0x6
   166ae:	03f5f593          	andi	a1,a1,63
   166b2:	fc07e793          	ori	a5,a5,-64
   166b6:	f805e593          	ori	a1,a1,-128
   166ba:	00f50023          	sb	a5,0(a0)
   166be:	00b500a3          	sb	a1,1(a0)
   166c2:	4509                	li	a0,2
   166c4:	bf45                	j	16674 <wcrtomb+0x1c>
   166c6:	6735                	lui	a4,0xd
   166c8:	7ff70713          	addi	a4,a4,2047 # d7ff <exit-0x2921>
   166cc:	00f77763          	bgeu	a4,a5,166da <wcrtomb+0x82>
   166d0:	7749                	lui	a4,0xffff2
   166d2:	9f3d                	addw	a4,a4,a5
   166d4:	6689                	lui	a3,0x2
   166d6:	02d77863          	bgeu	a4,a3,16706 <wcrtomb+0xae>
   166da:	40c5d79b          	sraiw	a5,a1,0xc
   166de:	fe07e793          	ori	a5,a5,-32
   166e2:	00f50023          	sb	a5,0(a0)
   166e6:	4065d79b          	sraiw	a5,a1,0x6
   166ea:	03f7f793          	andi	a5,a5,63
   166ee:	03f5f593          	andi	a1,a1,63
   166f2:	f807e793          	ori	a5,a5,-128
   166f6:	f805e593          	ori	a1,a1,-128
   166fa:	00f500a3          	sb	a5,1(a0)
   166fe:	00b50123          	sb	a1,2(a0)
   16702:	450d                	li	a0,3
   16704:	bf85                	j	16674 <wcrtomb+0x1c>
   16706:	7741                	lui	a4,0xffff0
   16708:	9fb9                	addw	a5,a5,a4
   1670a:	00100737          	lui	a4,0x100
   1670e:	f8e7f3e3          	bgeu	a5,a4,16694 <wcrtomb+0x3c>
   16712:	4125d79b          	sraiw	a5,a1,0x12
   16716:	ff07e793          	ori	a5,a5,-16
   1671a:	00f50023          	sb	a5,0(a0)
   1671e:	40c5d79b          	sraiw	a5,a1,0xc
   16722:	03f7f793          	andi	a5,a5,63
   16726:	f807e793          	ori	a5,a5,-128
   1672a:	00f500a3          	sb	a5,1(a0)
   1672e:	4065d79b          	sraiw	a5,a1,0x6
   16732:	03f7f793          	andi	a5,a5,63
   16736:	03f5f593          	andi	a1,a1,63
   1673a:	f807e793          	ori	a5,a5,-128
   1673e:	f805e593          	ori	a1,a1,-128
   16742:	00f50123          	sb	a5,2(a0)
   16746:	00b501a3          	sb	a1,3(a0)
   1674a:	4511                	li	a0,4
   1674c:	b725                	j	16674 <wcrtomb+0x1c>

000000000001674e <__do_cleanup_pop>:
   1674e:	8082                	ret

0000000000016750 <_pthread_cleanup_push>:
   16750:	e10c                	sd	a1,0(a0)
   16752:	e510                	sd	a2,8(a0)
   16754:	ffbff06f          	j	1674e <__do_cleanup_pop>

0000000000016758 <_pthread_cleanup_pop>:
   16758:	1101                	addi	sp,sp,-32
   1675a:	e426                	sd	s1,8(sp)
   1675c:	84ae                	mv	s1,a1
   1675e:	e822                	sd	s0,16(sp)
   16760:	ec06                	sd	ra,24(sp)
   16762:	842a                	mv	s0,a0
   16764:	febff0ef          	jal	ra,1674e <__do_cleanup_pop>
   16768:	c889                	beqz	s1,1677a <_pthread_cleanup_pop+0x22>
   1676a:	00043303          	ld	t1,0(s0)
   1676e:	6408                	ld	a0,8(s0)
   16770:	6442                	ld	s0,16(sp)
   16772:	60e2                	ld	ra,24(sp)
   16774:	64a2                	ld	s1,8(sp)
   16776:	6105                	addi	sp,sp,32
   16778:	8302                	jr	t1
   1677a:	60e2                	ld	ra,24(sp)
   1677c:	6442                	ld	s0,16(sp)
   1677e:	64a2                	ld	s1,8(sp)
   16780:	6105                	addi	sp,sp,32
   16782:	8082                	ret

0000000000016784 <__testcancel>:
   16784:	8082                	ret

0000000000016786 <__pthread_testcancel>:
   16786:	fffff06f          	j	16784 <__testcancel>

000000000001678a <__addtf3>:
#include "soft-fp.h"
#include "quad.h"

TFtype
__addtf3 (TFtype a, TFtype b)
{
   1678a:	7179                	addi	sp,sp,-48
   1678c:	f406                	sd	ra,40(sp)
   1678e:	f022                	sd	s0,32(sp)
   16790:	ec26                	sd	s1,24(sp)
   16792:	e84a                	sd	s2,16(sp)
   16794:	e44e                	sd	s3,8(sp)
   16796:	e052                	sd	s4,0(sp)
   16798:	82aa                	mv	t0,a0
  FP_DECL_Q (A);
  FP_DECL_Q (B);
  FP_DECL_Q (R);
  TFtype r;

  FP_INIT_ROUNDMODE;
   1679a:	002024f3          	frrm	s1
  FP_UNPACK_SEMIRAW_Q (A, a);
   1679e:	59fd                	li	s3,-1
   167a0:	0109d793          	srli	a5,s3,0x10
   167a4:	6f21                	lui	t5,0x8
   167a6:	00f5fe33          	and	t3,a1,a5
   167aa:	ffff0393          	addi	t2,t5,-1 # 7fff <exit-0x8121>
   167ae:	0305d513          	srli	a0,a1,0x30
  FP_UNPACK_SEMIRAW_Q (B, b);
   167b2:	8ff5                	and	a5,a5,a3
   167b4:	0306d313          	srli	t1,a3,0x30
   167b8:	078e                	slli	a5,a5,0x3
   167ba:	03d65f93          	srli	t6,a2,0x3d
  FP_UNPACK_SEMIRAW_Q (A, a);
   167be:	00757433          	and	s0,a0,t2
  FP_UNPACK_SEMIRAW_Q (B, b);
   167c2:	00737333          	and	t1,t1,t2
  FP_UNPACK_SEMIRAW_Q (A, a);
   167c6:	0e0e                	slli	t3,t3,0x3
   167c8:	03d2d713          	srli	a4,t0,0x3d
   167cc:	03f5da13          	srli	s4,a1,0x3f
  FP_UNPACK_SEMIRAW_Q (B, b);
   167d0:	00ffefb3          	or	t6,t6,a5
   167d4:	03f6d813          	srli	a6,a3,0x3f
  FP_ADD_Q (R, A, B);
   167d8:	406407bb          	subw	a5,s0,t1
  FP_INIT_ROUNDMODE;
   167dc:	2481                	sext.w	s1,s1
  FP_UNPACK_SEMIRAW_Q (A, a);
   167de:	01c76e33          	or	t3,a4,t3
   167e2:	00329593          	slli	a1,t0,0x3
  FP_UNPACK_SEMIRAW_Q (B, b);
   167e6:	00361e93          	slli	t4,a2,0x3
  FP_ADD_Q (R, A, B);
   167ea:	0007889b          	sext.w	a7,a5
   167ee:	1b0a0363          	beq	s4,a6,16994 <__addtf3+0x20a>
   167f2:	19105163          	blez	a7,16974 <__addtf3+0x1ea>
   167f6:	1e030663          	beqz	t1,169e2 <__addtf3+0x258>
   167fa:	6c740a63          	beq	s0,t2,16ece <__addtf3+0x744>
   167fe:	4785                	li	a5,1
   16800:	17ce                	slli	a5,a5,0x33
   16802:	00ffefb3          	or	t6,t6,a5
   16806:	07400793          	li	a5,116
   1680a:	3f17c963          	blt	a5,a7,16bfc <__addtf3+0x472>
   1680e:	03f00793          	li	a5,63
   16812:	5717c463          	blt	a5,a7,16d7a <__addtf3+0x5f0>
   16816:	04000793          	li	a5,64
   1681a:	411787bb          	subw	a5,a5,a7
   1681e:	011ed733          	srl	a4,t4,a7
   16822:	00ff9933          	sll	s2,t6,a5
   16826:	00fe9eb3          	sll	t4,t4,a5
   1682a:	00e96933          	or	s2,s2,a4
   1682e:	01d03eb3          	snez	t4,t4
   16832:	011fd8b3          	srl	a7,t6,a7
   16836:	01d96933          	or	s2,s2,t4
   1683a:	411e0e33          	sub	t3,t3,a7
   1683e:	41258933          	sub	s2,a1,s2
   16842:	0125b5b3          	sltu	a1,a1,s2
   16846:	40be0e33          	sub	t3,t3,a1
   1684a:	00ce1793          	slli	a5,t3,0xc
   1684e:	2a07d363          	bgez	a5,16af4 <__addtf3+0x36a>
   16852:	0e36                	slli	t3,t3,0xd
   16854:	00de5993          	srli	s3,t3,0xd
   16858:	36098963          	beqz	s3,16bca <__addtf3+0x440>
   1685c:	854e                	mv	a0,s3
   1685e:	5a9010ef          	jal	ra,18606 <__clzdi2>
   16862:	3551                	addiw	a0,a0,-12
   16864:	04000e13          	li	t3,64
   16868:	40ae0e3b          	subw	t3,t3,a0
   1686c:	00a999b3          	sll	s3,s3,a0
   16870:	01c95e33          	srl	t3,s2,t3
   16874:	013e6e33          	or	t3,t3,s3
   16878:	00a919b3          	sll	s3,s2,a0
   1687c:	32854f63          	blt	a0,s0,16bba <__addtf3+0x430>
   16880:	9d01                	subw	a0,a0,s0
   16882:	0015071b          	addiw	a4,a0,1
   16886:	03f00793          	li	a5,63
   1688a:	863a                	mv	a2,a4
   1688c:	4ce7c063          	blt	a5,a4,16d4c <__addtf3+0x5c2>
   16890:	04000793          	li	a5,64
   16894:	9f99                	subw	a5,a5,a4
   16896:	00fe1933          	sll	s2,t3,a5
   1689a:	00e9d633          	srl	a2,s3,a4
   1689e:	00f997b3          	sll	a5,s3,a5
   168a2:	00c96933          	or	s2,s2,a2
   168a6:	00f037b3          	snez	a5,a5
   168aa:	00f96933          	or	s2,s2,a5
   168ae:	00ee5e33          	srl	t3,t3,a4
  FP_PACK_SEMIRAW_Q (r, R);
   168b2:	01c967b3          	or	a5,s2,t3
   168b6:	1a078e63          	beqz	a5,16a72 <__addtf3+0x2e8>
   168ba:	00797793          	andi	a5,s2,7
   168be:	00191593          	slli	a1,s2,0x1
   168c2:	001e1613          	slli	a2,t3,0x1
   168c6:	03f95713          	srli	a4,s2,0x3f
   168ca:	0075f513          	andi	a0,a1,7
   168ce:	9732                	add	a4,a4,a2
   168d0:	4881                	li	a7,0
   168d2:	c911                	beqz	a0,168e6 <__addtf3+0x15c>
   168d4:	4609                	li	a2,2
   168d6:	66c48c63          	beq	s1,a2,16f4e <__addtf3+0x7c4>
   168da:	460d                	li	a2,3
   168dc:	4885                	li	a7,1
   168de:	64c48e63          	beq	s1,a2,16f3a <__addtf3+0x7b0>
   168e2:	62048f63          	beqz	s1,16f20 <__addtf3+0x796>
   168e6:	4605                	li	a2,1
   168e8:	03461593          	slli	a1,a2,0x34
   168ec:	8f6d                	and	a4,a4,a1
   168ee:	00173593          	seqz	a1,a4
   168f2:	4401                	li	s0,0
   168f4:	6a078563          	beqz	a5,16f9e <__addtf3+0x814>
   168f8:	4789                	li	a5,2
   168fa:	2af48363          	beq	s1,a5,16ba0 <__addtf3+0x416>
   168fe:	478d                	li	a5,3
   16900:	4885                	li	a7,1
   16902:	2af48963          	beq	s1,a5,16bb4 <__addtf3+0x42a>
   16906:	28048163          	beqz	s1,16b88 <__addtf3+0x3fe>
   1690a:	4605                	li	a2,1
   1690c:	164e                	slli	a2,a2,0x33
   1690e:	00ce7633          	and	a2,t3,a2
   16912:	c199                	beqz	a1,16918 <__addtf3+0x18e>
   16914:	0028e893          	ori	a7,a7,2
   16918:	1a060163          	beqz	a2,16aba <__addtf3+0x330>
   1691c:	6721                	lui	a4,0x8
   1691e:	00140793          	addi	a5,s0,1
   16922:	fff70593          	addi	a1,a4,-1 # 7fff <exit-0x8121>
   16926:	1eb78663          	beq	a5,a1,16b12 <__addtf3+0x388>
   1692a:	567d                	li	a2,-1
   1692c:	03361713          	slli	a4,a2,0x33
   16930:	177d                	addi	a4,a4,-1
   16932:	00ee7733          	and	a4,t3,a4
   16936:	8fed                	and	a5,a5,a1
   16938:	03d71513          	slli	a0,a4,0x3d
   1693c:	00395913          	srli	s2,s2,0x3
   16940:	830d                	srli	a4,a4,0x3
   16942:	8241                	srli	a2,a2,0x10
   16944:	17c2                	slli	a5,a5,0x30
   16946:	01256533          	or	a0,a0,s2
   1694a:	8f71                	and	a4,a4,a2
   1694c:	93c1                	srli	a5,a5,0x30
   1694e:	86d2                	mv	a3,s4
   16950:	06be                	slli	a3,a3,0xf
   16952:	01071593          	slli	a1,a4,0x10
   16956:	00f6e733          	or	a4,a3,a5
   1695a:	1742                	slli	a4,a4,0x30
   1695c:	81c1                	srli	a1,a1,0x10
   1695e:	8dd9                	or	a1,a1,a4
  FP_HANDLE_EXCEPTIONS;
   16960:	2a089c63          	bnez	a7,16c18 <__addtf3+0x48e>

  return r;
}
   16964:	70a2                	ld	ra,40(sp)
   16966:	7402                	ld	s0,32(sp)
   16968:	64e2                	ld	s1,24(sp)
   1696a:	6942                	ld	s2,16(sp)
   1696c:	69a2                	ld	s3,8(sp)
   1696e:	6a02                	ld	s4,0(sp)
   16970:	6145                	addi	sp,sp,48
   16972:	8082                	ret
  FP_ADD_Q (R, A, B);
   16974:	0a088663          	beqz	a7,16a20 <__addtf3+0x296>
   16978:	32041663          	bnez	s0,16ca4 <__addtf3+0x51a>
   1697c:	00be6733          	or	a4,t3,a1
   16980:	16070463          	beqz	a4,16ae8 <__addtf3+0x35e>
   16984:	6f388d63          	beq	a7,s3,1707e <__addtf3+0x8f4>
   16988:	5e730663          	beq	t1,t2,16f74 <__addtf3+0x7ea>
   1698c:	fff7c793          	not	a5,a5
   16990:	2781                	sext.w	a5,a5
   16992:	a60d                	j	16cb4 <__addtf3+0x52a>
   16994:	19105e63          	blez	a7,16b30 <__addtf3+0x3a6>
   16998:	0e030863          	beqz	t1,16a88 <__addtf3+0x2fe>
   1699c:	04740d63          	beq	s0,t2,169f6 <__addtf3+0x26c>
   169a0:	4785                	li	a5,1
   169a2:	17ce                	slli	a5,a5,0x33
   169a4:	00ffefb3          	or	t6,t6,a5
   169a8:	07400793          	li	a5,116
   169ac:	3517cb63          	blt	a5,a7,16d02 <__addtf3+0x578>
   169b0:	03f00793          	li	a5,63
   169b4:	4317db63          	bge	a5,a7,16dea <__addtf3+0x660>
   169b8:	fc08891b          	addiw	s2,a7,-64
   169bc:	04000793          	li	a5,64
   169c0:	012fd733          	srl	a4,t6,s2
   169c4:	00f88a63          	beq	a7,a5,169d8 <__addtf3+0x24e>
   169c8:	08000793          	li	a5,128
   169cc:	411787bb          	subw	a5,a5,a7
   169d0:	00ff97b3          	sll	a5,t6,a5
   169d4:	00feeeb3          	or	t4,t4,a5
   169d8:	01d03933          	snez	s2,t4
   169dc:	00e96933          	or	s2,s2,a4
   169e0:	a62d                	j	16d0a <__addtf3+0x580>
   169e2:	01dfe733          	or	a4,t6,t4
   169e6:	30070363          	beqz	a4,16cec <__addtf3+0x562>
   169ea:	fff7889b          	addiw	a7,a5,-1
   169ee:	4c088763          	beqz	a7,16ebc <__addtf3+0x732>
   169f2:	e0741ae3          	bne	s0,t2,16806 <__addtf3+0x7c>
   169f6:	00be65b3          	or	a1,t3,a1
   169fa:	4a058f63          	beqz	a1,16eb8 <__addtf3+0x72e>
   169fe:	032e5893          	srli	a7,t3,0x32
   16a02:	0039d713          	srli	a4,s3,0x3
   16a06:	0018f893          	andi	a7,a7,1
   16a0a:	03de1613          	slli	a2,t3,0x3d
   16a0e:	00e2f533          	and	a0,t0,a4
   16a12:	0018b893          	seqz	a7,a7
   16a16:	8d51                	or	a0,a0,a2
   16a18:	003e5793          	srli	a5,t3,0x3
   16a1c:	0892                	slli	a7,a7,0x4
   16a1e:	a855                	j	16ad2 <__addtf3+0x348>
   16a20:	00140793          	addi	a5,s0,1
   16a24:	1f79                	addi	t5,t5,-2
   16a26:	01e7ff33          	and	t5,a5,t5
   16a2a:	240f1663          	bnez	t5,16c76 <__addtf3+0x4ec>
   16a2e:	00be6733          	or	a4,t3,a1
   16a32:	01dfe7b3          	or	a5,t6,t4
   16a36:	40041063          	bnez	s0,16e36 <__addtf3+0x6ac>
   16a3a:	4a070e63          	beqz	a4,16ef6 <__addtf3+0x76c>
   16a3e:	4c078263          	beqz	a5,16f02 <__addtf3+0x778>
   16a42:	41d58633          	sub	a2,a1,t4
   16a46:	00c5b7b3          	sltu	a5,a1,a2
   16a4a:	41fe0733          	sub	a4,t3,t6
   16a4e:	8f1d                	sub	a4,a4,a5
   16a50:	00c71793          	slli	a5,a4,0xc
   16a54:	6a07d663          	bgez	a5,17100 <__addtf3+0x976>
   16a58:	40be8933          	sub	s2,t4,a1
   16a5c:	41cf8733          	sub	a4,t6,t3
   16a60:	012ebe33          	sltu	t3,t4,s2
   16a64:	41c70e33          	sub	t3,a4,t3
  FP_PACK_SEMIRAW_Q (r, R);
   16a68:	01c967b3          	or	a5,s2,t3
  FP_ADD_Q (R, A, B);
   16a6c:	8a42                	mv	s4,a6
  FP_PACK_SEMIRAW_Q (r, R);
   16a6e:	e40796e3          	bnez	a5,168ba <__addtf3+0x130>
   16a72:	4501                	li	a0,0
   16a74:	4401                	li	s0,0
   16a76:	4881                	li	a7,0
   16a78:	01079713          	slli	a4,a5,0x10
   16a7c:	03141793          	slli	a5,s0,0x31
   16a80:	8341                	srli	a4,a4,0x10
   16a82:	93c5                	srli	a5,a5,0x31
   16a84:	86d2                	mv	a3,s4
   16a86:	b5e9                	j	16950 <__addtf3+0x1c6>
  FP_ADD_Q (R, A, B);
   16a88:	01dfe733          	or	a4,t6,t4
   16a8c:	46070d63          	beqz	a4,16f06 <__addtf3+0x77c>
   16a90:	fff7889b          	addiw	a7,a5,-1
   16a94:	3c088c63          	beqz	a7,16e6c <__addtf3+0x6e2>
   16a98:	f07418e3          	bne	s0,t2,169a8 <__addtf3+0x21e>
   16a9c:	00be67b3          	or	a5,t3,a1
   16aa0:	40078c63          	beqz	a5,16eb8 <__addtf3+0x72e>
   16aa4:	032e5893          	srli	a7,t3,0x32
   16aa8:	0018f893          	andi	a7,a7,1
  FP_DECL_EX;
   16aac:	0018b893          	seqz	a7,a7
   16ab0:	0892                	slli	a7,a7,0x4
  FP_PACK_SEMIRAW_Q (r, R);
   16ab2:	67a1                	lui	a5,0x8
   16ab4:	892e                	mv	s2,a1
   16ab6:	fff78413          	addi	s0,a5,-1 # 7fff <exit-0x8121>
   16aba:	6721                	lui	a4,0x8
   16abc:	03de1513          	slli	a0,t3,0x3d
   16ac0:	00395913          	srli	s2,s2,0x3
   16ac4:	177d                	addi	a4,a4,-1
   16ac6:	01256533          	or	a0,a0,s2
   16aca:	003e5793          	srli	a5,t3,0x3
   16ace:	fae415e3          	bne	s0,a4,16a78 <__addtf3+0x2ee>
   16ad2:	8d5d                	or	a0,a0,a5
   16ad4:	10050d63          	beqz	a0,16bee <__addtf3+0x464>
   16ad8:	6721                	lui	a4,0x8
   16ada:	fff70793          	addi	a5,a4,-1 # 7fff <exit-0x8121>
   16ade:	4705                	li	a4,1
   16ae0:	4681                	li	a3,0
   16ae2:	173e                	slli	a4,a4,0x2f
   16ae4:	4501                	li	a0,0
   16ae6:	b5ad                	j	16950 <__addtf3+0x1c6>
  FP_ADD_Q (R, A, B);
   16ae8:	5c730563          	beq	t1,t2,170b2 <__addtf3+0x928>
  FP_UNPACK_SEMIRAW_Q (B, b);
   16aec:	8e7e                	mv	t3,t6
   16aee:	8976                	mv	s2,t4
   16af0:	841a                	mv	s0,t1
   16af2:	8a42                	mv	s4,a6
  FP_PACK_SEMIRAW_Q (r, R);
   16af4:	01c967b3          	or	a5,s2,t3
   16af8:	da040fe3          	beqz	s0,168b6 <__addtf3+0x12c>
   16afc:	00797793          	andi	a5,s2,7
   16b00:	4581                	li	a1,0
   16b02:	de079be3          	bnez	a5,168f8 <__addtf3+0x16e>
   16b06:	4605                	li	a2,1
   16b08:	164e                	slli	a2,a2,0x33
   16b0a:	00ce7633          	and	a2,t3,a2
   16b0e:	4881                	li	a7,0
   16b10:	b521                	j	16918 <__addtf3+0x18e>
   16b12:	cce1                	beqz	s1,16bea <__addtf3+0x460>
   16b14:	478d                	li	a5,3
   16b16:	2cf48063          	beq	s1,a5,16dd6 <__addtf3+0x64c>
   16b1a:	4789                	li	a5,2
   16b1c:	0cf48563          	beq	s1,a5,16be6 <__addtf3+0x45c>
   16b20:	557d                	li	a0,-1
   16b22:	6421                	lui	s0,0x8
   16b24:	0058e893          	ori	a7,a7,5
   16b28:	00355793          	srli	a5,a0,0x3
   16b2c:	1479                	addi	s0,s0,-2
   16b2e:	b7a9                	j	16a78 <__addtf3+0x2ee>
  FP_ADD_Q (R, A, B);
   16b30:	0e088e63          	beqz	a7,16c2c <__addtf3+0x4a2>
   16b34:	28040563          	beqz	s0,16dbe <__addtf3+0x634>
   16b38:	4c730a63          	beq	t1,t2,1700c <__addtf3+0x882>
   16b3c:	4705                	li	a4,1
   16b3e:	174e                	slli	a4,a4,0x33
   16b40:	40f007bb          	negw	a5,a5
   16b44:	00ee6e33          	or	t3,t3,a4
   16b48:	07400713          	li	a4,116
   16b4c:	46f74663          	blt	a4,a5,16fb8 <__addtf3+0x82e>
   16b50:	03f00713          	li	a4,63
   16b54:	50f74063          	blt	a4,a5,17054 <__addtf3+0x8ca>
   16b58:	04000713          	li	a4,64
   16b5c:	9f1d                	subw	a4,a4,a5
   16b5e:	00ee1933          	sll	s2,t3,a4
   16b62:	00f5d633          	srl	a2,a1,a5
   16b66:	00e59733          	sll	a4,a1,a4
   16b6a:	00c96933          	or	s2,s2,a2
   16b6e:	00e03733          	snez	a4,a4
   16b72:	00fe57b3          	srl	a5,t3,a5
   16b76:	00e96933          	or	s2,s2,a4
   16b7a:	9fbe                	add	t6,t6,a5
   16b7c:	9976                	add	s2,s2,t4
   16b7e:	01d93e33          	sltu	t3,s2,t4
   16b82:	9e7e                	add	t3,t3,t6
  FP_UNPACK_SEMIRAW_Q (B, b);
   16b84:	841a                	mv	s0,t1
   16b86:	a271                	j	16d12 <__addtf3+0x588>
  FP_PACK_SEMIRAW_Q (r, R);
   16b88:	00f97793          	andi	a5,s2,15
   16b8c:	4711                	li	a4,4
   16b8e:	d6e78ee3          	beq	a5,a4,1690a <__addtf3+0x180>
   16b92:	00490793          	addi	a5,s2,4
   16b96:	0127b933          	sltu	s2,a5,s2
   16b9a:	9e4a                	add	t3,t3,s2
   16b9c:	893e                	mv	s2,a5
   16b9e:	b3b5                	j	1690a <__addtf3+0x180>
   16ba0:	4885                	li	a7,1
   16ba2:	d60a04e3          	beqz	s4,1690a <__addtf3+0x180>
   16ba6:	00890793          	addi	a5,s2,8
   16baa:	0127b933          	sltu	s2,a5,s2
   16bae:	9e4a                	add	t3,t3,s2
   16bb0:	893e                	mv	s2,a5
   16bb2:	bba1                	j	1690a <__addtf3+0x180>
   16bb4:	fe0a09e3          	beqz	s4,16ba6 <__addtf3+0x41c>
   16bb8:	bb89                	j	1690a <__addtf3+0x180>
  FP_ADD_Q (R, A, B);
   16bba:	577d                	li	a4,-1
   16bbc:	174e                	slli	a4,a4,0x33
   16bbe:	177d                	addi	a4,a4,-1
   16bc0:	8c09                	sub	s0,s0,a0
   16bc2:	00ee7e33          	and	t3,t3,a4
   16bc6:	894e                	mv	s2,s3
   16bc8:	b735                	j	16af4 <__addtf3+0x36a>
   16bca:	854a                	mv	a0,s2
   16bcc:	23b010ef          	jal	ra,18606 <__clzdi2>
   16bd0:	ff450e1b          	addiw	t3,a0,-12
   16bd4:	03f00793          	li	a5,63
   16bd8:	0345051b          	addiw	a0,a0,52
   16bdc:	01c91e33          	sll	t3,s2,t3
   16be0:	c8a7cee3          	blt	a5,a0,1687c <__addtf3+0xf2>
   16be4:	b141                	j	16864 <__addtf3+0xda>
  FP_PACK_SEMIRAW_Q (r, R);
   16be6:	1e0a0a63          	beqz	s4,16dda <__addtf3+0x650>
   16bea:	0058e893          	ori	a7,a7,5
  FP_ADD_Q (R, A, B);
   16bee:	6721                	lui	a4,0x8
   16bf0:	fff70793          	addi	a5,a4,-1 # 7fff <exit-0x8121>
   16bf4:	86d2                	mv	a3,s4
   16bf6:	4701                	li	a4,0
   16bf8:	4501                	li	a0,0
   16bfa:	bb99                	j	16950 <__addtf3+0x1c6>
   16bfc:	01dfe933          	or	s2,t6,t4
   16c00:	01203933          	snez	s2,s2
   16c04:	b92d                	j	1683e <__addtf3+0xb4>
   16c06:	28830663          	beq	t1,s0,16e92 <__addtf3+0x708>
   16c0a:	28079c63          	bnez	a5,16ea2 <__addtf3+0x718>
  FP_PACK_SEMIRAW_Q (r, R);
   16c0e:	0ffff5b7          	lui	a1,0xffff
   16c12:	4501                	li	a0,0
   16c14:	158e                	slli	a1,a1,0x23
  FP_ADD_Q (R, A, B);
   16c16:	48c1                	li	a7,16
  FP_HANDLE_EXCEPTIONS;
   16c18:	0018a073          	csrs	fflags,a7
}
   16c1c:	70a2                	ld	ra,40(sp)
   16c1e:	7402                	ld	s0,32(sp)
   16c20:	64e2                	ld	s1,24(sp)
   16c22:	6942                	ld	s2,16(sp)
   16c24:	69a2                	ld	s3,8(sp)
   16c26:	6a02                	ld	s4,0(sp)
   16c28:	6145                	addi	sp,sp,48
   16c2a:	8082                	ret
  FP_ADD_Q (R, A, B);
   16c2c:	00140713          	addi	a4,s0,1 # 8001 <exit-0x811f>
   16c30:	1f79                	addi	t5,t5,-2
   16c32:	01e777b3          	and	a5,a4,t5
   16c36:	1c079e63          	bnez	a5,16e12 <__addtf3+0x688>
   16c3a:	00be6733          	or	a4,t3,a1
   16c3e:	3a041763          	bnez	s0,16fec <__addtf3+0x862>
   16c42:	01dfe7b3          	or	a5,t6,t4
   16c46:	40070463          	beqz	a4,1704e <__addtf3+0x8c4>
   16c4a:	2a078c63          	beqz	a5,16f02 <__addtf3+0x778>
   16c4e:	01d58933          	add	s2,a1,t4
   16c52:	9e7e                	add	t3,t3,t6
   16c54:	00b935b3          	sltu	a1,s2,a1
   16c58:	9e2e                	add	t3,t3,a1
   16c5a:	00ce1793          	slli	a5,t3,0xc
   16c5e:	c407dae3          	bgez	a5,168b2 <__addtf3+0x128>
   16c62:	03399713          	slli	a4,s3,0x33
   16c66:	fff70793          	addi	a5,a4,-1
   16c6a:	00fe7e33          	and	t3,t3,a5
   16c6e:	4405                	li	s0,1
   16c70:	00797793          	andi	a5,s2,7
   16c74:	b571                	j	16b00 <__addtf3+0x376>
   16c76:	41d58933          	sub	s2,a1,t4
   16c7a:	0125b7b3          	sltu	a5,a1,s2
   16c7e:	41fe09b3          	sub	s3,t3,t6
   16c82:	40f989b3          	sub	s3,s3,a5
   16c86:	00c99793          	slli	a5,s3,0xc
   16c8a:	1c07c763          	bltz	a5,16e58 <__addtf3+0x6ce>
   16c8e:	013967b3          	or	a5,s2,s3
   16c92:	bc0793e3          	bnez	a5,16858 <__addtf3+0xce>
   16c96:	ffe48693          	addi	a3,s1,-2
   16c9a:	0016ba13          	seqz	s4,a3
   16c9e:	4501                	li	a0,0
   16ca0:	4401                	li	s0,0
   16ca2:	bbd9                	j	16a78 <__addtf3+0x2ee>
   16ca4:	2c730863          	beq	t1,t2,16f74 <__addtf3+0x7ea>
   16ca8:	4705                	li	a4,1
   16caa:	174e                	slli	a4,a4,0x33
   16cac:	40f007bb          	negw	a5,a5
   16cb0:	00ee6e33          	or	t3,t3,a4
   16cb4:	07400713          	li	a4,116
   16cb8:	0ef74663          	blt	a4,a5,16da4 <__addtf3+0x61a>
   16cbc:	03f00713          	li	a4,63
   16cc0:	30f74163          	blt	a4,a5,16fc2 <__addtf3+0x838>
   16cc4:	04000713          	li	a4,64
   16cc8:	9f1d                	subw	a4,a4,a5
   16cca:	00f5d6b3          	srl	a3,a1,a5
   16cce:	00ee1933          	sll	s2,t3,a4
   16cd2:	00e595b3          	sll	a1,a1,a4
   16cd6:	00d96933          	or	s2,s2,a3
   16cda:	00b035b3          	snez	a1,a1
   16cde:	00fe57b3          	srl	a5,t3,a5
   16ce2:	00b96933          	or	s2,s2,a1
   16ce6:	40ff8fb3          	sub	t6,t6,a5
   16cea:	a0c9                	j	16dac <__addtf3+0x622>
  FP_UNPACK_SEMIRAW_Q (A, a);
   16cec:	892e                	mv	s2,a1
  FP_ADD_Q (R, A, B);
   16cee:	e07413e3          	bne	s0,t2,16af4 <__addtf3+0x36a>
   16cf2:	00be67b3          	or	a5,t3,a1
   16cf6:	da0797e3          	bnez	a5,16aa4 <__addtf3+0x31a>
  FP_DECL_EX;
   16cfa:	4501                	li	a0,0
   16cfc:	4781                	li	a5,0
   16cfe:	4881                	li	a7,0
   16d00:	bbc9                	j	16ad2 <__addtf3+0x348>
  FP_ADD_Q (R, A, B);
   16d02:	01dfe933          	or	s2,t6,t4
   16d06:	01203933          	snez	s2,s2
   16d0a:	992e                	add	s2,s2,a1
   16d0c:	00b935b3          	sltu	a1,s2,a1
   16d10:	9e2e                	add	t3,t3,a1
   16d12:	00ce1793          	slli	a5,t3,0xc
   16d16:	dc07dfe3          	bgez	a5,16af4 <__addtf3+0x36a>
   16d1a:	67a1                	lui	a5,0x8
   16d1c:	0405                	addi	s0,s0,1
   16d1e:	fff78713          	addi	a4,a5,-1 # 7fff <exit-0x8121>
   16d22:	22e40a63          	beq	s0,a4,16f56 <__addtf3+0x7cc>
   16d26:	577d                	li	a4,-1
   16d28:	174e                	slli	a4,a4,0x33
   16d2a:	177d                	addi	a4,a4,-1
   16d2c:	00ee7e33          	and	t3,t3,a4
   16d30:	00195793          	srli	a5,s2,0x1
   16d34:	00197913          	andi	s2,s2,1
   16d38:	0127e7b3          	or	a5,a5,s2
   16d3c:	03fe1913          	slli	s2,t3,0x3f
   16d40:	00f96933          	or	s2,s2,a5
   16d44:	001e5e13          	srli	t3,t3,0x1
   16d48:	8b9d                	andi	a5,a5,7
   16d4a:	bb5d                	j	16b00 <__addtf3+0x376>
   16d4c:	fc15079b          	addiw	a5,a0,-63
   16d50:	04000593          	li	a1,64
   16d54:	00fe57b3          	srl	a5,t3,a5
   16d58:	00b70a63          	beq	a4,a1,16d6c <__addtf3+0x5e2>
   16d5c:	08000713          	li	a4,128
   16d60:	40c7063b          	subw	a2,a4,a2
   16d64:	00ce1e33          	sll	t3,t3,a2
   16d68:	01c9e9b3          	or	s3,s3,t3
   16d6c:	013039b3          	snez	s3,s3
   16d70:	00f9e7b3          	or	a5,s3,a5
   16d74:	893e                	mv	s2,a5
   16d76:	4e01                	li	t3,0
   16d78:	be3d                	j	168b6 <__addtf3+0x12c>
   16d7a:	fc08891b          	addiw	s2,a7,-64
   16d7e:	04000793          	li	a5,64
   16d82:	012fd733          	srl	a4,t6,s2
   16d86:	00f88a63          	beq	a7,a5,16d9a <__addtf3+0x610>
   16d8a:	08000793          	li	a5,128
   16d8e:	411787bb          	subw	a5,a5,a7
   16d92:	00ff97b3          	sll	a5,t6,a5
   16d96:	00feeeb3          	or	t4,t4,a5
   16d9a:	01d03933          	snez	s2,t4
   16d9e:	00e96933          	or	s2,s2,a4
   16da2:	bc71                	j	1683e <__addtf3+0xb4>
   16da4:	00be6e33          	or	t3,t3,a1
   16da8:	01c03933          	snez	s2,t3
   16dac:	412e8933          	sub	s2,t4,s2
   16db0:	012ebe33          	sltu	t3,t4,s2
   16db4:	41cf8e33          	sub	t3,t6,t3
  FP_UNPACK_SEMIRAW_Q (B, b);
   16db8:	841a                	mv	s0,t1
   16dba:	8a42                	mv	s4,a6
   16dbc:	b479                	j	1684a <__addtf3+0xc0>
  FP_ADD_Q (R, A, B);
   16dbe:	00be6733          	or	a4,t3,a1
   16dc2:	26070a63          	beqz	a4,17036 <__addtf3+0x8ac>
   16dc6:	39388663          	beq	a7,s3,17152 <__addtf3+0x9c8>
   16dca:	24730163          	beq	t1,t2,1700c <__addtf3+0x882>
   16dce:	fff7c793          	not	a5,a5
   16dd2:	2781                	sext.w	a5,a5
   16dd4:	bb95                	j	16b48 <__addtf3+0x3be>
  FP_PACK_SEMIRAW_Q (r, R);
   16dd6:	e00a0ae3          	beqz	s4,16bea <__addtf3+0x460>
   16dda:	557d                	li	a0,-1
   16ddc:	0058e893          	ori	a7,a7,5
   16de0:	00355793          	srli	a5,a0,0x3
   16de4:	ffe70413          	addi	s0,a4,-2
   16de8:	b941                	j	16a78 <__addtf3+0x2ee>
  FP_ADD_Q (R, A, B);
   16dea:	04000793          	li	a5,64
   16dee:	411787bb          	subw	a5,a5,a7
   16df2:	011ed733          	srl	a4,t4,a7
   16df6:	00ff9933          	sll	s2,t6,a5
   16dfa:	00fe9eb3          	sll	t4,t4,a5
   16dfe:	00e96933          	or	s2,s2,a4
   16e02:	01d03eb3          	snez	t4,t4
   16e06:	011fd8b3          	srl	a7,t6,a7
   16e0a:	01d96933          	or	s2,s2,t4
   16e0e:	9e46                	add	t3,t3,a7
   16e10:	bded                	j	16d0a <__addtf3+0x580>
   16e12:	28770263          	beq	a4,t2,17096 <__addtf3+0x90c>
   16e16:	01d587b3          	add	a5,a1,t4
   16e1a:	00b7b5b3          	sltu	a1,a5,a1
   16e1e:	9e7e                	add	t3,t3,t6
   16e20:	9e2e                	add	t3,t3,a1
   16e22:	8385                	srli	a5,a5,0x1
   16e24:	03fe1913          	slli	s2,t3,0x3f
   16e28:	00f96933          	or	s2,s2,a5
   16e2c:	001e5e13          	srli	t3,t3,0x1
   16e30:	8b9d                	andi	a5,a5,7
   16e32:	843a                	mv	s0,a4
   16e34:	b1f1                	j	16b00 <__addtf3+0x376>
   16e36:	04740363          	beq	s0,t2,16e7c <__addtf3+0x6f2>
   16e3a:	04730c63          	beq	t1,t2,16e92 <__addtf3+0x708>
   16e3e:	dc0706e3          	beqz	a4,16c0a <__addtf3+0x480>
   16e42:	1a079e63          	bnez	a5,16ffe <__addtf3+0x874>
   16e46:	00329793          	slli	a5,t0,0x3
   16e4a:	838d                	srli	a5,a5,0x3
   16e4c:	03de1513          	slli	a0,t3,0x3d
   16e50:	8d5d                	or	a0,a0,a5
   16e52:	003e5793          	srli	a5,t3,0x3
  FP_PACK_SEMIRAW_Q (r, R);
   16e56:	b9b5                	j	16ad2 <__addtf3+0x348>
  FP_ADD_Q (R, A, B);
   16e58:	40be8933          	sub	s2,t4,a1
   16e5c:	41cf8e33          	sub	t3,t6,t3
   16e60:	012ebeb3          	sltu	t4,t4,s2
   16e64:	41de09b3          	sub	s3,t3,t4
   16e68:	8a42                	mv	s4,a6
   16e6a:	b2fd                	j	16858 <__addtf3+0xce>
   16e6c:	01d58933          	add	s2,a1,t4
   16e70:	01fe0733          	add	a4,t3,t6
   16e74:	00b93e33          	sltu	t3,s2,a1
   16e78:	9e3a                	add	t3,t3,a4
   16e7a:	bd61                	j	16d12 <__addtf3+0x588>
   16e7c:	d80705e3          	beqz	a4,16c06 <__addtf3+0x47c>
   16e80:	00de1693          	slli	a3,t3,0xd
   16e84:	0006c363          	bltz	a3,16e8a <__addtf3+0x700>
   16e88:	48c1                	li	a7,16
   16e8a:	65a1                	lui	a1,0x8
   16e8c:	15fd                	addi	a1,a1,-1
   16e8e:	fab31ae3          	bne	t1,a1,16e42 <__addtf3+0x6b8>
   16e92:	d7d5                	beqz	a5,16e3e <__addtf3+0x6b4>
   16e94:	00df9793          	slli	a5,t6,0xd
   16e98:	0007c363          	bltz	a5,16e9e <__addtf3+0x714>
   16e9c:	48c1                	li	a7,16
   16e9e:	16071063          	bnez	a4,16ffe <__addtf3+0x874>
   16ea2:	00361793          	slli	a5,a2,0x3
   16ea6:	838d                	srli	a5,a5,0x3
   16ea8:	03df9513          	slli	a0,t6,0x3d
   16eac:	8d5d                	or	a0,a0,a5
   16eae:	8a42                	mv	s4,a6
   16eb0:	003fd793          	srli	a5,t6,0x3
  FP_PACK_SEMIRAW_Q (r, R);
   16eb4:	b939                	j	16ad2 <__addtf3+0x348>
   16eb6:	8a42                	mv	s4,a6
  FP_DECL_EX;
   16eb8:	4881                	li	a7,0
   16eba:	bb15                	j	16bee <__addtf3+0x464>
  FP_ADD_Q (R, A, B);
   16ebc:	41d58933          	sub	s2,a1,t4
   16ec0:	41fe0733          	sub	a4,t3,t6
   16ec4:	0125be33          	sltu	t3,a1,s2
   16ec8:	41c70e33          	sub	t3,a4,t3
   16ecc:	babd                	j	1684a <__addtf3+0xc0>
   16ece:	00be65b3          	or	a1,t3,a1
   16ed2:	d1fd                	beqz	a1,16eb8 <__addtf3+0x72e>
   16ed4:	032e5893          	srli	a7,t3,0x32
   16ed8:	0039d613          	srli	a2,s3,0x3
   16edc:	0018f893          	andi	a7,a7,1
   16ee0:	03de1793          	slli	a5,t3,0x3d
   16ee4:	00c2f533          	and	a0,t0,a2
   16ee8:	0018b893          	seqz	a7,a7
   16eec:	8d5d                	or	a0,a0,a5
   16eee:	0892                	slli	a7,a7,0x4
   16ef0:	003e5793          	srli	a5,t3,0x3
   16ef4:	bef9                	j	16ad2 <__addtf3+0x348>
   16ef6:	14078663          	beqz	a5,17042 <__addtf3+0x8b8>
   16efa:	4781                	li	a5,0
  FP_UNPACK_SEMIRAW_Q (B, b);
   16efc:	8e7e                	mv	t3,t6
   16efe:	85f6                	mv	a1,t4
   16f00:	8a42                	mv	s4,a6
   16f02:	892e                	mv	s2,a1
   16f04:	ba6d                	j	168be <__addtf3+0x134>
  FP_UNPACK_SEMIRAW_Q (A, a);
   16f06:	892e                	mv	s2,a1
  FP_ADD_Q (R, A, B);
   16f08:	be7416e3          	bne	s0,t2,16af4 <__addtf3+0x36a>
   16f0c:	00be67b3          	or	a5,t3,a1
   16f10:	de0785e3          	beqz	a5,16cfa <__addtf3+0x570>
   16f14:	032e5893          	srli	a7,t3,0x32
  FP_DECL_EX;
   16f18:	0018b893          	seqz	a7,a7
   16f1c:	0892                	slli	a7,a7,0x4
   16f1e:	be51                	j	16ab2 <__addtf3+0x328>
  FP_PACK_SEMIRAW_Q (r, R);
   16f20:	00f5f613          	andi	a2,a1,15
   16f24:	4511                	li	a0,4
   16f26:	9ca600e3          	beq	a2,a0,168e6 <__addtf3+0x15c>
   16f2a:	ffc5b613          	sltiu	a2,a1,-4
   16f2e:	00164613          	xori	a2,a2,1
   16f32:	1602                	slli	a2,a2,0x20
   16f34:	9201                	srli	a2,a2,0x20
   16f36:	9732                	add	a4,a4,a2
   16f38:	b27d                	j	168e6 <__addtf3+0x15c>
   16f3a:	9a0a16e3          	bnez	s4,168e6 <__addtf3+0x15c>
   16f3e:	ff85b613          	sltiu	a2,a1,-8
   16f42:	00164613          	xori	a2,a2,1
   16f46:	1602                	slli	a2,a2,0x20
   16f48:	9201                	srli	a2,a2,0x20
   16f4a:	9732                	add	a4,a4,a2
   16f4c:	ba69                	j	168e6 <__addtf3+0x15c>
   16f4e:	4885                	li	a7,1
   16f50:	980a0be3          	beqz	s4,168e6 <__addtf3+0x15c>
   16f54:	b7ed                	j	16f3e <__addtf3+0x7b4>
  FP_ADD_Q (R, A, B);
   16f56:	ccb9                	beqz	s1,16fb4 <__addtf3+0x82a>
   16f58:	470d                	li	a4,3
   16f5a:	1ee48263          	beq	s1,a4,1713e <__addtf3+0x9b4>
   16f5e:	4709                	li	a4,2
   16f60:	4895                	li	a7,5
   16f62:	bae49fe3          	bne	s1,a4,16b20 <__addtf3+0x396>
   16f66:	c80a14e3          	bnez	s4,16bee <__addtf3+0x464>
   16f6a:	ffe78413          	addi	s0,a5,-2
   16f6e:	5e7d                	li	t3,-1
   16f70:	597d                	li	s2,-1
   16f72:	b26d                	j	1691c <__addtf3+0x192>
   16f74:	01dfeeb3          	or	t4,t6,t4
   16f78:	f20e8fe3          	beqz	t4,16eb6 <__addtf3+0x72c>
   16f7c:	0039d713          	srli	a4,s3,0x3
   16f80:	00e677b3          	and	a5,a2,a4
   16f84:	03df9513          	slli	a0,t6,0x3d
   16f88:	00df9713          	slli	a4,t6,0xd
   16f8c:	8d5d                	or	a0,a0,a5
   16f8e:	8a42                	mv	s4,a6
   16f90:	003fd793          	srli	a5,t6,0x3
   16f94:	48c1                	li	a7,16
   16f96:	b2075ee3          	bgez	a4,16ad2 <__addtf3+0x348>
  FP_DECL_EX;
   16f9a:	4881                	li	a7,0
   16f9c:	be1d                	j	16ad2 <__addtf3+0x348>
   16f9e:	164e                	slli	a2,a2,0x33
   16fa0:	00ce7633          	and	a2,t3,a2
  FP_PACK_SEMIRAW_Q (r, R);
   16fa4:	96071ae3          	bnez	a4,16918 <__addtf3+0x18e>
   16fa8:	843e                	mv	s0,a5
   16faa:	960887e3          	beqz	a7,16918 <__addtf3+0x18e>
   16fae:	0028e893          	ori	a7,a7,2
   16fb2:	b29d                	j	16918 <__addtf3+0x18e>
  FP_ADD_Q (R, A, B);
   16fb4:	4895                	li	a7,5
   16fb6:	b925                	j	16bee <__addtf3+0x464>
   16fb8:	00be6e33          	or	t3,t3,a1
   16fbc:	01c03933          	snez	s2,t3
   16fc0:	be75                	j	16b7c <__addtf3+0x3f2>
   16fc2:	fc07891b          	addiw	s2,a5,-64
   16fc6:	04000693          	li	a3,64
   16fca:	012e5733          	srl	a4,t3,s2
   16fce:	00d78a63          	beq	a5,a3,16fe2 <__addtf3+0x858>
   16fd2:	08000693          	li	a3,128
   16fd6:	40f687bb          	subw	a5,a3,a5
   16fda:	00fe1e33          	sll	t3,t3,a5
   16fde:	01c5e5b3          	or	a1,a1,t3
   16fe2:	00b03933          	snez	s2,a1
   16fe6:	00e96933          	or	s2,s2,a4
   16fea:	b3c9                	j	16dac <__addtf3+0x622>
   16fec:	12740463          	beq	s0,t2,17114 <__addtf3+0x98a>
   16ff0:	0e730963          	beq	t1,t2,170e2 <__addtf3+0x958>
   16ff4:	cf6d                	beqz	a4,170ee <__addtf3+0x964>
   16ff6:	01dfe7b3          	or	a5,t6,t4
   16ffa:	e40786e3          	beqz	a5,16e46 <__addtf3+0x6bc>
   16ffe:	67a1                	lui	a5,0x8
   17000:	4705                	li	a4,1
   17002:	4681                	li	a3,0
   17004:	17fd                	addi	a5,a5,-1
   17006:	173e                	slli	a4,a4,0x2f
  FP_PACK_SEMIRAW_Q (r, R);
   17008:	4501                	li	a0,0
   1700a:	b299                	j	16950 <__addtf3+0x1c6>
  FP_ADD_Q (R, A, B);
   1700c:	01dfeeb3          	or	t4,t6,t4
   17010:	ea0e84e3          	beqz	t4,16eb8 <__addtf3+0x72e>
   17014:	032fd893          	srli	a7,t6,0x32
   17018:	0039d713          	srli	a4,s3,0x3
   1701c:	0018f893          	andi	a7,a7,1
   17020:	03df9793          	slli	a5,t6,0x3d
   17024:	00e67533          	and	a0,a2,a4
   17028:	0018b893          	seqz	a7,a7
   1702c:	8d5d                	or	a0,a0,a5
   1702e:	0892                	slli	a7,a7,0x4
   17030:	003fd793          	srli	a5,t6,0x3
   17034:	bc79                	j	16ad2 <__addtf3+0x348>
   17036:	08730a63          	beq	t1,t2,170ca <__addtf3+0x940>
  FP_UNPACK_SEMIRAW_Q (B, b);
   1703a:	8e7e                	mv	t3,t6
   1703c:	8976                	mv	s2,t4
   1703e:	841a                	mv	s0,t1
   17040:	bc55                	j	16af4 <__addtf3+0x36a>
  FP_ADD_Q (R, A, B);
   17042:	ffe48693          	addi	a3,s1,-2
   17046:	0016ba13          	seqz	s4,a3
   1704a:	4501                	li	a0,0
   1704c:	b435                	j	16a78 <__addtf3+0x2ee>
  FP_UNPACK_SEMIRAW_Q (B, b);
   1704e:	8e7e                	mv	t3,t6
   17050:	8976                	mv	s2,t4
   17052:	b095                	j	168b6 <__addtf3+0x12c>
  FP_ADD_Q (R, A, B);
   17054:	fc07871b          	addiw	a4,a5,-64
   17058:	04000613          	li	a2,64
   1705c:	00ee5733          	srl	a4,t3,a4
   17060:	00c78a63          	beq	a5,a2,17074 <__addtf3+0x8ea>
   17064:	08000613          	li	a2,128
   17068:	40f607bb          	subw	a5,a2,a5
   1706c:	00fe1e33          	sll	t3,t3,a5
   17070:	01c5e5b3          	or	a1,a1,t3
   17074:	00b03933          	snez	s2,a1
   17078:	00e96933          	or	s2,s2,a4
   1707c:	b601                	j	16b7c <__addtf3+0x3f2>
   1707e:	40be8933          	sub	s2,t4,a1
   17082:	41cf8733          	sub	a4,t6,t3
   17086:	012ebe33          	sltu	t3,t4,s2
   1708a:	41c70e33          	sub	t3,a4,t3
  FP_UNPACK_SEMIRAW_Q (B, b);
   1708e:	841a                	mv	s0,t1
  FP_ADD_Q (R, A, B);
   17090:	8a42                	mv	s4,a6
   17092:	fb8ff06f          	j	1684a <__addtf3+0xc0>
   17096:	dc99                	beqz	s1,16fb4 <__addtf3+0x82a>
   17098:	478d                	li	a5,3
   1709a:	0cf48e63          	beq	s1,a5,17176 <__addtf3+0x9ec>
   1709e:	4789                	li	a5,2
   170a0:	4895                	li	a7,5
   170a2:	a6f49fe3          	bne	s1,a5,16b20 <__addtf3+0x396>
   170a6:	b40a14e3          	bnez	s4,16bee <__addtf3+0x464>
   170aa:	847a                	mv	s0,t5
   170ac:	5e7d                	li	t3,-1
   170ae:	597d                	li	s2,-1
   170b0:	b0b5                	j	1691c <__addtf3+0x192>
   170b2:	01dfe7b3          	or	a5,t6,t4
   170b6:	c7dd                	beqz	a5,17164 <__addtf3+0x9da>
   170b8:	00df9793          	slli	a5,t6,0xd
   170bc:	8e7e                	mv	t3,t6
  FP_UNPACK_SEMIRAW_Q (B, b);
   170be:	85f6                	mv	a1,t4
   170c0:	8a42                	mv	s4,a6
  FP_ADD_Q (R, A, B);
   170c2:	0007ce63          	bltz	a5,170de <__addtf3+0x954>
   170c6:	48c1                	li	a7,16
   170c8:	b2ed                	j	16ab2 <__addtf3+0x328>
   170ca:	01dfe7b3          	or	a5,t6,t4
   170ce:	c20786e3          	beqz	a5,16cfa <__addtf3+0x570>
   170d2:	00df9793          	slli	a5,t6,0xd
   170d6:	8e7e                	mv	t3,t6
  FP_UNPACK_SEMIRAW_Q (B, b);
   170d8:	85f6                	mv	a1,t4
  FP_ADD_Q (R, A, B);
   170da:	fe07d6e3          	bgez	a5,170c6 <__addtf3+0x93c>
  FP_DECL_EX;
   170de:	4881                	li	a7,0
   170e0:	bac9                	j	16ab2 <__addtf3+0x328>
  FP_ADD_Q (R, A, B);
   170e2:	01dfeeb3          	or	t4,t6,t4
   170e6:	040e9563          	bnez	t4,17130 <__addtf3+0x9a6>
   170ea:	d4071ee3          	bnez	a4,16e46 <__addtf3+0x6bc>
  FP_PACK_SEMIRAW_Q (r, R);
   170ee:	00361793          	slli	a5,a2,0x3
   170f2:	838d                	srli	a5,a5,0x3
   170f4:	03df9513          	slli	a0,t6,0x3d
   170f8:	8d5d                	or	a0,a0,a5
   170fa:	003fd793          	srli	a5,t6,0x3
   170fe:	bad1                	j	16ad2 <__addtf3+0x348>
  FP_ADD_Q (R, A, B);
   17100:	00e667b3          	or	a5,a2,a4
   17104:	df9d                	beqz	a5,17042 <__addtf3+0x8b8>
   17106:	85b2                	mv	a1,a2
   17108:	00767793          	andi	a5,a2,7
   1710c:	8e3a                	mv	t3,a4
  FP_UNPACK_SEMIRAW_Q (B, b);
   1710e:	892e                	mv	s2,a1
   17110:	faeff06f          	j	168be <__addtf3+0x134>
  FP_ADD_Q (R, A, B);
   17114:	cb31                	beqz	a4,17168 <__addtf3+0x9de>
   17116:	00de1793          	slli	a5,t3,0xd
   1711a:	0007c363          	bltz	a5,17120 <__addtf3+0x996>
   1711e:	48c1                	li	a7,16
   17120:	67a1                	lui	a5,0x8
   17122:	17fd                	addi	a5,a5,-1
   17124:	ecf319e3          	bne	t1,a5,16ff6 <__addtf3+0x86c>
   17128:	01dfe5b3          	or	a1,t6,t4
   1712c:	d0058de3          	beqz	a1,16e46 <__addtf3+0x6bc>
   17130:	00df9793          	slli	a5,t6,0xd
   17134:	0007c363          	bltz	a5,1713a <__addtf3+0x9b0>
   17138:	48c1                	li	a7,16
   1713a:	db55                	beqz	a4,170ee <__addtf3+0x964>
   1713c:	b5c9                	j	16ffe <__addtf3+0x874>
   1713e:	e60a0be3          	beqz	s4,16fb4 <__addtf3+0x82a>
   17142:	ffe78413          	addi	s0,a5,-2 # 7ffe <exit-0x8122>
   17146:	4581                	li	a1,0
   17148:	4895                	li	a7,5
   1714a:	5e7d                	li	t3,-1
   1714c:	597d                	li	s2,-1
   1714e:	fbcff06f          	j	1690a <__addtf3+0x180>
   17152:	01d58933          	add	s2,a1,t4
   17156:	01fe0733          	add	a4,t3,t6
   1715a:	01d93e33          	sltu	t3,s2,t4
   1715e:	9e3a                	add	t3,t3,a4
  FP_UNPACK_SEMIRAW_Q (B, b);
   17160:	841a                	mv	s0,t1
  FP_ADD_Q (R, A, B);
   17162:	be45                	j	16d12 <__addtf3+0x588>
   17164:	8a42                	mv	s4,a6
   17166:	be51                	j	16cfa <__addtf3+0x570>
   17168:	f88313e3          	bne	t1,s0,170ee <__addtf3+0x964>
   1716c:	01dfeeb3          	or	t4,t6,t4
   17170:	fc0e90e3          	bnez	t4,17130 <__addtf3+0x9a6>
   17174:	bfad                	j	170ee <__addtf3+0x964>
   17176:	e20a0fe3          	beqz	s4,16fb4 <__addtf3+0x82a>
   1717a:	4581                	li	a1,0
   1717c:	847a                	mv	s0,t5
   1717e:	4895                	li	a7,5
   17180:	5e7d                	li	t3,-1
   17182:	597d                	li	s2,-1
   17184:	f86ff06f          	j	1690a <__addtf3+0x180>

0000000000017188 <__eqtf2>:
#include "soft-fp.h"
#include "quad.h"

CMPtype
__eqtf2 (TFtype a, TFtype b)
{
   17188:	832a                	mv	t1,a0
  FP_DECL_EX;
  FP_DECL_Q (A);
  FP_DECL_Q (B);
  CMPtype r;

  FP_INIT_EXCEPTIONS;
   1718a:	002027f3          	frrm	a5
  FP_UNPACK_RAW_Q (A, a);
   1718e:	6721                	lui	a4,0x8
   17190:	177d                	addi	a4,a4,-1
   17192:	0305d813          	srli	a6,a1,0x30
   17196:	57fd                	li	a5,-1
   17198:	83c1                	srli	a5,a5,0x10
  FP_UNPACK_RAW_Q (B, b);
   1719a:	0306d893          	srli	a7,a3,0x30
  FP_UNPACK_RAW_Q (A, a);
   1719e:	00e87833          	and	a6,a6,a4
   171a2:	00f5fe33          	and	t3,a1,a5
  FP_UNPACK_RAW_Q (B, b);
   171a6:	00e8f8b3          	and	a7,a7,a4
   171aa:	8ff5                	and	a5,a5,a3
  FP_UNPACK_RAW_Q (A, a);
   171ac:	91fd                	srli	a1,a1,0x3f
  FP_UNPACK_RAW_Q (B, b);
   171ae:	92fd                	srli	a3,a3,0x3f
  FP_CMP_EQ_Q (r, A, B, 1);
   171b0:	00e80c63          	beq	a6,a4,171c8 <__eqtf2+0x40>
   171b4:	00e88663          	beq	a7,a4,171c0 <__eqtf2+0x38>
   171b8:	4505                	li	a0,1
   171ba:	03180063          	beq	a6,a7,171da <__eqtf2+0x52>
  FP_HANDLE_EXCEPTIONS;

  return r;
}
   171be:	8082                	ret
  FP_CMP_EQ_Q (r, A, B, 1);
   171c0:	8e5d                	or	a2,a2,a5
   171c2:	ea29                	bnez	a2,17214 <__eqtf2+0x8c>
   171c4:	4505                	li	a0,1
}
   171c6:	8082                	ret
  FP_CMP_EQ_Q (r, A, B, 1);
   171c8:	00ae6733          	or	a4,t3,a0
   171cc:	e71d                	bnez	a4,171fa <__eqtf2+0x72>
   171ce:	4505                	li	a0,1
   171d0:	ff0897e3          	bne	a7,a6,171be <__eqtf2+0x36>
   171d4:	00c7e733          	or	a4,a5,a2
   171d8:	ef15                	bnez	a4,17214 <__eqtf2+0x8c>
   171da:	4505                	li	a0,1
   171dc:	fefe11e3          	bne	t3,a5,171be <__eqtf2+0x36>
   171e0:	fcc31fe3          	bne	t1,a2,171be <__eqtf2+0x36>
   171e4:	4501                	li	a0,0
   171e6:	fcd58ce3          	beq	a1,a3,171be <__eqtf2+0x36>
   171ea:	4505                	li	a0,1
   171ec:	fc0819e3          	bnez	a6,171be <__eqtf2+0x36>
   171f0:	006e6533          	or	a0,t3,t1
   171f4:	00a03533          	snez	a0,a0
   171f8:	8082                	ret
   171fa:	02fe5e13          	srli	t3,t3,0x2f
   171fe:	000e1663          	bnez	t3,1720a <__eqtf2+0x82>
  FP_HANDLE_EXCEPTIONS;
   17202:	00186073          	csrsi	fflags,16
   17206:	4505                	li	a0,1
   17208:	8082                	ret
  FP_CMP_EQ_Q (r, A, B, 1);
   1720a:	4505                	li	a0,1
   1720c:	fb0899e3          	bne	a7,a6,171be <__eqtf2+0x36>
   17210:	8e5d                	or	a2,a2,a5
   17212:	d655                	beqz	a2,171be <__eqtf2+0x36>
   17214:	93bd                	srli	a5,a5,0x2f
   17216:	d7f5                	beqz	a5,17202 <__eqtf2+0x7a>
   17218:	b775                	j	171c4 <__eqtf2+0x3c>

000000000001721a <__multf3>:
#include "soft-fp.h"
#include "quad.h"

TFtype
__multf3 (TFtype a, TFtype b)
{
   1721a:	7159                	addi	sp,sp,-112
   1721c:	e8ca                	sd	s2,80(sp)
   1721e:	e0d2                	sd	s4,64(sp)
   17220:	ec66                	sd	s9,24(sp)
   17222:	f486                	sd	ra,104(sp)
   17224:	f0a2                	sd	s0,96(sp)
   17226:	eca6                	sd	s1,88(sp)
   17228:	e4ce                	sd	s3,72(sp)
   1722a:	fc56                	sd	s5,56(sp)
   1722c:	f85a                	sd	s6,48(sp)
   1722e:	f45e                	sd	s7,40(sp)
   17230:	f062                	sd	s8,32(sp)
   17232:	e86a                	sd	s10,16(sp)
   17234:	e46e                	sd	s11,8(sp)
   17236:	892a                	mv	s2,a0
   17238:	8cb2                	mv	s9,a2
   1723a:	8a36                	mv	s4,a3
  FP_DECL_Q (A);
  FP_DECL_Q (B);
  FP_DECL_Q (R);
  TFtype r;

  FP_INIT_ROUNDMODE;
   1723c:	00202b73          	frrm	s6
  FP_UNPACK_Q (A, a);
   17240:	6721                	lui	a4,0x8
   17242:	0305d793          	srli	a5,a1,0x30
   17246:	177d                	addi	a4,a4,-1
   17248:	01059413          	slli	s0,a1,0x10
   1724c:	8ff9                	and	a5,a5,a4
  FP_INIT_ROUNDMODE;
   1724e:	2b01                	sext.w	s6,s6
  FP_UNPACK_Q (A, a);
   17250:	8041                	srli	s0,s0,0x10
   17252:	03f5db93          	srli	s7,a1,0x3f
   17256:	cffd                	beqz	a5,17354 <__multf3+0x13a>
   17258:	00078a9b          	sext.w	s5,a5
   1725c:	12e78d63          	beq	a5,a4,17396 <__multf3+0x17c>
   17260:	040e                	slli	s0,s0,0x3
   17262:	03d55713          	srli	a4,a0,0x3d
   17266:	8f41                	or	a4,a4,s0
   17268:	7af1                	lui	s5,0xffffc
   1726a:	4405                	li	s0,1
   1726c:	144e                	slli	s0,s0,0x33
   1726e:	0a85                	addi	s5,s5,1
   17270:	8c59                	or	s0,s0,a4
   17272:	00351493          	slli	s1,a0,0x3
   17276:	9abe                	add	s5,s5,a5
   17278:	4c01                	li	s8,0
   1727a:	4d01                	li	s10,0
  FP_DECL_EX;
   1727c:	4981                	li	s3,0
  FP_UNPACK_Q (B, b);
   1727e:	66a1                	lui	a3,0x8
   17280:	030a5713          	srli	a4,s4,0x30
   17284:	16fd                	addi	a3,a3,-1
   17286:	010a1913          	slli	s2,s4,0x10
   1728a:	8f75                	and	a4,a4,a3
   1728c:	8de6                	mv	s11,s9
   1728e:	01095913          	srli	s2,s2,0x10
   17292:	03fa5a13          	srli	s4,s4,0x3f
   17296:	10070e63          	beqz	a4,173b2 <__multf3+0x198>
   1729a:	0007061b          	sext.w	a2,a4
   1729e:	04d70463          	beq	a4,a3,172e6 <__multf3+0xcc>
   172a2:	090e                	slli	s2,s2,0x3
   172a4:	03dcd793          	srli	a5,s9,0x3d
   172a8:	76f1                	lui	a3,0xffffc
   172aa:	0127e7b3          	or	a5,a5,s2
   172ae:	0685                	addi	a3,a3,1
   172b0:	4905                	li	s2,1
   172b2:	9736                	add	a4,a4,a3
   172b4:	194e                	slli	s2,s2,0x33
   172b6:	0127e933          	or	s2,a5,s2
   172ba:	003c9d93          	slli	s11,s9,0x3
   172be:	9aba                	add	s5,s5,a4
   172c0:	4601                	li	a2,0
  FP_MUL_Q (R, A, B);
   172c2:	014bc733          	xor	a4,s7,s4
   172c6:	46bd                	li	a3,15
   172c8:	883a                	mv	a6,a4
   172ca:	001a8593          	addi	a1,s5,1 # ffffffffffffc001 <__BSS_END__+0xfffffffffffdf5e1>
   172ce:	1386e263          	bltu	a3,s8,173f2 <__multf3+0x1d8>
   172d2:	00003697          	auipc	a3,0x3
   172d6:	dee68693          	addi	a3,a3,-530 # 1a0c0 <empty_mo+0x30>
   172da:	002c1793          	slli	a5,s8,0x2
   172de:	97b6                	add	a5,a5,a3
   172e0:	439c                	lw	a5,0(a5)
   172e2:	97b6                	add	a5,a5,a3
   172e4:	8782                	jr	a5
  FP_UNPACK_Q (B, b);
   172e6:	019967b3          	or	a5,s2,s9
   172ea:	9ab2                	add	s5,s5,a2
   172ec:	32078163          	beqz	a5,1760e <__multf3+0x3f4>
   172f0:	01091793          	slli	a5,s2,0x10
   172f4:	003c6c13          	ori	s8,s8,3
   172f8:	460d                	li	a2,3
   172fa:	fc07c4e3          	bltz	a5,172c2 <__multf3+0xa8>
   172fe:	49c1                	li	s3,16
   17300:	b7c9                	j	172c2 <__multf3+0xa8>
   17302:	8852                	mv	a6,s4
  FP_MUL_Q (R, A, B);
   17304:	844a                	mv	s0,s2
   17306:	84ee                	mv	s1,s11
   17308:	8d32                	mv	s10,a2
  FP_PACK_Q (r, R);
   1730a:	4789                	li	a5,2
   1730c:	2efd0963          	beq	s10,a5,175fe <__multf3+0x3e4>
   17310:	478d                	li	a5,3
   17312:	3efd0363          	beq	s10,a5,176f8 <__multf3+0x4de>
   17316:	4785                	li	a5,1
   17318:	8742                	mv	a4,a6
   1731a:	28fd1263          	bne	s10,a5,1759e <__multf3+0x384>
   1731e:	4681                	li	a3,0
   17320:	4781                	li	a5,0
   17322:	4501                	li	a0,0
   17324:	073e                	slli	a4,a4,0xf
   17326:	01079593          	slli	a1,a5,0x10
   1732a:	8f55                	or	a4,a4,a3
   1732c:	1742                	slli	a4,a4,0x30
   1732e:	81c1                	srli	a1,a1,0x10
   17330:	8dd9                	or	a1,a1,a4
  FP_HANDLE_EXCEPTIONS;
   17332:	3c099063          	bnez	s3,176f2 <__multf3+0x4d8>

  return r;
}
   17336:	70a6                	ld	ra,104(sp)
   17338:	7406                	ld	s0,96(sp)
   1733a:	64e6                	ld	s1,88(sp)
   1733c:	6946                	ld	s2,80(sp)
   1733e:	69a6                	ld	s3,72(sp)
   17340:	6a06                	ld	s4,64(sp)
   17342:	7ae2                	ld	s5,56(sp)
   17344:	7b42                	ld	s6,48(sp)
   17346:	7ba2                	ld	s7,40(sp)
   17348:	7c02                	ld	s8,32(sp)
   1734a:	6ce2                	ld	s9,24(sp)
   1734c:	6d42                	ld	s10,16(sp)
   1734e:	6da2                	ld	s11,8(sp)
   17350:	6165                	addi	sp,sp,112
   17352:	8082                	ret
  FP_UNPACK_Q (A, a);
   17354:	00a464b3          	or	s1,s0,a0
   17358:	2c048c63          	beqz	s1,17630 <__multf3+0x416>
   1735c:	3c040a63          	beqz	s0,17730 <__multf3+0x516>
   17360:	8522                	mv	a0,s0
   17362:	2a4010ef          	jal	ra,18606 <__clzdi2>
   17366:	0005071b          	sext.w	a4,a0
   1736a:	3545                	addiw	a0,a0,-15
   1736c:	03d00793          	li	a5,61
   17370:	0035049b          	addiw	s1,a0,3
   17374:	40a7853b          	subw	a0,a5,a0
   17378:	00941433          	sll	s0,s0,s1
   1737c:	00a95533          	srl	a0,s2,a0
   17380:	8c49                	or	s0,s0,a0
   17382:	009914b3          	sll	s1,s2,s1
   17386:	77f1                	lui	a5,0xffffc
   17388:	07c5                	addi	a5,a5,17
   1738a:	40e78ab3          	sub	s5,a5,a4
   1738e:	4c01                	li	s8,0
   17390:	4d01                	li	s10,0
   17392:	4981                	li	s3,0
   17394:	b5ed                	j	1727e <__multf3+0x64>
   17396:	00a464b3          	or	s1,s0,a0
   1739a:	28048663          	beqz	s1,17626 <__multf3+0x40c>
   1739e:	01041793          	slli	a5,s0,0x10
   173a2:	84aa                	mv	s1,a0
   173a4:	4c31                	li	s8,12
   173a6:	4d0d                	li	s10,3
   173a8:	49c1                	li	s3,16
   173aa:	ec07dae3          	bgez	a5,1727e <__multf3+0x64>
  FP_DECL_EX;
   173ae:	4981                	li	s3,0
   173b0:	b5f9                	j	1727e <__multf3+0x64>
  FP_UNPACK_Q (B, b);
   173b2:	019967b3          	or	a5,s2,s9
   173b6:	26078263          	beqz	a5,1761a <__multf3+0x400>
   173ba:	34090663          	beqz	s2,17706 <__multf3+0x4ec>
   173be:	854a                	mv	a0,s2
   173c0:	246010ef          	jal	ra,18606 <__clzdi2>
   173c4:	0005071b          	sext.w	a4,a0
   173c8:	3545                	addiw	a0,a0,-15
   173ca:	03d00793          	li	a5,61
   173ce:	00350d9b          	addiw	s11,a0,3
   173d2:	40a7853b          	subw	a0,a5,a0
   173d6:	01b91933          	sll	s2,s2,s11
   173da:	00acd533          	srl	a0,s9,a0
   173de:	01256933          	or	s2,a0,s2
   173e2:	01bc9db3          	sll	s11,s9,s11
   173e6:	77f1                	lui	a5,0xffffc
   173e8:	07c5                	addi	a5,a5,17
   173ea:	8f99                	sub	a5,a5,a4
   173ec:	9abe                	add	s5,s5,a5
   173ee:	4601                	li	a2,0
   173f0:	bdc9                	j	172c2 <__multf3+0xa8>
  FP_MUL_Q (R, A, B);
   173f2:	57fd                	li	a5,-1
   173f4:	9381                	srli	a5,a5,0x20
   173f6:	020dd613          	srli	a2,s11,0x20
   173fa:	00f4f6b3          	and	a3,s1,a5
   173fe:	0204dc13          	srli	s8,s1,0x20
   17402:	00fdf7b3          	and	a5,s11,a5
   17406:	02d78333          	mul	t1,a5,a3
   1740a:	012d82b3          	add	t0,s11,s2
   1740e:	009403b3          	add	t2,s0,s1
   17412:	0093ba33          	sltu	s4,t2,s1
   17416:	01b2bbb3          	sltu	s7,t0,s11
   1741a:	8ed2                	mv	t4,s4
   1741c:	8dde                	mv	s11,s7
   1741e:	02fc07b3          	mul	a5,s8,a5
   17422:	02035513          	srli	a0,t1,0x20
   17426:	02d606b3          	mul	a3,a2,a3
   1742a:	96be                	add	a3,a3,a5
   1742c:	9536                	add	a0,a0,a3
   1742e:	02cc0c33          	mul	s8,s8,a2
   17432:	00f57563          	bgeu	a0,a5,1743c <__multf3+0x222>
   17436:	4785                	li	a5,1
   17438:	1782                	slli	a5,a5,0x20
   1743a:	9c3e                	add	s8,s8,a5
   1743c:	567d                	li	a2,-1
   1743e:	9201                	srli	a2,a2,0x20
   17440:	00c3f7b3          	and	a5,t2,a2
   17444:	0203df13          	srli	t5,t2,0x20
   17448:	0202d893          	srli	a7,t0,0x20
   1744c:	00c2ffb3          	and	t6,t0,a2
   17450:	00c574b3          	and	s1,a0,a2
   17454:	02ff8d33          	mul	s10,t6,a5
   17458:	00c37333          	and	t1,t1,a2
   1745c:	9101                	srli	a0,a0,0x20
   1745e:	1482                	slli	s1,s1,0x20
   17460:	9562                	add	a0,a0,s8
   17462:	9326                	add	t1,t1,s1
   17464:	03ff0633          	mul	a2,t5,t6
   17468:	020d5693          	srli	a3,s10,0x20
   1746c:	02f887b3          	mul	a5,a7,a5
   17470:	97b2                	add	a5,a5,a2
   17472:	97b6                	add	a5,a5,a3
   17474:	031f0f33          	mul	t5,t5,a7
   17478:	00c7f563          	bgeu	a5,a2,17482 <__multf3+0x268>
   1747c:	4685                	li	a3,1
   1747e:	1682                	slli	a3,a3,0x20
   17480:	9f36                	add	t5,t5,a3
   17482:	5ffd                	li	t6,-1
   17484:	020fdf93          	srli	t6,t6,0x20
   17488:	02045c93          	srli	s9,s0,0x20
   1748c:	02095613          	srli	a2,s2,0x20
   17490:	01f47433          	and	s0,s0,t6
   17494:	01f97933          	and	s2,s2,t6
   17498:	03240c33          	mul	s8,s0,s2
   1749c:	01f7fe33          	and	t3,a5,t6
   174a0:	1e02                	slli	t3,t3,0x20
   174a2:	01fd7fb3          	and	t6,s10,t6
   174a6:	9381                	srli	a5,a5,0x20
   174a8:	9e7e                	add	t3,t3,t6
   174aa:	032c8933          	mul	s2,s9,s2
   174ae:	020c5893          	srli	a7,s8,0x20
   174b2:	02860433          	mul	s0,a2,s0
   174b6:	944a                	add	s0,s0,s2
   174b8:	98a2                	add	a7,a7,s0
   174ba:	02cc8cb3          	mul	s9,s9,a2
   174be:	0128f563          	bgeu	a7,s2,174c8 <__multf3+0x2ae>
   174c2:	4685                	li	a3,1
   174c4:	1682                	slli	a3,a3,0x20
   174c6:	9cb6                	add	s9,s9,a3
   174c8:	9e2a                	add	t3,t3,a0
   174ca:	00ae3633          	sltu	a2,t3,a0
   174ce:	97b2                	add	a5,a5,a2
   174d0:	41b00db3          	neg	s11,s11
   174d4:	01e786b3          	add	a3,a5,t5
   174d8:	41d004b3          	neg	s1,t4
   174dc:	007df3b3          	and	t2,s11,t2
   174e0:	597d                	li	s2,-1
   174e2:	93b6                	add	t2,t2,a3
   174e4:	02095913          	srli	s2,s2,0x20
   174e8:	00c6b633          	sltu	a2,a3,a2
   174ec:	0054f2b3          	and	t0,s1,t0
   174f0:	017a7a33          	and	s4,s4,s7
   174f4:	929e                	add	t0,t0,t2
   174f6:	0128f7b3          	and	a5,a7,s2
   174fa:	9652                	add	a2,a2,s4
   174fc:	00d3b6b3          	sltu	a3,t2,a3
   17500:	406e0fb3          	sub	t6,t3,t1
   17504:	40a28f33          	sub	t5,t0,a0
   17508:	96b2                	add	a3,a3,a2
   1750a:	0208d893          	srli	a7,a7,0x20
   1750e:	1782                	slli	a5,a5,0x20
   17510:	012c7c33          	and	s8,s8,s2
   17514:	0072b3b3          	sltu	t2,t0,t2
   17518:	01fe3633          	sltu	a2,t3,t6
   1751c:	98e6                	add	a7,a7,s9
   1751e:	97e2                	add	a5,a5,s8
   17520:	969e                	add	a3,a3,t2
   17522:	01e2beb3          	sltu	t4,t0,t5
   17526:	40cf0633          	sub	a2,t5,a2
   1752a:	01fe7563          	bgeu	t3,t6,17534 <__multf3+0x31a>
   1752e:	00551363          	bne	a0,t0,17534 <__multf3+0x31a>
   17532:	4e85                	li	t4,1
   17534:	40ff8e33          	sub	t3,t6,a5
   17538:	41160533          	sub	a0,a2,a7
   1753c:	01cfb4b3          	sltu	s1,t6,t3
   17540:	96c6                	add	a3,a3,a7
   17542:	00a63633          	sltu	a2,a2,a0
   17546:	409784b3          	sub	s1,a5,s1
   1754a:	01cff863          	bgeu	t6,t3,1755a <__multf3+0x340>
   1754e:	00153893          	seqz	a7,a0
   17552:	01166633          	or	a2,a2,a7
   17556:	0ff67613          	andi	a2,a2,255
   1755a:	9526                	add	a0,a0,s1
   1755c:	00f537b3          	sltu	a5,a0,a5
   17560:	97b6                	add	a5,a5,a3
   17562:	00de1493          	slli	s1,t3,0xd
   17566:	41d787b3          	sub	a5,a5,t4
   1756a:	0064e4b3          	or	s1,s1,t1
   1756e:	8f91                	sub	a5,a5,a2
   17570:	033e5693          	srli	a3,t3,0x33
   17574:	07b6                	slli	a5,a5,0xd
   17576:	009034b3          	snez	s1,s1
   1757a:	03355413          	srli	s0,a0,0x33
   1757e:	8cd5                	or	s1,s1,a3
   17580:	0536                	slli	a0,a0,0xd
   17582:	00b79693          	slli	a3,a5,0xb
   17586:	8c5d                	or	s0,s0,a5
   17588:	8cc9                	or	s1,s1,a0
   1758a:	2206d863          	bgez	a3,177ba <__multf3+0x5a0>
   1758e:	0014d793          	srli	a5,s1,0x1
   17592:	8885                	andi	s1,s1,1
   17594:	03f41693          	slli	a3,s0,0x3f
   17598:	8cdd                	or	s1,s1,a5
   1759a:	8cd5                	or	s1,s1,a3
   1759c:	8005                	srli	s0,s0,0x1
  FP_PACK_Q (r, R);
   1759e:	6691                	lui	a3,0x4
   175a0:	16fd                	addi	a3,a3,-1
   175a2:	00d58633          	add	a2,a1,a3
   175a6:	08c05b63          	blez	a2,1763c <__multf3+0x422>
   175aa:	0074f793          	andi	a5,s1,7
   175ae:	cb99                	beqz	a5,175c4 <__multf3+0x3aa>
   175b0:	4789                	li	a5,2
   175b2:	0019e993          	ori	s3,s3,1
   175b6:	28fb0b63          	beq	s6,a5,1784c <__multf3+0x632>
   175ba:	478d                	li	a5,3
   175bc:	24fb0163          	beq	s6,a5,177fe <__multf3+0x5e4>
   175c0:	220b0363          	beqz	s6,177e6 <__multf3+0x5cc>
   175c4:	00b41793          	slli	a5,s0,0xb
   175c8:	0007d963          	bgez	a5,175da <__multf3+0x3c0>
   175cc:	57fd                	li	a5,-1
   175ce:	17d2                	slli	a5,a5,0x34
   175d0:	17fd                	addi	a5,a5,-1
   175d2:	6691                	lui	a3,0x4
   175d4:	8c7d                	and	s0,s0,a5
   175d6:	00d58633          	add	a2,a1,a3
   175da:	67a1                	lui	a5,0x8
   175dc:	ffe78593          	addi	a1,a5,-2 # 7ffe <exit-0x8122>
   175e0:	1ac5ce63          	blt	a1,a2,1779c <__multf3+0x582>
   175e4:	fff78693          	addi	a3,a5,-1
   175e8:	8ef1                	and	a3,a3,a2
   175ea:	808d                	srli	s1,s1,0x3
   175ec:	03d41513          	slli	a0,s0,0x3d
   175f0:	00d41793          	slli	a5,s0,0xd
   175f4:	16c2                	slli	a3,a3,0x30
   175f6:	8d45                	or	a0,a0,s1
   175f8:	83c1                	srli	a5,a5,0x10
   175fa:	92c1                	srli	a3,a3,0x30
   175fc:	b325                	j	17324 <__multf3+0x10a>
   175fe:	66a1                	lui	a3,0x8
   17600:	8742                	mv	a4,a6
   17602:	16fd                	addi	a3,a3,-1
   17604:	4781                	li	a5,0
   17606:	4501                	li	a0,0
   17608:	bb31                	j	17324 <__multf3+0x10a>
  FP_UNPACK_Q (A, a);
   1760a:	885e                	mv	a6,s7
   1760c:	b9fd                	j	1730a <__multf3+0xf0>
   1760e:	002c6c13          	ori	s8,s8,2
  FP_UNPACK_Q (B, b);
   17612:	4901                	li	s2,0
   17614:	4d81                	li	s11,0
   17616:	4609                	li	a2,2
   17618:	b16d                	j	172c2 <__multf3+0xa8>
   1761a:	001c6c13          	ori	s8,s8,1
   1761e:	4901                	li	s2,0
   17620:	4d81                	li	s11,0
   17622:	4605                	li	a2,1
   17624:	b979                	j	172c2 <__multf3+0xa8>
   17626:	4401                	li	s0,0
   17628:	4c21                	li	s8,8
  FP_UNPACK_Q (A, a);
   1762a:	4d09                	li	s10,2
  FP_DECL_EX;
   1762c:	4981                	li	s3,0
   1762e:	b981                	j	1727e <__multf3+0x64>
   17630:	4401                	li	s0,0
   17632:	4c11                	li	s8,4
   17634:	4a81                	li	s5,0
  FP_UNPACK_Q (A, a);
   17636:	4d05                	li	s10,1
  FP_DECL_EX;
   17638:	4981                	li	s3,0
   1763a:	b191                	j	1727e <__multf3+0x64>
  FP_PACK_Q (r, R);
   1763c:	4785                	li	a5,1
   1763e:	40c785b3          	sub	a1,a5,a2
   17642:	10061b63          	bnez	a2,17758 <__multf3+0x53e>
   17646:	0074f693          	andi	a3,s1,7
   1764a:	87a2                	mv	a5,s0
   1764c:	ca85                	beqz	a3,1767c <__multf3+0x462>
   1764e:	4789                	li	a5,2
   17650:	0019e993          	ori	s3,s3,1
   17654:	24fb0463          	beq	s6,a5,1789c <__multf3+0x682>
   17658:	478d                	li	a5,3
   1765a:	22fb0663          	beq	s6,a5,17886 <__multf3+0x66c>
   1765e:	87a2                	mv	a5,s0
   17660:	000b1e63          	bnez	s6,1767c <__multf3+0x462>
   17664:	00f4f693          	andi	a3,s1,15
   17668:	4611                	li	a2,4
   1766a:	00c68963          	beq	a3,a2,1767c <__multf3+0x462>
   1766e:	ffc4b793          	sltiu	a5,s1,-4
   17672:	0017c793          	xori	a5,a5,1
   17676:	1782                	slli	a5,a5,0x20
   17678:	9381                	srli	a5,a5,0x20
   1767a:	97a2                	add	a5,a5,s0
   1767c:	93d1                	srli	a5,a5,0x34
   1767e:	0017c793          	xori	a5,a5,1
   17682:	0017f893          	andi	a7,a5,1
   17686:	0005869b          	sext.w	a3,a1
   1768a:	04000613          	li	a2,64
   1768e:	9e15                	subw	a2,a2,a3
   17690:	00d4d7b3          	srl	a5,s1,a3
   17694:	00c41533          	sll	a0,s0,a2
   17698:	00c494b3          	sll	s1,s1,a2
   1769c:	8d5d                	or	a0,a0,a5
   1769e:	009034b3          	snez	s1,s1
   176a2:	8d45                	or	a0,a0,s1
   176a4:	00757793          	andi	a5,a0,7
   176a8:	00d45433          	srl	s0,s0,a3
   176ac:	c795                	beqz	a5,176d8 <__multf3+0x4be>
   176ae:	4789                	li	a5,2
   176b0:	0019e993          	ori	s3,s3,1
   176b4:	20fb0163          	beq	s6,a5,178b6 <__multf3+0x69c>
   176b8:	478d                	li	a5,3
   176ba:	1efb0563          	beq	s6,a5,178a4 <__multf3+0x68a>
   176be:	000b1d63          	bnez	s6,176d8 <__multf3+0x4be>
   176c2:	00f57793          	andi	a5,a0,15
   176c6:	4691                	li	a3,4
   176c8:	00d78863          	beq	a5,a3,176d8 <__multf3+0x4be>
   176cc:	00450793          	addi	a5,a0,4
   176d0:	00a7b533          	sltu	a0,a5,a0
   176d4:	942a                	add	s0,s0,a0
   176d6:	853e                	mv	a0,a5
   176d8:	00c41793          	slli	a5,s0,0xc
   176dc:	1807db63          	bgez	a5,17872 <__multf3+0x658>
   176e0:	0019e993          	ori	s3,s3,1
   176e4:	18089163          	bnez	a7,17866 <__multf3+0x64c>
   176e8:	4585                	li	a1,1
   176ea:	177e                	slli	a4,a4,0x3f
   176ec:	15c2                	slli	a1,a1,0x30
   176ee:	4501                	li	a0,0
   176f0:	8dd9                	or	a1,a1,a4
  FP_HANDLE_EXCEPTIONS;
   176f2:	0019a073          	csrs	fflags,s3
  return r;
   176f6:	b181                	j	17336 <__multf3+0x11c>
  FP_PACK_Q (r, R);
   176f8:	66a1                	lui	a3,0x8
   176fa:	4785                	li	a5,1
   176fc:	4701                	li	a4,0
   176fe:	16fd                	addi	a3,a3,-1
   17700:	17be                	slli	a5,a5,0x2f
   17702:	4501                	li	a0,0
   17704:	b105                	j	17324 <__multf3+0x10a>
  FP_UNPACK_Q (B, b);
   17706:	8566                	mv	a0,s9
   17708:	6ff000ef          	jal	ra,18606 <__clzdi2>
   1770c:	0005079b          	sext.w	a5,a0
   17710:	03178693          	addi	a3,a5,49
   17714:	03c00613          	li	a2,60
   17718:	04078713          	addi	a4,a5,64
   1771c:	0006851b          	sext.w	a0,a3
   17720:	cad655e3          	bge	a2,a3,173ca <__multf3+0x1b0>
   17724:	fc35051b          	addiw	a0,a0,-61
   17728:	00ac9933          	sll	s2,s9,a0
   1772c:	4d81                	li	s11,0
   1772e:	b965                	j	173e6 <__multf3+0x1cc>
  FP_UNPACK_Q (A, a);
   17730:	6d7000ef          	jal	ra,18606 <__clzdi2>
   17734:	00050a9b          	sext.w	s5,a0
   17738:	031a8793          	addi	a5,s5,49
   1773c:	03c00693          	li	a3,60
   17740:	040a8713          	addi	a4,s5,64
   17744:	0007851b          	sext.w	a0,a5
   17748:	c2f6d2e3          	bge	a3,a5,1736c <__multf3+0x152>
   1774c:	fc35051b          	addiw	a0,a0,-61
   17750:	00a91433          	sll	s0,s2,a0
   17754:	4481                	li	s1,0
   17756:	b905                	j	17386 <__multf3+0x16c>
  FP_PACK_Q (r, R);
   17758:	07400693          	li	a3,116
   1775c:	0ab6da63          	bge	a3,a1,17810 <__multf3+0x5f6>
   17760:	00946533          	or	a0,s0,s1
   17764:	cd09                	beqz	a0,1777e <__multf3+0x564>
   17766:	4689                	li	a3,2
   17768:	0019e993          	ori	s3,s3,1
   1776c:	8542                	mv	a0,a6
   1776e:	00db0863          	beq	s6,a3,1777e <__multf3+0x564>
   17772:	468d                	li	a3,3
   17774:	4501                	li	a0,0
   17776:	00db1463          	bne	s6,a3,1777e <__multf3+0x564>
   1777a:	41078533          	sub	a0,a5,a6
   1777e:	0029e993          	ori	s3,s3,2
   17782:	4681                	li	a3,0
   17784:	4781                	li	a5,0
   17786:	01079593          	slli	a1,a5,0x10
   1778a:	81c1                	srli	a1,a1,0x10
   1778c:	03069793          	slli	a5,a3,0x30
   17790:	177e                	slli	a4,a4,0x3f
   17792:	8ddd                	or	a1,a1,a5
   17794:	8dd9                	or	a1,a1,a4
  FP_HANDLE_EXCEPTIONS;
   17796:	0019a073          	csrs	fflags,s3
  return r;
   1779a:	be71                	j	17336 <__multf3+0x11c>
  FP_PACK_Q (r, R);
   1779c:	4689                	li	a3,2
   1779e:	02db0863          	beq	s6,a3,177ce <__multf3+0x5b4>
   177a2:	468d                	li	a3,3
   177a4:	02db0e63          	beq	s6,a3,177e0 <__multf3+0x5c6>
   177a8:	020b0563          	beqz	s6,177d2 <__multf3+0x5b8>
   177ac:	57fd                	li	a5,-1
   177ae:	86ae                	mv	a3,a1
   177b0:	83c1                	srli	a5,a5,0x10
   177b2:	557d                	li	a0,-1
   177b4:	0059e993          	ori	s3,s3,5
   177b8:	b7f9                	j	17786 <__multf3+0x56c>
  FP_MUL_Q (R, A, B);
   177ba:	85d6                	mv	a1,s5
   177bc:	b3cd                	j	1759e <__multf3+0x384>
  FP_PACK_Q (r, R);
   177be:	0ffff5b7          	lui	a1,0xffff
   177c2:	4501                	li	a0,0
   177c4:	158e                	slli	a1,a1,0x23
  FP_MUL_Q (R, A, B);
   177c6:	49c1                	li	s3,16
  FP_HANDLE_EXCEPTIONS;
   177c8:	0019a073          	csrs	fflags,s3
  return r;
   177cc:	b6ad                	j	17336 <__multf3+0x11c>
  FP_PACK_Q (r, R);
   177ce:	fc080fe3          	beqz	a6,177ac <__multf3+0x592>
   177d2:	fff78693          	addi	a3,a5,-1
   177d6:	4501                	li	a0,0
   177d8:	4781                	li	a5,0
   177da:	0059e993          	ori	s3,s3,5
   177de:	b765                	j	17786 <__multf3+0x56c>
   177e0:	fc0816e3          	bnez	a6,177ac <__multf3+0x592>
   177e4:	b7fd                	j	177d2 <__multf3+0x5b8>
   177e6:	00f4f793          	andi	a5,s1,15
   177ea:	4691                	li	a3,4
   177ec:	dcd78ce3          	beq	a5,a3,175c4 <__multf3+0x3aa>
   177f0:	00448793          	addi	a5,s1,4
   177f4:	0097b4b3          	sltu	s1,a5,s1
   177f8:	9426                	add	s0,s0,s1
   177fa:	84be                	mv	s1,a5
   177fc:	b3e1                	j	175c4 <__multf3+0x3aa>
   177fe:	dc0813e3          	bnez	a6,175c4 <__multf3+0x3aa>
   17802:	00848793          	addi	a5,s1,8
   17806:	0097b4b3          	sltu	s1,a5,s1
   1780a:	9426                	add	s0,s0,s1
   1780c:	84be                	mv	s1,a5
   1780e:	bb5d                	j	175c4 <__multf3+0x3aa>
   17810:	03f00793          	li	a5,63
   17814:	0005869b          	sext.w	a3,a1
   17818:	0ab7d263          	bge	a5,a1,178bc <__multf3+0x6a2>
   1781c:	fc06851b          	addiw	a0,a3,-64
   17820:	04000613          	li	a2,64
   17824:	00a457b3          	srl	a5,s0,a0
   17828:	00c58963          	beq	a1,a2,1783a <__multf3+0x620>
   1782c:	08000613          	li	a2,128
   17830:	40d606bb          	subw	a3,a2,a3
   17834:	00d41433          	sll	s0,s0,a3
   17838:	8cc1                	or	s1,s1,s0
   1783a:	00903533          	snez	a0,s1
   1783e:	8d5d                	or	a0,a0,a5
   17840:	00757793          	andi	a5,a0,7
   17844:	c799                	beqz	a5,17852 <__multf3+0x638>
   17846:	4885                	li	a7,1
   17848:	4401                	li	s0,0
   1784a:	b595                	j	176ae <__multf3+0x494>
   1784c:	d6080ce3          	beqz	a6,175c4 <__multf3+0x3aa>
   17850:	bf4d                	j	17802 <__multf3+0x5e8>
   17852:	810d                	srli	a0,a0,0x3
   17854:	4781                	li	a5,0
   17856:	0019f613          	andi	a2,s3,1
   1785a:	4681                	li	a3,0
   1785c:	ac0604e3          	beqz	a2,17324 <__multf3+0x10a>
   17860:	0029e993          	ori	s3,s3,2
   17864:	b70d                	j	17786 <__multf3+0x56c>
   17866:	4685                	li	a3,1
   17868:	4781                	li	a5,0
   1786a:	4501                	li	a0,0
   1786c:	0029e993          	ori	s3,s3,2
   17870:	bf19                	j	17786 <__multf3+0x56c>
   17872:	00d41793          	slli	a5,s0,0xd
   17876:	810d                	srli	a0,a0,0x3
   17878:	1476                	slli	s0,s0,0x3d
   1787a:	83c1                	srli	a5,a5,0x10
   1787c:	8d41                	or	a0,a0,s0
   1787e:	fc089ce3          	bnez	a7,17856 <__multf3+0x63c>
   17882:	4681                	li	a3,0
   17884:	b445                	j	17324 <__multf3+0x10a>
   17886:	87a2                	mv	a5,s0
   17888:	de081ae3          	bnez	a6,1767c <__multf3+0x462>
   1788c:	ff84b793          	sltiu	a5,s1,-8
   17890:	0017c793          	xori	a5,a5,1
   17894:	1782                	slli	a5,a5,0x20
   17896:	9381                	srli	a5,a5,0x20
   17898:	97a2                	add	a5,a5,s0
   1789a:	b3cd                	j	1767c <__multf3+0x462>
   1789c:	87a2                	mv	a5,s0
   1789e:	dc080fe3          	beqz	a6,1767c <__multf3+0x462>
   178a2:	b7ed                	j	1788c <__multf3+0x672>
   178a4:	e2081ae3          	bnez	a6,176d8 <__multf3+0x4be>
   178a8:	00850793          	addi	a5,a0,8
   178ac:	00a7b533          	sltu	a0,a5,a0
   178b0:	942a                	add	s0,s0,a0
   178b2:	853e                	mv	a0,a5
   178b4:	b515                	j	176d8 <__multf3+0x4be>
   178b6:	e20801e3          	beqz	a6,176d8 <__multf3+0x4be>
   178ba:	b7fd                	j	178a8 <__multf3+0x68e>
   178bc:	4885                	li	a7,1
   178be:	b3f1                	j	1768a <__multf3+0x470>

00000000000178c0 <__subtf3>:
#include "soft-fp.h"
#include "quad.h"

TFtype
__subtf3 (TFtype a, TFtype b)
{
   178c0:	7179                	addi	sp,sp,-48
   178c2:	f406                	sd	ra,40(sp)
   178c4:	f022                	sd	s0,32(sp)
   178c6:	ec26                	sd	s1,24(sp)
   178c8:	e84a                	sd	s2,16(sp)
   178ca:	e44e                	sd	s3,8(sp)
   178cc:	e052                	sd	s4,0(sp)
   178ce:	8faa                	mv	t6,a0
  FP_DECL_Q (A);
  FP_DECL_Q (B);
  FP_DECL_Q (R);
  TFtype r;

  FP_INIT_ROUNDMODE;
   178d0:	002029f3          	frrm	s3
  FP_UNPACK_SEMIRAW_Q (A, a);
   178d4:	6821                	lui	a6,0x8
   178d6:	57fd                	li	a5,-1
   178d8:	187d                	addi	a6,a6,-1
   178da:	83c1                	srli	a5,a5,0x10
   178dc:	0305d413          	srli	s0,a1,0x30
  FP_UNPACK_SEMIRAW_Q (B, b);
   178e0:	0306d313          	srli	t1,a3,0x30
  FP_UNPACK_SEMIRAW_Q (A, a);
   178e4:	00f5feb3          	and	t4,a1,a5
   178e8:	01047433          	and	s0,s0,a6
  FP_UNPACK_SEMIRAW_Q (B, b);
   178ec:	8ff5                	and	a5,a5,a3
   178ee:	01037333          	and	t1,t1,a6
  FP_UNPACK_SEMIRAW_Q (A, a);
   178f2:	0e8e                	slli	t4,t4,0x3
   178f4:	03d55713          	srli	a4,a0,0x3d
  FP_UNPACK_SEMIRAW_Q (B, b);
   178f8:	03d65893          	srli	a7,a2,0x3d
   178fc:	078e                	slli	a5,a5,0x3
   178fe:	40640f3b          	subw	t5,s0,t1
  FP_UNPACK_SEMIRAW_Q (A, a);
   17902:	03f5d913          	srli	s2,a1,0x3f
  FP_INIT_ROUNDMODE;
   17906:	2981                	sext.w	s3,s3
  FP_UNPACK_SEMIRAW_Q (A, a);
   17908:	01d765b3          	or	a1,a4,t4
   1790c:	050e                	slli	a0,a0,0x3
  FP_UNPACK_SEMIRAW_Q (B, b);
   1790e:	00f8eeb3          	or	t4,a7,a5
   17912:	92fd                	srli	a3,a3,0x3f
   17914:	00361e13          	slli	t3,a2,0x3
  FP_SUB_Q (R, A, B);
   17918:	000f089b          	sext.w	a7,t5
   1791c:	1b030563          	beq	t1,a6,17ac6 <__subtf3+0x206>
   17920:	0016c693          	xori	a3,a3,1
   17924:	1cd90a63          	beq	s2,a3,17af8 <__subtf3+0x238>
   17928:	17105c63          	blez	a7,17aa0 <__subtf3+0x1e0>
   1792c:	1a031563          	bnez	t1,17ad6 <__subtf3+0x216>
   17930:	01cee7b3          	or	a5,t4,t3
   17934:	42078063          	beqz	a5,17d54 <__subtf3+0x494>
   17938:	ffff089b          	addiw	a7,t5,-1
   1793c:	6a088e63          	beqz	a7,17ff8 <__subtf3+0x738>
   17940:	67a1                	lui	a5,0x8
   17942:	17fd                	addi	a5,a5,-1
   17944:	74f40863          	beq	s0,a5,18094 <__subtf3+0x7d4>
   17948:	07400793          	li	a5,116
   1794c:	1b17c163          	blt	a5,a7,17aee <__subtf3+0x22e>
   17950:	03f00793          	li	a5,63
   17954:	5717c963          	blt	a5,a7,17ec6 <__subtf3+0x606>
   17958:	04000793          	li	a5,64
   1795c:	411787bb          	subw	a5,a5,a7
   17960:	011e5733          	srl	a4,t3,a7
   17964:	00fe94b3          	sll	s1,t4,a5
   17968:	00fe1e33          	sll	t3,t3,a5
   1796c:	8cd9                	or	s1,s1,a4
   1796e:	01c03e33          	snez	t3,t3
   17972:	011ed8b3          	srl	a7,t4,a7
   17976:	01c4e4b3          	or	s1,s1,t3
   1797a:	411585b3          	sub	a1,a1,a7
   1797e:	409504b3          	sub	s1,a0,s1
   17982:	00953533          	sltu	a0,a0,s1
   17986:	8d89                	sub	a1,a1,a0
   17988:	00c59793          	slli	a5,a1,0xc
   1798c:	2807db63          	bgez	a5,17c22 <__subtf3+0x362>
   17990:	00d59713          	slli	a4,a1,0xd
   17994:	00d75a13          	srli	s4,a4,0xd
   17998:	380a0463          	beqz	s4,17d20 <__subtf3+0x460>
   1799c:	8552                	mv	a0,s4
   1799e:	469000ef          	jal	ra,18606 <__clzdi2>
   179a2:	3551                	addiw	a0,a0,-12
   179a4:	04000713          	li	a4,64
   179a8:	9f09                	subw	a4,a4,a0
   179aa:	00aa15b3          	sll	a1,s4,a0
   179ae:	00e4d733          	srl	a4,s1,a4
   179b2:	8f4d                	or	a4,a4,a1
   179b4:	00a49a33          	sll	s4,s1,a0
   179b8:	34854d63          	blt	a0,s0,17d12 <__subtf3+0x452>
   179bc:	9d01                	subw	a0,a0,s0
   179be:	0015059b          	addiw	a1,a0,1
   179c2:	03f00793          	li	a5,63
   179c6:	4cb7ca63          	blt	a5,a1,17e9a <__subtf3+0x5da>
   179ca:	04000793          	li	a5,64
   179ce:	9f8d                	subw	a5,a5,a1
   179d0:	00f714b3          	sll	s1,a4,a5
   179d4:	00ba56b3          	srl	a3,s4,a1
   179d8:	00fa17b3          	sll	a5,s4,a5
   179dc:	8cd5                	or	s1,s1,a3
   179de:	00f037b3          	snez	a5,a5
   179e2:	8cdd                	or	s1,s1,a5
   179e4:	00b755b3          	srl	a1,a4,a1
  FP_PACK_SEMIRAW_Q (r, R);
   179e8:	00b4e7b3          	or	a5,s1,a1
   179ec:	1a078963          	beqz	a5,17b9e <__subtf3+0x2de>
   179f0:	0074f793          	andi	a5,s1,7
   179f4:	00149613          	slli	a2,s1,0x1
   179f8:	00159713          	slli	a4,a1,0x1
   179fc:	03f4d693          	srli	a3,s1,0x3f
   17a00:	00767513          	andi	a0,a2,7
   17a04:	96ba                	add	a3,a3,a4
   17a06:	4881                	li	a7,0
   17a08:	c911                	beqz	a0,17a1c <__subtf3+0x15c>
   17a0a:	4709                	li	a4,2
   17a0c:	66e98063          	beq	s3,a4,1806c <__subtf3+0x7ac>
   17a10:	470d                	li	a4,3
   17a12:	4885                	li	a7,1
   17a14:	64e98263          	beq	s3,a4,18058 <__subtf3+0x798>
   17a18:	62098363          	beqz	s3,1803e <__subtf3+0x77e>
   17a1c:	4705                	li	a4,1
   17a1e:	03471613          	slli	a2,a4,0x34
   17a22:	8ef1                	and	a3,a3,a2
   17a24:	0016b613          	seqz	a2,a3
   17a28:	4401                	li	s0,0
   17a2a:	68078763          	beqz	a5,180b8 <__subtf3+0x7f8>
   17a2e:	4789                	li	a5,2
   17a30:	2af98863          	beq	s3,a5,17ce0 <__subtf3+0x420>
   17a34:	478d                	li	a5,3
   17a36:	2cf98463          	beq	s3,a5,17cfe <__subtf3+0x43e>
   17a3a:	26098f63          	beqz	s3,17cb8 <__subtf3+0x3f8>
   17a3e:	4705                	li	a4,1
   17a40:	174e                	slli	a4,a4,0x33
   17a42:	8f6d                	and	a4,a4,a1
   17a44:	4885                	li	a7,1
   17a46:	c219                	beqz	a2,17a4c <__subtf3+0x18c>
   17a48:	0028e893          	ori	a7,a7,2
   17a4c:	18070e63          	beqz	a4,17be8 <__subtf3+0x328>
   17a50:	66a1                	lui	a3,0x8
   17a52:	0405                	addi	s0,s0,1
   17a54:	16fd                	addi	a3,a3,-1
   17a56:	1ed40463          	beq	s0,a3,17c3e <__subtf3+0x37e>
   17a5a:	567d                	li	a2,-1
   17a5c:	03361713          	slli	a4,a2,0x33
   17a60:	177d                	addi	a4,a4,-1
   17a62:	8f6d                	and	a4,a4,a1
   17a64:	8c75                	and	s0,s0,a3
   17a66:	03d71513          	slli	a0,a4,0x3d
   17a6a:	808d                	srli	s1,s1,0x3
   17a6c:	830d                	srli	a4,a4,0x3
   17a6e:	8241                	srli	a2,a2,0x10
   17a70:	1442                	slli	s0,s0,0x30
   17a72:	8d45                	or	a0,a0,s1
   17a74:	8f71                	and	a4,a4,a2
   17a76:	9041                	srli	s0,s0,0x30
   17a78:	00197693          	andi	a3,s2,1
   17a7c:	06be                	slli	a3,a3,0xf
   17a7e:	01071593          	slli	a1,a4,0x10
   17a82:	0086e733          	or	a4,a3,s0
   17a86:	1742                	slli	a4,a4,0x30
   17a88:	81c1                	srli	a1,a1,0x10
   17a8a:	8dd9                	or	a1,a1,a4
  FP_HANDLE_EXCEPTIONS;
   17a8c:	2e089a63          	bnez	a7,17d80 <__subtf3+0x4c0>

  return r;
}
   17a90:	70a2                	ld	ra,40(sp)
   17a92:	7402                	ld	s0,32(sp)
   17a94:	64e2                	ld	s1,24(sp)
   17a96:	6942                	ld	s2,16(sp)
   17a98:	69a2                	ld	s3,8(sp)
   17a9a:	6a02                	ld	s4,0(sp)
   17a9c:	6145                	addi	sp,sp,48
   17a9e:	8082                	ret
  FP_SUB_Q (R, A, B);
   17aa0:	0a088463          	beqz	a7,17b48 <__subtf3+0x288>
   17aa4:	32041f63          	bnez	s0,17de2 <__subtf3+0x522>
   17aa8:	00a5e7b3          	or	a5,a1,a0
   17aac:	16078363          	beqz	a5,17c12 <__subtf3+0x352>
   17ab0:	577d                	li	a4,-1
   17ab2:	72e88663          	beq	a7,a4,181de <__subtf3+0x91e>
   17ab6:	67a1                	lui	a5,0x8
   17ab8:	17fd                	addi	a5,a5,-1
   17aba:	6af30b63          	beq	t1,a5,18170 <__subtf3+0x8b0>
   17abe:	ffff4f13          	not	t5,t5
   17ac2:	2f01                	sext.w	t5,t5
   17ac4:	ae05                	j	17df4 <__subtf3+0x534>
   17ac6:	01cee7b3          	or	a5,t4,t3
   17aca:	e4078be3          	beqz	a5,17920 <__subtf3+0x60>
   17ace:	18d90863          	beq	s2,a3,17c5e <__subtf3+0x39e>
   17ad2:	fd1057e3          	blez	a7,17aa0 <__subtf3+0x1e0>
   17ad6:	67a1                	lui	a5,0x8
   17ad8:	17fd                	addi	a5,a5,-1
   17ada:	52f40863          	beq	s0,a5,1800a <__subtf3+0x74a>
   17ade:	4785                	li	a5,1
   17ae0:	17ce                	slli	a5,a5,0x33
   17ae2:	00feeeb3          	or	t4,t4,a5
   17ae6:	07400793          	li	a5,116
   17aea:	e717d3e3          	bge	a5,a7,17950 <__subtf3+0x90>
   17aee:	01cee4b3          	or	s1,t4,t3
   17af2:	009034b3          	snez	s1,s1
   17af6:	b561                	j	1797e <__subtf3+0xbe>
   17af8:	17105363          	blez	a7,17c5e <__subtf3+0x39e>
   17afc:	0a030c63          	beqz	t1,17bb4 <__subtf3+0x2f4>
   17b00:	67a1                	lui	a5,0x8
   17b02:	17fd                	addi	a5,a5,-1
   17b04:	5cf40663          	beq	s0,a5,180d0 <__subtf3+0x810>
   17b08:	4785                	li	a5,1
   17b0a:	17ce                	slli	a5,a5,0x33
   17b0c:	00feeeb3          	or	t4,t4,a5
   17b10:	07400793          	li	a5,116
   17b14:	3517c263          	blt	a5,a7,17e58 <__subtf3+0x598>
   17b18:	03f00793          	li	a5,63
   17b1c:	4117d463          	bge	a5,a7,17f24 <__subtf3+0x664>
   17b20:	fc08849b          	addiw	s1,a7,-64
   17b24:	04000793          	li	a5,64
   17b28:	009ed733          	srl	a4,t4,s1
   17b2c:	00f88a63          	beq	a7,a5,17b40 <__subtf3+0x280>
   17b30:	08000793          	li	a5,128
   17b34:	411788bb          	subw	a7,a5,a7
   17b38:	011e98b3          	sll	a7,t4,a7
   17b3c:	011e6e33          	or	t3,t3,a7
   17b40:	01c034b3          	snez	s1,t3
   17b44:	8cd9                	or	s1,s1,a4
   17b46:	ae29                	j	17e60 <__subtf3+0x5a0>
   17b48:	6721                	lui	a4,0x8
   17b4a:	00140793          	addi	a5,s0,1
   17b4e:	ffe70813          	addi	a6,a4,-2 # 7ffe <exit-0x8122>
   17b52:	0107f7b3          	and	a5,a5,a6
   17b56:	2c079a63          	bnez	a5,17e2a <__subtf3+0x56a>
   17b5a:	00a5e833          	or	a6,a1,a0
   17b5e:	01cee7b3          	or	a5,t4,t3
   17b62:	40041563          	bnez	s0,17f6c <__subtf3+0x6ac>
   17b66:	4c080463          	beqz	a6,1802e <__subtf3+0x76e>
   17b6a:	4c078863          	beqz	a5,1803a <__subtf3+0x77a>
   17b6e:	41c50633          	sub	a2,a0,t3
   17b72:	00c537b3          	sltu	a5,a0,a2
   17b76:	41d58733          	sub	a4,a1,t4
   17b7a:	8f1d                	sub	a4,a4,a5
   17b7c:	00c71793          	slli	a5,a4,0xc
   17b80:	6e07d263          	bgez	a5,18264 <__subtf3+0x9a4>
   17b84:	40ae04b3          	sub	s1,t3,a0
   17b88:	40be8733          	sub	a4,t4,a1
   17b8c:	009e3e33          	sltu	t3,t3,s1
   17b90:	41c705b3          	sub	a1,a4,t3
  FP_PACK_SEMIRAW_Q (r, R);
   17b94:	00b4e7b3          	or	a5,s1,a1
  FP_SUB_Q (R, A, B);
   17b98:	8936                	mv	s2,a3
  FP_PACK_SEMIRAW_Q (r, R);
   17b9a:	e4079be3          	bnez	a5,179f0 <__subtf3+0x130>
   17b9e:	4501                	li	a0,0
   17ba0:	4401                	li	s0,0
   17ba2:	4881                	li	a7,0
   17ba4:	01079713          	slli	a4,a5,0x10
   17ba8:	1446                	slli	s0,s0,0x31
   17baa:	8341                	srli	a4,a4,0x10
   17bac:	9045                	srli	s0,s0,0x31
   17bae:	00197693          	andi	a3,s2,1
   17bb2:	b5e9                	j	17a7c <__subtf3+0x1bc>
  FP_SUB_Q (R, A, B);
   17bb4:	01cee7b3          	or	a5,t4,t3
   17bb8:	18078e63          	beqz	a5,17d54 <__subtf3+0x494>
   17bbc:	ffff089b          	addiw	a7,t5,-1
   17bc0:	3e088263          	beqz	a7,17fa4 <__subtf3+0x6e4>
   17bc4:	67a1                	lui	a5,0x8
   17bc6:	17fd                	addi	a5,a5,-1
   17bc8:	f4f414e3          	bne	s0,a5,17b10 <__subtf3+0x250>
   17bcc:	00a5e7b3          	or	a5,a1,a0
   17bd0:	42078263          	beqz	a5,17ff4 <__subtf3+0x734>
   17bd4:	0325d893          	srli	a7,a1,0x32
   17bd8:	0018f893          	andi	a7,a7,1
  FP_DECL_EX;
   17bdc:	0018b893          	seqz	a7,a7
   17be0:	0892                	slli	a7,a7,0x4
  FP_PACK_SEMIRAW_Q (r, R);
   17be2:	6421                	lui	s0,0x8
   17be4:	84aa                	mv	s1,a0
   17be6:	147d                	addi	s0,s0,-1
   17be8:	6721                	lui	a4,0x8
   17bea:	03d59513          	slli	a0,a1,0x3d
   17bee:	808d                	srli	s1,s1,0x3
   17bf0:	177d                	addi	a4,a4,-1
   17bf2:	8d45                	or	a0,a0,s1
   17bf4:	0035d793          	srli	a5,a1,0x3
   17bf8:	fae416e3          	bne	s0,a4,17ba4 <__subtf3+0x2e4>
   17bfc:	8d5d                	or	a0,a0,a5
   17bfe:	14050363          	beqz	a0,17d44 <__subtf3+0x484>
   17c02:	6721                	lui	a4,0x8
   17c04:	fff70413          	addi	s0,a4,-1 # 7fff <exit-0x8121>
   17c08:	4705                	li	a4,1
   17c0a:	4681                	li	a3,0
   17c0c:	173e                	slli	a4,a4,0x2f
   17c0e:	4501                	li	a0,0
   17c10:	b5b5                	j	17a7c <__subtf3+0x1bc>
  FP_SUB_Q (R, A, B);
   17c12:	67a1                	lui	a5,0x8
   17c14:	17fd                	addi	a5,a5,-1
   17c16:	5ef30f63          	beq	t1,a5,18214 <__subtf3+0x954>
  FP_UNPACK_SEMIRAW_Q (B, b);
   17c1a:	85f6                	mv	a1,t4
   17c1c:	84f2                	mv	s1,t3
   17c1e:	841a                	mv	s0,t1
   17c20:	8936                	mv	s2,a3
  FP_PACK_SEMIRAW_Q (r, R);
   17c22:	00b4e7b3          	or	a5,s1,a1
   17c26:	dc0403e3          	beqz	s0,179ec <__subtf3+0x12c>
   17c2a:	0074f793          	andi	a5,s1,7
   17c2e:	4601                	li	a2,0
   17c30:	de079fe3          	bnez	a5,17a2e <__subtf3+0x16e>
   17c34:	4705                	li	a4,1
   17c36:	174e                	slli	a4,a4,0x33
   17c38:	8f6d                	and	a4,a4,a1
   17c3a:	4881                	li	a7,0
   17c3c:	bd01                	j	17a4c <__subtf3+0x18c>
   17c3e:	10098163          	beqz	s3,17d40 <__subtf3+0x480>
   17c42:	478d                	li	a5,3
   17c44:	0ef98c63          	beq	s3,a5,17d3c <__subtf3+0x47c>
   17c48:	4789                	li	a5,2
   17c4a:	36f98463          	beq	s3,a5,17fb2 <__subtf3+0x6f2>
   17c4e:	557d                	li	a0,-1
   17c50:	6421                	lui	s0,0x8
   17c52:	0058e893          	ori	a7,a7,5
   17c56:	00355793          	srli	a5,a0,0x3
   17c5a:	1479                	addi	s0,s0,-2
   17c5c:	b7a1                	j	17ba4 <__subtf3+0x2e4>
  FP_SUB_Q (R, A, B);
   17c5e:	12088b63          	beqz	a7,17d94 <__subtf3+0x4d4>
   17c62:	2a040263          	beqz	s0,17f06 <__subtf3+0x646>
   17c66:	67a1                	lui	a5,0x8
   17c68:	17fd                	addi	a5,a5,-1
   17c6a:	60f30763          	beq	t1,a5,18278 <__subtf3+0x9b8>
   17c6e:	4785                	li	a5,1
   17c70:	17ce                	slli	a5,a5,0x33
   17c72:	41e00f3b          	negw	t5,t5
   17c76:	8ddd                	or	a1,a1,a5
   17c78:	07400793          	li	a5,116
   17c7c:	47e7cc63          	blt	a5,t5,180f4 <__subtf3+0x834>
   17c80:	03f00793          	li	a5,63
   17c84:	53e7ca63          	blt	a5,t5,181b8 <__subtf3+0x8f8>
   17c88:	04000793          	li	a5,64
   17c8c:	41e787bb          	subw	a5,a5,t5
   17c90:	00f594b3          	sll	s1,a1,a5
   17c94:	01e55733          	srl	a4,a0,t5
   17c98:	00f517b3          	sll	a5,a0,a5
   17c9c:	8cd9                	or	s1,s1,a4
   17c9e:	00f037b3          	snez	a5,a5
   17ca2:	01e5df33          	srl	t5,a1,t5
   17ca6:	8cdd                	or	s1,s1,a5
   17ca8:	9efa                	add	t4,t4,t5
   17caa:	94f2                	add	s1,s1,t3
   17cac:	01c4b733          	sltu	a4,s1,t3
   17cb0:	01d705b3          	add	a1,a4,t4
  FP_UNPACK_SEMIRAW_Q (B, b);
   17cb4:	841a                	mv	s0,t1
   17cb6:	aa4d                	j	17e68 <__subtf3+0x5a8>
  FP_PACK_SEMIRAW_Q (r, R);
   17cb8:	00f4f793          	andi	a5,s1,15
   17cbc:	4711                	li	a4,4
   17cbe:	d8e780e3          	beq	a5,a4,17a3e <__subtf3+0x17e>
   17cc2:	ffc4b793          	sltiu	a5,s1,-4
   17cc6:	0017c793          	xori	a5,a5,1
   17cca:	1782                	slli	a5,a5,0x20
   17ccc:	9381                	srli	a5,a5,0x20
   17cce:	4705                	li	a4,1
   17cd0:	95be                	add	a1,a1,a5
   17cd2:	174e                	slli	a4,a4,0x33
   17cd4:	8f6d                	and	a4,a4,a1
   17cd6:	0491                	addi	s1,s1,4
   17cd8:	4885                	li	a7,1
   17cda:	d60617e3          	bnez	a2,17a48 <__subtf3+0x188>
   17cde:	b3bd                	j	17a4c <__subtf3+0x18c>
   17ce0:	d4090fe3          	beqz	s2,17a3e <__subtf3+0x17e>
   17ce4:	00848793          	addi	a5,s1,8
   17ce8:	0097b4b3          	sltu	s1,a5,s1
   17cec:	4705                	li	a4,1
   17cee:	95a6                	add	a1,a1,s1
   17cf0:	174e                	slli	a4,a4,0x33
   17cf2:	8f6d                	and	a4,a4,a1
   17cf4:	84be                	mv	s1,a5
   17cf6:	4885                	li	a7,1
   17cf8:	d40618e3          	bnez	a2,17a48 <__subtf3+0x188>
   17cfc:	bb81                	j	17a4c <__subtf3+0x18c>
   17cfe:	fe0903e3          	beqz	s2,17ce4 <__subtf3+0x424>
   17d02:	4885                	li	a7,1
   17d04:	4705                	li	a4,1
   17d06:	174e                	slli	a4,a4,0x33
   17d08:	8f6d                	and	a4,a4,a1
  FP_SUB_Q (R, A, B);
   17d0a:	4905                	li	s2,1
  FP_PACK_SEMIRAW_Q (r, R);
   17d0c:	d2061ee3          	bnez	a2,17a48 <__subtf3+0x188>
   17d10:	bb35                	j	17a4c <__subtf3+0x18c>
  FP_SUB_Q (R, A, B);
   17d12:	55fd                	li	a1,-1
   17d14:	15ce                	slli	a1,a1,0x33
   17d16:	15fd                	addi	a1,a1,-1
   17d18:	8c09                	sub	s0,s0,a0
   17d1a:	8df9                	and	a1,a1,a4
   17d1c:	84d2                	mv	s1,s4
   17d1e:	b711                	j	17c22 <__subtf3+0x362>
   17d20:	8526                	mv	a0,s1
   17d22:	0e5000ef          	jal	ra,18606 <__clzdi2>
   17d26:	ff45071b          	addiw	a4,a0,-12
   17d2a:	03f00793          	li	a5,63
   17d2e:	0345051b          	addiw	a0,a0,52
   17d32:	00e49733          	sll	a4,s1,a4
   17d36:	c8a7c1e3          	blt	a5,a0,179b8 <__subtf3+0xf8>
   17d3a:	b1ad                	j	179a4 <__subtf3+0xe4>
  FP_PACK_SEMIRAW_Q (r, R);
   17d3c:	f00919e3          	bnez	s2,17c4e <__subtf3+0x38e>
   17d40:	0058e893          	ori	a7,a7,5
  FP_SUB_Q (R, A, B);
   17d44:	6721                	lui	a4,0x8
   17d46:	fff70413          	addi	s0,a4,-1 # 7fff <exit-0x8121>
   17d4a:	00197693          	andi	a3,s2,1
   17d4e:	4701                	li	a4,0
   17d50:	4501                	li	a0,0
   17d52:	b32d                	j	17a7c <__subtf3+0x1bc>
   17d54:	67a1                	lui	a5,0x8
   17d56:	17fd                	addi	a5,a5,-1
  FP_UNPACK_SEMIRAW_Q (A, a);
   17d58:	84aa                	mv	s1,a0
  FP_SUB_Q (R, A, B);
   17d5a:	ecf414e3          	bne	s0,a5,17c22 <__subtf3+0x362>
   17d5e:	00a5e7b3          	or	a5,a1,a0
   17d62:	e60799e3          	bnez	a5,17bd4 <__subtf3+0x314>
  FP_DECL_EX;
   17d66:	4501                	li	a0,0
   17d68:	4781                	li	a5,0
   17d6a:	4881                	li	a7,0
   17d6c:	bd41                	j	17bfc <__subtf3+0x33c>
  FP_SUB_Q (R, A, B);
   17d6e:	26830063          	beq	t1,s0,17fce <__subtf3+0x70e>
   17d72:	26079663          	bnez	a5,17fde <__subtf3+0x71e>
  FP_PACK_SEMIRAW_Q (r, R);
   17d76:	0ffff5b7          	lui	a1,0xffff
   17d7a:	4501                	li	a0,0
   17d7c:	158e                	slli	a1,a1,0x23
  FP_SUB_Q (R, A, B);
   17d7e:	48c1                	li	a7,16
  FP_HANDLE_EXCEPTIONS;
   17d80:	0018a073          	csrs	fflags,a7
}
   17d84:	70a2                	ld	ra,40(sp)
   17d86:	7402                	ld	s0,32(sp)
   17d88:	64e2                	ld	s1,24(sp)
   17d8a:	6942                	ld	s2,16(sp)
   17d8c:	69a2                	ld	s3,8(sp)
   17d8e:	6a02                	ld	s4,0(sp)
   17d90:	6145                	addi	sp,sp,48
   17d92:	8082                	ret
  FP_SUB_Q (R, A, B);
   17d94:	67a1                	lui	a5,0x8
   17d96:	00140713          	addi	a4,s0,1 # 8001 <exit-0x811f>
   17d9a:	ffe78693          	addi	a3,a5,-2 # 7ffe <exit-0x8122>
   17d9e:	00d77833          	and	a6,a4,a3
   17da2:	1a081463          	bnez	a6,17f4a <__subtf3+0x68a>
   17da6:	00a5e733          	or	a4,a1,a0
   17daa:	36041c63          	bnez	s0,18122 <__subtf3+0x862>
   17dae:	01cee7b3          	or	a5,t4,t3
   17db2:	40070063          	beqz	a4,181b2 <__subtf3+0x8f2>
   17db6:	28078263          	beqz	a5,1803a <__subtf3+0x77a>
   17dba:	01c504b3          	add	s1,a0,t3
   17dbe:	01d58733          	add	a4,a1,t4
   17dc2:	00a4b533          	sltu	a0,s1,a0
   17dc6:	00a705b3          	add	a1,a4,a0
   17dca:	00c59793          	slli	a5,a1,0xc
   17dce:	c007dde3          	bgez	a5,179e8 <__subtf3+0x128>
   17dd2:	57fd                	li	a5,-1
   17dd4:	17ce                	slli	a5,a5,0x33
   17dd6:	17fd                	addi	a5,a5,-1
   17dd8:	8dfd                	and	a1,a1,a5
   17dda:	4405                	li	s0,1
   17ddc:	0074f793          	andi	a5,s1,7
   17de0:	b5b9                	j	17c2e <__subtf3+0x36e>
   17de2:	67a1                	lui	a5,0x8
   17de4:	17fd                	addi	a5,a5,-1
   17de6:	36f30263          	beq	t1,a5,1814a <__subtf3+0x88a>
   17dea:	4785                	li	a5,1
   17dec:	17ce                	slli	a5,a5,0x33
   17dee:	41e00f3b          	negw	t5,t5
   17df2:	8ddd                	or	a1,a1,a5
   17df4:	07400793          	li	a5,116
   17df8:	0fe7cb63          	blt	a5,t5,17eee <__subtf3+0x62e>
   17dfc:	03f00793          	li	a5,63
   17e00:	2fe7ce63          	blt	a5,t5,180fc <__subtf3+0x83c>
   17e04:	04000793          	li	a5,64
   17e08:	41e787bb          	subw	a5,a5,t5
   17e0c:	01e55733          	srl	a4,a0,t5
   17e10:	00f594b3          	sll	s1,a1,a5
   17e14:	00f51533          	sll	a0,a0,a5
   17e18:	8cd9                	or	s1,s1,a4
   17e1a:	00a03533          	snez	a0,a0
   17e1e:	01e5df33          	srl	t5,a1,t5
   17e22:	8cc9                	or	s1,s1,a0
   17e24:	41ee8eb3          	sub	t4,t4,t5
   17e28:	a0f1                	j	17ef4 <__subtf3+0x634>
   17e2a:	41c504b3          	sub	s1,a0,t3
   17e2e:	41d58a33          	sub	s4,a1,t4
   17e32:	00953733          	sltu	a4,a0,s1
   17e36:	40ea0a33          	sub	s4,s4,a4
   17e3a:	00ca1793          	slli	a5,s4,0xc
   17e3e:	1407c963          	bltz	a5,17f90 <__subtf3+0x6d0>
   17e42:	0144e7b3          	or	a5,s1,s4
   17e46:	b40799e3          	bnez	a5,17998 <__subtf3+0xd8>
   17e4a:	ffe98693          	addi	a3,s3,-2
   17e4e:	0016b913          	seqz	s2,a3
   17e52:	4501                	li	a0,0
   17e54:	4401                	li	s0,0
   17e56:	b3b9                	j	17ba4 <__subtf3+0x2e4>
   17e58:	01cee4b3          	or	s1,t4,t3
   17e5c:	009034b3          	snez	s1,s1
   17e60:	94aa                	add	s1,s1,a0
   17e62:	00a4b533          	sltu	a0,s1,a0
   17e66:	95aa                	add	a1,a1,a0
   17e68:	00c59793          	slli	a5,a1,0xc
   17e6c:	da07dbe3          	bgez	a5,17c22 <__subtf3+0x362>
   17e70:	67a1                	lui	a5,0x8
   17e72:	0405                	addi	s0,s0,1
   17e74:	fff78713          	addi	a4,a5,-1 # 7fff <exit-0x8121>
   17e78:	1ee40e63          	beq	s0,a4,18074 <__subtf3+0x7b4>
   17e7c:	577d                	li	a4,-1
   17e7e:	174e                	slli	a4,a4,0x33
   17e80:	177d                	addi	a4,a4,-1
   17e82:	8f6d                	and	a4,a4,a1
   17e84:	0014d793          	srli	a5,s1,0x1
   17e88:	8885                	andi	s1,s1,1
   17e8a:	8fc5                	or	a5,a5,s1
   17e8c:	03f71493          	slli	s1,a4,0x3f
   17e90:	8cdd                	or	s1,s1,a5
   17e92:	00175593          	srli	a1,a4,0x1
   17e96:	8b9d                	andi	a5,a5,7
   17e98:	bb59                	j	17c2e <__subtf3+0x36e>
   17e9a:	fc15051b          	addiw	a0,a0,-63
   17e9e:	04000793          	li	a5,64
   17ea2:	00a75533          	srl	a0,a4,a0
   17ea6:	00f58a63          	beq	a1,a5,17eba <__subtf3+0x5fa>
   17eaa:	08000793          	li	a5,128
   17eae:	40b786bb          	subw	a3,a5,a1
   17eb2:	00d716b3          	sll	a3,a4,a3
   17eb6:	00da6a33          	or	s4,s4,a3
   17eba:	014037b3          	snez	a5,s4
   17ebe:	8fc9                	or	a5,a5,a0
   17ec0:	84be                	mv	s1,a5
   17ec2:	4581                	li	a1,0
   17ec4:	b625                	j	179ec <__subtf3+0x12c>
   17ec6:	fc08849b          	addiw	s1,a7,-64
   17eca:	04000793          	li	a5,64
   17ece:	009ed733          	srl	a4,t4,s1
   17ed2:	00f88a63          	beq	a7,a5,17ee6 <__subtf3+0x626>
   17ed6:	08000793          	li	a5,128
   17eda:	411788bb          	subw	a7,a5,a7
   17ede:	011e98b3          	sll	a7,t4,a7
   17ee2:	011e6e33          	or	t3,t3,a7
   17ee6:	01c034b3          	snez	s1,t3
   17eea:	8cd9                	or	s1,s1,a4
   17eec:	bc49                	j	1797e <__subtf3+0xbe>
   17eee:	8dc9                	or	a1,a1,a0
   17ef0:	00b034b3          	snez	s1,a1
   17ef4:	409e04b3          	sub	s1,t3,s1
   17ef8:	009e3733          	sltu	a4,t3,s1
   17efc:	40ee85b3          	sub	a1,t4,a4
  FP_UNPACK_SEMIRAW_Q (B, b);
   17f00:	841a                	mv	s0,t1
   17f02:	8936                	mv	s2,a3
   17f04:	b451                	j	17988 <__subtf3+0xc8>
  FP_SUB_Q (R, A, B);
   17f06:	00a5e7b3          	or	a5,a1,a0
   17f0a:	28078663          	beqz	a5,18196 <__subtf3+0x8d6>
   17f0e:	577d                	li	a4,-1
   17f10:	3ee88a63          	beq	a7,a4,18304 <__subtf3+0xa44>
   17f14:	67a1                	lui	a5,0x8
   17f16:	17fd                	addi	a5,a5,-1
   17f18:	38f30463          	beq	t1,a5,182a0 <__subtf3+0x9e0>
   17f1c:	ffff4f13          	not	t5,t5
   17f20:	2f01                	sext.w	t5,t5
   17f22:	bb99                	j	17c78 <__subtf3+0x3b8>
   17f24:	04000793          	li	a5,64
   17f28:	411787bb          	subw	a5,a5,a7
   17f2c:	011e5733          	srl	a4,t3,a7
   17f30:	00fe94b3          	sll	s1,t4,a5
   17f34:	00fe1e33          	sll	t3,t3,a5
   17f38:	8cd9                	or	s1,s1,a4
   17f3a:	01c03e33          	snez	t3,t3
   17f3e:	011ed8b3          	srl	a7,t4,a7
   17f42:	01c4e4b3          	or	s1,s1,t3
   17f46:	95c6                	add	a1,a1,a7
   17f48:	bf21                	j	17e60 <__subtf3+0x5a0>
   17f4a:	17fd                	addi	a5,a5,-1
   17f4c:	2af70563          	beq	a4,a5,181f6 <__subtf3+0x936>
   17f50:	01c507b3          	add	a5,a0,t3
   17f54:	00a7b533          	sltu	a0,a5,a0
   17f58:	95f6                	add	a1,a1,t4
   17f5a:	95aa                	add	a1,a1,a0
   17f5c:	8385                	srli	a5,a5,0x1
   17f5e:	03f59493          	slli	s1,a1,0x3f
   17f62:	8cdd                	or	s1,s1,a5
   17f64:	8185                	srli	a1,a1,0x1
   17f66:	8b9d                	andi	a5,a5,7
   17f68:	843a                	mv	s0,a4
   17f6a:	b1d1                	j	17c2e <__subtf3+0x36e>
   17f6c:	177d                	addi	a4,a4,-1
   17f6e:	04e40563          	beq	s0,a4,17fb8 <__subtf3+0x6f8>
   17f72:	04e30e63          	beq	t1,a4,17fce <__subtf3+0x70e>
   17f76:	de080ee3          	beqz	a6,17d72 <__subtf3+0x4b2>
   17f7a:	c80794e3          	bnez	a5,17c02 <__subtf3+0x342>
   17f7e:	003f9513          	slli	a0,t6,0x3
   17f82:	03d59793          	slli	a5,a1,0x3d
   17f86:	810d                	srli	a0,a0,0x3
   17f88:	8d5d                	or	a0,a0,a5
   17f8a:	0035d793          	srli	a5,a1,0x3
  FP_PACK_SEMIRAW_Q (r, R);
   17f8e:	b1bd                	j	17bfc <__subtf3+0x33c>
  FP_SUB_Q (R, A, B);
   17f90:	40ae04b3          	sub	s1,t3,a0
   17f94:	40be85b3          	sub	a1,t4,a1
   17f98:	009e3e33          	sltu	t3,t3,s1
   17f9c:	41c58a33          	sub	s4,a1,t3
   17fa0:	8936                	mv	s2,a3
   17fa2:	badd                	j	17998 <__subtf3+0xd8>
   17fa4:	01c504b3          	add	s1,a0,t3
   17fa8:	95f6                	add	a1,a1,t4
   17faa:	00a4b733          	sltu	a4,s1,a0
   17fae:	95ba                	add	a1,a1,a4
   17fb0:	bd65                	j	17e68 <__subtf3+0x5a8>
  FP_PACK_SEMIRAW_Q (r, R);
   17fb2:	c8090ee3          	beqz	s2,17c4e <__subtf3+0x38e>
   17fb6:	b369                	j	17d40 <__subtf3+0x480>
  FP_SUB_Q (R, A, B);
   17fb8:	da080be3          	beqz	a6,17d6e <__subtf3+0x4ae>
   17fbc:	00d59713          	slli	a4,a1,0xd
   17fc0:	00074363          	bltz	a4,17fc6 <__subtf3+0x706>
   17fc4:	48c1                	li	a7,16
   17fc6:	6721                	lui	a4,0x8
   17fc8:	177d                	addi	a4,a4,-1
   17fca:	fae318e3          	bne	t1,a4,17f7a <__subtf3+0x6ba>
   17fce:	d7c5                	beqz	a5,17f76 <__subtf3+0x6b6>
   17fd0:	00de9793          	slli	a5,t4,0xd
   17fd4:	0007c363          	bltz	a5,17fda <__subtf3+0x71a>
   17fd8:	48c1                	li	a7,16
   17fda:	c20814e3          	bnez	a6,17c02 <__subtf3+0x342>
   17fde:	00361513          	slli	a0,a2,0x3
   17fe2:	03de9793          	slli	a5,t4,0x3d
   17fe6:	810d                	srli	a0,a0,0x3
   17fe8:	8d5d                	or	a0,a0,a5
   17fea:	8936                	mv	s2,a3
   17fec:	003ed793          	srli	a5,t4,0x3
  FP_PACK_SEMIRAW_Q (r, R);
   17ff0:	b131                	j	17bfc <__subtf3+0x33c>
   17ff2:	8936                	mv	s2,a3
  FP_DECL_EX;
   17ff4:	4881                	li	a7,0
   17ff6:	b3b9                	j	17d44 <__subtf3+0x484>
  FP_SUB_Q (R, A, B);
   17ff8:	41c504b3          	sub	s1,a0,t3
   17ffc:	41d58733          	sub	a4,a1,t4
   18000:	00953533          	sltu	a0,a0,s1
   18004:	40a705b3          	sub	a1,a4,a0
   18008:	b241                	j	17988 <__subtf3+0xc8>
   1800a:	8d4d                	or	a0,a0,a1
   1800c:	d565                	beqz	a0,17ff4 <__subtf3+0x734>
   1800e:	0325d893          	srli	a7,a1,0x32
   18012:	0f8e                	slli	t6,t6,0x3
   18014:	0018f893          	andi	a7,a7,1
   18018:	03d59513          	slli	a0,a1,0x3d
   1801c:	003fd493          	srli	s1,t6,0x3
   18020:	0018b893          	seqz	a7,a7
   18024:	8d45                	or	a0,a0,s1
   18026:	0035d793          	srli	a5,a1,0x3
   1802a:	0892                	slli	a7,a7,0x4
   1802c:	bec1                	j	17bfc <__subtf3+0x33c>
   1802e:	16078c63          	beqz	a5,181a6 <__subtf3+0x8e6>
   18032:	4781                	li	a5,0
  FP_UNPACK_SEMIRAW_Q (B, b);
   18034:	85f6                	mv	a1,t4
   18036:	8572                	mv	a0,t3
   18038:	8936                	mv	s2,a3
   1803a:	84aa                	mv	s1,a0
   1803c:	ba65                	j	179f4 <__subtf3+0x134>
  FP_PACK_SEMIRAW_Q (r, R);
   1803e:	00f67713          	andi	a4,a2,15
   18042:	4511                	li	a0,4
   18044:	9ca70ce3          	beq	a4,a0,17a1c <__subtf3+0x15c>
   18048:	ffc63713          	sltiu	a4,a2,-4
   1804c:	00174713          	xori	a4,a4,1
   18050:	1702                	slli	a4,a4,0x20
   18052:	9301                	srli	a4,a4,0x20
   18054:	96ba                	add	a3,a3,a4
   18056:	b2d9                	j	17a1c <__subtf3+0x15c>
   18058:	9c0912e3          	bnez	s2,17a1c <__subtf3+0x15c>
   1805c:	ff863713          	sltiu	a4,a2,-8
   18060:	00174713          	xori	a4,a4,1
   18064:	1702                	slli	a4,a4,0x20
   18066:	9301                	srli	a4,a4,0x20
   18068:	96ba                	add	a3,a3,a4
   1806a:	ba4d                	j	17a1c <__subtf3+0x15c>
   1806c:	4885                	li	a7,1
   1806e:	9a0907e3          	beqz	s2,17a1c <__subtf3+0x15c>
   18072:	b7ed                	j	1805c <__subtf3+0x79c>
  FP_SUB_Q (R, A, B);
   18074:	04098c63          	beqz	s3,180cc <__subtf3+0x80c>
   18078:	470d                	li	a4,3
   1807a:	26e98c63          	beq	s3,a4,182f2 <__subtf3+0xa32>
   1807e:	4709                	li	a4,2
   18080:	4895                	li	a7,5
   18082:	bce990e3          	bne	s3,a4,17c42 <__subtf3+0x382>
   18086:	ca091fe3          	bnez	s2,17d44 <__subtf3+0x484>
   1808a:	ffe78413          	addi	s0,a5,-2 # 7ffe <exit-0x8122>
   1808e:	55fd                	li	a1,-1
   18090:	54fd                	li	s1,-1
   18092:	ba7d                	j	17a50 <__subtf3+0x190>
   18094:	8d4d                	or	a0,a0,a1
   18096:	dd39                	beqz	a0,17ff4 <__subtf3+0x734>
   18098:	0325d893          	srli	a7,a1,0x32
   1809c:	003f9793          	slli	a5,t6,0x3
   180a0:	0018f893          	andi	a7,a7,1
   180a4:	838d                	srli	a5,a5,0x3
   180a6:	03d59513          	slli	a0,a1,0x3d
   180aa:	0018b893          	seqz	a7,a7
   180ae:	8d5d                	or	a0,a0,a5
   180b0:	0892                	slli	a7,a7,0x4
   180b2:	0035d793          	srli	a5,a1,0x3
   180b6:	b699                	j	17bfc <__subtf3+0x33c>
   180b8:	174e                	slli	a4,a4,0x33
   180ba:	8f6d                	and	a4,a4,a1
  FP_PACK_SEMIRAW_Q (r, R);
   180bc:	980698e3          	bnez	a3,17a4c <__subtf3+0x18c>
   180c0:	843e                	mv	s0,a5
   180c2:	980885e3          	beqz	a7,17a4c <__subtf3+0x18c>
   180c6:	0028e893          	ori	a7,a7,2
   180ca:	b249                	j	17a4c <__subtf3+0x18c>
  FP_SUB_Q (R, A, B);
   180cc:	4895                	li	a7,5
   180ce:	b99d                	j	17d44 <__subtf3+0x484>
   180d0:	8d4d                	or	a0,a0,a1
   180d2:	d10d                	beqz	a0,17ff4 <__subtf3+0x734>
   180d4:	0325d893          	srli	a7,a1,0x32
   180d8:	003f9513          	slli	a0,t6,0x3
   180dc:	0018f893          	andi	a7,a7,1
   180e0:	03d59793          	slli	a5,a1,0x3d
   180e4:	810d                	srli	a0,a0,0x3
  FP_DECL_EX;
   180e6:	0018b893          	seqz	a7,a7
   180ea:	8d5d                	or	a0,a0,a5
   180ec:	0892                	slli	a7,a7,0x4
   180ee:	0035d793          	srli	a5,a1,0x3
   180f2:	b629                	j	17bfc <__subtf3+0x33c>
  FP_SUB_Q (R, A, B);
   180f4:	8dc9                	or	a1,a1,a0
   180f6:	00b034b3          	snez	s1,a1
   180fa:	be45                	j	17caa <__subtf3+0x3ea>
   180fc:	fc0f049b          	addiw	s1,t5,-64
   18100:	04000713          	li	a4,64
   18104:	0095d7b3          	srl	a5,a1,s1
   18108:	00ef0963          	beq	t5,a4,1811a <__subtf3+0x85a>
   1810c:	08000713          	li	a4,128
   18110:	41e70f3b          	subw	t5,a4,t5
   18114:	01e59733          	sll	a4,a1,t5
   18118:	8d59                	or	a0,a0,a4
   1811a:	00a034b3          	snez	s1,a0
   1811e:	8cdd                	or	s1,s1,a5
   18120:	bbd1                	j	17ef4 <__subtf3+0x634>
   18122:	17fd                	addi	a5,a5,-1
   18124:	1af40263          	beq	s0,a5,182c8 <__subtf3+0xa08>
   18128:	1ef30a63          	beq	t1,a5,1831c <__subtf3+0xa5c>
   1812c:	12070363          	beqz	a4,18252 <__subtf3+0x992>
   18130:	01cee7b3          	or	a5,t4,t3
   18134:	ac0797e3          	bnez	a5,17c02 <__subtf3+0x342>
  FP_PACK_SEMIRAW_Q (r, R);
   18138:	003f9793          	slli	a5,t6,0x3
   1813c:	838d                	srli	a5,a5,0x3
   1813e:	03d59513          	slli	a0,a1,0x3d
   18142:	8d5d                	or	a0,a0,a5
   18144:	0035d793          	srli	a5,a1,0x3
   18148:	bc55                	j	17bfc <__subtf3+0x33c>
  FP_SUB_Q (R, A, B);
   1814a:	01ceee33          	or	t3,t4,t3
   1814e:	ea0e02e3          	beqz	t3,17ff2 <__subtf3+0x732>
   18152:	00361513          	slli	a0,a2,0x3
   18156:	03de9793          	slli	a5,t4,0x3d
   1815a:	810d                	srli	a0,a0,0x3
   1815c:	00de9713          	slli	a4,t4,0xd
   18160:	8d5d                	or	a0,a0,a5
   18162:	8936                	mv	s2,a3
   18164:	003ed793          	srli	a5,t4,0x3
   18168:	c00741e3          	bltz	a4,17d6a <__subtf3+0x4aa>
   1816c:	48c1                	li	a7,16
   1816e:	b479                	j	17bfc <__subtf3+0x33c>
   18170:	01ceee33          	or	t3,t4,t3
   18174:	e60e0fe3          	beqz	t3,17ff2 <__subtf3+0x732>
   18178:	00375513          	srli	a0,a4,0x3
   1817c:	8d71                	and	a0,a0,a2
   1817e:	03de9713          	slli	a4,t4,0x3d
   18182:	8d59                	or	a0,a0,a4
   18184:	00de9713          	slli	a4,t4,0xd
   18188:	003ed793          	srli	a5,t4,0x3
   1818c:	8936                	mv	s2,a3
   1818e:	bc074ee3          	bltz	a4,17d6a <__subtf3+0x4aa>
   18192:	48c1                	li	a7,16
   18194:	b4a5                	j	17bfc <__subtf3+0x33c>
   18196:	67a1                	lui	a5,0x8
   18198:	17fd                	addi	a5,a5,-1
   1819a:	08f30a63          	beq	t1,a5,1822e <__subtf3+0x96e>
  FP_UNPACK_SEMIRAW_Q (B, b);
   1819e:	85f6                	mv	a1,t4
   181a0:	84f2                	mv	s1,t3
   181a2:	841a                	mv	s0,t1
   181a4:	bcbd                	j	17c22 <__subtf3+0x362>
  FP_SUB_Q (R, A, B);
   181a6:	ffe98693          	addi	a3,s3,-2
   181aa:	0016b913          	seqz	s2,a3
   181ae:	4501                	li	a0,0
   181b0:	bad5                	j	17ba4 <__subtf3+0x2e4>
  FP_UNPACK_SEMIRAW_Q (B, b);
   181b2:	85f6                	mv	a1,t4
   181b4:	84f2                	mv	s1,t3
   181b6:	b81d                	j	179ec <__subtf3+0x12c>
  FP_SUB_Q (R, A, B);
   181b8:	fc0f079b          	addiw	a5,t5,-64
   181bc:	04000713          	li	a4,64
   181c0:	00f5d7b3          	srl	a5,a1,a5
   181c4:	00ef0963          	beq	t5,a4,181d6 <__subtf3+0x916>
   181c8:	08000713          	li	a4,128
   181cc:	41e70f3b          	subw	t5,a4,t5
   181d0:	01e59733          	sll	a4,a1,t5
   181d4:	8d59                	or	a0,a0,a4
   181d6:	00a034b3          	snez	s1,a0
   181da:	8cdd                	or	s1,s1,a5
   181dc:	b4f9                	j	17caa <__subtf3+0x3ea>
   181de:	40ae04b3          	sub	s1,t3,a0
   181e2:	40be8733          	sub	a4,t4,a1
   181e6:	009e3e33          	sltu	t3,t3,s1
   181ea:	41c705b3          	sub	a1,a4,t3
  FP_UNPACK_SEMIRAW_Q (B, b);
   181ee:	841a                	mv	s0,t1
  FP_SUB_Q (R, A, B);
   181f0:	8936                	mv	s2,a3
   181f2:	f96ff06f          	j	17988 <__subtf3+0xc8>
   181f6:	ec098be3          	beqz	s3,180cc <__subtf3+0x80c>
   181fa:	478d                	li	a5,3
   181fc:	12f98763          	beq	s3,a5,1832a <__subtf3+0xa6a>
   18200:	4789                	li	a5,2
   18202:	4895                	li	a7,5
   18204:	a4f995e3          	bne	s3,a5,17c4e <__subtf3+0x38e>
   18208:	b2091ee3          	bnez	s2,17d44 <__subtf3+0x484>
   1820c:	8436                	mv	s0,a3
   1820e:	55fd                	li	a1,-1
   18210:	54fd                	li	s1,-1
   18212:	b83d                	j	17a50 <__subtf3+0x190>
   18214:	01cee7b3          	or	a5,t4,t3
   18218:	10078063          	beqz	a5,18318 <__subtf3+0xa58>
   1821c:	00de9793          	slli	a5,t4,0xd
   18220:	85f6                	mv	a1,t4
  FP_UNPACK_SEMIRAW_Q (B, b);
   18222:	8572                	mv	a0,t3
   18224:	8936                	mv	s2,a3
  FP_SUB_Q (R, A, B);
   18226:	0007ce63          	bltz	a5,18242 <__subtf3+0x982>
   1822a:	48c1                	li	a7,16
   1822c:	ba5d                	j	17be2 <__subtf3+0x322>
   1822e:	01cee7b3          	or	a5,t4,t3
   18232:	b2078ae3          	beqz	a5,17d66 <__subtf3+0x4a6>
   18236:	00de9793          	slli	a5,t4,0xd
   1823a:	85f6                	mv	a1,t4
  FP_UNPACK_SEMIRAW_Q (B, b);
   1823c:	8572                	mv	a0,t3
  FP_SUB_Q (R, A, B);
   1823e:	fe07d6e3          	bgez	a5,1822a <__subtf3+0x96a>
  FP_DECL_EX;
   18242:	4881                	li	a7,0
   18244:	ba79                	j	17be2 <__subtf3+0x322>
  FP_SUB_Q (R, A, B);
   18246:	00831663          	bne	t1,s0,18252 <__subtf3+0x992>
   1824a:	01ceee33          	or	t3,t4,t3
   1824e:	080e1b63          	bnez	t3,182e4 <__subtf3+0xa24>
  FP_PACK_SEMIRAW_Q (r, R);
   18252:	00361793          	slli	a5,a2,0x3
   18256:	838d                	srli	a5,a5,0x3
   18258:	03de9513          	slli	a0,t4,0x3d
   1825c:	8d5d                	or	a0,a0,a5
   1825e:	003ed793          	srli	a5,t4,0x3
   18262:	ba69                	j	17bfc <__subtf3+0x33c>
  FP_SUB_Q (R, A, B);
   18264:	00e667b3          	or	a5,a2,a4
   18268:	df9d                	beqz	a5,181a6 <__subtf3+0x8e6>
   1826a:	8532                	mv	a0,a2
   1826c:	00767793          	andi	a5,a2,7
   18270:	85ba                	mv	a1,a4
  FP_UNPACK_SEMIRAW_Q (B, b);
   18272:	84aa                	mv	s1,a0
   18274:	f80ff06f          	j	179f4 <__subtf3+0x134>
  FP_SUB_Q (R, A, B);
   18278:	01ceee33          	or	t3,t4,t3
   1827c:	d60e0ce3          	beqz	t3,17ff4 <__subtf3+0x734>
   18280:	032ed893          	srli	a7,t4,0x32
   18284:	00361513          	slli	a0,a2,0x3
   18288:	0018f893          	andi	a7,a7,1
   1828c:	03de9793          	slli	a5,t4,0x3d
   18290:	810d                	srli	a0,a0,0x3
   18292:	0018b893          	seqz	a7,a7
   18296:	8d5d                	or	a0,a0,a5
   18298:	0892                	slli	a7,a7,0x4
   1829a:	003ed793          	srli	a5,t4,0x3
   1829e:	bab9                	j	17bfc <__subtf3+0x33c>
   182a0:	01ceee33          	or	t3,t4,t3
   182a4:	d40e08e3          	beqz	t3,17ff4 <__subtf3+0x734>
   182a8:	032ed893          	srli	a7,t4,0x32
   182ac:	00375513          	srli	a0,a4,0x3
   182b0:	0018f893          	andi	a7,a7,1
   182b4:	03de9713          	slli	a4,t4,0x3d
   182b8:	8d71                	and	a0,a0,a2
   182ba:	0018b893          	seqz	a7,a7
   182be:	8d59                	or	a0,a0,a4
   182c0:	003ed793          	srli	a5,t4,0x3
   182c4:	0892                	slli	a7,a7,0x4
   182c6:	ba1d                	j	17bfc <__subtf3+0x33c>
   182c8:	df3d                	beqz	a4,18246 <__subtf3+0x986>
   182ca:	00d59793          	slli	a5,a1,0xd
   182ce:	0007c363          	bltz	a5,182d4 <__subtf3+0xa14>
   182d2:	48c1                	li	a7,16
   182d4:	67a1                	lui	a5,0x8
   182d6:	17fd                	addi	a5,a5,-1
   182d8:	e4f31ce3          	bne	t1,a5,18130 <__subtf3+0x870>
   182dc:	01ceee33          	or	t3,t4,t3
   182e0:	e40e0ce3          	beqz	t3,18138 <__subtf3+0x878>
   182e4:	00de9793          	slli	a5,t4,0xd
   182e8:	0007c363          	bltz	a5,182ee <__subtf3+0xa2e>
   182ec:	48c1                	li	a7,16
   182ee:	d335                	beqz	a4,18252 <__subtf3+0x992>
   182f0:	ba09                	j	17c02 <__subtf3+0x342>
   182f2:	dc090de3          	beqz	s2,180cc <__subtf3+0x80c>
   182f6:	54fd                	li	s1,-1
   182f8:	ffe78413          	addi	s0,a5,-2 # 7ffe <exit-0x8122>
   182fc:	55fd                	li	a1,-1
   182fe:	4895                	li	a7,5
   18300:	4601                	li	a2,0
   18302:	b409                	j	17d04 <__subtf3+0x444>
   18304:	01c504b3          	add	s1,a0,t3
   18308:	01d58733          	add	a4,a1,t4
   1830c:	01c4be33          	sltu	t3,s1,t3
   18310:	01c705b3          	add	a1,a4,t3
  FP_UNPACK_SEMIRAW_Q (B, b);
   18314:	841a                	mv	s0,t1
  FP_SUB_Q (R, A, B);
   18316:	be89                	j	17e68 <__subtf3+0x5a8>
   18318:	8936                	mv	s2,a3
   1831a:	b4b1                	j	17d66 <__subtf3+0x4a6>
   1831c:	01ceee33          	or	t3,t4,t3
   18320:	fc0e12e3          	bnez	t3,182e4 <__subtf3+0xa24>
   18324:	e0071ae3          	bnez	a4,18138 <__subtf3+0x878>
   18328:	b72d                	j	18252 <__subtf3+0x992>
   1832a:	da0901e3          	beqz	s2,180cc <__subtf3+0x80c>
   1832e:	4601                	li	a2,0
   18330:	54fd                	li	s1,-1
   18332:	8436                	mv	s0,a3
   18334:	55fd                	li	a1,-1
   18336:	4895                	li	a7,5
   18338:	b2f1                	j	17d04 <__subtf3+0x444>

000000000001833a <__fixtfsi>:
#include "soft-fp.h"
#include "quad.h"

SItype
__fixtfsi (TFtype a)
{
   1833a:	882a                	mv	a6,a0
  FP_DECL_EX;
  FP_DECL_Q (A);
  USItype r;

  FP_INIT_EXCEPTIONS;
   1833c:	002027f3          	frrm	a5
  FP_UNPACK_RAW_Q (A, a);
  FP_TO_INT_Q (r, A, SI_BITS, 1);
   18340:	6611                	lui	a2,0x4
  FP_UNPACK_RAW_Q (A, a);
   18342:	00159793          	slli	a5,a1,0x1
   18346:	01059713          	slli	a4,a1,0x10
   1834a:	93c5                	srli	a5,a5,0x31
  FP_TO_INT_Q (r, A, SI_BITS, 1);
   1834c:	ffe60693          	addi	a3,a2,-2 # 3ffe <exit-0xc122>
  FP_UNPACK_RAW_Q (A, a);
   18350:	8341                	srli	a4,a4,0x10
   18352:	91fd                	srli	a1,a1,0x3f
  FP_TO_INT_Q (r, A, SI_BITS, 1);
   18354:	00f6cf63          	blt	a3,a5,18372 <__fixtfsi+0x38>
   18358:	eb81                	bnez	a5,18368 <__fixtfsi+0x2e>
   1835a:	8f49                	or	a4,a4,a0
   1835c:	4501                	li	a0,0
   1835e:	cb3d                	beqz	a4,183d4 <__fixtfsi+0x9a>
   18360:	4685                	li	a3,1
  FP_HANDLE_EXCEPTIONS;
   18362:	0016a073          	csrs	fflags,a3

  return r;
}
   18366:	8082                	ret
   18368:	4501                	li	a0,0
  FP_TO_INT_Q (r, A, SI_BITS, 1);
   1836a:	4685                	li	a3,1
  FP_HANDLE_EXCEPTIONS;
   1836c:	0016a073          	csrs	fflags,a3
   18370:	bfdd                	j	18366 <__fixtfsi+0x2c>
  FP_TO_INT_Q (r, A, SI_BITS, 1);
   18372:	01d60693          	addi	a3,a2,29
   18376:	02f6d663          	bge	a3,a5,183a2 <__fixtfsi+0x68>
   1837a:	800006b7          	lui	a3,0x80000
   1837e:	fff6c693          	not	a3,a3
   18382:	00b6853b          	addw	a0,a3,a1
   18386:	c9a1                	beqz	a1,183d6 <__fixtfsi+0x9c>
   18388:	0679                	addi	a2,a2,30
   1838a:	46c1                	li	a3,16
   1838c:	fcc79be3          	bne	a5,a2,18362 <__fixtfsi+0x28>
   18390:	01175793          	srli	a5,a4,0x11
   18394:	f7f9                	bnez	a5,18362 <__fixtfsi+0x28>
   18396:	02f71693          	slli	a3,a4,0x2f
   1839a:	0106e7b3          	or	a5,a3,a6
   1839e:	f3e9                	bnez	a5,18360 <__fixtfsi+0x26>
   183a0:	8082                	ret
   183a2:	4685                	li	a3,1
   183a4:	16c2                	slli	a3,a3,0x30
   183a6:	8f55                	or	a4,a4,a3
   183a8:	6611                	lui	a2,0x4
   183aa:	76f1                	lui	a3,0xffffc
   183ac:	26c5                	addiw	a3,a3,17
   183ae:	02f6061b          	addiw	a2,a2,47
   183b2:	9ebd                	addw	a3,a3,a5
   183b4:	40f607bb          	subw	a5,a2,a5
   183b8:	00d716b3          	sll	a3,a4,a3
   183bc:	00f75733          	srl	a4,a4,a5
   183c0:	2701                	sext.w	a4,a4
   183c2:	00a6e7b3          	or	a5,a3,a0
   183c6:	c199                	beqz	a1,183cc <__fixtfsi+0x92>
   183c8:	40e0073b          	negw	a4,a4
  FP_HANDLE_EXCEPTIONS;
   183cc:	0007051b          	sext.w	a0,a4
   183d0:	fbc1                	bnez	a5,18360 <__fixtfsi+0x26>
}
   183d2:	8082                	ret
   183d4:	8082                	ret
  FP_TO_INT_Q (r, A, SI_BITS, 1);
   183d6:	46c1                	li	a3,16
  FP_HANDLE_EXCEPTIONS;
   183d8:	0016a073          	csrs	fflags,a3
   183dc:	b769                	j	18366 <__fixtfsi+0x2c>

00000000000183de <__fixunstfsi>:
{
  FP_DECL_EX;
  FP_DECL_Q (A);
  USItype r;

  FP_INIT_EXCEPTIONS;
   183de:	002027f3          	frrm	a5
  FP_UNPACK_RAW_Q (A, a);
  FP_TO_INT_Q (r, A, SI_BITS, 0);
   183e2:	6611                	lui	a2,0x4
  FP_UNPACK_RAW_Q (A, a);
   183e4:	00159793          	slli	a5,a1,0x1
   183e8:	01059713          	slli	a4,a1,0x10
   183ec:	93c5                	srli	a5,a5,0x31
  FP_TO_INT_Q (r, A, SI_BITS, 0);
   183ee:	ffe60693          	addi	a3,a2,-2 # 3ffe <exit-0xc122>
  FP_UNPACK_RAW_Q (A, a);
   183f2:	8341                	srli	a4,a4,0x10
   183f4:	91fd                	srli	a1,a1,0x3f
  FP_TO_INT_Q (r, A, SI_BITS, 0);
   183f6:	00f6ca63          	blt	a3,a5,1840a <__fixunstfsi+0x2c>
   183fa:	e399                	bnez	a5,18400 <__fixunstfsi+0x22>
   183fc:	8f49                	or	a4,a4,a0
   183fe:	cf29                	beqz	a4,18458 <__fixunstfsi+0x7a>
   18400:	4785                	li	a5,1
   18402:	4501                	li	a0,0
  FP_HANDLE_EXCEPTIONS;
   18404:	0017a073          	csrs	fflags,a5

  return r;
}
   18408:	8082                	ret
  FP_TO_INT_Q (r, A, SI_BITS, 0);
   1840a:	0015b693          	seqz	a3,a1
   1840e:	96b2                	add	a3,a3,a2
   18410:	06f9                	addi	a3,a3,30
   18412:	00d7c863          	blt	a5,a3,18422 <__fixunstfsi+0x44>
   18416:	fff58513          	addi	a0,a1,-1 # fffefff <__BSS_END__+0xffe25df>
   1841a:	47c1                	li	a5,16
  FP_HANDLE_EXCEPTIONS;
   1841c:	0017a073          	csrs	fflags,a5
   18420:	b7e5                	j	18408 <__fixunstfsi+0x2a>
  FP_TO_INT_Q (r, A, SI_BITS, 0);
   18422:	c591                	beqz	a1,1842e <__fixunstfsi+0x50>
   18424:	47c1                	li	a5,16
   18426:	4501                	li	a0,0
  FP_HANDLE_EXCEPTIONS;
   18428:	0017a073          	csrs	fflags,a5
   1842c:	bff1                	j	18408 <__fixunstfsi+0x2a>
  FP_TO_INT_Q (r, A, SI_BITS, 0);
   1842e:	4585                	li	a1,1
   18430:	15c2                	slli	a1,a1,0x30
   18432:	8dd9                	or	a1,a1,a4
   18434:	7771                	lui	a4,0xffffc
   18436:	2745                	addiw	a4,a4,17
   18438:	6691                	lui	a3,0x4
   1843a:	9f3d                	addw	a4,a4,a5
   1843c:	02f6869b          	addiw	a3,a3,47
   18440:	40f687bb          	subw	a5,a3,a5
   18444:	00e59733          	sll	a4,a1,a4
   18448:	8f49                	or	a4,a4,a0
   1844a:	00f5d5b3          	srl	a1,a1,a5
   1844e:	0005851b          	sext.w	a0,a1
  FP_HANDLE_EXCEPTIONS;
   18452:	4785                	li	a5,1
   18454:	fb45                	bnez	a4,18404 <__fixunstfsi+0x26>
}
   18456:	8082                	ret
  FP_TO_INT_Q (r, A, SI_BITS, 0);
   18458:	4501                	li	a0,0
   1845a:	8082                	ret

000000000001845c <__floatsitf>:
#include "soft-fp.h"
#include "quad.h"

TFtype
__floatsitf (SItype i)
{
   1845c:	1101                	addi	sp,sp,-32
   1845e:	ec06                	sd	ra,24(sp)
   18460:	e822                	sd	s0,16(sp)
   18462:	e426                	sd	s1,8(sp)
  FP_DECL_Q (A);
  TFtype a;

  FP_FROM_INT_Q (A, i, SI_BITS, USItype);
   18464:	c539                	beqz	a0,184b2 <__floatsitf+0x56>
   18466:	0005059b          	sext.w	a1,a0
   1846a:	01f5549b          	srliw	s1,a0,0x1f
   1846e:	04054663          	bltz	a0,184ba <__floatsitf+0x5e>
   18472:	02059413          	slli	s0,a1,0x20
   18476:	9001                	srli	s0,s0,0x20
   18478:	8522                	mv	a0,s0
   1847a:	18c000ef          	jal	ra,18606 <__clzdi2>
   1847e:	6591                	lui	a1,0x4
   18480:	03e5879b          	addiw	a5,a1,62
   18484:	40a7853b          	subw	a0,a5,a0
   18488:	02f5859b          	addiw	a1,a1,47
   1848c:	9d89                	subw	a1,a1,a0
   1848e:	00b415b3          	sll	a1,s0,a1
   18492:	05c2                	slli	a1,a1,0x10
   18494:	1546                	slli	a0,a0,0x31
   18496:	81c1                	srli	a1,a1,0x10
   18498:	9145                	srli	a0,a0,0x31
  FP_PACK_RAW_Q (a, A);

  return a;
}
   1849a:	60e2                	ld	ra,24(sp)
   1849c:	6442                	ld	s0,16(sp)
  FP_PACK_RAW_Q (a, A);
   1849e:	04be                	slli	s1,s1,0xf
   184a0:	8cc9                	or	s1,s1,a0
   184a2:	05c2                	slli	a1,a1,0x10
   184a4:	14c2                	slli	s1,s1,0x30
   184a6:	81c1                	srli	a1,a1,0x10
}
   184a8:	8dc5                	or	a1,a1,s1
   184aa:	4501                	li	a0,0
   184ac:	64a2                	ld	s1,8(sp)
   184ae:	6105                	addi	sp,sp,32
   184b0:	8082                	ret
   184b2:	4481                	li	s1,0
   184b4:	4501                	li	a0,0
   184b6:	4581                	li	a1,0
   184b8:	b7cd                	j	1849a <__floatsitf+0x3e>
  FP_FROM_INT_Q (A, i, SI_BITS, USItype);
   184ba:	40b005bb          	negw	a1,a1
   184be:	bf55                	j	18472 <__floatsitf+0x16>

00000000000184c0 <__floatunsitf>:
__floatunsitf (USItype i)
{
  FP_DECL_Q (A);
  TFtype a;

  FP_FROM_INT_Q (A, i, SI_BITS, USItype);
   184c0:	c121                	beqz	a0,18500 <__floatunsitf+0x40>
{
   184c2:	1141                	addi	sp,sp,-16
   184c4:	e022                	sd	s0,0(sp)
  FP_FROM_INT_Q (A, i, SI_BITS, USItype);
   184c6:	02051413          	slli	s0,a0,0x20
   184ca:	9001                	srli	s0,s0,0x20
   184cc:	8522                	mv	a0,s0
{
   184ce:	e406                	sd	ra,8(sp)
  FP_FROM_INT_Q (A, i, SI_BITS, USItype);
   184d0:	136000ef          	jal	ra,18606 <__clzdi2>
   184d4:	6791                	lui	a5,0x4
   184d6:	03e7859b          	addiw	a1,a5,62
   184da:	9d89                	subw	a1,a1,a0
   184dc:	02f7879b          	addiw	a5,a5,47
   184e0:	9f8d                	subw	a5,a5,a1
   184e2:	00f417b3          	sll	a5,s0,a5
   184e6:	07c2                	slli	a5,a5,0x10
  FP_PACK_RAW_Q (a, A);

  return a;
}
   184e8:	60a2                	ld	ra,8(sp)
   184ea:	6402                	ld	s0,0(sp)
   184ec:	15c6                	slli	a1,a1,0x31
   184ee:	83c1                	srli	a5,a5,0x10
   184f0:	91c5                	srli	a1,a1,0x31
  FP_PACK_RAW_Q (a, A);
   184f2:	07c2                	slli	a5,a5,0x10
   184f4:	15c2                	slli	a1,a1,0x30
   184f6:	83c1                	srli	a5,a5,0x10
}
   184f8:	4501                	li	a0,0
   184fa:	8ddd                	or	a1,a1,a5
   184fc:	0141                	addi	sp,sp,16
   184fe:	8082                	ret
   18500:	4781                	li	a5,0
  FP_PACK_RAW_Q (a, A);
   18502:	4581                	li	a1,0
   18504:	07c2                	slli	a5,a5,0x10
   18506:	83c1                	srli	a5,a5,0x10
   18508:	15c2                	slli	a1,a1,0x30
}
   1850a:	4501                	li	a0,0
   1850c:	8ddd                	or	a1,a1,a5
   1850e:	8082                	ret

0000000000018510 <__extenddftf2>:
#include "double.h"
#include "quad.h"

TFtype
__extenddftf2 (DFtype a)
{
   18510:	1101                	addi	sp,sp,-32
   18512:	e20505d3          	fmv.x.d	a1,fa0
   18516:	ec06                	sd	ra,24(sp)
   18518:	e822                	sd	s0,16(sp)
   1851a:	e426                	sd	s1,8(sp)
  FP_DECL_EX;
  FP_DECL_D (A);
  FP_DECL_Q (R);
  TFtype r;

  FP_INIT_EXCEPTIONS;
   1851c:	002027f3          	frrm	a5
  FP_UNPACK_RAW_D (A, a);
   18520:	0345d793          	srli	a5,a1,0x34
   18524:	7ff7f793          	andi	a5,a5,2047
   18528:	567d                	li	a2,-1
#if (2 * _FP_W_TYPE_SIZE) < _FP_FRACBITS_Q
  FP_EXTEND (Q, D, 4, 2, R, A);
#else
  FP_EXTEND (Q, D, 2, 1, R, A);
   1852a:	00178693          	addi	a3,a5,1 # 4001 <exit-0xc11f>
  FP_UNPACK_RAW_D (A, a);
   1852e:	00c65413          	srli	s0,a2,0xc
  FP_EXTEND (Q, D, 2, 1, R, A);
   18532:	7fe6f693          	andi	a3,a3,2046
  FP_UNPACK_RAW_D (A, a);
   18536:	8c6d                	and	s0,s0,a1
   18538:	03f5d493          	srli	s1,a1,0x3f
  FP_EXTEND (Q, D, 2, 1, R, A);
   1853c:	c695                	beqz	a3,18568 <__extenddftf2+0x58>
   1853e:	6711                	lui	a4,0x4
   18540:	c0070713          	addi	a4,a4,-1024 # 3c00 <exit-0xc520>
   18544:	97ba                	add	a5,a5,a4
   18546:	17c6                	slli	a5,a5,0x31
   18548:	00445593          	srli	a1,s0,0x4
   1854c:	93c5                	srli	a5,a5,0x31
   1854e:	1472                	slli	s0,s0,0x3c
#endif
  FP_PACK_RAW_Q (r, R);
   18550:	17c6                	slli	a5,a5,0x31
   18552:	8385                	srli	a5,a5,0x1
   18554:	14fe                	slli	s1,s1,0x3f
   18556:	8fcd                	or	a5,a5,a1
   18558:	8522                	mv	a0,s0
   1855a:	0097e5b3          	or	a1,a5,s1
  FP_HANDLE_EXCEPTIONS;

  return r;
}
   1855e:	60e2                	ld	ra,24(sp)
   18560:	6442                	ld	s0,16(sp)
   18562:	64a2                	ld	s1,8(sp)
   18564:	6105                	addi	sp,sp,32
   18566:	8082                	ret
  FP_EXTEND (Q, D, 2, 1, R, A);
   18568:	ef85                	bnez	a5,185a0 <__extenddftf2+0x90>
   1856a:	c035                	beqz	s0,185ce <__extenddftf2+0xbe>
   1856c:	8522                	mv	a0,s0
   1856e:	098000ef          	jal	ra,18606 <__clzdi2>
   18572:	0005071b          	sext.w	a4,a0
   18576:	47b9                	li	a5,14
   18578:	08e7c163          	blt	a5,a4,185fa <__extenddftf2+0xea>
   1857c:	473d                	li	a4,15
   1857e:	9f09                	subw	a4,a4,a0
   18580:	0315079b          	addiw	a5,a0,49
   18584:	00e45733          	srl	a4,s0,a4
   18588:	00f41433          	sll	s0,s0,a5
   1858c:	6791                	lui	a5,0x4
   1858e:	c0c7879b          	addiw	a5,a5,-1012
   18592:	9f89                	subw	a5,a5,a0
   18594:	0742                	slli	a4,a4,0x10
   18596:	17c6                	slli	a5,a5,0x31
   18598:	01075593          	srli	a1,a4,0x10
   1859c:	93c5                	srli	a5,a5,0x31
   1859e:	bf4d                	j	18550 <__extenddftf2+0x40>
   185a0:	c815                	beqz	s0,185d4 <__extenddftf2+0xc4>
   185a2:	4785                	li	a5,1
   185a4:	02f79593          	slli	a1,a5,0x2f
   185a8:	00445713          	srli	a4,s0,0x4
   185ac:	17ce                	slli	a5,a5,0x33
   185ae:	8f4d                	or	a4,a4,a1
   185b0:	8fe1                	and	a5,a5,s0
   185b2:	01065593          	srli	a1,a2,0x10
   185b6:	1472                	slli	s0,s0,0x3c
   185b8:	8f6d                	and	a4,a4,a1
   185ba:	c38d                	beqz	a5,185dc <__extenddftf2+0xcc>
  FP_PACK_RAW_Q (r, R);
   185bc:	8f6d                	and	a4,a4,a1
   185be:	07fff5b7          	lui	a1,0x7fff
   185c2:	1592                	slli	a1,a1,0x24
   185c4:	14fe                	slli	s1,s1,0x3f
   185c6:	8dd9                	or	a1,a1,a4
   185c8:	8522                	mv	a0,s0
   185ca:	8dc5                	or	a1,a1,s1
  FP_HANDLE_EXCEPTIONS;
   185cc:	bf49                	j	1855e <__extenddftf2+0x4e>
   185ce:	4781                	li	a5,0
   185d0:	4581                	li	a1,0
   185d2:	bfbd                	j	18550 <__extenddftf2+0x40>
   185d4:	67a1                	lui	a5,0x8
   185d6:	17fd                	addi	a5,a5,-1
   185d8:	4581                	li	a1,0
   185da:	bf9d                	j	18550 <__extenddftf2+0x40>
  FP_PACK_RAW_Q (r, R);
   185dc:	8f6d                	and	a4,a4,a1
   185de:	07fff5b7          	lui	a1,0x7fff
   185e2:	1592                	slli	a1,a1,0x24
   185e4:	14fe                	slli	s1,s1,0x3f
   185e6:	8dd9                	or	a1,a1,a4
   185e8:	8522                	mv	a0,s0
   185ea:	8dc5                	or	a1,a1,s1
  FP_HANDLE_EXCEPTIONS;
   185ec:	00186073          	csrsi	fflags,16
}
   185f0:	60e2                	ld	ra,24(sp)
   185f2:	6442                	ld	s0,16(sp)
   185f4:	64a2                	ld	s1,8(sp)
   185f6:	6105                	addi	sp,sp,32
   185f8:	8082                	ret
  FP_EXTEND (Q, D, 2, 1, R, A);
   185fa:	ff15071b          	addiw	a4,a0,-15
   185fe:	00e41733          	sll	a4,s0,a4
   18602:	4401                	li	s0,0
   18604:	b761                	j	1858c <__extenddftf2+0x7c>

0000000000018606 <__clzdi2>:
int
__clzSI2 (UWtype x)
{
  Wtype ret;

  count_leading_zeros (ret, x);
   18606:	03800793          	li	a5,56
   1860a:	00f55733          	srl	a4,a0,a5
   1860e:	0ff77693          	andi	a3,a4,255
   18612:	ee99                	bnez	a3,18630 <__clzdi2+0x2a>
   18614:	17e1                	addi	a5,a5,-8
   18616:	fbf5                	bnez	a5,1860a <__clzdi2+0x4>
   18618:	00004717          	auipc	a4,0x4
   1861c:	ae873703          	ld	a4,-1304(a4) # 1c100 <_GLOBAL_OFFSET_TABLE_+0x8>
   18620:	953a                	add	a0,a0,a4
   18622:	00054503          	lbu	a0,0(a0)
   18626:	04000793          	li	a5,64

  return ret;
}
   1862a:	40a7853b          	subw	a0,a5,a0
   1862e:	8082                	ret
   18630:	04000513          	li	a0,64
   18634:	40f507b3          	sub	a5,a0,a5
  count_leading_zeros (ret, x);
   18638:	853a                	mv	a0,a4
   1863a:	00004717          	auipc	a4,0x4
   1863e:	ac673703          	ld	a4,-1338(a4) # 1c100 <_GLOBAL_OFFSET_TABLE_+0x8>
   18642:	953a                	add	a0,a0,a4
   18644:	00054503          	lbu	a0,0(a0)
}
   18648:	40a7853b          	subw	a0,a5,a0
   1864c:	8082                	ret
