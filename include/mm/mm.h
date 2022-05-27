

// vm.h is defined as _H_MM_
#ifndef _H_MM2_
#define _H_MM2_

uint64 do_mmap(struct file *file, unsigned long addr, unsigned long len, int prot, int flag, unsigned long offset);
unsigned long do_mmap_pgoff(struct file * file, unsigned long addr, unsigned long len, int prot, int flags, unsigned long pgoff);

uint64 find_get_page(struct address_space *mapping, unsigned long offset);
int filemap_nopage(uint64 address);
void add_to_page_cache(uint64 pa, struct address_space *mapping, pgoff_t offset);
void free_mapping(entry_t *entry);
int do_generic_mapping_read(struct address_space *mapping, int user, uint64_t buff, int off, int n);

#endif