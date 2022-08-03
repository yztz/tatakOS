


int foo(int t) {
    1149:	f3 0f 1e fa          	endbr64 
    114d:	55                   	push   %rbp
    114e:	48 89 e5             	mov    %rsp,%rbp
    1151:	89 7d fc             	mov    %edi,-0x4(%rbp)
    return t + 1;
    1154:	8b 45 fc             	mov    -0x4(%rbp),%eax
    1157:	83 c0 01             	add    $0x1,%eax
}
    115a:	5d                   	pop    %rbp
    115b:	c3                   	ret    

000000000000115c <main>:


int main() {
    115c:	f3 0f 1e fa          	endbr64 
    1160:	55                   	push   %rbp
    1161:	48 89 e5             	mov    %rsp,%rbp
    1164:	48 83 ec 10          	sub    $0x10,%rsp
    int a = 3;
    1168:	c7 45 f8 03 00 00 00 	movl   $0x3,-0x8(%rbp)
    int ans = foo(a) ? foo(a) : 0;
    116f:	8b 45 f8             	mov    -0x8(%rbp),%eax
    1172:	89 c7                	mov    %eax,%edi
    1174:	e8 d0 ff ff ff       	call   1149 <foo>
    1179:	85 c0                	test   %eax,%eax
    117b:	74 0c                	je     1189 <main+0x2d>
    117d:	8b 45 f8             	mov    -0x8(%rbp),%eax
    1180:	89 c7                	mov    %eax,%edi
    1182:	e8 c2 ff ff ff       	call   1149 <foo>
    1187:	eb 05                	jmp    118e <main+0x32>
    1189:	b8 00 00 00 00       	mov    $0x0,%eax
    118e:	89 45 fc             	mov    %eax,-0x4(%rbp)

    printf("ans is %d\n", ans);
    1191:	8b 45 fc             	mov    -0x4(%rbp),%eax
    1194:	89 c6                	mov    %eax,%esi
    1196:	48 8d 05 67 0e 00 00 	lea    0xe67(%rip),%rax        # 2004 <_IO_stdin_used+0x4>
    119d:	48 89 c7             	mov    %rax,%rdi
    11a0:	b8 00 00 00 00       	mov    $0x0,%eax
    11a5:	e8 a6 fe ff ff       	call   1050 <printf@plt>

    return 0;
    11aa:	b8 00 00 00 00       	mov    $0x0,%eax
    11af:	c9                   	leave  
    11b0:	c3                   	ret    

Disassembly of section .fini:

00000000000011b4 <_fini>:
    11b4:	f3 0f 1e fa          	endbr64 
    11b8:	48 83 ec 08          	sub    $0x8,%rsp
    11bc:	48 83 c4 08          	add    $0x8,%rsp
    11c0:	c3                   	ret    
