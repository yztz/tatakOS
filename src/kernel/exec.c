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

static int loadseg(mm_t *mm, uint64 va, entry_t *ip, uint offset, uint sz) {
  // debug("load %lx", va);
  if(reade(ip, 1, va, offset, sz) != sz)
      return -1;
  // debug("load done");
  return 0;
}

#define UPOS(ustack, ustackbase) (USERSPACE_END - PGSIZE + (ustack) - (ustackbase))

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

  if((ep = namee(NULL, path)) == 0){
    debug("entry %s acquire failure", path);
    kfree(newmm);
    kfree((void *)ustackbase);
    return -1;
  }

  /* load */
  if(mmap_init(newmm) == -1) {
    debug("newmm init fail");
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

  uint64 phdr;
  // Load program into memory.
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)) {
    if(reade(ep, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph)){
      goto bad;
    }
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(do_mmap(newmm, NULL, ph.vaddr, ph.memsz, 0, MAP_PROT_READ|MAP_PROT_WRITE|MAP_PROT_EXEC|MAP_PROT_USER) == -1)
      goto bad;
    if(loadseg(newmm, ph.vaddr, ep, ph.off, ph.filesz) < 0)
      goto bad;
    // 当发现从0开始的程序段时，elf文件头也被包含在其中加载
    // 因此，可以计算出程序段头的地址为程序的虚拟地址+程序段头地址偏移量
    if(ph.off == 0) 
      phdr = ph.vaddr + elf.phoff;
  }
  debug("%s: loadseg done", path);
  eunlockput(ep);
  ep = NULL;

  p = myproc();

  //////////////STACK & HEAP////////////////
  if(mmap_map_stack_heap(newmm, oldmm->ustack->len, oldmm->uheap->len) == -1)
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

  uint64 auxs[][2] = {
    {AT_PAGESZ, PGSIZE},
    {AT_PHDR, phdr},
		{AT_PHENT, elf.phentsize},
		{AT_PHNUM, elf.phnum},
    {AT_UID, 0},
		{AT_EUID, 0},
		{AT_GID, 0},
		{AT_EGID, 0},
		{AT_SECURE, 0},
		{AT_RANDOM, ustack},
    {AT_NULL, 0},
  };

  envps[envpc] = 0;
  envpc += 1;
  argcv[0] = argc;
  argvs[argc] = 0;
  argc += 2;

  ustack -= sizeof(uint64) * (envpc + argc + sizeof(auxs));
  ustack -= ustack % 16;
  if(ustack < ustackbase)
    goto bad;
  ustack += sizeof(uint64) * (envpc + argc);

  // 复制辅助变量
  memcpy((void *)ustack, auxs, sizeof(uint64) * sizeof(auxs));

  // 复制环境变量字符串地址
  ustack -= sizeof(uint64) * envpc;
  memcpy((void *)ustack, envps, sizeof(uint64) * envpc);
  
  // 复制参数字符串地址
  ustack -= sizeof(uint64) * argc;
  memcpy((void *)ustack, argcv, sizeof(uint64) * argc);
  
  // 初始化栈地址
  proc_get_tf(p)->sp = UPOS(ustack, ustackbase);

  if(mappages(newmm->pagetable, USERSPACE_END - PGSIZE, PGSIZE, ustackbase, newmm->ustack->prot) == -1) {
    goto bad;
  }

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(p->name, last, sizeof(p->name));
  // debug("pid %d proc %s entry is %lx", p->pid, p->name, elf.entry);
  proc_get_tf(p)->epc = elf.entry;  // initial program counter = main
  mmap_free(&oldmm);
  return argc; // this ends up in a0, the first argument to main(argc, argv)

 bad:
  // TODO:
  panic("bad");
  // return -1;
}


