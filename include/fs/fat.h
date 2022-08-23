#ifndef _H_FAT_
#define _H_FAT_

#include "types.h"
#include "atomic/spinlock.h"
#include "list.h"


#define FAT_SFN_LENGTH (8 + 3)      // 短文件名长度
#define FAT_LFN_LENGTH (5 + 6 + 2)  // 长文件名长度

#define MSDOS_NAME FAT_SFN_LENGTH

#define FAT_CLUS_LOW_MASK  ((1<<16) - 1)
#define FAT_CLUS_HIGH_MASK (~FAT_CLUS_LOW_MASK)

#define FAT_ATTR_DIR 0b10000
#define FAT_ATTR_FILE 0b100000

#define FAT_IS_DIR(attr) ((attr) & FAT_ATTR_DIR)
#define FAT_IS_FILE(attr) ((attr) & FAT_ATTR_FILE)

// 获取目录项中的簇号
#define FAT_FETCH_CLUS(item) (((item)->starth << 16) | (item)->startl)

/* 引导扇区 JUMP + BPB + EXT-BPB */
struct fat_boot_sector {
	uint8_t	ignored[3];	    /* Boot strap short or near jump 引导程序短跳或近跳 */
	uint8_t	system_id[8];	/* Name - can be used to special case partition manager volumes 名称——可以用于特殊情况下分区管理器的卷*/
	uint16_t sector_size;	/* bytes per logical sector 每个逻辑扇区字节数 */
	uint8_t	sec_per_clus;	/* sectors/cluster 扇区/簇 */
	uint16_t	reserved;	/* reserved sectors 保留扇区  */
	uint8_t	fats;		    /* number of FATs 文件分配表的数量 */
	uint8_t	dir_entries[2];	/* root directory entries  根目录条目 fat32必须为0*/
	uint8_t	sectors[2];	    /* number of sectors 小扇区数 fat32必须为0*/
	uint8_t	media;		    /* media code 媒体码 */
	uint16_t	fat_length;	/* sectors/FAT 扇区/分配表 扇区数 fat32必须为0*/
	uint16_t	secs_track;	/* sectors per track 每磁轨扇区数 */
	uint16_t	heads;		/* number of heads 磁头数量 */
	uint32_t	hidden;		/* hidden sectors (unused) 隐藏扇区数 */
	uint32_t	total_sect;	/* number of sectors (if sectors == 0) 总扇区数量  */

	union {
		struct {
			/*  Extended BPB Fields for FAT16 为FAT16扩展了BPB字段 */
			uint8_t	drive_number;	        /* Physical drive number 物理驱动编号*/
			uint8_t	state;		            /* undocumented, but used for mount state.  没有记录，但用于(嵌入式)*/
			uint8_t	signature;              /* extended boot signature 延长启动签名*/
			uint8_t	vol_id[4];	            /* volume ID 卷ID*/
			uint8_t	vol_label[MSDOS_NAME];	/* volume label 卷表*/
			uint8_t	fs_type[8];		        /* file system type 文件系统类型*/
			/* other fields are not added here */
		} fat16;

		struct {
			/* only used by FAT32 仅仅用于FAT32 */
			uint32_t	length;		        /* sectors/FAT 扇区/簇 长度 */
			uint16_t	flags;		        /* bit 8: fat mirroring, 簇镜像
						   		            low 4: active fat    活跃簇 */ 
			uint8_t	version[2];	            /* major, minor filesystem version version[0]:主 version[1]:次文件系统版本 */
			uint32_t	root_cluster;	    /* first cluster in root directory 在根目录第一个簇 */
			uint16_t	info_sector;	    /* filesystem info sector 文件系统信息扇区 */
			uint16_t	backup_boot;	    /* backup boot sector 备份引导扇区 */
			uint16_t	reserved2[6];	    /* Unused */
			/* Extended BPB Fields for FAT32 为FAT32扩展了BPB字段*/
			uint8_t	drive_number;           /* Physical drive number */
			uint8_t    state;       	    /* undocumented, but used for mount state. */
			uint8_t	signature;              /* extended boot signature */
			uint8_t	vol_id[4];	            /* volume ID */
			uint8_t	vol_label[MSDOS_NAME];	/* volume label */
			uint8_t	fs_type[8];		        /* file system type */
			/* other fields are not added here */
		} fat32;
	};
} __attribute__ ((packed));


struct fat_fsinfo {
	char signature[4];
	uint32_t free_clusters;
	uint32_t next_free_cluster;
};


typedef struct _entry_date_t {
    uint16_t day : 5;                  // 日
    uint16_t month : 4;                // 月
    uint16_t year_from_1980 : 7;       // 年
} entry_date_t;


typedef struct _entry_time_t {
    uint16_t second_2 : 5;             // 2秒
    uint16_t minute : 6;               // 分
    uint16_t hour : 5;                 // 时
} entry_time_t;

/* 短文件名目录项 */ 
struct dir_item {
	uint8_t	name[MSDOS_NAME];   /* name and extension 名称和扩展 */
	uint8_t	attr;		        /* attribute bits 属性位 */
	uint8_t    lcase;		    /* Case for base and extension 基础和扩展的案例 */
	uint8_t	ctime_cs;	        /* Creation time, centiseconds (0-199) 创建时间 毫秒 */
	entry_time_t	ctime;		/* Creation time 创建时间 */
	entry_date_t	cdate;		/* Creation date 创建日期 */
	entry_date_t	adate;		/* Last access date 最后访问日期 */
	uint16_t	starth;	        /* High 16 bits of cluster in FAT32 FAT32簇高16位 */
	entry_time_t	mtime;		/* 修改时间 */
	entry_date_t	mdate;		/* 修改日期 */
	uint16_t	startl;		    /* High 16 bits of cluster in FAT32 FAT32簇低16位 */
	uint32_t	size;		    /* file size (in bytes) 文件大小 */
} __attribute__ ((packed));

typedef struct dir_item dir_item_t;

/* 长文件名插槽 */
struct dir_slot {
	uint8_t    id;		        /* sequence number for slot 槽序列号 */
	uint8_t    name0_4[10];	    /* first 5 characters in name 名称的前5个字符 */
	uint8_t    attr;		    /* attribute byte 属性类型 */
	uint8_t    reserved;	    /* always 0 */
	uint8_t    alias_checksum;	/* checksum for 8.3 alias 8.3别名校验和 */
	uint8_t    name5_10[12];	/* 6 more characters in name 姓名中多6个字符 */
	uint16_t   start;		    /* starting cluster number, 0 in long slots 起始簇编号，长槽为0*/
	uint8_t    name11_12[4];	/* last 2 characters in name 名称的最后2个字符 */
} __attribute__ ((packed));

typedef struct dir_slot dir_slot_t;

struct fat_entry;
typedef struct _fat32_t {
	uint dev;	                /* 设备号 */
	struct fat_entry *root;		/* 根目录 */
	spinlock_t lock;            /* 保护字段 */

	uint32_t fat_start_sector; 	/* FAT起始扇区号 */
	uint32_t fat_tbl_sectors; 	/* FAT表扇区数 */
	uint32_t fat_tbl_num;		/* FAT表数目 */

	uint32_t sec_per_cluster;	/* 每簇扇区数 */
	uint32_t bytes_per_sec;		/* 扇区字节数 */
	uint32_t bytes_per_cluster;	/* 每簇字节数 */

	uint32_t root_cluster;		/* 根目录扇区号 */

	spinlock_t cache_lock;		/* 保护缓存 */

	struct fat_fsinfo fsinfo;

	/* added for entry write back to disk */
	list_head_t fat_dirty;   	/* 所有dirty的entry的链表 */
	// list_head_t fat_io;		/* 所有进行io的entry的链表 */
} fat32_t;

typedef enum _FAT_RESULT_t{
	FR_OK,
	FR_ERR,
	FR_CONTINUE, // 用于目录继续遍历的标识
} FR_t;

typedef FR_t (*travs_handler_t)(dir_item_t *item, const char *name, off_t offset, void *state);

/* 读取fat超级块并做解析 */
FR_t fat_mount(uint dev, fat32_t **ppfat);
/* 获取下一个簇号 */
uint32_t fat_next_cluster(fat32_t *fat, uint32_t cclus);
/**
 * 在指定目录簇下创建一个新的（空的）目录项，无论长短文件名，都将创建长目录项
 * todo: 不是本函数的TODO！主要是上层函数调用时，可能还要增加父entry的文件大小？
 * @dir_clus: 目录簇号
 * @cname: 目录项名称
 * @attr: 目录项属性
 * @offset: 用于返回的目录项偏移量
 */
FR_t fat_create_entry(fat32_t *fat, uint32_t dir_clus, const char *cname, uint8_t attr, dir_item_t *item, uint32_t *offset);

FR_t __fat_alloc_cluster_reversed_order(fat32_t *fat, uint32_t *news, int n);
FR_t __fat_alloc_cluster_order(fat32_t *fat, uint32_t *news, int n);
/**
 * 实现了两类簇申请策略：反向申请与正向申请
 * 正向分配更加利于多块连读与连写
 */ 
static inline FR_t fat_alloc_cluster(fat32_t *fat, uint32_t *news, int n) {
	return __fat_alloc_cluster_order(fat, news, n);
	// return __fat_alloc_cluster_reversed_order(fat, news, n);
}
/**
 * @brief 文件扩大了，在写回磁盘之前，预先分配新的簇。
 * 
 * @param fat 
 * @param clus_start 
 * @param clus_end 
 * @param clus_cnt 
 * @param size_in_mem 
 * @return int 
 */
FR_t fat_alloc_append_clusters(fat32_t *fat, uint32_t clus_start, uint32_t *clus_end, uint64_t *clus_cnt, uint32_t size_in_mem);
FR_t fat_dirlookup(fat32_t *fat, uint32_t dir_clus, const char *name, struct dir_item *ret_item, uint32_t *offset);
FR_t fat_traverse_dir(fat32_t *fat, uint32_t dir_clus, uint32_t dir_offset, travs_handler_t handler, void *state);
FR_t fat_trunc(fat32_t *fat, uint32_t dir_clus, uint32_t offset, dir_item_t *item);
int fat_read(fat32_t *fat, uint32_t cclus, int user, uint64_t buffer, off_t off, int n);
int fat_write(fat32_t *fat, uint32_t cclus, int user, uint64_t buffer, off_t off, int n);
FR_t fat_update(fat32_t *fat, uint32_t dir_clus, uint32_t offset, dir_item_t *item);
FR_t fat_unlink(fat32_t *fat, uint32_t dir_clus, uint32_t offset, dir_item_t *item);
FR_t fat_rename(fat32_t *fat, uint32_t dir_clus, uint32_t dir_offset, dir_item_t* item, const char *newname, uint32_t *offset);
/**
 * @brief 根据文件的偏移和长度找到对应的磁盘块号(sector)，并且分段，每段包含一组连续的sectors，
 * 使用一个bio_vec结构体表示。
 * 以簇cluster为单位进行查找，因为一个簇内的扇区sector是连续的。
 * 
 * 给出一段文件在内存空间中连续的地址，长度为n，找到对应的sectors，尽量使其连续。
 * 要注意没有给disk上的entry alloc and append足够的cluster的情况！
 * @param fat 
 * @param cclus 
 * @param off 
 * @param n
 * @return  
 */
struct bio_vec *fat_get_sectors(fat32_t *fat, uint32_t cclus, int off, int n);

/**
 * @return 文件的结束簇号
 */
uint32_t get_clus_end(fat32_t *fat, uint32_t cur_clus);
uint32_t get_clus_cnt(fat32_t *fat, uint32_t cur_clus);
#endif
