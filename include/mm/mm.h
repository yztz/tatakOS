

// vm.h is defined as _H_MM_
#ifndef _H_MM2_
#define _H_MM2_

#include "fs/fs.h"
#define PAGECACHE_TAG_DIRTY 0
#define PAGECACHE_TAG_WRITEBACK 1

/*
 * vm_flags..
 */
#define VM_READ		0x00000001	/* currently active flags */
#define VM_WRITE	0x00000002
#define VM_EXEC		0x00000004
#define VM_SHARED	0x00000008

/* functions defined in mmap.c */
uint64 do_mmap(struct file *file, unsigned long addr, unsigned long len, int prot, int flag, unsigned long offset);
unsigned long do_mmap_pgoff(struct file * file, unsigned long addr, unsigned long len, int prot, int flags, unsigned long pgoff);
void exit_mmap(mm_struct_t *mm);

/* functions defined in filemap.c */
uint64 find_get_page(struct address_space *mapping, unsigned long offset);
int filemap_nopage(uint64 address);
void add_to_page_cache(uint64 pa, struct address_space *mapping, pgoff_t offset);
void free_mapping(entry_t *entry);
int do_generic_mapping_read(struct address_space *mapping, int user, uint64_t buff, int off, int n);
uint64_t do_generic_mapping_write(struct address_space *mapping, int user, uint64_t buff, int off, int n);
pages_be_found_head_t * find_pages_tag(address_space_t *mapping, uint32_t tag);
void writeback_file_to_disk(entry_t *entry);



/* functions defined in write-pageback.c */
void set_pg_rdt_dirty(uint64_t pa, radix_tree_root_t *root, uint64_t pg_id, uint tag_type);
#endif