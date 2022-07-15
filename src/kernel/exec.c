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
#include "mm/mm.h"

// #define QUIET
#define __MODULE_NAME__ EXEC
#include "debug.h"
#include "fs/fcntl.h"
#include "kernel/proc.h"

static int loadseg(pde_t *pgdir, uint64 addr, entry_t *ip, uint offset, uint sz);

// int
// exec(char *path, char **argv)
// {
//   // for(int i = 0; i < 10; i++){
//   //   uint64_t va = (uint64_t)kalloc();
//   //   printf(bl("va: %p\n"), va);
//   // }
//   // for(;;);

//   char *s, *last;
//   int i, off;
//   uint64 argc, sz = 0, sp, ustack[MAXARG + 1], stackbase;
//   struct elfhdr elf;
//   entry_t *ep;
//   struct proghdr ph;
//   pagetable_t pagetable = 0, oldpagetable;
//   struct proc *p = myproc();


//   if((ep = namee(NULL, path)) == 0){
//     return -1;
//   }
//   elock(ep);

//   // Check ELF header
//   if(reade(ep, 0, (uint64)&elf, 0, sizeof(elf)) != sizeof(elf))
//     goto bad;
//   if(elf.magic != ELF_MAGIC)
//     goto bad;

//   if((pagetable = proc_pagetable(p)) == 0)
//     goto bad;


//   // vmprint(pagetable);
//   // Load program into memory.
//   for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
//     if(reade(ep, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph)){
//       goto bad;
//     }
//     if(ph.type != ELF_PROG_LOAD)
//       continue;
//     if(ph.memsz < ph.filesz)
//       goto bad;
//     if(ph.vaddr + ph.memsz < ph.vaddr)
//       goto bad;
//     uint64 sz1;
//     if((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz)) == 0)
//       goto bad;
//     sz = sz1;
//     if((ph.vaddr % PGSIZE) != 0)
//       goto bad;
//     if(loadseg(pagetable, ph.vaddr, ep, ph.off, ph.filesz) < 0)
//       goto bad;
//   }

//   // printf("ph: %d\n", ph.vaddr + ph.memsz);
//   // vmprint(pagetable);
//   eunlockput(ep);
//   ep = NULL;

//   p = myproc();
//   uint64 oldsz = p->sz;

//   // alloc for user stack
//   sz = PGROUNDUP(sz);
//   uint64 sz1;
//   if((sz1 = uvmalloc(pagetable, sz, sz + PGSIZE + USTACKSIZE)) == 0)
//     goto bad;
//   sz = sz1;
//   uvmclear(pagetable, sz - (PGSIZE + USTACKSIZE));
//   sp = sz;
//   stackbase = sp - USTACKSIZE;

//   // Push argument strings, prepare rest of stack in ustack.
  
//   for(argc = 0; argv[argc]; argc++) {
//     if(argc >= MAXARG)
//       goto bad;
//     sp -= strlen(argv[argc]) + 1;
//     sp -= sp % 16; // riscv sp must be 16-byte aligned
//     if(sp < stackbase)
//       goto bad;
//     if(copyout(pagetable, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
//       goto bad;
//     ustack[argc + 1] = sp;
//   }
//   ustack[0]= argc;
//   ustack[argc + 1] = 0;

//   // push the array of argv[] pointers.
//   sp -= (argc+1+1) * sizeof(uint64);
//   sp -= sp % 16;
//   if(sp < stackbase)
//     goto bad;
//   if(copyout(pagetable, sp, (char *)ustack, (argc+1+1)*sizeof(uint64)) < 0)
//     goto bad;

//   // arguments to user main(argc, argv)
//   // argc is returned via the system call return
//   // value, which goes in a0.
//   p->trapframe->a1 = sp;

//   // Save program name for debugging.
//   for(last=s=path; *s; s++)
//     if(*s == '/')
//       last = s+1;
//   safestrcpy(p->name, last, sizeof(p->name));
    
//   // Commit to the user image.
//   oldpagetable = p->pagetable;
//   p->pagetable = pagetable;
//   p->sz = sz;
//   // printf(rd("sz: %x\n"), sz);
//   p->trapframe->epc = elf.entry;  // initial program counter = main
//   // debug("entry addr is %lx", elf.entry);
//   p->trapframe->sp = sp; // initial stack pointer
//   switchuvm(p);
//   proc_freepagetable(oldpagetable, oldsz, MMAP_BASE);
//   return argc; // this ends up in a0, the first argument to main(argc, argv)

//  bad:
//   if(pagetable)
//     proc_freepagetable(pagetable, sz, MMAP_BASE);
//   if(ep){
//     eunlockput(ep);
//   }
//   return -1;
// }

// Load a program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
static int
loadseg(pagetable_t pagetable, uint64 va, entry_t *ip, uint offset, uint sz)
{
  uint i, n;
  uint64 pa;

  for(i = 0; i < sz; i += PGSIZE){
    pa = walkaddr(pagetable, va + i);
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

extern uint64_t sys_memuse(void);
/**
 * @brief 更换mm_struct结构体
 * 
 */
int
exec(char *path, char **argv)
{
  // for(int i = 0; i < 10; i++){
  //   uint64_t va = (uint64_t)kalloc();
  //   printf(bl("va: %p\n"), va);
  // }
  // sys_memuse();
  // for(;;);

  char *s, *last;
  int i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG + 1], stackbase;
  struct elfhdr elf;
  entry_t *ep;
  struct proghdr ph;
  pagetable_t pagetable;
  struct proc *p = myproc();

  /* 切换新页表 */
  mm_struct_t *newmm = kzalloc(sizeof(mm_struct_t));
  mm_init(newmm, p);

  /* 切换页表自动刷新快表 */
  switchuvm(newmm);
  // sfence_vma();

  // sys_memuse();
  exit_mm(myproc());
  // sys_memuse();

  myproc()->mm = newmm;
  
  // /* 释放用户页表和vma */
  // exit_mmap(p->mm);

  if((ep = namee(NULL, path)) == 0){
    ERROR("open excutable file failed, maybe need to modify CMakelists.txt, line 55.");
    // return -1;
  }
  elock(ep);
  // sys_memuse();

  // Check ELF header
  if(reade(ep, 0, (uint64)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  // if((pagetable = proc_pagetable(p->mm)) == 0)
  //   goto bad;
  pagetable = newmm->pagetable;

  // vmprint(pagetable);
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
    uint64 sz1;
    if((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz)) == 0)
      goto bad;
    
    sz = sz1;
    if((ph.vaddr % PGSIZE) != 0)
      goto bad;
    if(loadseg(pagetable, ph.vaddr, ep, ph.off, ph.filesz) < 0)
      goto bad;

    /* 在初始用户进程启动时，把程序加载到0地址，执行这个exec的进程通过fork初始进程
    得到，所以0地址也已经有东西了，这里重新装载程序将其覆盖，do_mmap需要先删除旧的
    vma，再创建新的vma，或者再前面的步骤里进行 */
    /* 创建可执行程序的vma */
    do_mmap(NULL, PGROUNDUP(sz)-PGROUNDUP(ph.vaddr+ph.memsz), PGROUNDUP(ph.vaddr+ph.memsz), PROT_READ|PROT_EXEC, MAP_EXECUTABLE, 0, LOAD); 
    // ER();
  }

  // printf("ph: %d\n", ph.vaddr + ph.memsz);
  // vmprint(pagetable);
  // sys_memuse();

  eunlockput(ep);
  ep = NULL;

  p = myproc();
  // uint64 oldsz = p->sz;

  // alloc for user stack
  sz = PGROUNDUP(sz);
  uint64 sz1;
  if((sz1 = uvmalloc(pagetable, sz, sz + PGSIZE + USTACKSIZE)) == 0)
    goto bad;
  sz = sz1;
  uvmclear(pagetable, sz - (PGSIZE + USTACKSIZE));
  sp = sz;
  stackbase = sp - USTACKSIZE;

  do_mmap(NULL, stackbase-PGSIZE, PGSIZE, 0, 0, 0, GUARD);

  /* 创建栈的vma */
  do_mmap(NULL, stackbase, USTACKSIZE, PROT_READ|PROT_WRITE, VM_GROWSDOWN, 0, STACK);

  // Push argument strings, prepare rest of stack in ustack.
  
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    if(sp < stackbase)
      goto bad;
    if(copyout(pagetable, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[argc + 1] = sp;
  }
  ustack[0]= argc;
  ustack[argc + 1] = 0;

  // push the array of argv[] pointers.
  sp -= (argc+1+1) * sizeof(uint64);
  sp -= sp % 16;
  if(sp < stackbase)
    goto bad;
  if(copyout(pagetable, sp, (char *)ustack, (argc+1+1)*sizeof(uint64)) < 0)
    goto bad;

  // arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(p->name, last, sizeof(p->name));
    
  // // Commit to the user image.
  // oldpagetable = p->mm->pagetable;
  // p->mm->pagetable = pagetable;
  // // p->sz = sz;
  // // printf(rd("sz: %x\n"), sz);
  p->trapframe->epc = elf.entry;  // initial program counter = main
  // // debug("entry addr is %lx", elf.entry);
  p->trapframe->sp = sp; // initial stack pointer

  // /* set the start heap */
  // // p->mm->start_brk= p->sz;
  p->mm->start_brk= sz;
  p->mm->start_stack = sp;

  // switchuvm(p);
  // /* 释放内核页表 */
  // proc_freepagetable(oldpagetable);

  return argc; // this ends up in a0, the first argument to main(argc, argv)

 bad:
  if(pagetable)
    proc_freepagetable(pagetable);
  if(ep){
    eunlockput(ep);
  }
  return -1;
}