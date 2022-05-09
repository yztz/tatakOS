#ifndef _H_FAT_
#define _H_FAT_

#include "types.h"

#define MSDOS_NAME 11

/* 引导扇区 JUMP + BPB + EXT-BPB */
struct fat_boot_sector {
	uint8_t	ignored[3];	/* Boot strap short or near jump 引导程序短跳或近跳 */
	uint8_t	system_id[8];	/* Name - can be used to special case partition manager volumes 名称——可以用于特殊情况下分区管理器的卷*/
	uint16_t sector_size;	/* bytes per logical sector 每个逻辑扇区字节数 */
	uint8_t	sec_per_clus;	/* sectors/cluster 扇区/簇 */
	uint16_t	reserved;	/* reserved sectors 保留扇区  */
	uint8_t	fats;		/* number of FATs 文件分配表的数量 */
	uint8_t	dir_entries[2];	/* root directory entries  根目录条目 fat32必须为0*/
	uint8_t	sectors[2];	/* number of sectors 小扇区数 fat32必须为0*/
	uint8_t	media;		/* media code 媒体码 */
	uint16_t	fat_length;	/* sectors/FAT 扇区/分配表 扇区数 fat32必须为0*/
	uint16_t	secs_track;	/* sectors per track 每磁轨扇区数 */
	uint16_t	heads;		/* number of heads 磁头数量 */
	uint32_t	hidden;		/* hidden sectors (unused) 隐藏扇区数 */
	uint32_t	total_sect;	/* number of sectors (if sectors == 0) 总扇区数量  */

	union {
		struct {
			/*  Extended BPB Fields for FAT16 为FAT16扩展了BPB字段 */
			uint8_t	drive_number;	/* Physical drive number 物理驱动编号*/
			uint8_t	state;		/* undocumented, but used for mount state.  没有记录，但用于(嵌入式)*/
			uint8_t	signature;  /* extended boot signature 延长启动签名*/
			uint8_t	vol_id[4];	/* volume ID 卷ID*/
			uint8_t	vol_label[MSDOS_NAME];	/* volume label 卷表*/
			uint8_t	fs_type[8];		/* file system type 文件系统类型*/
			/* other fields are not added here */
		} fat16;

		struct {
			/* only used by FAT32 仅仅用于FAT32 */
			uint32_t	length;		/* sectors/FAT 扇区/簇 长度 */
			uint16_t	flags;		/* bit 8: fat mirroring, 簇镜像
						   		   low 4: active fat    活跃簇 */ 
			uint8_t	version[2];	/* major, minor filesystem version version[0]:主 version[1]:次文件系统版本 */
			uint32_t	root_cluster;	/* first cluster in root directory 在根目录第一个簇 */
			uint16_t	info_sector;	/* filesystem info sector 文件系统信息扇区 */
			uint16_t	backup_boot;	/* backup boot sector 备份引导扇区 */
			uint16_t	reserved2[6];	/* Unused */
			/* Extended BPB Fields for FAT32 为FAT32扩展了BPB字段*/
			uint8_t	drive_number;   /* Physical drive number */
			uint8_t    state;       	/* undocumented, but used for mount state. */
			uint8_t	signature;  /* extended boot signature */
			uint8_t	vol_id[4];	/* volume ID */
			uint8_t	vol_label[MSDOS_NAME];	/* volume label */
			uint8_t	fs_type[8];		/* file system type */
			/* other fields are not added here */
		} fat32;
	};
} __attribute__ ((packed));

// struct fat_boot_fsinfo {
// 	uint32_t   signature1;	/* 0x41615252L signature:署名,签名,信号*/
// 	uint32_t   reserved1[120];	/* Nothing as far as I can tell 据我所知没有 */
// 	uint32_t   signature2;	/* 0x61417272L */
// 	uint32_t   free_clusters;	/* Free cluster count.  -1 if unknown 空闲簇数量 */
// 	uint32_t   next_cluster;	/* Most recently allocated cluster 最近分配的簇 */
// 	uint32_t   reserved2[4];
// } __attribute__ ((packed));

/* 短文件名目录项 */ 
struct msdos_dir_entry {
	uint8_t	name[MSDOS_NAME];/* name and extension 名称和扩展 */
	uint8_t	attr;		/* attribute bits 属性位 */
	uint8_t    lcase;		/* Case for base and extension 基础和扩展的案例 */
	uint8_t	ctime_cs;	/* Creation time, centiseconds (0-199) 创建时间、厘秒 */
	uint16_t	ctime;		/* Creation time 创建时间 */
	uint16_t	cdate;		/* Creation date 创建日期 */
	uint16_t	adate;		/* Last access date 最后访问日期 */
	uint16_t	starth;	/* High 16 bits of cluster in FAT32 FAT32簇高16位 */
	uint16_t	mtime;		/* 修改时间 */
	uint16_t	mdate;		/* 修改日期 */
	uint16_t	startl;		/* High 16 bits of cluster in FAT32 FAT32簇低16位 */
	uint32_t	size;		/* file size (in bytes) 文件大小 */
} __attribute__ ((packed));

/* 长文件名插槽 */
struct msdos_dir_slot {
	uint8_t    id;		/* sequence number for slot 槽序列号 */
	uint8_t    name0_4[10];	/* first 5 characters in name 名称的前5个字符 */
	uint8_t    attr;		/* attribute byte 属性类型 */
	uint8_t    reserved;	/* always 0 */
	uint8_t    alias_checksum;	/* checksum for 8.3 alias 8.3别名校验和 */
	uint8_t    name5_10[12];	/* 6 more characters in name 姓名中多6个字符 */
	uint16_t   start;		/* starting cluster number, 0 in long slots 起始簇编号，长槽为0*/
	uint8_t    name11_12[4];	/* last 2 characters in name 名称的最后2个字符 */
} __attribute__ ((packed));


typedef struct _clus32_t {
	struct {
		uint32_t next : 28;
		uint32_t resv : 4;	// 保留为0
	};
} clus32_t;


typedef struct _fat32_t {
	uint dev;	/* 设备号 */

	uint32_t fat_start_sector; 	/* FAT起始扇区号 */
	uint32_t fat_tbl_sectors; 	/* FAT表扇区数 */
	uint32_t fat_tbl_num;		/* FAT表数目 */

	uint32_t sec_per_cluster;	/* 每簇扇区数 */
	uint32_t bytes_per_sec;		/* 扇区字节数 */

	uint32_t root_cluster;		/* 根目录扇区号 */

} fat32_t;

typedef enum _FAT_RESULT_t{
	FR_OK,
	FR_ERR,

} FR_t;


#endif