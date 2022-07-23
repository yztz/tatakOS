#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "atomic/spinlock.h"
#include "kernel/proc.h"
#include "kernel/elf.h"
#include "defs.h"
#include "common.h"
#include "mm/vm.h"
#include "fs/fs.h"

// ref: https://github.com/torvalds/linux/blob/v3.19/include/uapi/linux/auxvec.h
#define AT_NULL   0	/* end of vector */
#define AT_IGNORE 1	/* entry should be ignored */
#define AT_EXECFD 2	/* file descriptor of program */
#define AT_PHDR   3	/* program headers for program */
#define AT_PHENT  4	/* size of program header entry */
#define AT_PHNUM  5	/* number of program headers */
#define AT_PAGESZ 6	/* system page size */
#define AT_BASE   7	/* base address of interpreter */
#define AT_FLAGS  8	/* flags */
#define AT_ENTRY  9	/* entry point of program */
#define AT_NOTELF 10	/* program is not ELF */
#define AT_UID    11	/* real uid */
#define AT_EUID   12	/* effective uid */
#define AT_GID    13	/* real gid */
#define AT_EGID   14	/* effective gid */
#define AT_PLATFORM 15  /* string identifying CPU for optimizations */
#define AT_HWCAP  16    /* arch dependent hints at CPU capabilities */
#define AT_CLKTCK 17	/* frequency at which times() increments */
/* AT_* values 18 through 22 are reserved */
#define AT_SECURE 23   /* secure mode boolean */
#define AT_BASE_PLATFORM 24	/* string identifying real platform, may differ from AT_PLATFORM. */
#define AT_RANDOM 25	/* address of 16 random bytes */
#define AT_HWCAP2 26	/* extension of AT_HWCAP */
#define AT_EXECFN  31	/* filename of program */

// #define QUIET
#define __MODULE_NAME__ EXEC
#include "debug.h"

static inline uint32_t elf_map_prot(uint32_t prot) {
  // uint32 ans = 0;
  // if(prot & PF_R) ans |= PROT_READ;
  // if(prot & PF_W) ans |= PROT_WRITE;
  // if(prot & PF_X) ans |= PROT_EXEC;
  // return (ans | PROT_USER);
  return PROT_READ | PROT_WRITE | PROT_EXEC | PROT_USER;
}

static int loadseg(mm_t *mm, uint64 va, entry_t *ip, uint offset, uint sz) {
  if(reade(ip, 1, va, offset, sz) != sz)
      return -1;
  return 0;
}

#define UPOS(ustack, ustackbase) (USERSPACE_END - PGSIZE + (ustack) - (ustackbase))


static uint64_t loadinterp(mm_t *mm) {
  entry_t *ep;
  struct elfhdr elf;
  int i, off;
  struct proghdr ph;

  // debug("start load");
  if((ep = namee(NULL, "libc.so")) == 0){
    debug("libc.so acquire failure");
    return -1;
  }
  elock(ep);

  if(reade(ep, 0, (uint64)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)) {
    if(reade(ep, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph)){
      goto bad;
    }

    if(ph.type != PT_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(do_mmap(mm, NULL, 0, ph.vaddr + INTERP_BASE, ph.memsz, 0, elf_map_prot(ph.flags)) == -1)
      goto bad;
    if(loadseg(mm, ph.vaddr + INTERP_BASE, ep, ph.off, ph.filesz) < 0)
      goto bad;
  }
  eunlock(ep);
  // mmap_print(mm);
  // debug("load done");
  return elf.entry + INTERP_BASE;

 bad:
  eunlock(ep);
  return 0;

}

int exec(char *path, char **argv, char **envp) {
  char *s, *last;
  int i, off;
  uint64 ustack, ustackbase;
  struct elfhdr elf;
  entry_t *ep;
  struct proghdr ph;
  struct proc *p = myproc();
  mm_t *newmm;
  mm_t *oldmm = p->mm;

  uint64_t aux[AUX_CNT][2];
  memset(aux, 0, sizeof(aux));
  int auxcnt = 0;

#define putaux(key, val) {\
  aux[auxcnt][0] = (key);\
  aux[auxcnt][1] = (val);\
  auxcnt++;\
  aux[auxcnt][0] = AT_NULL;}

  putaux(AT_PAGESZ, PGSIZE);
  putaux(AT_UID, 0);
  putaux(AT_EUID, 0);
  putaux(AT_GID, 0);
  putaux(AT_EGID, 0);
  putaux(AT_SECURE, 0);

  /* alloc */
  if((newmm = kzalloc(sizeof(mm_t))) == NULL) {
    debug("newmm alloc failure");
    return -1;
  }

  if((ustackbase = (uint64)kalloc()) == 0) {
    debug("stack alloc failure");
    kfree(newmm);
    return -1;
  }

  ustack = ustackbase + PGSIZE;

  /* load */
  if(mmap_init(newmm) == -1) {
    debug("newmm init fail");
    kfree(newmm);
    kfree((void *)ustackbase);
    return -1;
  }

  if((ep = namee(NULL, path)) == 0){
    debug("entry %s acquire failure", path);
    kfree(newmm);
    kfree((void *)ustackbase);
    return -1;
  }

  proc_switchmm(p, newmm);

  elock(ep);

  // Check ELF header
  if(reade(ep, 0, (uint64)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  uint64_t elfentry = elf.entry;
  uint64_t brk;
  // Load program into memory.
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)) {
    if(reade(ep, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph)){
      goto bad;
    }
    if(ph.type == PT_PHDR) {
      putaux(AT_PHDR, ph.vaddr);
      putaux(AT_PHENT, elf.phentsize);
      putaux(AT_PHNUM, elf.phnum);
      continue;
    }

    if(ph.type == PT_INTERP) {
      // debug("elfentry is %#lx", elfentry);
      putaux(AT_ENTRY, elfentry);
      putaux(AT_BASE, INTERP_BASE);
      if((elfentry = loadinterp(newmm)) == 0) goto bad;
    }

    if(ph.type != PT_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(do_mmap(newmm, NULL, 0, ph.vaddr, ph.memsz, 0, elf_map_prot(ph.flags)) == -1)
      goto bad;
    if(loadseg(newmm, ph.vaddr, ep, ph.off, ph.filesz) < 0)
      goto bad;

    brk = ph.vaddr + ph.memsz;
  }
  debug("%s: loadseg done entry is %#lx", path, elfentry);
  // mmap_print(newmm);
  eunlockput(ep);
  ep = NULL;

  p = myproc();

  //////////////STACK & HEAP////////////////
  if(mmap_map_stack_heap(newmm, brk, oldmm->ustack->len, oldmm->uheap->len) == -1)
    goto bad;
  
  uint64 envpc;
  uint64 envps[MAXENV + 1];

  uint64 argc;
  uint64 argcv[MAXARG + 1 + 1];
  uint64 *argvs = argcv + 1;

  // 复制环境变量字符串
  for(envpc = 0; envp[envpc]; envpc++) {
    if(envpc >= MAXENV)
      goto bad;
    ustack -= strlen(envp[envpc]) + 1;
    ustack -= ustack % 16; // riscv sp must be 16-byte aligned
    if(ustack < ustackbase)
      goto bad;
    memcpy((void *)ustack, envp[envpc], strlen(envp[envpc]) + 1);
    envps[envpc] = UPOS(ustack, ustackbase);
  }
  
  // 复制参数字符串
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    ustack -= strlen(argv[argc]) + 1;
    if(ustack < ustackbase)
      goto bad;
    memcpy((void *)ustack, argv[argc], strlen(argv[argc]) + 1);
    argvs[argc] = UPOS(ustack, ustackbase);
  }

  uint64 random[2] = { 0xea0dad5a44586952, 0x5a1fa5497a4a283d };
  ustack -= 16;
  if(ustack < ustackbase)
    goto bad;
  memcpy((void *)ustack, random, 16);

  putaux(AT_RANDOM, UPOS(ustack, ustackbase));

  envps[envpc] = 0;
  envpc += 1;
  argcv[0] = argc;
  argvs[argc] = 0;
  argc += 2;
  auxcnt += 1;

  ustack -= sizeof(uint64) * (envpc + argc + auxcnt * 16);
  ustack -= ustack % 16;
  if(ustack < ustackbase)
    goto bad;
  ustack += sizeof(uint64) * (envpc + argc);

  // 复制辅助变量
  memcpy((void *)ustack, aux, auxcnt * 16);

  // 复制环境变量字符串地址
  ustack -= sizeof(uint64) * envpc;
  memcpy((void *)ustack, envps, sizeof(uint64) * envpc);
  
  // 复制参数字符串地址
  ustack -= sizeof(uint64) * argc;
  memcpy((void *)ustack, argcv, sizeof(uint64) * argc);
  
  // 初始化栈地址
  proc_get_tf(p)->sp = UPOS(ustack, ustackbase);

  if(mappages(newmm->pagetable, USERSPACE_END - PGSIZE, PGSIZE, ustackbase, riscv_map_prot(newmm->ustack->prot)) == -1) {
    goto bad;
  }

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(p->name, last, sizeof(p->name));
  // debug("pid %d proc %s entry is %lx", p->pid, p->name, elf.entry);
  proc_get_tf(p)->epc = elfentry;  // initial program counter = main
  mmap_free(&oldmm);
  return argc; // this ends up in a0, the first argument to main(argc, argv)

 bad:
  // TODO:
  panic("bad");
  // return -1;
}


