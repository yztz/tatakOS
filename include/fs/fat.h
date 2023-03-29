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

#define FAT_ATTR_READ_ONLY  0x01
#define FAT_ATTR_HIDDEN     0x02
#define FAT_ATTR_SYSTEM     0x04
#define FAT_ATTR_VOLUME_ID  0x08
#define FAT_ATTR_DIRECTORY  0x10
#define FAT_ATTR_ARCHIVE    0x20

#define FAT_IS_DIR(attr) ((attr) & FAT_ATTR_DIRECTORY)
#define FAT_IS_FILE(attr) ((attr) & FAT_ATTR_ARCHIVE)

#define FAT_NAME_BODY_L_CASE 0x08
#define FAT_NAME_EXT_L_CASE 0x10

#define FAT_DOT     ".          "
#define FAT_DOTDOT  "..         "


// 获取目录项中的簇号
#define FAT_FETCH_CLUS(item) (((item)->starth << 16) | (item)->startl)


/// @brief 引导扇区 JUMP + BPB + EXT-BPB
struct fat_boot_sector {
	/// @brief Boot strap short or near jump 引导程序短跳或近跳
	uint8_t	ignored[3];
	/// @brief Name - can be used to special case partition manager volumes 名称——可以用于特殊情况下分区管理器的卷
	uint8_t	system_id[8];
	/// @brief bytes per logical sector 每个逻辑扇区字节数
	uint16_t sector_size;
	/// @brief sectors/cluster 扇区/簇
	uint8_t	sec_per_clus;
	/// @brief reserved sectors 保留扇区
	uint16_t	reserved;
	/// @brief number of FATs 文件分配表的数量
	uint8_t	fats;
	/// @brief root directory entries  根目录条目 fat32必须为0
	uint8_t	dir_entries[2];
	/// @brief number of sectors 小扇区数 fat32必须为0
	uint8_t	sectors[2];
	/// @brief media code 媒体码
	uint8_t	media;
	/// @brief sectors/FAT 扇区/分配表 扇区数 fat32必须为0
	uint16_t	fat_length;
	/// @brief sectors per track 每磁轨扇区数
	uint16_t	secs_track;
	/// @brief number of heads 磁头数量
	uint16_t	heads;
	/// @brief hidden sectors (unused) 隐藏扇区数
	uint32_t	hidden;
	/// @brief number of sectors (if sectors == 0) 总扇区数量
	uint32_t	total_sect;

	union {
		struct {
			/*  Extended BPB Fields for FAT16 为FAT16扩展了BPB字段 */
            
			/// @brief Physical drive number 物理驱动编号
			uint8_t	drive_number;
			/// @brief undocumented, but used for mount state.  没有记录，但用于(嵌入式)
			uint8_t	state;
			/// @brief extended boot signature 延长启动签名
			uint8_t	signature;
			/// @brief volume ID 卷ID
			uint8_t	vol_id[4];
			/// @brief volume label 卷表
			uint8_t	vol_label[MSDOS_NAME];
			/// @brief file system type 文件系统类型
			uint8_t	fs_type[8];

			/* other fields are not added here */
		} fat16;

		struct {
			/* only used by FAT32 仅仅用于FAT32 */

			/// @brief sectors/FAT 扇区/簇 长度
			uint32_t	length;
			/**
			 * @brief bit 8: fat mirroring, 簇镜像
			 *	      low 4: active fat    活跃簇
			 */
			uint16_t	flags;
			/// @brief major, minor filesystem version version[0]:主 version[1]:次文件系统版本 
			uint8_t	version[2];
			/// @brief first cluster in root directory 在根目录第一个簇
			uint32_t	root_cluster;
			/// @brief filesystem info sector 文件系统信息扇区
			uint16_t	info_sector;
			/// @brief backup boot sector 备份引导扇区
			uint16_t	backup_boot;
			/// @brief Unused
			uint16_t	reserved2[6];

			/* Extended BPB Fields for FAT32 为FAT32扩展了BPB字段*/

			/// @brief Physical drive number
			uint8_t	drive_number;
			/// @brief undocumented, but used for mount state.
			uint8_t    state;
			/// @brief extended boot signature
			uint8_t	signature;
			/// @brief volume ID
			uint8_t	vol_id[4];
			/// @brief volume label
			uint8_t	vol_label[MSDOS_NAME];
			/// @brief file system type
			uint8_t	fs_type[8];

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
    /// @brief day
    uint16_t day : 5;
    /// @brief month
    uint16_t month : 4;
    /// @brief year
    uint16_t year_from_1980 : 7;
} entry_date_t;


typedef struct _entry_time_t {
    /// @brief `2` seconds
    uint16_t second_2 : 5;
    /// @brief minutes
    uint16_t minute : 6;
    /// @brief hours
    uint16_t hour : 5;
} entry_time_t;

/// @brief diritem of short filename 
struct dir_item {
	/// @brief name and extension 名称和扩展
	uint8_t	name[MSDOS_NAME];
	/// @brief attribute bits 属性位
	uint8_t	attr;
	/// @brief Case for base and extension 基础和扩展的案例
	uint8_t    lcase;
	/// @brief Creation time, centiseconds (0-199) 创建时间 毫秒
	uint8_t	ctime_cs;
	/// @brief Creation time 创建时间
	entry_time_t	ctime;
	/// @brief Creation date 创建日期
	entry_date_t	cdate;
	/// @brief Last access date 最后访问日期
	entry_date_t	adate;
	/// @brief High 16 bits of cluster in FAT32 FAT32簇高16位
	uint16_t	starth;
	/// @brief Modify time 修改时间
	entry_time_t	mtime;
	/// @brief Modify date 修改日期
	entry_date_t	mdate;
	/// @brief High 16 bits of cluster in FAT32 FAT32簇低16位
	uint16_t	startl;
	/// @brief file size (in bytes) 文件大小
	uint32_t	size;
} __attribute__ ((packed));

typedef struct dir_item dir_item_t;

/// @brief long filename slot
struct dir_slot {
	/// @brief sequence number for slot 槽序列号
	uint8_t    id;
	/// @brief first 5 characters in name 名称的前5个字符
	uint8_t    name0_4[10];
	/// @brief attribute byte 属性类型
	uint8_t    attr;
	/// @brief always 0
	uint8_t    reserved;
	/// @brief checksum for 8.3 alias 8.3别名校验和
	uint8_t    alias_checksum;
	/// @brief 6 more characters in name 姓名中多6个字符
	uint8_t    name5_10[12];
	/// @brief starting cluster number, 0 in long slots 起始簇编号，长槽为0
	uint16_t   start;
	/// @brief last 2 characters in name 名称的最后2个字符
	uint8_t    name11_12[4];
} __attribute__ ((packed));

typedef struct dir_slot dir_slot_t;

typedef struct _fat32_t {
	/// @brief dev no
	uint dev;
	/// @brief root entry
	struct fat_entry *root;
	/// @brief protect own fields
	spinlock_t lock;
	/// @brief fat begin sector
	uint32_t fat_start_sector;
	/// @brief the num of sectors of fat table
	uint32_t fat_tbl_sectors;
	/// @brief the num of fat tables
	uint32_t fat_tbl_num;
	/// @brief the num of sectors per cluster
	uint32_t sec_per_cluster;
	/// @brief the num of bytes per sector
	uint32_t bytes_per_sec;
	/// @brief the num of bytes per cluster
	uint32_t bytes_per_cluster;
	/// @brief root sector no
	uint32_t root_cluster;
	/// @brief lock to protect entry cache
	spinlock_t cache_lock;
	/// @brief fat32 fsinfo
	struct fat_fsinfo fsinfo;
	/// @brief list of dirty entry
	list_head_t fat_dirty;
} fat32_t;

typedef enum _FAT_RESULT_t{
	FR_OK,
	FR_ERR,
	FR_CONTINUE, // indicates to continue
} FR_t;

/**
 * @brief The handler used to traverse a directory.
 *        It will receive 4 params: 
 *          1. item    - current dir entry
 *          2. name    - current dir entry name
 *          3. offset  - current dir entry offset in directory
 *          4. __state - used to save state in this round of traversal
 *        Returning FR_OK/FR_ERR means end, or FR_CONTINUE to continue.
 */
typedef FR_t (*travs_handler_t)(dir_item_t *item, const char *name, off_t offset, void *__state);

/**
 * @brief read, parse fat superblock, and mount it
 * 
 * @param dev 
 * @param ppfat 
 * @return FR_t 
 */
FR_t fat_mount(uint dev, fat32_t **ppfat);

/**
 * @brief 在指定目录簇下创建一个新的（空的）目录项，无论长短文件名，都将创建长目录项
 * @dir_clus: 目录簇号
 * @cname: 目录项名称
 * @attr: 目录项属性
 * @offset: 用于返回的目录项偏移量
 */
FR_t fat_create_entry(fat32_t *fat, uint32_t dir_clus, const char *cname, uint8_t attr, dir_item_t *item, uint32_t *offset);


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
FR_t fat_dirlookup(fat32_t *fat, uint32_t dir_clus, const char *name, dir_item_t *ret_item, uint32_t *offset);

/**
 * @brief a utility to 
 * 
 * @param fat 
 * @param dir_clus 
 * @param dir_offset 
 * @param handler 
 * @param state 
 * @return FR_t 
 */
FR_t fat_travs_logical_dir(fat32_t *fat, uint32_t dir_clus, uint32_t dir_offset, travs_handler_t handler, void *state);
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
