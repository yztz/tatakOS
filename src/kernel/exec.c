#include "common.h"
#include "elf.h"
#include "fs/fs.h"
#include "kernel/proc.h"
#include "mm/vm.h"
#include "mm/alloc.h"
#include "mm/mmap.h"
#include "mm/trapframe.h"
#include "riscv.h"

// #define QUIET
#define __MODULE_NAME__ EXEC
#include "debug.h"

static inline uint32_t elf_map_prot(uint32_t prot) {
    /*
        Why I comment the things below?
        Because we load data with demend-paging mechanism,
        which ask us to make it privileged(means writable).
        A important task to do is to implement mprotect to 
        change map protection dynamicly, so we can change it 
        after loading...
    */

    // uint32_t ans = 0;
    // if(prot & PF_R) ans |= PROT_READ;
    // if(prot & PF_W) ans |= PROT_WRITE;
    // if(prot & PF_X) ans |= PROT_EXEC;
    // return (ans | PROT_USER);

    // Does RISC-V have a mechanism liKe cr0.WP in X86?
    // If we do not set prot PROT_WRITE, we could not write data to this memory region, even if SUM is set.
    // Another solution is that we can map twice: writeable first, and then turn to readonly
    return PROT_READ | PROT_WRITE | PROT_EXEC | PROT_USER;
}

static inline int is_elf(struct elfhdr *hdr) {
    return strncmp((char *)hdr->ident, ELF_MAGIC, ELF_MAGIC_SIZE) == 0;
}

static int loadseg(mm_t *mm, uint64_t va, entry_t *ip, uint offset, uint sz) {
    if (reade(ip, 1, va, offset, sz) != sz)
        return -1;
    return 0;
}


int exec(char *path, char *argv[]) {
    char *s, *last;
    int i, off;
    struct elfhdr elf;
    entry_t *ep;
    struct proghdr ph;
    struct proc *p = current;
    mm_t *newmm;
    mm_t *oldmm = p->mm;


    if ((newmm = mmap_new()) == NULL) {
        debug("newmm alloc failure");
        return -1;
    }

    if ((ep = namee(p->cwd, path)) == 0) {
        debug("entry %s acquire failure", path);
        kfree(newmm);
        return -1;
    }

    proc_switchmm(p, newmm);

    elock(ep);

    // 检查ELF文件头部
    if (reade(ep, 0, (uint64_t)&elf, 0, sizeof(elf)) != sizeof(elf) || !is_elf(&elf)) {
        eunlock(ep);
        goto bad;
    }

    uint64_t elfentry = elf.entry;
    // 加载程序
    for (i = 0, off = elf.phoff; i < elf.phnum; i++, off += sizeof(ph)) {
        if (reade(ep, 0, (uint64_t)&ph, off, sizeof(ph)) != sizeof(ph)) {
            eunlock(ep);
            goto bad;
        }
        

        if (ph.type != PT_LOAD)
            continue;
            
        if (ph.memsz < ph.filesz) {
            eunlock(ep);
            goto bad;
        }

        if (mmap_map(newmm, NULL, 0, ph.vaddr, ph.memsz, 0, elf_map_prot(ph.flags)) == NULL) {
            eunlock(ep);
            goto bad;
        }

        if (loadseg(newmm, ph.vaddr, ep, ph.off, ph.filesz) < 0) {
            eunlock(ep);
            goto bad;
        }

    }
    // debug("%s: loadseg done entry is %#lx", path, elfentry);
    eunlock(ep);

    // initcode has not exe
    if (p->exe) {
        eput(p->exe);
    }
    p->exe = ep;


    //////////////STACK & HEAP////////////////
    uint64_t ustack, ustackbase;

    if (mmap_map_heap(newmm) == NULL) {
        goto bad;
    }

    uint64_t ustack_size = oldmm->ustack ? oldmm->ustack->len : USTACKSIZE;
    if (mmap_map_stack(newmm, ustack_size) == NULL) {
        goto bad;
    }

    ustackbase = newmm->ustack->addr;
    ustack = ustackbase + ustack_size;

    // 参数个数
    uint64_t argc;
    // 参数字符串指针数组 第一个位置将被用来存储argc
    uint64_t argcv[MAXARG + 1 + 1]; // +1 -> argc, +1 -> '\0'
    // 不包含argc的参数字符串指针数组
    uint64_t *argvs = argcv + 1;

    // 复制参数字符串
    for (argc = 0; argv[argc]; argc++) {
        if (argc >= MAXARG) {
            goto bad;
        }
        size_t len = strlen(argv[argc]);
        ustack -= len + 1;
        if (copy_to_user(ustack, argv[argc], len + 1) == -1) {
            goto bad;
        }

        argvs[argc] = ustack;
    }

    argcv[0] = argc;
    argvs[argc] = 0;
    argc += 2;


    ustack -= sizeof(uint64_t) * argc;
    ustack -= ustack % 16; // riscv sp必须16字节对齐

    if (ustack < ustackbase) {
        goto bad;
    }

    // 复制参数字符串地址
    ustack -= sizeof(uint64_t) * argc;
    copy_to_user(ustack, argcv, sizeof(uint64_t) * argc);

    tf_reset(proc_get_tf(p), elfentry, ustack);
    mmap_free(&oldmm);

    // 保存新的进程名称
    for (last = s = path; *s; s++)
        if (*s == '/')
            last = s + 1;
    safestrcpy(p->name, last, sizeof(p->name));

    return 0;

bad:
    proc_switchmm(p, oldmm);
    mmap_free(&newmm);
    return -1;
}


