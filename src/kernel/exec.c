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

// #define QUIET
#define __MODULE_NAME__ EXEC
#include "debug.h"

// Load a program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
static int loadseg(mm_t *mm, uint64 va, entry_t *ip, uint offset, uint sz) {
  uint i, n;
  uint64 pa;

  for(i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(mm->pagetable, va + i);
    if(pa == 0)
      panic("loadseg: address should exist");
    if(sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;
    if(reade(ip, 0, (uint64)pa, offset+i, n) != n)
      return -1;
  }
  
  return 0;
}

int exec(char *path, char **argv) {
  char *s, *last;
  int i, off;
  uint64 argc;
  uint64 ustack, ustackbase;
  struct elfhdr elf;
  entry_t *ep;
  struct proghdr ph;
  struct proc *p = myproc();
  mm_t *newmm;
  /* alloc */
  if((newmm = kzalloc(sizeof(mm_t))) == NULL) {
    debug("newmm alloc failure");
    return -1;
  }

  if((ustackbase = (uint64)kmalloc(USTACKSIZE)) == 0) {
    debug("stack alloc failure");
    kfree(newmm);
    return -1;
  }

  ustack = ustackbase + USTACKSIZE;

  if((ep = namee(NULL, path)) == 0){
    debug("entry acquire failure");
    kfree(newmm);
    kfree((void *)ustackbase);
    return -1;
  }

  /* load */
  if(mmap_init(newmm, p->mm) == -1) {
    debug("newmm init fail");
    kfree(newmm);
    kfree((void *)ustackbase);
    return -1;
  }

  elock(ep);

  // Check ELF header
  if(reade(ep, 0, (uint64)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  // Load program into memory.
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(reade(ep, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph)){
      goto bad;
    }
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    if(do_mmap_alloc(newmm, NULL, ph.vaddr, ph.memsz, 0, MAP_PROT_READ|MAP_PROT_WRITE|MAP_PROT_EXEC|MAP_PROT_USER) == -1)
      goto bad;
    if(loadseg(newmm, ph.vaddr, ep, ph.off, ph.filesz) < 0)
      goto bad;
  }

  eunlockput(ep);
  ep = NULL;

  p = myproc();

  /* stack */
  if(do_mmap(newmm, NULL, USERSPACE_END - USTACKSIZE, USTACKSIZE, MAP_STACK, MAP_PROT_READ|MAP_PROT_WRITE|MAP_PROT_USER) == -1)
    goto bad;
  
  uint64 argcv[MAXARG + 1];
  uint64 *argvs = argcv + 1;
  // Push argument strings, prepare rest of stack in ustack.
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    ustack -= strlen(argv[argc]) + 1;
    ustack -= ustack % 16; // riscv sp must be 16-byte aligned
    if(ustack < ustackbase)
      goto bad;
    memcpy((void *)ustack, argv[argc], strlen(argv[argc]) + 1);
    argvs[argc] = ustack;
  }
  argcv[0] = argc;
  argvs[argc] = 0;

  ustack -= sizeof(uint64) * (1 + 1 + argc); // argc + agrv + 0
  ustack -= ustack % 16;
  if(ustack < ustackbase)
    goto bad;

  memcpy((void *)ustack, argcv, sizeof(uint64) * (1 + 1 + argc));
  // arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  get_trapframe(newmm)->sp = USERSPACE_END - USTACKSIZE + ustack - ustackbase;

  if(mappages(newmm->pagetable, USERSPACE_END - USTACKSIZE, USTACKSIZE, ustackbase, MAP_PROT_READ|MAP_PROT_WRITE|MAP_PROT_USER) == -1) {
    goto bad;
  }

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(p->name, last, sizeof(p->name));
    
  get_trapframe(newmm)->epc = elf.entry;  // initial program counter = main
  proc_setmm(p, newmm);
  return argc; // this ends up in a0, the first argument to main(argc, argv)

 bad:
  // TODO:
  panic("bad");
  return -1;
}


