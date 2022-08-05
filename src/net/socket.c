#include "common.h"
#include "kernel/proc.h"
#include "mm/alloc.h"
#include "mm/vm.h"
#include "defs.h"
#include "fs/file.h"


// 大无语 SOCKET
// 请无视此文件

socket_t sock;

uint64_t sys_socket(void) {
    proc_t *p = myproc();

    file_t *f = filealloc();

    if(f == NULL) {
        return -1;
    }

    f->socket = &sock;
    f->type = FD_SOCKET;
    f->readable = 1;
    f->writable = 1;

    return fdtbl_fdalloc(p->fdtable, f, -1, 0);
}

uint64_t sys_sendto(void) {
    int sockfd;
    uint64_t buff_addr;
    size_t len;
    proc_t *p = myproc();

    if(argint(0, &sockfd) < 0 || argaddr(1, &buff_addr) < 0 || argaddr(2, &len) < 0)
        return -1;
    
    file_t *f = fdtbl_getfile(p->fdtable, sockfd);

    if(f == NULL || f->type != FD_SOCKET)
        return -1;
    
    socket_t *sock = f->socket;

    if(len > 20) {
        panic("too long");
    }


    if(copy_from_user(sock->buf + sock->pos, buff_addr, len) < 0)
        return -1;

    sock->pos += len;
    
    
    return len;
}

uint64_t sys_recvfrom(void) {
    int sockfd;
    uint64_t buff_addr;
    size_t len;
    proc_t *p = myproc();
    
    if(argint(0, &sockfd) < 0 || argaddr(1, &buff_addr) < 0 || argaddr(2, &len) < 0)
        return -1;

    file_t *f = fdtbl_getfile(p->fdtable, sockfd);

    if(f == NULL || f->type != FD_SOCKET)
        return -1;

    socket_t *sock = f->socket;
    len = min(len, sock->pos);

    if(copy_to_user(buff_addr, sock->buf, len) < 0)
        return -1;

    sock->pos -= len;

    return len;
}

uint64_t sys_bind(void) {
    return 0;
}

uint64_t sys_listen(void) {
    return 0;
}

uint64_t sys_accept(void) {
    return 0;
}

uint64_t sys_connect(void) {
    return 0;
}

uint64_t sys_getsockname(void) {
    return 0;
}

uint64_t sys_setsockopt(void) {
    return 0;
}

