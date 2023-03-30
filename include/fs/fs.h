#ifndef _H_FS_
#define _H_FS_

#include "types.h"
#include "atomic/sleeplock.h"
#include "atomic/spinlock.h"
#include "list.h"
#include "radix-tree.h"

#define E_ISDIR(entry) (((entry)->raw.attr & FAT_ATTR_DIRECTORY) > 0)
#define E_ISFILE(entry) (((entry)->raw.attr & FAT_ATTR_ARCHIVE) > 0)
#define E_FILESIZE(entry) ((entry)->raw.size)

typedef struct address_space {
	/// @brief owner: entry
	struct fat_entry *host;
	/// @brief radix tree of all pages 
	struct radix_tree_root	page_tree;
	/// @brief lock
	spinlock_t tree_lock;
	/// @brief number of total pages
	unsigned long nrpages;
} address_space_t;


struct linux_dirent64 {
    uint64_t      d_ino;
    off_t         d_off;
    unsigned short  d_reclen;
    unsigned char   d_type;
    char            d_name[];
};

//IN RAM
struct fat_entry {
    /// @brief reference count
    int ref;
    /// @brief copy of file attr
    dir_item_t raw;
    /// @brief reference of fat object
    fat32_t *fat;
    /// @brief filename
    char name[MAX_FILE_NAME];
    /// @brief link count: always 1 in fat
    uint nlink;
    /// @brief data cluster begin
    uint32_t clus_start;
    /// @brief parent entry
    struct fat_entry* parent;
    /// @brief offset in parent's data cluster 簇内字节偏移量(在父目录中的偏移量)
    uint32_t    clus_offset;
    /// @brief used for I/O
    sleeplock_t  lock;
    /// @brief pagecache
    struct address_space *i_mapping;
    /// @brief data cluster end (NOT means FAT_CLUS_END) 数据结束簇号(下一个簇为FAT_CLUS_END)
    uint32_t    clus_end;
    /// @brief cluster count
    uint64_t    clus_cnt;
    /// @brief file real size (need sync) 内存中保留的当前文件的大小，在文件写回时，如果和磁盘上的不一致，则更新磁盘
    uint32_t    size_in_mem;
    /// @brief is dirty?
    int dirty;
    /// @brief used by fat dirty list
    list_head_t e_list;
};


entry_t *namee(entry_t *from, char *path);
entry_t *nameeparent(entry_t *from, char *path, char *name);
void etrunc(entry_t *entry, off_t size);
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

page_t *find_get_page(address_space_t *mapping, unsigned long offset);
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