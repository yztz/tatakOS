
testcases/mount:     file format elf64-littleriscv


Disassembly of section .text:

0000000000001000 <_start>:
    1000:	850a                	mv	a0,sp
    1002:	aa31                	j	111e <__start_main>

0000000000001004 <test_mount>:
    1004:	1141                	addi	sp,sp,-16
    1006:	00001517          	auipc	a0,0x1
    100a:	f5250513          	addi	a0,a0,-174 # 1f58 <__clone+0x28>
    100e:	e406                	sd	ra,8(sp)
    1010:	e022                	sd	s0,0(sp)
    1012:	37e000ef          	jal	ra,1390 <puts>
    1016:	00001517          	auipc	a0,0x1
    101a:	0a250513          	addi	a0,a0,162 # 20b8 <__func__.0>
    101e:	372000ef          	jal	ra,1390 <puts>
    1022:	00001517          	auipc	a0,0x1
    1026:	f4e50513          	addi	a0,a0,-178 # 1f70 <__clone+0x40>
    102a:	366000ef          	jal	ra,1390 <puts>
    102e:	00001617          	auipc	a2,0x1
    1032:	00a60613          	addi	a2,a2,10 # 2038 <mntpoint>
    1036:	00001597          	auipc	a1,0x1
    103a:	04258593          	addi	a1,a1,66 # 2078 <device>
    103e:	00001517          	auipc	a0,0x1
    1042:	f4250513          	addi	a0,a0,-190 # 1f80 <__clone+0x50>
    1046:	36c000ef          	jal	ra,13b2 <printf>
    104a:	00001597          	auipc	a1,0x1
    104e:	fee58593          	addi	a1,a1,-18 # 2038 <mntpoint>
    1052:	4701                	li	a4,0
    1054:	4681                	li	a3,0
    1056:	00001617          	auipc	a2,0x1
    105a:	f4260613          	addi	a2,a2,-190 # 1f98 <__clone+0x68>
    105e:	00001517          	auipc	a0,0x1
    1062:	01a50513          	addi	a0,a0,26 # 2078 <device>
    1066:	6b1000ef          	jal	ra,1f16 <mount>
    106a:	842a                	mv	s0,a0
    106c:	85aa                	mv	a1,a0
    106e:	00001517          	auipc	a0,0x1
    1072:	f3250513          	addi	a0,a0,-206 # 1fa0 <__clone+0x70>
    1076:	33c000ef          	jal	ra,13b2 <printf>
    107a:	e821                	bnez	s0,10ca <test_mount+0xc6>
    107c:	00001517          	auipc	a0,0x1
    1080:	f6c50513          	addi	a0,a0,-148 # 1fe8 <__clone+0xb8>
    1084:	32e000ef          	jal	ra,13b2 <printf>
    1088:	00001517          	auipc	a0,0x1
    108c:	fb050513          	addi	a0,a0,-80 # 2038 <mntpoint>
    1090:	693000ef          	jal	ra,1f22 <umount>
    1094:	85aa                	mv	a1,a0
    1096:	00001517          	auipc	a0,0x1
    109a:	f6a50513          	addi	a0,a0,-150 # 2000 <__clone+0xd0>
    109e:	314000ef          	jal	ra,13b2 <printf>
    10a2:	00001517          	auipc	a0,0x1
    10a6:	f3650513          	addi	a0,a0,-202 # 1fd8 <__clone+0xa8>
    10aa:	2e6000ef          	jal	ra,1390 <puts>
    10ae:	00001517          	auipc	a0,0x1
    10b2:	00a50513          	addi	a0,a0,10 # 20b8 <__func__.0>
    10b6:	2da000ef          	jal	ra,1390 <puts>
    10ba:	6402                	ld	s0,0(sp)
    10bc:	60a2                	ld	ra,8(sp)
    10be:	00001517          	auipc	a0,0x1
    10c2:	eb250513          	addi	a0,a0,-334 # 1f70 <__clone+0x40>
    10c6:	0141                	addi	sp,sp,16
    10c8:	a4e1                	j	1390 <puts>
    10ca:	00001517          	auipc	a0,0x1
    10ce:	eee50513          	addi	a0,a0,-274 # 1fb8 <__clone+0x88>
    10d2:	564000ef          	jal	ra,1636 <panic>
    10d6:	b7f1                	j	10a2 <test_mount+0x9e>

00000000000010d8 <main>:
    10d8:	1101                	addi	sp,sp,-32
    10da:	ec06                	sd	ra,24(sp)
    10dc:	e822                	sd	s0,16(sp)
    10de:	e426                	sd	s1,8(sp)
    10e0:	4785                	li	a5,1
    10e2:	00a7ca63          	blt	a5,a0,10f6 <main+0x1e>
    10e6:	f1fff0ef          	jal	ra,1004 <test_mount>
    10ea:	60e2                	ld	ra,24(sp)
    10ec:	6442                	ld	s0,16(sp)
    10ee:	64a2                	ld	s1,8(sp)
    10f0:	4501                	li	a0,0
    10f2:	6105                	addi	sp,sp,32
    10f4:	8082                	ret
    10f6:	84ae                	mv	s1,a1
    10f8:	658c                	ld	a1,8(a1)
    10fa:	842a                	mv	s0,a0
    10fc:	00001517          	auipc	a0,0x1
    1100:	f7c50513          	addi	a0,a0,-132 # 2078 <device>
    1104:	12b000ef          	jal	ra,1a2e <strcpy>
    1108:	4789                	li	a5,2
    110a:	fcf40ee3          	beq	s0,a5,10e6 <main+0xe>
    110e:	688c                	ld	a1,16(s1)
    1110:	00001517          	auipc	a0,0x1
    1114:	f2850513          	addi	a0,a0,-216 # 2038 <mntpoint>
    1118:	117000ef          	jal	ra,1a2e <strcpy>
    111c:	b7e9                	j	10e6 <main+0xe>

000000000000111e <__start_main>:
    111e:	85aa                	mv	a1,a0
    1120:	4108                	lw	a0,0(a0)
    1122:	1141                	addi	sp,sp,-16
    1124:	05a1                	addi	a1,a1,8
    1126:	e406                	sd	ra,8(sp)
    1128:	fb1ff0ef          	jal	ra,10d8 <main>
    112c:	41d000ef          	jal	ra,1d48 <exit>
    1130:	60a2                	ld	ra,8(sp)
    1132:	4501                	li	a0,0
    1134:	0141                	addi	sp,sp,16
    1136:	8082                	ret

0000000000001138 <printint.constprop.0>:
    1138:	7179                	addi	sp,sp,-48
    113a:	f406                	sd	ra,40(sp)
    113c:	12054b63          	bltz	a0,1272 <printint.constprop.0+0x13a>
    1140:	02b577bb          	remuw	a5,a0,a1
    1144:	00001617          	auipc	a2,0x1
    1148:	f8460613          	addi	a2,a2,-124 # 20c8 <digits>
    114c:	00010c23          	sb	zero,24(sp)
    1150:	0005871b          	sext.w	a4,a1
    1154:	1782                	slli	a5,a5,0x20
    1156:	9381                	srli	a5,a5,0x20
    1158:	97b2                	add	a5,a5,a2
    115a:	0007c783          	lbu	a5,0(a5)
    115e:	02b5583b          	divuw	a6,a0,a1
    1162:	00f10ba3          	sb	a5,23(sp)
    1166:	1cb56363          	bltu	a0,a1,132c <printint.constprop.0+0x1f4>
    116a:	45b9                	li	a1,14
    116c:	02e877bb          	remuw	a5,a6,a4
    1170:	1782                	slli	a5,a5,0x20
    1172:	9381                	srli	a5,a5,0x20
    1174:	97b2                	add	a5,a5,a2
    1176:	0007c783          	lbu	a5,0(a5)
    117a:	02e856bb          	divuw	a3,a6,a4
    117e:	00f10b23          	sb	a5,22(sp)
    1182:	0ce86e63          	bltu	a6,a4,125e <printint.constprop.0+0x126>
    1186:	02e6f5bb          	remuw	a1,a3,a4
    118a:	02e6d7bb          	divuw	a5,a3,a4
    118e:	1582                	slli	a1,a1,0x20
    1190:	9181                	srli	a1,a1,0x20
    1192:	95b2                	add	a1,a1,a2
    1194:	0005c583          	lbu	a1,0(a1)
    1198:	00b10aa3          	sb	a1,21(sp)
    119c:	0007859b          	sext.w	a1,a5
    11a0:	12e6ec63          	bltu	a3,a4,12d8 <printint.constprop.0+0x1a0>
    11a4:	02e7f6bb          	remuw	a3,a5,a4
    11a8:	1682                	slli	a3,a3,0x20
    11aa:	9281                	srli	a3,a3,0x20
    11ac:	96b2                	add	a3,a3,a2
    11ae:	0006c683          	lbu	a3,0(a3)
    11b2:	02e7d83b          	divuw	a6,a5,a4
    11b6:	00d10a23          	sb	a3,20(sp)
    11ba:	12e5e863          	bltu	a1,a4,12ea <printint.constprop.0+0x1b2>
    11be:	02e876bb          	remuw	a3,a6,a4
    11c2:	1682                	slli	a3,a3,0x20
    11c4:	9281                	srli	a3,a3,0x20
    11c6:	96b2                	add	a3,a3,a2
    11c8:	0006c683          	lbu	a3,0(a3)
    11cc:	02e855bb          	divuw	a1,a6,a4
    11d0:	00d109a3          	sb	a3,19(sp)
    11d4:	12e86463          	bltu	a6,a4,12fc <printint.constprop.0+0x1c4>
    11d8:	02e5f6bb          	remuw	a3,a1,a4
    11dc:	1682                	slli	a3,a3,0x20
    11de:	9281                	srli	a3,a3,0x20
    11e0:	96b2                	add	a3,a3,a2
    11e2:	0006c683          	lbu	a3,0(a3)
    11e6:	02e5d83b          	divuw	a6,a1,a4
    11ea:	00d10923          	sb	a3,18(sp)
    11ee:	0ce5ec63          	bltu	a1,a4,12c6 <printint.constprop.0+0x18e>
    11f2:	02e876bb          	remuw	a3,a6,a4
    11f6:	1682                	slli	a3,a3,0x20
    11f8:	9281                	srli	a3,a3,0x20
    11fa:	96b2                	add	a3,a3,a2
    11fc:	0006c683          	lbu	a3,0(a3)
    1200:	02e855bb          	divuw	a1,a6,a4
    1204:	00d108a3          	sb	a3,17(sp)
    1208:	10e86963          	bltu	a6,a4,131a <printint.constprop.0+0x1e2>
    120c:	02e5f6bb          	remuw	a3,a1,a4
    1210:	1682                	slli	a3,a3,0x20
    1212:	9281                	srli	a3,a3,0x20
    1214:	96b2                	add	a3,a3,a2
    1216:	0006c683          	lbu	a3,0(a3)
    121a:	02e5d83b          	divuw	a6,a1,a4
    121e:	00d10823          	sb	a3,16(sp)
    1222:	10e5e763          	bltu	a1,a4,1330 <printint.constprop.0+0x1f8>
    1226:	02e876bb          	remuw	a3,a6,a4
    122a:	1682                	slli	a3,a3,0x20
    122c:	9281                	srli	a3,a3,0x20
    122e:	96b2                	add	a3,a3,a2
    1230:	0006c683          	lbu	a3,0(a3)
    1234:	02e857bb          	divuw	a5,a6,a4
    1238:	00d107a3          	sb	a3,15(sp)
    123c:	10e86363          	bltu	a6,a4,1342 <printint.constprop.0+0x20a>
    1240:	1782                	slli	a5,a5,0x20
    1242:	9381                	srli	a5,a5,0x20
    1244:	97b2                	add	a5,a5,a2
    1246:	0007c783          	lbu	a5,0(a5)
    124a:	4599                	li	a1,6
    124c:	00f10723          	sb	a5,14(sp)
    1250:	00055763          	bgez	a0,125e <printint.constprop.0+0x126>
    1254:	02d00793          	li	a5,45
    1258:	00f106a3          	sb	a5,13(sp)
    125c:	4595                	li	a1,5
    125e:	003c                	addi	a5,sp,8
    1260:	4641                	li	a2,16
    1262:	9e0d                	subw	a2,a2,a1
    1264:	4505                	li	a0,1
    1266:	95be                	add	a1,a1,a5
    1268:	291000ef          	jal	ra,1cf8 <write>
    126c:	70a2                	ld	ra,40(sp)
    126e:	6145                	addi	sp,sp,48
    1270:	8082                	ret
    1272:	40a0083b          	negw	a6,a0
    1276:	02b877bb          	remuw	a5,a6,a1
    127a:	00001617          	auipc	a2,0x1
    127e:	e4e60613          	addi	a2,a2,-434 # 20c8 <digits>
    1282:	00010c23          	sb	zero,24(sp)
    1286:	0005871b          	sext.w	a4,a1
    128a:	1782                	slli	a5,a5,0x20
    128c:	9381                	srli	a5,a5,0x20
    128e:	97b2                	add	a5,a5,a2
    1290:	0007c783          	lbu	a5,0(a5)
    1294:	02b858bb          	divuw	a7,a6,a1
    1298:	00f10ba3          	sb	a5,23(sp)
    129c:	06b86963          	bltu	a6,a1,130e <printint.constprop.0+0x1d6>
    12a0:	02e8f7bb          	remuw	a5,a7,a4
    12a4:	1782                	slli	a5,a5,0x20
    12a6:	9381                	srli	a5,a5,0x20
    12a8:	97b2                	add	a5,a5,a2
    12aa:	0007c783          	lbu	a5,0(a5)
    12ae:	02e8d6bb          	divuw	a3,a7,a4
    12b2:	00f10b23          	sb	a5,22(sp)
    12b6:	ece8f8e3          	bgeu	a7,a4,1186 <printint.constprop.0+0x4e>
    12ba:	02d00793          	li	a5,45
    12be:	00f10aa3          	sb	a5,21(sp)
    12c2:	45b5                	li	a1,13
    12c4:	bf69                	j	125e <printint.constprop.0+0x126>
    12c6:	45a9                	li	a1,10
    12c8:	f8055be3          	bgez	a0,125e <printint.constprop.0+0x126>
    12cc:	02d00793          	li	a5,45
    12d0:	00f108a3          	sb	a5,17(sp)
    12d4:	45a5                	li	a1,9
    12d6:	b761                	j	125e <printint.constprop.0+0x126>
    12d8:	45b5                	li	a1,13
    12da:	f80552e3          	bgez	a0,125e <printint.constprop.0+0x126>
    12de:	02d00793          	li	a5,45
    12e2:	00f10a23          	sb	a5,20(sp)
    12e6:	45b1                	li	a1,12
    12e8:	bf9d                	j	125e <printint.constprop.0+0x126>
    12ea:	45b1                	li	a1,12
    12ec:	f60559e3          	bgez	a0,125e <printint.constprop.0+0x126>
    12f0:	02d00793          	li	a5,45
    12f4:	00f109a3          	sb	a5,19(sp)
    12f8:	45ad                	li	a1,11
    12fa:	b795                	j	125e <printint.constprop.0+0x126>
    12fc:	45ad                	li	a1,11
    12fe:	f60550e3          	bgez	a0,125e <printint.constprop.0+0x126>
    1302:	02d00793          	li	a5,45
    1306:	00f10923          	sb	a5,18(sp)
    130a:	45a9                	li	a1,10
    130c:	bf89                	j	125e <printint.constprop.0+0x126>
    130e:	02d00793          	li	a5,45
    1312:	00f10b23          	sb	a5,22(sp)
    1316:	45b9                	li	a1,14
    1318:	b799                	j	125e <printint.constprop.0+0x126>
    131a:	45a5                	li	a1,9
    131c:	f40551e3          	bgez	a0,125e <printint.constprop.0+0x126>
    1320:	02d00793          	li	a5,45
    1324:	00f10823          	sb	a5,16(sp)
    1328:	45a1                	li	a1,8
    132a:	bf15                	j	125e <printint.constprop.0+0x126>
    132c:	45bd                	li	a1,15
    132e:	bf05                	j	125e <printint.constprop.0+0x126>
    1330:	45a1                	li	a1,8
    1332:	f20556e3          	bgez	a0,125e <printint.constprop.0+0x126>
    1336:	02d00793          	li	a5,45
    133a:	00f107a3          	sb	a5,15(sp)
    133e:	459d                	li	a1,7
    1340:	bf39                	j	125e <printint.constprop.0+0x126>
    1342:	459d                	li	a1,7
    1344:	f0055de3          	bgez	a0,125e <printint.constprop.0+0x126>
    1348:	02d00793          	li	a5,45
    134c:	00f10723          	sb	a5,14(sp)
    1350:	4599                	li	a1,6
    1352:	b731                	j	125e <printint.constprop.0+0x126>

0000000000001354 <getchar>:
    1354:	1101                	addi	sp,sp,-32
    1356:	00f10593          	addi	a1,sp,15
    135a:	4605                	li	a2,1
    135c:	4501                	li	a0,0
    135e:	ec06                	sd	ra,24(sp)
    1360:	000107a3          	sb	zero,15(sp)
    1364:	18b000ef          	jal	ra,1cee <read>
    1368:	60e2                	ld	ra,24(sp)
    136a:	00f14503          	lbu	a0,15(sp)
    136e:	6105                	addi	sp,sp,32
    1370:	8082                	ret

0000000000001372 <putchar>:
    1372:	1101                	addi	sp,sp,-32
    1374:	87aa                	mv	a5,a0
    1376:	00f10593          	addi	a1,sp,15
    137a:	4605                	li	a2,1
    137c:	4505                	li	a0,1
    137e:	ec06                	sd	ra,24(sp)
    1380:	00f107a3          	sb	a5,15(sp)
    1384:	175000ef          	jal	ra,1cf8 <write>
    1388:	60e2                	ld	ra,24(sp)
    138a:	2501                	sext.w	a0,a0
    138c:	6105                	addi	sp,sp,32
    138e:	8082                	ret

0000000000001390 <puts>:
    1390:	1141                	addi	sp,sp,-16
    1392:	e406                	sd	ra,8(sp)
    1394:	e022                	sd	s0,0(sp)
    1396:	842a                	mv	s0,a0
    1398:	57c000ef          	jal	ra,1914 <strlen>
    139c:	862a                	mv	a2,a0
    139e:	85a2                	mv	a1,s0
    13a0:	4505                	li	a0,1
    13a2:	157000ef          	jal	ra,1cf8 <write>
    13a6:	60a2                	ld	ra,8(sp)
    13a8:	6402                	ld	s0,0(sp)
    13aa:	957d                	srai	a0,a0,0x3f
    13ac:	2501                	sext.w	a0,a0
    13ae:	0141                	addi	sp,sp,16
    13b0:	8082                	ret

00000000000013b2 <printf>:
    13b2:	7171                	addi	sp,sp,-176
    13b4:	fc56                	sd	s5,56(sp)
    13b6:	ed3e                	sd	a5,152(sp)
    13b8:	7ae1                	lui	s5,0xffff8
    13ba:	18bc                	addi	a5,sp,120
    13bc:	e8ca                	sd	s2,80(sp)
    13be:	e4ce                	sd	s3,72(sp)
    13c0:	e0d2                	sd	s4,64(sp)
    13c2:	f85a                	sd	s6,48(sp)
    13c4:	f486                	sd	ra,104(sp)
    13c6:	f0a2                	sd	s0,96(sp)
    13c8:	eca6                	sd	s1,88(sp)
    13ca:	fcae                	sd	a1,120(sp)
    13cc:	e132                	sd	a2,128(sp)
    13ce:	e536                	sd	a3,136(sp)
    13d0:	e93a                	sd	a4,144(sp)
    13d2:	f142                	sd	a6,160(sp)
    13d4:	f546                	sd	a7,168(sp)
    13d6:	e03e                	sd	a5,0(sp)
    13d8:	02500913          	li	s2,37
    13dc:	07300a13          	li	s4,115
    13e0:	00001b17          	auipc	s6,0x1
    13e4:	c38b0b13          	addi	s6,s6,-968 # 2018 <__clone+0xe8>
    13e8:	830aca93          	xori	s5,s5,-2000
    13ec:	00001997          	auipc	s3,0x1
    13f0:	cdc98993          	addi	s3,s3,-804 # 20c8 <digits>
    13f4:	00054783          	lbu	a5,0(a0)
    13f8:	16078a63          	beqz	a5,156c <printf+0x1ba>
    13fc:	862a                	mv	a2,a0
    13fe:	19278163          	beq	a5,s2,1580 <printf+0x1ce>
    1402:	00164783          	lbu	a5,1(a2)
    1406:	0605                	addi	a2,a2,1
    1408:	fbfd                	bnez	a5,13fe <printf+0x4c>
    140a:	84b2                	mv	s1,a2
    140c:	40a6043b          	subw	s0,a2,a0
    1410:	85aa                	mv	a1,a0
    1412:	8622                	mv	a2,s0
    1414:	4505                	li	a0,1
    1416:	0e3000ef          	jal	ra,1cf8 <write>
    141a:	18041c63          	bnez	s0,15b2 <printf+0x200>
    141e:	0014c783          	lbu	a5,1(s1)
    1422:	14078563          	beqz	a5,156c <printf+0x1ba>
    1426:	1d478063          	beq	a5,s4,15e6 <printf+0x234>
    142a:	18fa6663          	bltu	s4,a5,15b6 <printf+0x204>
    142e:	06400713          	li	a4,100
    1432:	1ae78063          	beq	a5,a4,15d2 <printf+0x220>
    1436:	07000713          	li	a4,112
    143a:	1ce79963          	bne	a5,a4,160c <printf+0x25a>
    143e:	6702                	ld	a4,0(sp)
    1440:	01511423          	sh	s5,8(sp)
    1444:	4649                	li	a2,18
    1446:	631c                	ld	a5,0(a4)
    1448:	0721                	addi	a4,a4,8
    144a:	e03a                	sd	a4,0(sp)
    144c:	00479293          	slli	t0,a5,0x4
    1450:	00879f93          	slli	t6,a5,0x8
    1454:	00c79f13          	slli	t5,a5,0xc
    1458:	01079e93          	slli	t4,a5,0x10
    145c:	01479e13          	slli	t3,a5,0x14
    1460:	01879313          	slli	t1,a5,0x18
    1464:	01c79893          	slli	a7,a5,0x1c
    1468:	02479813          	slli	a6,a5,0x24
    146c:	02879513          	slli	a0,a5,0x28
    1470:	02c79593          	slli	a1,a5,0x2c
    1474:	03079693          	slli	a3,a5,0x30
    1478:	03479713          	slli	a4,a5,0x34
    147c:	03c7d413          	srli	s0,a5,0x3c
    1480:	01c7d39b          	srliw	t2,a5,0x1c
    1484:	03c2d293          	srli	t0,t0,0x3c
    1488:	03cfdf93          	srli	t6,t6,0x3c
    148c:	03cf5f13          	srli	t5,t5,0x3c
    1490:	03cede93          	srli	t4,t4,0x3c
    1494:	03ce5e13          	srli	t3,t3,0x3c
    1498:	03c35313          	srli	t1,t1,0x3c
    149c:	03c8d893          	srli	a7,a7,0x3c
    14a0:	03c85813          	srli	a6,a6,0x3c
    14a4:	9171                	srli	a0,a0,0x3c
    14a6:	91f1                	srli	a1,a1,0x3c
    14a8:	92f1                	srli	a3,a3,0x3c
    14aa:	9371                	srli	a4,a4,0x3c
    14ac:	96ce                	add	a3,a3,s3
    14ae:	974e                	add	a4,a4,s3
    14b0:	944e                	add	s0,s0,s3
    14b2:	92ce                	add	t0,t0,s3
    14b4:	9fce                	add	t6,t6,s3
    14b6:	9f4e                	add	t5,t5,s3
    14b8:	9ece                	add	t4,t4,s3
    14ba:	9e4e                	add	t3,t3,s3
    14bc:	934e                	add	t1,t1,s3
    14be:	98ce                	add	a7,a7,s3
    14c0:	93ce                	add	t2,t2,s3
    14c2:	984e                	add	a6,a6,s3
    14c4:	954e                	add	a0,a0,s3
    14c6:	95ce                	add	a1,a1,s3
    14c8:	0006c083          	lbu	ra,0(a3)
    14cc:	0002c283          	lbu	t0,0(t0)
    14d0:	00074683          	lbu	a3,0(a4)
    14d4:	000fcf83          	lbu	t6,0(t6)
    14d8:	000f4f03          	lbu	t5,0(t5)
    14dc:	000ece83          	lbu	t4,0(t4)
    14e0:	000e4e03          	lbu	t3,0(t3)
    14e4:	00034303          	lbu	t1,0(t1)
    14e8:	0008c883          	lbu	a7,0(a7)
    14ec:	0003c383          	lbu	t2,0(t2)
    14f0:	00084803          	lbu	a6,0(a6)
    14f4:	00054503          	lbu	a0,0(a0)
    14f8:	0005c583          	lbu	a1,0(a1)
    14fc:	00044403          	lbu	s0,0(s0)
    1500:	03879713          	slli	a4,a5,0x38
    1504:	9371                	srli	a4,a4,0x3c
    1506:	8bbd                	andi	a5,a5,15
    1508:	974e                	add	a4,a4,s3
    150a:	97ce                	add	a5,a5,s3
    150c:	005105a3          	sb	t0,11(sp)
    1510:	01f10623          	sb	t6,12(sp)
    1514:	01e106a3          	sb	t5,13(sp)
    1518:	01d10723          	sb	t4,14(sp)
    151c:	01c107a3          	sb	t3,15(sp)
    1520:	00610823          	sb	t1,16(sp)
    1524:	011108a3          	sb	a7,17(sp)
    1528:	00710923          	sb	t2,18(sp)
    152c:	010109a3          	sb	a6,19(sp)
    1530:	00a10a23          	sb	a0,20(sp)
    1534:	00b10aa3          	sb	a1,21(sp)
    1538:	00110b23          	sb	ra,22(sp)
    153c:	00d10ba3          	sb	a3,23(sp)
    1540:	00810523          	sb	s0,10(sp)
    1544:	00074703          	lbu	a4,0(a4)
    1548:	0007c783          	lbu	a5,0(a5)
    154c:	002c                	addi	a1,sp,8
    154e:	4505                	li	a0,1
    1550:	00e10c23          	sb	a4,24(sp)
    1554:	00f10ca3          	sb	a5,25(sp)
    1558:	00010d23          	sb	zero,26(sp)
    155c:	79c000ef          	jal	ra,1cf8 <write>
    1560:	00248513          	addi	a0,s1,2
    1564:	00054783          	lbu	a5,0(a0)
    1568:	e8079ae3          	bnez	a5,13fc <printf+0x4a>
    156c:	70a6                	ld	ra,104(sp)
    156e:	7406                	ld	s0,96(sp)
    1570:	64e6                	ld	s1,88(sp)
    1572:	6946                	ld	s2,80(sp)
    1574:	69a6                	ld	s3,72(sp)
    1576:	6a06                	ld	s4,64(sp)
    1578:	7ae2                	ld	s5,56(sp)
    157a:	7b42                	ld	s6,48(sp)
    157c:	614d                	addi	sp,sp,176
    157e:	8082                	ret
    1580:	00064783          	lbu	a5,0(a2)
    1584:	84b2                	mv	s1,a2
    1586:	01278963          	beq	a5,s2,1598 <printf+0x1e6>
    158a:	b549                	j	140c <printf+0x5a>
    158c:	0024c783          	lbu	a5,2(s1)
    1590:	0605                	addi	a2,a2,1
    1592:	0489                	addi	s1,s1,2
    1594:	e7279ce3          	bne	a5,s2,140c <printf+0x5a>
    1598:	0014c783          	lbu	a5,1(s1)
    159c:	ff2788e3          	beq	a5,s2,158c <printf+0x1da>
    15a0:	40a6043b          	subw	s0,a2,a0
    15a4:	85aa                	mv	a1,a0
    15a6:	8622                	mv	a2,s0
    15a8:	4505                	li	a0,1
    15aa:	74e000ef          	jal	ra,1cf8 <write>
    15ae:	e60408e3          	beqz	s0,141e <printf+0x6c>
    15b2:	8526                	mv	a0,s1
    15b4:	b581                	j	13f4 <printf+0x42>
    15b6:	07800713          	li	a4,120
    15ba:	04e79963          	bne	a5,a4,160c <printf+0x25a>
    15be:	6782                	ld	a5,0(sp)
    15c0:	45c1                	li	a1,16
    15c2:	4388                	lw	a0,0(a5)
    15c4:	07a1                	addi	a5,a5,8
    15c6:	e03e                	sd	a5,0(sp)
    15c8:	b71ff0ef          	jal	ra,1138 <printint.constprop.0>
    15cc:	00248513          	addi	a0,s1,2
    15d0:	bf51                	j	1564 <printf+0x1b2>
    15d2:	6782                	ld	a5,0(sp)
    15d4:	45a9                	li	a1,10
    15d6:	4388                	lw	a0,0(a5)
    15d8:	07a1                	addi	a5,a5,8
    15da:	e03e                	sd	a5,0(sp)
    15dc:	b5dff0ef          	jal	ra,1138 <printint.constprop.0>
    15e0:	00248513          	addi	a0,s1,2
    15e4:	b741                	j	1564 <printf+0x1b2>
    15e6:	6782                	ld	a5,0(sp)
    15e8:	6380                	ld	s0,0(a5)
    15ea:	07a1                	addi	a5,a5,8
    15ec:	e03e                	sd	a5,0(sp)
    15ee:	c031                	beqz	s0,1632 <printf+0x280>
    15f0:	0c800593          	li	a1,200
    15f4:	8522                	mv	a0,s0
    15f6:	40a000ef          	jal	ra,1a00 <strnlen>
    15fa:	0005061b          	sext.w	a2,a0
    15fe:	85a2                	mv	a1,s0
    1600:	4505                	li	a0,1
    1602:	6f6000ef          	jal	ra,1cf8 <write>
    1606:	00248513          	addi	a0,s1,2
    160a:	bfa9                	j	1564 <printf+0x1b2>
    160c:	4605                	li	a2,1
    160e:	002c                	addi	a1,sp,8
    1610:	4505                	li	a0,1
    1612:	01210423          	sb	s2,8(sp)
    1616:	6e2000ef          	jal	ra,1cf8 <write>
    161a:	0014c783          	lbu	a5,1(s1)
    161e:	4605                	li	a2,1
    1620:	002c                	addi	a1,sp,8
    1622:	4505                	li	a0,1
    1624:	00f10423          	sb	a5,8(sp)
    1628:	6d0000ef          	jal	ra,1cf8 <write>
    162c:	00248513          	addi	a0,s1,2
    1630:	bf15                	j	1564 <printf+0x1b2>
    1632:	845a                	mv	s0,s6
    1634:	bf75                	j	15f0 <printf+0x23e>

0000000000001636 <panic>:
    1636:	1141                	addi	sp,sp,-16
    1638:	e406                	sd	ra,8(sp)
    163a:	d57ff0ef          	jal	ra,1390 <puts>
    163e:	60a2                	ld	ra,8(sp)
    1640:	f9c00513          	li	a0,-100
    1644:	0141                	addi	sp,sp,16
    1646:	a709                	j	1d48 <exit>

0000000000001648 <isspace>:
    1648:	02000793          	li	a5,32
    164c:	00f50663          	beq	a0,a5,1658 <isspace+0x10>
    1650:	355d                	addiw	a0,a0,-9
    1652:	00553513          	sltiu	a0,a0,5
    1656:	8082                	ret
    1658:	4505                	li	a0,1
    165a:	8082                	ret

000000000000165c <isdigit>:
    165c:	fd05051b          	addiw	a0,a0,-48
    1660:	00a53513          	sltiu	a0,a0,10
    1664:	8082                	ret

0000000000001666 <atoi>:
    1666:	02000613          	li	a2,32
    166a:	4591                	li	a1,4
    166c:	00054703          	lbu	a4,0(a0)
    1670:	ff77069b          	addiw	a3,a4,-9
    1674:	04c70d63          	beq	a4,a2,16ce <atoi+0x68>
    1678:	0007079b          	sext.w	a5,a4
    167c:	04d5f963          	bgeu	a1,a3,16ce <atoi+0x68>
    1680:	02b00693          	li	a3,43
    1684:	04d70a63          	beq	a4,a3,16d8 <atoi+0x72>
    1688:	02d00693          	li	a3,45
    168c:	06d70463          	beq	a4,a3,16f4 <atoi+0x8e>
    1690:	fd07859b          	addiw	a1,a5,-48
    1694:	4625                	li	a2,9
    1696:	873e                	mv	a4,a5
    1698:	86aa                	mv	a3,a0
    169a:	4e01                	li	t3,0
    169c:	04b66a63          	bltu	a2,a1,16f0 <atoi+0x8a>
    16a0:	4501                	li	a0,0
    16a2:	4825                	li	a6,9
    16a4:	0016c603          	lbu	a2,1(a3)
    16a8:	0025179b          	slliw	a5,a0,0x2
    16ac:	9d3d                	addw	a0,a0,a5
    16ae:	fd07031b          	addiw	t1,a4,-48
    16b2:	0015189b          	slliw	a7,a0,0x1
    16b6:	fd06059b          	addiw	a1,a2,-48
    16ba:	0685                	addi	a3,a3,1
    16bc:	4068853b          	subw	a0,a7,t1
    16c0:	0006071b          	sext.w	a4,a2
    16c4:	feb870e3          	bgeu	a6,a1,16a4 <atoi+0x3e>
    16c8:	000e0563          	beqz	t3,16d2 <atoi+0x6c>
    16cc:	8082                	ret
    16ce:	0505                	addi	a0,a0,1
    16d0:	bf71                	j	166c <atoi+0x6>
    16d2:	4113053b          	subw	a0,t1,a7
    16d6:	8082                	ret
    16d8:	00154783          	lbu	a5,1(a0)
    16dc:	4625                	li	a2,9
    16de:	00150693          	addi	a3,a0,1
    16e2:	fd07859b          	addiw	a1,a5,-48
    16e6:	0007871b          	sext.w	a4,a5
    16ea:	4e01                	li	t3,0
    16ec:	fab67ae3          	bgeu	a2,a1,16a0 <atoi+0x3a>
    16f0:	4501                	li	a0,0
    16f2:	8082                	ret
    16f4:	00154783          	lbu	a5,1(a0)
    16f8:	4625                	li	a2,9
    16fa:	00150693          	addi	a3,a0,1
    16fe:	fd07859b          	addiw	a1,a5,-48
    1702:	0007871b          	sext.w	a4,a5
    1706:	feb665e3          	bltu	a2,a1,16f0 <atoi+0x8a>
    170a:	4e05                	li	t3,1
    170c:	bf51                	j	16a0 <atoi+0x3a>

000000000000170e <memset>:
    170e:	16060d63          	beqz	a2,1888 <memset+0x17a>
    1712:	40a007b3          	neg	a5,a0
    1716:	8b9d                	andi	a5,a5,7
    1718:	00778713          	addi	a4,a5,7
    171c:	482d                	li	a6,11
    171e:	0ff5f593          	zext.b	a1,a1
    1722:	fff60693          	addi	a3,a2,-1
    1726:	17076263          	bltu	a4,a6,188a <memset+0x17c>
    172a:	16e6ea63          	bltu	a3,a4,189e <memset+0x190>
    172e:	16078563          	beqz	a5,1898 <memset+0x18a>
    1732:	00b50023          	sb	a1,0(a0)
    1736:	4705                	li	a4,1
    1738:	00150e93          	addi	t4,a0,1
    173c:	14e78c63          	beq	a5,a4,1894 <memset+0x186>
    1740:	00b500a3          	sb	a1,1(a0)
    1744:	4709                	li	a4,2
    1746:	00250e93          	addi	t4,a0,2
    174a:	14e78d63          	beq	a5,a4,18a4 <memset+0x196>
    174e:	00b50123          	sb	a1,2(a0)
    1752:	470d                	li	a4,3
    1754:	00350e93          	addi	t4,a0,3
    1758:	12e78b63          	beq	a5,a4,188e <memset+0x180>
    175c:	00b501a3          	sb	a1,3(a0)
    1760:	4711                	li	a4,4
    1762:	00450e93          	addi	t4,a0,4
    1766:	14e78163          	beq	a5,a4,18a8 <memset+0x19a>
    176a:	00b50223          	sb	a1,4(a0)
    176e:	4715                	li	a4,5
    1770:	00550e93          	addi	t4,a0,5
    1774:	12e78c63          	beq	a5,a4,18ac <memset+0x19e>
    1778:	00b502a3          	sb	a1,5(a0)
    177c:	471d                	li	a4,7
    177e:	00650e93          	addi	t4,a0,6
    1782:	12e79763          	bne	a5,a4,18b0 <memset+0x1a2>
    1786:	00750e93          	addi	t4,a0,7
    178a:	00b50323          	sb	a1,6(a0)
    178e:	4f1d                	li	t5,7
    1790:	00859713          	slli	a4,a1,0x8
    1794:	8f4d                	or	a4,a4,a1
    1796:	01059e13          	slli	t3,a1,0x10
    179a:	01c76e33          	or	t3,a4,t3
    179e:	01859313          	slli	t1,a1,0x18
    17a2:	006e6333          	or	t1,t3,t1
    17a6:	02059893          	slli	a7,a1,0x20
    17aa:	011368b3          	or	a7,t1,a7
    17ae:	02859813          	slli	a6,a1,0x28
    17b2:	40f60333          	sub	t1,a2,a5
    17b6:	0108e833          	or	a6,a7,a6
    17ba:	03059693          	slli	a3,a1,0x30
    17be:	00d866b3          	or	a3,a6,a3
    17c2:	03859713          	slli	a4,a1,0x38
    17c6:	97aa                	add	a5,a5,a0
    17c8:	ff837813          	andi	a6,t1,-8
    17cc:	8f55                	or	a4,a4,a3
    17ce:	00f806b3          	add	a3,a6,a5
    17d2:	e398                	sd	a4,0(a5)
    17d4:	07a1                	addi	a5,a5,8
    17d6:	fed79ee3          	bne	a5,a3,17d2 <memset+0xc4>
    17da:	ff837693          	andi	a3,t1,-8
    17de:	00de87b3          	add	a5,t4,a3
    17e2:	01e6873b          	addw	a4,a3,t5
    17e6:	0ad30663          	beq	t1,a3,1892 <memset+0x184>
    17ea:	00b78023          	sb	a1,0(a5)
    17ee:	0017069b          	addiw	a3,a4,1
    17f2:	08c6fb63          	bgeu	a3,a2,1888 <memset+0x17a>
    17f6:	00b780a3          	sb	a1,1(a5)
    17fa:	0027069b          	addiw	a3,a4,2
    17fe:	08c6f563          	bgeu	a3,a2,1888 <memset+0x17a>
    1802:	00b78123          	sb	a1,2(a5)
    1806:	0037069b          	addiw	a3,a4,3
    180a:	06c6ff63          	bgeu	a3,a2,1888 <memset+0x17a>
    180e:	00b781a3          	sb	a1,3(a5)
    1812:	0047069b          	addiw	a3,a4,4
    1816:	06c6f963          	bgeu	a3,a2,1888 <memset+0x17a>
    181a:	00b78223          	sb	a1,4(a5)
    181e:	0057069b          	addiw	a3,a4,5
    1822:	06c6f363          	bgeu	a3,a2,1888 <memset+0x17a>
    1826:	00b782a3          	sb	a1,5(a5)
    182a:	0067069b          	addiw	a3,a4,6
    182e:	04c6fd63          	bgeu	a3,a2,1888 <memset+0x17a>
    1832:	00b78323          	sb	a1,6(a5)
    1836:	0077069b          	addiw	a3,a4,7
    183a:	04c6f763          	bgeu	a3,a2,1888 <memset+0x17a>
    183e:	00b783a3          	sb	a1,7(a5)
    1842:	0087069b          	addiw	a3,a4,8
    1846:	04c6f163          	bgeu	a3,a2,1888 <memset+0x17a>
    184a:	00b78423          	sb	a1,8(a5)
    184e:	0097069b          	addiw	a3,a4,9
    1852:	02c6fb63          	bgeu	a3,a2,1888 <memset+0x17a>
    1856:	00b784a3          	sb	a1,9(a5)
    185a:	00a7069b          	addiw	a3,a4,10
    185e:	02c6f563          	bgeu	a3,a2,1888 <memset+0x17a>
    1862:	00b78523          	sb	a1,10(a5)
    1866:	00b7069b          	addiw	a3,a4,11
    186a:	00c6ff63          	bgeu	a3,a2,1888 <memset+0x17a>
    186e:	00b785a3          	sb	a1,11(a5)
    1872:	00c7069b          	addiw	a3,a4,12
    1876:	00c6f963          	bgeu	a3,a2,1888 <memset+0x17a>
    187a:	00b78623          	sb	a1,12(a5)
    187e:	2735                	addiw	a4,a4,13
    1880:	00c77463          	bgeu	a4,a2,1888 <memset+0x17a>
    1884:	00b786a3          	sb	a1,13(a5)
    1888:	8082                	ret
    188a:	472d                	li	a4,11
    188c:	bd79                	j	172a <memset+0x1c>
    188e:	4f0d                	li	t5,3
    1890:	b701                	j	1790 <memset+0x82>
    1892:	8082                	ret
    1894:	4f05                	li	t5,1
    1896:	bded                	j	1790 <memset+0x82>
    1898:	8eaa                	mv	t4,a0
    189a:	4f01                	li	t5,0
    189c:	bdd5                	j	1790 <memset+0x82>
    189e:	87aa                	mv	a5,a0
    18a0:	4701                	li	a4,0
    18a2:	b7a1                	j	17ea <memset+0xdc>
    18a4:	4f09                	li	t5,2
    18a6:	b5ed                	j	1790 <memset+0x82>
    18a8:	4f11                	li	t5,4
    18aa:	b5dd                	j	1790 <memset+0x82>
    18ac:	4f15                	li	t5,5
    18ae:	b5cd                	j	1790 <memset+0x82>
    18b0:	4f19                	li	t5,6
    18b2:	bdf9                	j	1790 <memset+0x82>

00000000000018b4 <strcmp>:
    18b4:	00054783          	lbu	a5,0(a0)
    18b8:	0005c703          	lbu	a4,0(a1)
    18bc:	00e79863          	bne	a5,a4,18cc <strcmp+0x18>
    18c0:	0505                	addi	a0,a0,1
    18c2:	0585                	addi	a1,a1,1
    18c4:	fbe5                	bnez	a5,18b4 <strcmp>
    18c6:	4501                	li	a0,0
    18c8:	9d19                	subw	a0,a0,a4
    18ca:	8082                	ret
    18cc:	0007851b          	sext.w	a0,a5
    18d0:	bfe5                	j	18c8 <strcmp+0x14>

00000000000018d2 <strncmp>:
    18d2:	ce05                	beqz	a2,190a <strncmp+0x38>
    18d4:	00054703          	lbu	a4,0(a0)
    18d8:	0005c783          	lbu	a5,0(a1)
    18dc:	cb0d                	beqz	a4,190e <strncmp+0x3c>
    18de:	167d                	addi	a2,a2,-1
    18e0:	00c506b3          	add	a3,a0,a2
    18e4:	a819                	j	18fa <strncmp+0x28>
    18e6:	00a68e63          	beq	a3,a0,1902 <strncmp+0x30>
    18ea:	0505                	addi	a0,a0,1
    18ec:	00e79b63          	bne	a5,a4,1902 <strncmp+0x30>
    18f0:	00054703          	lbu	a4,0(a0)
    18f4:	0005c783          	lbu	a5,0(a1)
    18f8:	cb19                	beqz	a4,190e <strncmp+0x3c>
    18fa:	0005c783          	lbu	a5,0(a1)
    18fe:	0585                	addi	a1,a1,1
    1900:	f3fd                	bnez	a5,18e6 <strncmp+0x14>
    1902:	0007051b          	sext.w	a0,a4
    1906:	9d1d                	subw	a0,a0,a5
    1908:	8082                	ret
    190a:	4501                	li	a0,0
    190c:	8082                	ret
    190e:	4501                	li	a0,0
    1910:	9d1d                	subw	a0,a0,a5
    1912:	8082                	ret

0000000000001914 <strlen>:
    1914:	00757793          	andi	a5,a0,7
    1918:	cf89                	beqz	a5,1932 <strlen+0x1e>
    191a:	87aa                	mv	a5,a0
    191c:	a029                	j	1926 <strlen+0x12>
    191e:	0785                	addi	a5,a5,1
    1920:	0077f713          	andi	a4,a5,7
    1924:	cb01                	beqz	a4,1934 <strlen+0x20>
    1926:	0007c703          	lbu	a4,0(a5)
    192a:	fb75                	bnez	a4,191e <strlen+0xa>
    192c:	40a78533          	sub	a0,a5,a0
    1930:	8082                	ret
    1932:	87aa                	mv	a5,a0
    1934:	6394                	ld	a3,0(a5)
    1936:	00000597          	auipc	a1,0x0
    193a:	6ea5b583          	ld	a1,1770(a1) # 2020 <__clone+0xf0>
    193e:	00000617          	auipc	a2,0x0
    1942:	6ea63603          	ld	a2,1770(a2) # 2028 <__clone+0xf8>
    1946:	a019                	j	194c <strlen+0x38>
    1948:	6794                	ld	a3,8(a5)
    194a:	07a1                	addi	a5,a5,8
    194c:	00b68733          	add	a4,a3,a1
    1950:	fff6c693          	not	a3,a3
    1954:	8f75                	and	a4,a4,a3
    1956:	8f71                	and	a4,a4,a2
    1958:	db65                	beqz	a4,1948 <strlen+0x34>
    195a:	0007c703          	lbu	a4,0(a5)
    195e:	d779                	beqz	a4,192c <strlen+0x18>
    1960:	0017c703          	lbu	a4,1(a5)
    1964:	0785                	addi	a5,a5,1
    1966:	d379                	beqz	a4,192c <strlen+0x18>
    1968:	0017c703          	lbu	a4,1(a5)
    196c:	0785                	addi	a5,a5,1
    196e:	fb6d                	bnez	a4,1960 <strlen+0x4c>
    1970:	bf75                	j	192c <strlen+0x18>

0000000000001972 <memchr>:
    1972:	00757713          	andi	a4,a0,7
    1976:	87aa                	mv	a5,a0
    1978:	0ff5f593          	zext.b	a1,a1
    197c:	cb19                	beqz	a4,1992 <memchr+0x20>
    197e:	ce25                	beqz	a2,19f6 <memchr+0x84>
    1980:	0007c703          	lbu	a4,0(a5)
    1984:	04b70e63          	beq	a4,a1,19e0 <memchr+0x6e>
    1988:	0785                	addi	a5,a5,1
    198a:	0077f713          	andi	a4,a5,7
    198e:	167d                	addi	a2,a2,-1
    1990:	f77d                	bnez	a4,197e <memchr+0xc>
    1992:	4501                	li	a0,0
    1994:	c235                	beqz	a2,19f8 <memchr+0x86>
    1996:	0007c703          	lbu	a4,0(a5)
    199a:	04b70363          	beq	a4,a1,19e0 <memchr+0x6e>
    199e:	00000517          	auipc	a0,0x0
    19a2:	69253503          	ld	a0,1682(a0) # 2030 <__clone+0x100>
    19a6:	471d                	li	a4,7
    19a8:	02a58533          	mul	a0,a1,a0
    19ac:	02c77a63          	bgeu	a4,a2,19e0 <memchr+0x6e>
    19b0:	00000897          	auipc	a7,0x0
    19b4:	6708b883          	ld	a7,1648(a7) # 2020 <__clone+0xf0>
    19b8:	00000817          	auipc	a6,0x0
    19bc:	67083803          	ld	a6,1648(a6) # 2028 <__clone+0xf8>
    19c0:	431d                	li	t1,7
    19c2:	a029                	j	19cc <memchr+0x5a>
    19c4:	1661                	addi	a2,a2,-8
    19c6:	07a1                	addi	a5,a5,8
    19c8:	02c37963          	bgeu	t1,a2,19fa <memchr+0x88>
    19cc:	6398                	ld	a4,0(a5)
    19ce:	8f29                	xor	a4,a4,a0
    19d0:	011706b3          	add	a3,a4,a7
    19d4:	fff74713          	not	a4,a4
    19d8:	8f75                	and	a4,a4,a3
    19da:	01077733          	and	a4,a4,a6
    19de:	d37d                	beqz	a4,19c4 <memchr+0x52>
    19e0:	853e                	mv	a0,a5
    19e2:	97b2                	add	a5,a5,a2
    19e4:	a021                	j	19ec <memchr+0x7a>
    19e6:	0505                	addi	a0,a0,1
    19e8:	00f50763          	beq	a0,a5,19f6 <memchr+0x84>
    19ec:	00054703          	lbu	a4,0(a0)
    19f0:	feb71be3          	bne	a4,a1,19e6 <memchr+0x74>
    19f4:	8082                	ret
    19f6:	4501                	li	a0,0
    19f8:	8082                	ret
    19fa:	4501                	li	a0,0
    19fc:	f275                	bnez	a2,19e0 <memchr+0x6e>
    19fe:	8082                	ret

0000000000001a00 <strnlen>:
    1a00:	1101                	addi	sp,sp,-32
    1a02:	e822                	sd	s0,16(sp)
    1a04:	862e                	mv	a2,a1
    1a06:	842e                	mv	s0,a1
    1a08:	4581                	li	a1,0
    1a0a:	e426                	sd	s1,8(sp)
    1a0c:	ec06                	sd	ra,24(sp)
    1a0e:	84aa                	mv	s1,a0
    1a10:	f63ff0ef          	jal	ra,1972 <memchr>
    1a14:	c519                	beqz	a0,1a22 <strnlen+0x22>
    1a16:	60e2                	ld	ra,24(sp)
    1a18:	6442                	ld	s0,16(sp)
    1a1a:	8d05                	sub	a0,a0,s1
    1a1c:	64a2                	ld	s1,8(sp)
    1a1e:	6105                	addi	sp,sp,32
    1a20:	8082                	ret
    1a22:	60e2                	ld	ra,24(sp)
    1a24:	8522                	mv	a0,s0
    1a26:	6442                	ld	s0,16(sp)
    1a28:	64a2                	ld	s1,8(sp)
    1a2a:	6105                	addi	sp,sp,32
    1a2c:	8082                	ret

0000000000001a2e <strcpy>:
    1a2e:	00b547b3          	xor	a5,a0,a1
    1a32:	8b9d                	andi	a5,a5,7
    1a34:	eb95                	bnez	a5,1a68 <strcpy+0x3a>
    1a36:	0075f793          	andi	a5,a1,7
    1a3a:	e7b1                	bnez	a5,1a86 <strcpy+0x58>
    1a3c:	6198                	ld	a4,0(a1)
    1a3e:	00000617          	auipc	a2,0x0
    1a42:	5e263603          	ld	a2,1506(a2) # 2020 <__clone+0xf0>
    1a46:	00000817          	auipc	a6,0x0
    1a4a:	5e283803          	ld	a6,1506(a6) # 2028 <__clone+0xf8>
    1a4e:	a029                	j	1a58 <strcpy+0x2a>
    1a50:	e118                	sd	a4,0(a0)
    1a52:	6598                	ld	a4,8(a1)
    1a54:	05a1                	addi	a1,a1,8
    1a56:	0521                	addi	a0,a0,8
    1a58:	00c707b3          	add	a5,a4,a2
    1a5c:	fff74693          	not	a3,a4
    1a60:	8ff5                	and	a5,a5,a3
    1a62:	0107f7b3          	and	a5,a5,a6
    1a66:	d7ed                	beqz	a5,1a50 <strcpy+0x22>
    1a68:	0005c783          	lbu	a5,0(a1)
    1a6c:	00f50023          	sb	a5,0(a0)
    1a70:	c785                	beqz	a5,1a98 <strcpy+0x6a>
    1a72:	0015c783          	lbu	a5,1(a1)
    1a76:	0505                	addi	a0,a0,1
    1a78:	0585                	addi	a1,a1,1
    1a7a:	00f50023          	sb	a5,0(a0)
    1a7e:	fbf5                	bnez	a5,1a72 <strcpy+0x44>
    1a80:	8082                	ret
    1a82:	0505                	addi	a0,a0,1
    1a84:	df45                	beqz	a4,1a3c <strcpy+0xe>
    1a86:	0005c783          	lbu	a5,0(a1)
    1a8a:	0585                	addi	a1,a1,1
    1a8c:	0075f713          	andi	a4,a1,7
    1a90:	00f50023          	sb	a5,0(a0)
    1a94:	f7fd                	bnez	a5,1a82 <strcpy+0x54>
    1a96:	8082                	ret
    1a98:	8082                	ret

0000000000001a9a <strncpy>:
    1a9a:	00b547b3          	xor	a5,a0,a1
    1a9e:	8b9d                	andi	a5,a5,7
    1aa0:	1a079863          	bnez	a5,1c50 <strncpy+0x1b6>
    1aa4:	0075f793          	andi	a5,a1,7
    1aa8:	16078463          	beqz	a5,1c10 <strncpy+0x176>
    1aac:	ea01                	bnez	a2,1abc <strncpy+0x22>
    1aae:	a421                	j	1cb6 <strncpy+0x21c>
    1ab0:	167d                	addi	a2,a2,-1
    1ab2:	0505                	addi	a0,a0,1
    1ab4:	14070e63          	beqz	a4,1c10 <strncpy+0x176>
    1ab8:	1a060863          	beqz	a2,1c68 <strncpy+0x1ce>
    1abc:	0005c783          	lbu	a5,0(a1)
    1ac0:	0585                	addi	a1,a1,1
    1ac2:	0075f713          	andi	a4,a1,7
    1ac6:	00f50023          	sb	a5,0(a0)
    1aca:	f3fd                	bnez	a5,1ab0 <strncpy+0x16>
    1acc:	4805                	li	a6,1
    1ace:	1a061863          	bnez	a2,1c7e <strncpy+0x1e4>
    1ad2:	40a007b3          	neg	a5,a0
    1ad6:	8b9d                	andi	a5,a5,7
    1ad8:	4681                	li	a3,0
    1ada:	18061a63          	bnez	a2,1c6e <strncpy+0x1d4>
    1ade:	00778713          	addi	a4,a5,7
    1ae2:	45ad                	li	a1,11
    1ae4:	18b76363          	bltu	a4,a1,1c6a <strncpy+0x1d0>
    1ae8:	1ae6eb63          	bltu	a3,a4,1c9e <strncpy+0x204>
    1aec:	1a078363          	beqz	a5,1c92 <strncpy+0x1f8>
    1af0:	00050023          	sb	zero,0(a0)
    1af4:	4685                	li	a3,1
    1af6:	00150713          	addi	a4,a0,1
    1afa:	18d78f63          	beq	a5,a3,1c98 <strncpy+0x1fe>
    1afe:	000500a3          	sb	zero,1(a0)
    1b02:	4689                	li	a3,2
    1b04:	00250713          	addi	a4,a0,2
    1b08:	18d78e63          	beq	a5,a3,1ca4 <strncpy+0x20a>
    1b0c:	00050123          	sb	zero,2(a0)
    1b10:	468d                	li	a3,3
    1b12:	00350713          	addi	a4,a0,3
    1b16:	16d78c63          	beq	a5,a3,1c8e <strncpy+0x1f4>
    1b1a:	000501a3          	sb	zero,3(a0)
    1b1e:	4691                	li	a3,4
    1b20:	00450713          	addi	a4,a0,4
    1b24:	18d78263          	beq	a5,a3,1ca8 <strncpy+0x20e>
    1b28:	00050223          	sb	zero,4(a0)
    1b2c:	4695                	li	a3,5
    1b2e:	00550713          	addi	a4,a0,5
    1b32:	16d78d63          	beq	a5,a3,1cac <strncpy+0x212>
    1b36:	000502a3          	sb	zero,5(a0)
    1b3a:	469d                	li	a3,7
    1b3c:	00650713          	addi	a4,a0,6
    1b40:	16d79863          	bne	a5,a3,1cb0 <strncpy+0x216>
    1b44:	00750713          	addi	a4,a0,7
    1b48:	00050323          	sb	zero,6(a0)
    1b4c:	40f80833          	sub	a6,a6,a5
    1b50:	ff887593          	andi	a1,a6,-8
    1b54:	97aa                	add	a5,a5,a0
    1b56:	95be                	add	a1,a1,a5
    1b58:	0007b023          	sd	zero,0(a5)
    1b5c:	07a1                	addi	a5,a5,8
    1b5e:	feb79de3          	bne	a5,a1,1b58 <strncpy+0xbe>
    1b62:	ff887593          	andi	a1,a6,-8
    1b66:	9ead                	addw	a3,a3,a1
    1b68:	00b707b3          	add	a5,a4,a1
    1b6c:	12b80863          	beq	a6,a1,1c9c <strncpy+0x202>
    1b70:	00078023          	sb	zero,0(a5)
    1b74:	0016871b          	addiw	a4,a3,1
    1b78:	0ec77863          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1b7c:	000780a3          	sb	zero,1(a5)
    1b80:	0026871b          	addiw	a4,a3,2
    1b84:	0ec77263          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1b88:	00078123          	sb	zero,2(a5)
    1b8c:	0036871b          	addiw	a4,a3,3
    1b90:	0cc77c63          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1b94:	000781a3          	sb	zero,3(a5)
    1b98:	0046871b          	addiw	a4,a3,4
    1b9c:	0cc77663          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1ba0:	00078223          	sb	zero,4(a5)
    1ba4:	0056871b          	addiw	a4,a3,5
    1ba8:	0cc77063          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1bac:	000782a3          	sb	zero,5(a5)
    1bb0:	0066871b          	addiw	a4,a3,6
    1bb4:	0ac77a63          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1bb8:	00078323          	sb	zero,6(a5)
    1bbc:	0076871b          	addiw	a4,a3,7
    1bc0:	0ac77463          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1bc4:	000783a3          	sb	zero,7(a5)
    1bc8:	0086871b          	addiw	a4,a3,8
    1bcc:	08c77e63          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1bd0:	00078423          	sb	zero,8(a5)
    1bd4:	0096871b          	addiw	a4,a3,9
    1bd8:	08c77863          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1bdc:	000784a3          	sb	zero,9(a5)
    1be0:	00a6871b          	addiw	a4,a3,10
    1be4:	08c77263          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1be8:	00078523          	sb	zero,10(a5)
    1bec:	00b6871b          	addiw	a4,a3,11
    1bf0:	06c77c63          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1bf4:	000785a3          	sb	zero,11(a5)
    1bf8:	00c6871b          	addiw	a4,a3,12
    1bfc:	06c77663          	bgeu	a4,a2,1c68 <strncpy+0x1ce>
    1c00:	00078623          	sb	zero,12(a5)
    1c04:	26b5                	addiw	a3,a3,13
    1c06:	06c6f163          	bgeu	a3,a2,1c68 <strncpy+0x1ce>
    1c0a:	000786a3          	sb	zero,13(a5)
    1c0e:	8082                	ret
    1c10:	c645                	beqz	a2,1cb8 <strncpy+0x21e>
    1c12:	0005c783          	lbu	a5,0(a1)
    1c16:	ea078be3          	beqz	a5,1acc <strncpy+0x32>
    1c1a:	479d                	li	a5,7
    1c1c:	02c7ff63          	bgeu	a5,a2,1c5a <strncpy+0x1c0>
    1c20:	00000897          	auipc	a7,0x0
    1c24:	4008b883          	ld	a7,1024(a7) # 2020 <__clone+0xf0>
    1c28:	00000817          	auipc	a6,0x0
    1c2c:	40083803          	ld	a6,1024(a6) # 2028 <__clone+0xf8>
    1c30:	431d                	li	t1,7
    1c32:	6198                	ld	a4,0(a1)
    1c34:	011707b3          	add	a5,a4,a7
    1c38:	fff74693          	not	a3,a4
    1c3c:	8ff5                	and	a5,a5,a3
    1c3e:	0107f7b3          	and	a5,a5,a6
    1c42:	ef81                	bnez	a5,1c5a <strncpy+0x1c0>
    1c44:	e118                	sd	a4,0(a0)
    1c46:	1661                	addi	a2,a2,-8
    1c48:	05a1                	addi	a1,a1,8
    1c4a:	0521                	addi	a0,a0,8
    1c4c:	fec363e3          	bltu	t1,a2,1c32 <strncpy+0x198>
    1c50:	e609                	bnez	a2,1c5a <strncpy+0x1c0>
    1c52:	a08d                	j	1cb4 <strncpy+0x21a>
    1c54:	167d                	addi	a2,a2,-1
    1c56:	0505                	addi	a0,a0,1
    1c58:	ca01                	beqz	a2,1c68 <strncpy+0x1ce>
    1c5a:	0005c783          	lbu	a5,0(a1)
    1c5e:	0585                	addi	a1,a1,1
    1c60:	00f50023          	sb	a5,0(a0)
    1c64:	fbe5                	bnez	a5,1c54 <strncpy+0x1ba>
    1c66:	b59d                	j	1acc <strncpy+0x32>
    1c68:	8082                	ret
    1c6a:	472d                	li	a4,11
    1c6c:	bdb5                	j	1ae8 <strncpy+0x4e>
    1c6e:	00778713          	addi	a4,a5,7
    1c72:	45ad                	li	a1,11
    1c74:	fff60693          	addi	a3,a2,-1
    1c78:	e6b778e3          	bgeu	a4,a1,1ae8 <strncpy+0x4e>
    1c7c:	b7fd                	j	1c6a <strncpy+0x1d0>
    1c7e:	40a007b3          	neg	a5,a0
    1c82:	8832                	mv	a6,a2
    1c84:	8b9d                	andi	a5,a5,7
    1c86:	4681                	li	a3,0
    1c88:	e4060be3          	beqz	a2,1ade <strncpy+0x44>
    1c8c:	b7cd                	j	1c6e <strncpy+0x1d4>
    1c8e:	468d                	li	a3,3
    1c90:	bd75                	j	1b4c <strncpy+0xb2>
    1c92:	872a                	mv	a4,a0
    1c94:	4681                	li	a3,0
    1c96:	bd5d                	j	1b4c <strncpy+0xb2>
    1c98:	4685                	li	a3,1
    1c9a:	bd4d                	j	1b4c <strncpy+0xb2>
    1c9c:	8082                	ret
    1c9e:	87aa                	mv	a5,a0
    1ca0:	4681                	li	a3,0
    1ca2:	b5f9                	j	1b70 <strncpy+0xd6>
    1ca4:	4689                	li	a3,2
    1ca6:	b55d                	j	1b4c <strncpy+0xb2>
    1ca8:	4691                	li	a3,4
    1caa:	b54d                	j	1b4c <strncpy+0xb2>
    1cac:	4695                	li	a3,5
    1cae:	bd79                	j	1b4c <strncpy+0xb2>
    1cb0:	4699                	li	a3,6
    1cb2:	bd69                	j	1b4c <strncpy+0xb2>
    1cb4:	8082                	ret
    1cb6:	8082                	ret
    1cb8:	8082                	ret

0000000000001cba <open>:
    1cba:	87aa                	mv	a5,a0
    1cbc:	862e                	mv	a2,a1
    1cbe:	03800893          	li	a7,56
    1cc2:	f9c00513          	li	a0,-100
    1cc6:	85be                	mv	a1,a5
    1cc8:	4689                	li	a3,2
    1cca:	00000073          	ecall
    1cce:	2501                	sext.w	a0,a0
    1cd0:	8082                	ret

0000000000001cd2 <openat>:
    1cd2:	03800893          	li	a7,56
    1cd6:	18000693          	li	a3,384
    1cda:	00000073          	ecall
    1cde:	2501                	sext.w	a0,a0
    1ce0:	8082                	ret

0000000000001ce2 <close>:
    1ce2:	03900893          	li	a7,57
    1ce6:	00000073          	ecall
    1cea:	2501                	sext.w	a0,a0
    1cec:	8082                	ret

0000000000001cee <read>:
    1cee:	03f00893          	li	a7,63
    1cf2:	00000073          	ecall
    1cf6:	8082                	ret

0000000000001cf8 <write>:
    1cf8:	04000893          	li	a7,64
    1cfc:	00000073          	ecall
    1d00:	8082                	ret

0000000000001d02 <getpid>:
    1d02:	0ac00893          	li	a7,172
    1d06:	00000073          	ecall
    1d0a:	2501                	sext.w	a0,a0
    1d0c:	8082                	ret

0000000000001d0e <getppid>:
    1d0e:	0ad00893          	li	a7,173
    1d12:	00000073          	ecall
    1d16:	2501                	sext.w	a0,a0
    1d18:	8082                	ret

0000000000001d1a <sched_yield>:
    1d1a:	07c00893          	li	a7,124
    1d1e:	00000073          	ecall
    1d22:	2501                	sext.w	a0,a0
    1d24:	8082                	ret

0000000000001d26 <fork>:
    1d26:	0dc00893          	li	a7,220
    1d2a:	4545                	li	a0,17
    1d2c:	4581                	li	a1,0
    1d2e:	00000073          	ecall
    1d32:	2501                	sext.w	a0,a0
    1d34:	8082                	ret

0000000000001d36 <clone>:
    1d36:	85b2                	mv	a1,a2
    1d38:	863a                	mv	a2,a4
    1d3a:	c191                	beqz	a1,1d3e <clone+0x8>
    1d3c:	95b6                	add	a1,a1,a3
    1d3e:	4781                	li	a5,0
    1d40:	4701                	li	a4,0
    1d42:	4681                	li	a3,0
    1d44:	2601                	sext.w	a2,a2
    1d46:	a2ed                	j	1f30 <__clone>

0000000000001d48 <exit>:
    1d48:	05d00893          	li	a7,93
    1d4c:	00000073          	ecall
    1d50:	8082                	ret

0000000000001d52 <waitpid>:
    1d52:	10400893          	li	a7,260
    1d56:	4681                	li	a3,0
    1d58:	00000073          	ecall
    1d5c:	2501                	sext.w	a0,a0
    1d5e:	8082                	ret

0000000000001d60 <exec>:
    1d60:	0dd00893          	li	a7,221
    1d64:	00000073          	ecall
    1d68:	2501                	sext.w	a0,a0
    1d6a:	8082                	ret

0000000000001d6c <execve>:
    1d6c:	0dd00893          	li	a7,221
    1d70:	00000073          	ecall
    1d74:	2501                	sext.w	a0,a0
    1d76:	8082                	ret

0000000000001d78 <times>:
    1d78:	09900893          	li	a7,153
    1d7c:	00000073          	ecall
    1d80:	2501                	sext.w	a0,a0
    1d82:	8082                	ret

0000000000001d84 <get_time>:
    1d84:	1141                	addi	sp,sp,-16
    1d86:	0a900893          	li	a7,169
    1d8a:	850a                	mv	a0,sp
    1d8c:	4581                	li	a1,0
    1d8e:	00000073          	ecall
    1d92:	2501                	sext.w	a0,a0
    1d94:	ed09                	bnez	a0,1dae <get_time+0x2a>
    1d96:	67a2                	ld	a5,8(sp)
    1d98:	3e800713          	li	a4,1000
    1d9c:	00015503          	lhu	a0,0(sp)
    1da0:	02e7d7b3          	divu	a5,a5,a4
    1da4:	02e50533          	mul	a0,a0,a4
    1da8:	953e                	add	a0,a0,a5
    1daa:	0141                	addi	sp,sp,16
    1dac:	8082                	ret
    1dae:	557d                	li	a0,-1
    1db0:	bfed                	j	1daa <get_time+0x26>

0000000000001db2 <sys_get_time>:
    1db2:	0a900893          	li	a7,169
    1db6:	00000073          	ecall
    1dba:	2501                	sext.w	a0,a0
    1dbc:	8082                	ret

0000000000001dbe <time>:
    1dbe:	42600893          	li	a7,1062
    1dc2:	00000073          	ecall
    1dc6:	2501                	sext.w	a0,a0
    1dc8:	8082                	ret

0000000000001dca <sleep>:
    1dca:	1141                	addi	sp,sp,-16
    1dcc:	e02a                	sd	a0,0(sp)
    1dce:	850a                	mv	a0,sp
    1dd0:	e402                	sd	zero,8(sp)
    1dd2:	06500893          	li	a7,101
    1dd6:	85aa                	mv	a1,a0
    1dd8:	00000073          	ecall
    1ddc:	e501                	bnez	a0,1de4 <sleep+0x1a>
    1dde:	4501                	li	a0,0
    1de0:	0141                	addi	sp,sp,16
    1de2:	8082                	ret
    1de4:	4502                	lw	a0,0(sp)
    1de6:	0141                	addi	sp,sp,16
    1de8:	8082                	ret

0000000000001dea <set_priority>:
    1dea:	08c00893          	li	a7,140
    1dee:	00000073          	ecall
    1df2:	2501                	sext.w	a0,a0
    1df4:	8082                	ret

0000000000001df6 <mmap>:
    1df6:	0de00893          	li	a7,222
    1dfa:	00000073          	ecall
    1dfe:	8082                	ret

0000000000001e00 <munmap>:
    1e00:	0d700893          	li	a7,215
    1e04:	00000073          	ecall
    1e08:	2501                	sext.w	a0,a0
    1e0a:	8082                	ret

0000000000001e0c <wait>:
    1e0c:	85aa                	mv	a1,a0
    1e0e:	10400893          	li	a7,260
    1e12:	557d                	li	a0,-1
    1e14:	4601                	li	a2,0
    1e16:	4681                	li	a3,0
    1e18:	00000073          	ecall
    1e1c:	2501                	sext.w	a0,a0
    1e1e:	8082                	ret

0000000000001e20 <spawn>:
    1e20:	19000893          	li	a7,400
    1e24:	00000073          	ecall
    1e28:	2501                	sext.w	a0,a0
    1e2a:	8082                	ret

0000000000001e2c <mailread>:
    1e2c:	19100893          	li	a7,401
    1e30:	00000073          	ecall
    1e34:	2501                	sext.w	a0,a0
    1e36:	8082                	ret

0000000000001e38 <mailwrite>:
    1e38:	19200893          	li	a7,402
    1e3c:	00000073          	ecall
    1e40:	2501                	sext.w	a0,a0
    1e42:	8082                	ret

0000000000001e44 <fstat>:
    1e44:	05000893          	li	a7,80
    1e48:	00000073          	ecall
    1e4c:	2501                	sext.w	a0,a0
    1e4e:	8082                	ret

0000000000001e50 <sys_linkat>:
    1e50:	1702                	slli	a4,a4,0x20
    1e52:	02500893          	li	a7,37
    1e56:	9301                	srli	a4,a4,0x20
    1e58:	00000073          	ecall
    1e5c:	2501                	sext.w	a0,a0
    1e5e:	8082                	ret

0000000000001e60 <sys_unlinkat>:
    1e60:	1602                	slli	a2,a2,0x20
    1e62:	02300893          	li	a7,35
    1e66:	9201                	srli	a2,a2,0x20
    1e68:	00000073          	ecall
    1e6c:	2501                	sext.w	a0,a0
    1e6e:	8082                	ret

0000000000001e70 <link>:
    1e70:	87aa                	mv	a5,a0
    1e72:	86ae                	mv	a3,a1
    1e74:	02500893          	li	a7,37
    1e78:	f9c00513          	li	a0,-100
    1e7c:	85be                	mv	a1,a5
    1e7e:	f9c00613          	li	a2,-100
    1e82:	4701                	li	a4,0
    1e84:	00000073          	ecall
    1e88:	2501                	sext.w	a0,a0
    1e8a:	8082                	ret

0000000000001e8c <unlink>:
    1e8c:	85aa                	mv	a1,a0
    1e8e:	02300893          	li	a7,35
    1e92:	f9c00513          	li	a0,-100
    1e96:	4601                	li	a2,0
    1e98:	00000073          	ecall
    1e9c:	2501                	sext.w	a0,a0
    1e9e:	8082                	ret

0000000000001ea0 <uname>:
    1ea0:	0a000893          	li	a7,160
    1ea4:	00000073          	ecall
    1ea8:	2501                	sext.w	a0,a0
    1eaa:	8082                	ret

0000000000001eac <brk>:
    1eac:	0d600893          	li	a7,214
    1eb0:	00000073          	ecall
    1eb4:	2501                	sext.w	a0,a0
    1eb6:	8082                	ret

0000000000001eb8 <getcwd>:
    1eb8:	48c5                	li	a7,17
    1eba:	00000073          	ecall
    1ebe:	8082                	ret

0000000000001ec0 <chdir>:
    1ec0:	03100893          	li	a7,49
    1ec4:	00000073          	ecall
    1ec8:	2501                	sext.w	a0,a0
    1eca:	8082                	ret

0000000000001ecc <mkdir>:
    1ecc:	862e                	mv	a2,a1
    1ece:	87aa                	mv	a5,a0
    1ed0:	1602                	slli	a2,a2,0x20
    1ed2:	02200893          	li	a7,34
    1ed6:	f9c00513          	li	a0,-100
    1eda:	85be                	mv	a1,a5
    1edc:	9201                	srli	a2,a2,0x20
    1ede:	00000073          	ecall
    1ee2:	2501                	sext.w	a0,a0
    1ee4:	8082                	ret

0000000000001ee6 <getdents>:
    1ee6:	03d00893          	li	a7,61
    1eea:	00000073          	ecall
    1eee:	2501                	sext.w	a0,a0
    1ef0:	8082                	ret

0000000000001ef2 <pipe>:
    1ef2:	03b00893          	li	a7,59
    1ef6:	4581                	li	a1,0
    1ef8:	00000073          	ecall
    1efc:	2501                	sext.w	a0,a0
    1efe:	8082                	ret

0000000000001f00 <dup>:
    1f00:	48dd                	li	a7,23
    1f02:	00000073          	ecall
    1f06:	2501                	sext.w	a0,a0
    1f08:	8082                	ret

0000000000001f0a <dup2>:
    1f0a:	48e1                	li	a7,24
    1f0c:	4601                	li	a2,0
    1f0e:	00000073          	ecall
    1f12:	2501                	sext.w	a0,a0
    1f14:	8082                	ret

0000000000001f16 <mount>:
    1f16:	02800893          	li	a7,40
    1f1a:	00000073          	ecall
    1f1e:	2501                	sext.w	a0,a0
    1f20:	8082                	ret

0000000000001f22 <umount>:
    1f22:	02700893          	li	a7,39
    1f26:	4581                	li	a1,0
    1f28:	00000073          	ecall
    1f2c:	2501                	sext.w	a0,a0
    1f2e:	8082                	ret

0000000000001f30 <__clone>:
    1f30:	15c1                	addi	a1,a1,-16
    1f32:	e188                	sd	a0,0(a1)
    1f34:	e594                	sd	a3,8(a1)
    1f36:	8532                	mv	a0,a2
    1f38:	863a                	mv	a2,a4
    1f3a:	86be                	mv	a3,a5
    1f3c:	8742                	mv	a4,a6
    1f3e:	0dc00893          	li	a7,220
    1f42:	00000073          	ecall
    1f46:	c111                	beqz	a0,1f4a <__clone+0x1a>
    1f48:	8082                	ret
    1f4a:	6582                	ld	a1,0(sp)
    1f4c:	6522                	ld	a0,8(sp)
    1f4e:	9582                	jalr	a1
    1f50:	05d00893          	li	a7,93
    1f54:	00000073          	ecall
