

// vm.h is defined as _H_MM_
#ifndef _H_MM2_
#define _H_MM2_

uint64
do_mmap(struct file *file, unsigned long addr,
	unsigned long len, int prot,
	int flag, unsigned long offset);
unsigned long do_mmap_pgoff(struct file * file, unsigned long addr, unsigned long len, int prot, int flags, unsigned long pgoff);


#endif