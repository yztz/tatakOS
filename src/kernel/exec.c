#include "kernel/proc.h"
#include "elf.h"
#include "defs.h"
#include "common.h"
#include "mm/vm.h"
#include "fs/fs.h"


// #define QUIET
#define __MODULE_NAME__ EXEC
#include "debug.h"
#include "fs/fcntl.h"
#include "kernel/proc.h"

static inline uint32_t elf_map_prot(uint32_t prot) {
    // uint32 ans = 0;
    // if(prot & PF_R) ans |= PROT_READ;
    // if(prot & PF_W) ans |= PROT_WRITE;
    // if(prot & PF_X) ans |= PROT_EXEC;
    // return (ans | PROT_USER);
    return PROT_READ | PROT_WRITE | PROT_EXEC | PROT_USER;
}

static int loadseg(mm_t *mm, uint64 va, entry_t *ip, uint offset, uint sz) {
    if (reade(ip, 1, va, offset, sz) != sz)
        return -1;
    return 0;
}


int exec(char *path, char *argv[], char *envp[]) {
    char *s, *last;
    int i, off;
    struct elfhdr elf;
    entry_t *ep;
    struct proghdr ph;
    struct proc *p = current;
    mm_t *newmm;
    mm_t *oldmm = p->mm;

    uint64_t aux[AUX_CNT][2];
    memset(aux, 0, sizeof(aux));
    int auxcnt = 0;

#define putaux(key, val) do{\
  aux[auxcnt][0] = (key);\
  aux[auxcnt][1] = (val);\
  auxcnt++;}while(0);

    putaux(AT_PAGESZ, PGSIZE);
    putaux(AT_UID, 0);
    putaux(AT_EUID, 0);
    putaux(AT_GID, 0);
    putaux(AT_EGID, 0);
    putaux(AT_SECURE, 0);

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
    if (reade(ep, 0, (uint64)&elf, 0, sizeof(elf)) != sizeof(elf) || elf.magic != ELF_MAGIC) {
        eunlock(ep);
        goto bad;
    }

    uint64_t elfentry = elf.entry;
    // 加载程序
    for (i = 0, off = elf.phoff; i < elf.phnum; i++, off += sizeof(ph)) {
        if (reade(ep, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph)) {
            eunlock(ep);
            goto bad;
        }
        
        if (ph.type == PT_PHDR) {
            putaux(AT_PHDR, ph.vaddr);
            putaux(AT_PHENT, elf.phentsize);
            putaux(AT_PHNUM, elf.phnum);
            continue;
        }

        if (ph.type != PT_LOAD)
            continue;
            
        if (ph.memsz < ph.filesz) {
            eunlock(ep);
            goto bad;
        }

        if (do_mmap(newmm, NULL, 0, ph.vaddr, ph.memsz, 0, elf_map_prot(ph.flags)) == -1) {
            eunlock(ep);
            goto bad;
        }

        if (loadseg(newmm, ph.vaddr, ep, ph.off, ph.filesz) < 0) {
            eunlock(ep);
            goto bad;
        }

    }
    // debug("%s: loadseg done entry is %#lx", path, elfentry);
    // mmap_print(newmm);
    eunlock(ep);

    // initcode has not exe
    if (p->exe) {
        eput(p->exe);
    }
    p->exe = ep;


    //////////////STACK & HEAP////////////////
    uint64 ustack, ustackbase;

    if (mmap_map_stack(newmm, oldmm->ustack->len) == -1) {
        goto bad;
    }


    ustackbase = newmm->ustack->addr;
    ustack = ustackbase + newmm->ustack->len;


    // 环境变量数目
    uint64 envpc;
    // 环境变量字符串指针数组
    uint64 envps[MAXENV + 1]; // +1 -> '\0'

    // 复制环境变量字符串
    for (envpc = 0; envp[envpc]; envpc++) {
        if (envpc >= MAXENV) {
            goto bad;
        }
        size_t len = strlen(envp[envpc]);
        ustack -= len + 1;

        if (copy_to_user(ustack, envp[envpc], len + 1) == -1) {
            goto bad;
        }
        envps[envpc] = ustack;
    }

    envps[envpc] = 0;
    envpc += 1; // 还需要包含末尾的NULL

    // 参数个数
    uint64 argc;
    // 参数字符串指针数组 第一个位置将被用来存储argc
    uint64 argcv[MAXARG + 1 + 1]; // +1 -> argc, +1 -> '\0'
    // 不包含argc的参数字符串指针数组
    uint64 *argvs = argcv + 1;

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


    // 设置random
    uint64 random[2] = { 0xea0dad5a44586952, 0x5a1fa5497a4a283d };
    ustack -= 16;
    ustack -= ustack % 8; // uint64参数8字节宽度对齐

    if (copy_to_user(ustack, random, 16) == -1) {
        goto bad;
    }

    putaux(AT_RANDOM, ustack);
    putaux(AT_NULL, 0);

    ustack -= sizeof(uint64) * (envpc + argc + auxcnt * 2);
    ustack -= ustack % 16; // riscv sp必须16字节对齐

    if (ustack < ustackbase) {
        goto bad;
    }

    ustack += sizeof(uint64) * (envpc + argc);

    // 复制辅助变量
    copy_to_user(ustack, aux, auxcnt * 2 * sizeof(uint64));

    // 复制环境变量字符串地址
    ustack -= sizeof(uint64) * envpc;
    copy_to_user(ustack, envps, sizeof(uint64) * envpc);

    // 复制参数字符串地址
    ustack -= sizeof(uint64) * argc;
    copy_to_user(ustack, argcv, sizeof(uint64) * argc);

    tf_reset(proc_get_tf(p), elfentry, ustack);
    mmap_free(&oldmm);
    sig_reset(p->signal);

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


