#ifndef _H_FS_
#define _H_FS_

#include "types.h"
#include "atomic/sleeplock.h"
#include "atomic/atomic.h"
#include "hlist.h"
#include "fs/fat.h"
#include "fs/stat.h"
#include "param.h"
#include "radix-tree.h"

#define E_ISDIR(entry) (((entry)->raw.attr & FAT_ATTR_DIR) > 0)
#define E_ISFILE(entry) (((entry)->raw.attr & FAT_ATTR_FILE) > 0)
#define E_FILESIZE(entry) ((entry)->raw.size)

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
int writee(entry_t *entry, int user, uint64_t buff, int off, int n);
int reade(entry_t *entry, int user, uint64_t buff, int off, int n);
void eput(entry_t *entry);
entry_t *edup(entry_t *entry);
char *getcwd(entry_t *entry, char *buf);
int read_dents(entry_t *entry, uint32_t *offset, char *buf, int n);
void estat(entry_t *entry, struct kstat *stat);

#define ROOTINO  1   // root i-number

struct linux_dirent64 {
        uint64        d_ino;
        int64_t         d_off;
        unsigned short  d_reclen;
        unsigned char   d_type;
        char            d_name[];
};

struct address_space {
	entry_t *host;		/* owner: inode, block_device */
	struct radix_tree_root	page_tree;	/* radix tree of all pages */
	// rwlock_t		tree_lock;	/* and rwlock protecting it */
	// unsigned int		i_mmap_writable;/* count VM_SHARED mappings */
	// struct prio_tree_root	i_mmap;		/* tree of private and shared mappings */
	// struct list_head	i_mmap_nonlinear;/*list VM_NONLINEAR mappings */
	// spinlock_t		i_mmap_lock;	/* protect tree, count, list */
	// unsigned int		truncate_count;	/* Cover race condition with truncate */
	unsigned long		nrpages;	/* number of total pages */
	spinlock_t		page_lock;	
} __attribute__((aligned(sizeof(long))));
	/*
	 * On most architectures that alignment is already the case; but
	 * must be enforced here for CRIS, to let the least signficant bit
	 * of struct page's "mapping" pointer be used for PAGE_MAPPING_ANON.
	 */

typedef struct address_space address_space_t;
#endif