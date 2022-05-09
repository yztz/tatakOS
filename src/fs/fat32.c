#include "fs/fat.h"
#include "fs/blk_device.h"
#include "str.h"

#ifdef DEBUG 
#include "printf.h"
#define debug(fmt, ...) printf("[FAT] "fmt"\n", __VA_ARGS__);
#else 
#define debug(fmt, ...)
#endif

#define FAT_CLUS_END 0x0FFFFFFF // 簇结束标识
#define FAT_LFN_ATTR 0xF    // 长文件名标识


#define FAT_SEC(fat) (fat->fat_start_sector)
#define FAT_NUMS(fat) (fat->fat_tbl_num)
#define BPS(fat) (fat->bytes_per_sec)
#define SPC(fat) (fat->sec_per_cluster)
#define SPFAT(fat) (fat->fat_tbl_sectors)

// 将簇号转为fat表中对应的扇区号
#define clus2fatsec(fat, clus) (((clus)*4)/BPS(fat)+FAT_SEC(fat))
// 簇号在扇区内的偏移量
#define clus2offset(fat, clus) (((clus)*4)%BPS(fat))
// 簇号对应在数据区内的扇区号
#define clus2datsec(fat, clus) (((clus)-2)*SPC(fat)+FAT_SEC(fat)+FAT_NUMS(fat)*SPFAT(fat))

typedef struct buf buf_t;

/*  解析fat dbr */
void fat_parse_hdr(fat32_t *fat, struct fat_boot_sector* dbr) {
    // fat表起始扇区
    fat->fat_start_sector = dbr->reserved; 
    // fat表扇区数
    fat->fat_tbl_sectors = dbr->fat32.length;
    // 每簇扇区数
    fat->sec_per_cluster = dbr->sec_per_clus;
    // 扇区字节数
    fat->bytes_per_sec = dbr->sector_size;
    // fat表数目
    fat->fat_tbl_num = dbr->fats;

    debug("start sector   is %d", fat->fat_start_sector);
    debug("fat sectors    is %d", fat->fat_tbl_sectors);
    debug("sec per clus   is %d", fat->sec_per_cluster);
    debug("bytes per clus is %d", fat->bytes_per_sec);
    debug("fat table num  is %d", fat->fat_tbl_num);
}

/* 读取fat超级块并做解析 */
FR_t fat_read_sb(uint dev, fat32_t *fat) {
    fat->dev = dev;
    buf_t *buffer = bread(dev, 0);
    // 解析fatDBR
    fat_parse_hdr(fat, (struct fat_boot_sector*)buffer->data);
    brelse(buffer);

    return FR_OK;
}

/* 获取下一个簇号 */
uint32_t fat_next_cluster(fat32_t *fat, uint32_t cclus) {
    buf_t *buf = bread(fat->dev, clus2fatsec(fat, cclus));
    uint32_t offset = clus2offset(fat, cclus);
    uint32_t next = *(uint32_t *)(buf->data + offset);
    brelse(buf);

    return next;
}

/* 读取簇 */ // TODO:
FR_t fat_read_cluster(fat32_t *fat, char *buffer, uint32_t cclus) {
    uint32_t sect = clus2datsec(fat, cclus);

    for(int i = 0; i < SPC(fat); i++) {
        // 读取扇区
        buf_t *b = bread(fat->dev, sect + i);
        // 复制
        memmove(buffer, b->data, BPS(fat));
        brelse(b);
        buffer += BPS(fat);
    }

    return FR_OK;
}


FR_t fat_alloc_cluster(fat32_t *fat, uint32_t *new) {
    const int entry_per_sect = BPS(fat) / 4;
    uint32_t sect = fat->fat_start_sector;

    for(int i = 0; i < fat->fat_tbl_sectors; i++, sect++) {
        buf_t *b = bread(fat->dev, sect);
        uint32_t *entry = (uint32_t *)b->data;
        for(int j = 0; j < entry_per_sect; j++) {
            if(i == 0 && j < 2) continue; // 跳过第0,1号簇
            if(*(entry + j) == 0) { // 找到了
                *entry = FAT_CLUS_END;
                bwrite(b);
                brelse(b);
                *new = i * entry_per_sect + j;
                return FR_OK;
            } 
        }
        brelse(b);
    }

    return FR_ERR;
}








