#ifndef _H_FS_
#define _H_FS_

#include "common.h"
#include "mm/page.h"
#include "atomic/sleeplock.h"
#include "atomic/spinlock.h"
#include "atomic/atomic.h"
#include "radix-tree.h"
#include "fs/fat.h"
#include "fs/stat.h"
#include "bio.h"
#include "radix-tree.h"
#include "list.h"

#define E_ISDIR(entry) (((entry)->raw.attr & FAT_ATTR_DIR) > 0)
#define E_ISFILE(entry) (((entry)->raw.attr & FAT_ATTR_FILE) > 0)
#define E_FILESIZE(entry) ((entry)->raw.size)

struct fat_entry;

typedef struct address_space {
	struct fat_entry *host;		/* owner: inode, block_device */
	struct radix_tree_root	page_tree;	/* radix tree of all pages */
	spinlock_t		tree_lock;	/* and spinlock protecting it */
	// unsigned int		i_mmap_writable;/* count VM_SHARED mappings */
	// struct prio_tree_root	i_mmap;		/* tree of private and shared mappings */
	// spinlock_t		i_mmap_lock;	/* protect tree, count, list */
	// unsigned int		truncate_count;	/* Cover race condition with truncate */
	unsigned long		nrpages;	/* number of total pages */
	// spinlock_t		page_lock;	
} address_space_t;


struct linux_dirent64 {
    uint64        d_ino;
    off_t         d_off;
    unsigned short  d_reclen;
    unsigned char   d_type;
    char            d_name[];
};

//IN RAM
struct fat_entry {
    // uint        dev;        /* 所属设备号 */
    int    ref;        /* 引用数 */

    // struct hlist_node node;
    dir_item_t raw;
    fat32_t *fat;       /* fat对象引用 */
    char name[MAX_FILE_NAME];
    uint nlink;
    // uint32_t    file_size;  /* 文件大小 */
    uint32_t    clus_start; /* 数据起始簇号 */
    
    struct fat_entry* parent; /* 父目录 */
    uint32_t    clus_offset; /* 簇内字节偏移量(在父目录中的偏移量) */

    sleeplock_t  lock;       /* io */

    /* 在entry读到内存中时，分配对象 */
    struct address_space *i_mapping;
    uint32_t    clus_end; /* 数据结束簇号(下一个簇为FAT_CLUS_END) */
    uint64_t    clus_cnt; /* 总数据簇号 */
    uint32_t    size_in_mem; /* 内存中保留的当前文件的大小，在文件写回时，如果和磁盘上的不一致，则更新磁盘 */

    int dirty; /* 脏位 */

    /* 链表，串进超级块的dirty链表或者io链表中 */
    list_head_t e_list;
};

typedef struct fat_entry entry_t;

void fs_init();
entry_t *namee(entry_t *from, char *path);
entry_t *nameeparent(entry_t *from, char *path, char *name);
void etrunc(entry_t *entry);
void elock(entry_t *entry);
void eunlock(entry_t *entry);
void eunlockput(entry_t *entry);
entry_t *create(entry_t *from, char *path, short type);
int writee(entry_t *entry, int user, uint64_t buff, off_t off, int n);
int reade(entry_t *entry, int user, uint64_t buff, off_t off, int n);
void eput(entry_t *entry);
entry_t *edup(entry_t *entry);
char *namepath(entry_t *entry, char *buf);
// int namepath(entry_t *from, char *path);
int entry_rename(entry_t *entry, const char *newname);
int read_dents(entry_t *entry, off_t *offset, char *buf, int n);
void estat(entry_t *entry, struct kstat *stat);
void sych_entry_in_disk(entry_t *entry);

#define ROOTINO  1   // root i-number


page_t *find_get_page(address_space_t *mapping, unsigned long offset);
// int filemap_nopage(uint64 address);
void add_to_page_cache(page_t *page, struct address_space *mapping, pgoff_t offset);
void free_mapping(entry_t *entry);
int do_generic_mapping_read(struct address_space *mapping, int user, uint64_t buff, int off, int n);
uint64_t do_generic_mapping_write(struct address_space *mapping, int user, uint64_t buff, int off, int n);
rw_page_list_t* find_pages_tag(address_space_t *mapping, uint32_t tag);
void writeback_file_to_disk(entry_t *entry);
void __remove_from_page_cache(page_t *page);
void remove_from_page_cache(page_t *page);
void *radix_tree_delete(struct radix_tree_root *root, unsigned long index);

#endif