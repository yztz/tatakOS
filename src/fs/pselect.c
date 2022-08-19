#include "common.h"
#include "defs.h"
#include "fdtable.h"
#include "fs/file.h"
#include "fs/pipe.h"
#include "kernel/proc.h"
#include "kernel/signal.h"
#include "kernel/time.h"
#include "mm/vm.h"

#define __MODULE_NAME__ PSELECT
#include "debug.h"

uint64_t sys_pselect(void) {
    int nfds;
    uint64_t raddr, waddr, eaddr, taddr;
    uint64_t timeout;
    struct fdset readfds;
    struct fdset u_readfds;
    struct fdset writefds;
    struct fdset u_writefds;
    struct fdset exceptfds;
    timespec_t ts;

    proc_t *p = myproc(); 

    argint(0, &nfds);
    if(nfds > FD_SETSIZE) return -1;
    int fds_bytes = DIV_ROUND_UP(nfds, 8);
    argaddr(1, &raddr);
    if(raddr && copy_from_user(&u_readfds, raddr, fds_bytes) < 0) return -1;
    argaddr(2, &waddr);
    if(waddr && copy_from_user(&u_writefds, waddr, fds_bytes) < 0) return -1;
    argaddr(3, &eaddr);
    if(eaddr && copy_from_user(&exceptfds, eaddr, fds_bytes) < 0) return -1;
    argaddr(4, &taddr);
    if(taddr && copy_from_user(&ts, taddr, sizeof(struct timespec)) < 0) return -1;

    timeout = taddr ? ts2ticks(&ts) : -1;

    int ans = 0;

    while(1) {
        memcpy(&readfds, &u_readfds, fds_bytes);
        memcpy(&writefds, &u_writefds, fds_bytes);
        for(int i = 0; i < nfds && raddr; i++) {
            if(!fdset_test(&readfds, i)) continue;
            file_t *f = fdtbl_getfile(p->fdtable, i);
            switch(f->type) {
                case FD_PIPE:
                    if(!f->readable) {fdset_clear(&readfds, i);break;}
                    if(pipe_empty(f->pipe)) {fdset_clear(&readfds, i);break;}
                    // fdset_set(&readfds, i);
                case FD_ENTRY:
                case FD_DEVICE:
                    ans++;
                    break;
                default:
                    panic("unknow file type");
            }
        }

        for(int i = 0; i < nfds && waddr; i++) {
            if(!fdset_test(&writefds, i)) continue;
            file_t *f = fdtbl_getfile(p->fdtable, i);
            switch(f->type) {
                case FD_PIPE:
                    // if(!f->writable || pipe_full(f->pipe)){fdset_clear(&writefds, i);break;}
                    if(!f->writable){fdset_clear(&writefds, i);break;}
                    if(pipe_full(f->pipe)) {fdset_clear(&writefds, i);break;}
                case FD_ENTRY:
                case FD_DEVICE:
                    ans++;
                    break;
                default:
                    panic("unknow file type");
            }
        }

        if(ans > 0) break;

        if(timeout == -1) continue;

        if(timeout) {
            // debug("sleep");
            acquire(&p->lock);
            sleep(&ticks, &p->lock);
            release(&p->lock);
            timeout--;
        } else {
            break;
        }
    }

    if(raddr)
        if(copy_to_user(raddr, &readfds, sizeof(struct fdset)) < 0) return -1;
    if(waddr)
        if(copy_to_user(waddr, &writefds, sizeof(struct fdset)) < 0) return -1;
    if(eaddr)
        if(memset_user(eaddr, 0, sizeof(struct fdset)) < 0) return -1;
    
    return ans;
}

#include "poll.h"
uint64 sys_ppoll(void) {
    uint64_t pfdaddr;
    int nfds;
    uint64_t tsaddr;
    uint64_t sigmaskaddr;
    proc_t *p = myproc();

    argaddr(0, &pfdaddr);
    argint(1, &nfds);
    argaddr(2, &tsaddr);
    argaddr(3, &sigmaskaddr);

    assert(nfds == 1);
    // printf("ts addr is %#lx\n", tsaddr);

    struct pollfd pfd;

    if(copy_from_user(&pfd, pfdaddr, sizeof(pfd)) < 0)
        return -1;
    
    assert(pfd.events == POLLIN);
    assert(pfd.fd == 0);

    file_t *f = fdtbl_getfile(p->fdtable, pfd.fd);

    timespec_t ts;
    if(tsaddr && copy_from_user(&ts, tsaddr, sizeof(timespec_t)) < 0)
        return -1;
    
   
    uint64_t timeout = tsaddr ? ts2ticks(&ts) : -1;

    while(1) {
        switch(f->type) {
            case FD_DEVICE:
                assert(f->dev == &devs[CONSOLE]);
                if(consoleready()) goto ret;
                break;
            case FD_PIPE:
                return 1;
            default:
                ER();
        }

        if(timeout == -1) continue;
        if(timeout) {
            acquire(&p->lock);
            sleep(&ticks, &p->lock);
            release(&p->lock);
            timeout--;
        } else {
            break;
        }
    }
  ret:
    if(timeout == 0) return 0;

    pfd.revents = pfd.events;
    if(copy_to_user(pfdaddr, &pfd, sizeof(pfd)) < 0)
        return -1;

    return 1;
}