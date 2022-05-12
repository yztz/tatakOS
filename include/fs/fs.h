#ifndef _H_FS_
#define _H_FS_

#include "types.h"
#include "atomic/sleeplock.h"
#include "atomic/atomic.h"
#include "hlist.h"
#include "fs/fat.h"

#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device

#define E_ISDIR(entry) (((entry)->raw.attr & FAT_ATTR_DIR) > 0)
#define E_ISFILE(entry) (((entry)->raw.attr & FAT_ATTR_FILE) > 0)
#define E_FILESIZE(entry) ((entry)->raw.size)

//IN RAM
struct fat_entry {
    // uint        dev;        /* 所属设备号 */
    int    ref;        /* 引用数 */

    // uint8_t     attr;       /* 文件属性 */
    // uint16_t	ctime;		/* 创建时间 */
	  // uint16_t	cdate;		/* 创建日期 */
	  // uint16_t	adate;		/* 最后访问日期 */
    // uint16_t	mtime;		/* 修改时间 */
	  // uint16_t	mdate;		/* 修改日期 */

    // struct hlist_node node;
    dir_item_t raw;
    fat32_t *fat;       /* fat对象引用 */
    
    uint nlink;
    // uint32_t    file_size;  /* 文件大小 */
    uint32_t    clus_start; /* 数据起始簇号 */

    struct fat_entry* parent; /* 父目录 */
    uint32_t    clus_offset; /* 簇内字节偏移量 */

    sleeplock_t  lock;       /* io */
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
entry_t *create(entry_t *from, char *path, short type);
////////////////////////OLD///////////////////////////
// On-disk file system format.
// Both the kernel and user programs use this header file.


#define ROOTINO  1   // root i-number

// // Disk layout:


// // Inodes per block.
// #define IPB           (BSIZE / sizeof(struct dinode))

// // Block containing inode i
// #define IBLOCK(i, sb)     ((i) / IPB + sb.inodestart)

// // Bitmap bits per block
// #define BPB           (BSIZE*8)

// // Block of free map containing bit for block b
// #define BBLOCK(b, sb) ((b)/BPB + sb.bmapstart)

// // Directory is a file containing a sequence of dirent structures.
// #define DIRSIZ 14

// struct dirent {
//   ushort inum;
//   char name[DIRSIZ];
// };



#endif