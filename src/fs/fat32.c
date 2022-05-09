#include "fs/fs.h"
#include "fs/blk_device.h"
#include "mm/alloc.h"
#include "common.h"
#include "str.h"

#ifdef DEBUG 
#include "printf.h"
#define debug(fmt, ...) printf("[FAT] "fmt"\n", ##__VA_ARGS__);
#else 
#define debug(fmt, ...)
#endif

#define FAT_CLUS_END 0x0FFFFFFF // 簇结束标识
#define FAT_CLUS_FREE 0x0
#define __FAT_LFN_ATTR 0xF    // 长文件名标识
#define FAT_IS_LFN(attr) ((attr) == __FAT_LFN_ATTR)

#define FAT_NAME_END 0x0
#define FAT_NAME_FREE 0xe5
#define FAT_NAME_SPEC 0x2e

#define __FAT_LFN_END 0x40
#define __FAT_LFN_ID_MASK (0x20 - 1)
#define FAT_LFN_ID(id) ((id) & __FAT_LFN_ID_MASK)
#define FAT_LFN_END(id) (((id) & __FAT_LFN_END) > 0)


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

    fat->root_cluster = dbr->fat32.root_cluster;

    debug("start sector   is %d", fat->fat_start_sector);
    debug("fat sectors    is %d", fat->fat_tbl_sectors);
    debug("sec per clus   is %d", fat->sec_per_cluster);
    debug("bytes per clus is %d", fat->bytes_per_sec);
    debug("fat table num  is %d", fat->fat_tbl_num);
    debug("root cluster   is %d", fat->root_cluster);
}

static entry_t *get_root(fat32_t *fat) {
    entry_t *root = (entry_t *)kmalloc(sizeof(entry_t));
    if(!root) {
        panic("get_root: alloc fail");
    }
    root->ref = 0;
    root->fat = fat;
    root->nlink = 1;
    root->clus_start = fat->root_cluster;
    initsleeplock(&root->lock, "root_lock");
    return root;
}

/* 读取fat超级块并做解析 */
FR_t fat_mount(uint dev, fat32_t **ppfat) {
    fat32_t *fat = (fat32_t *)kmalloc(sizeof(fat32_t));
    if(!fat)
        panic("fat_mount: alloc fail");
    // 常规字段初始化
    fat->dev = dev;
    fat->cache_lock = INIT_SPINLOCK(fat_cache_lock);

    buf_t *buffer = bread(dev, 0);
    // 解析fatDBR
    fat_parse_hdr(fat, (struct fat_boot_sector*)buffer->data);
    brelse(buffer);

    fat->root = get_root(fat);

    *ppfat = fat;

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


FR_t fat_alloc_cluster(fat32_t *fat, uint32_t *news, int n) {
    const int entry_per_sect = BPS(fat) / 4;
    uint32_t sect = fat->fat_start_sector;
    int cnt = 0;

    for(int i = 0; i < fat->fat_tbl_sectors; i++, sect++) {
        int changed = 0; // 用于标记当前的块是否被写
        buf_t *b = bread(fat->dev, sect);
        uint32_t *entry = (uint32_t *)b->data;
        for(int j = 0; j < entry_per_sect; j++) { // 遍历FAT项
            if(i == 0 && j < 2) continue; // 跳过第0,1号簇
            if(*(entry + j) == FAT_CLUS_FREE) { // 找到标记之
                *entry = FAT_CLUS_END;
                changed = 1;
                news[cnt++] = i * entry_per_sect + j;
                if(cnt == n) {
                    bwrite(b);
                    brelse(b);
                    return FR_OK;
                }
            } 
        }
        if(changed) {
            bwrite(b);
        }
        brelse(b);
    }

    return FR_ERR;
}

FR_t fat_destory_clus_chain(fat32_t *fat, uint32_t clus) {
    uint32 cclus = clus;

    while(cclus != FAT_CLUS_END) {
        uint32_t *pclus;

        buf_t *b = bread(fat->dev, clus2fatsec(fat, cclus));
        pclus = (uint32_t *)(b->data + clus2offset(fat, cclus));
        cclus = *pclus;
        *pclus = FAT_CLUS_FREE;

        bwrite(b);
        brelse(b);
    }

    return FR_OK;
}


/* 
*假设扩展名是合法的，否则将被截断*
shortname: 生成的短文件名
name     : 输入原始文件名
 */
void generate_shortname(char *shortname, char *name) {
    memset(shortname, ' ', FAT_SFN_LENGTH);
    char *ext = NULL;
    
     // 转换大写
    to_upper(name);
    
    for(int i = strlen(name) - 1; i >= 0; i--) {
        if(*(name + i) == '.') {
            *(name + i) = '\0';
            ext = name + i + 1;
            break;
        }
    }
   
    // 拷贝
    int len = strlen(name);
    if(len > 8) {
        strncpy(shortname, name, 6);
        *(shortname + 6) = '~';
        *(shortname + 7) = '1'; // todo: 暂时先不检查重名了..
    } else {
        strncpy(shortname, name, len);
    }
    // 拷贝扩展名
    if(ext) {
        strncpy(shortname + 8, ext, min(strlen(ext), 3));
    }

    shortname[FAT_SFN_LENGTH] = '\0';
}





static uint8_t cal_checksum(char* shortname) {
    uint8_t sum = 0;
    for (int i = FAT_SFN_LENGTH; i != 0; i--) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *shortname++;
    }
    return sum;
}




/* 在指定目录簇下寻找名为name的目录项 */
FR_t fat_dirlookup(fat32_t *fat, uint32_t dir_clus, const char *cname, struct dir_item *ret_item) {
    uint32_t curr_clus = dir_clus;
    // 生成对应的短文件名
    char short_name[FAT_SFN_LENGTH + 1];
    char name[MAX_FILE_NAME];
    strncpy(name, cname, MAX_FILE_NAME);
    generate_shortname(short_name, name);
    debug("the shorname is: %s", short_name);
    while(curr_clus != FAT_CLUS_END) {
        // 簇起始扇区
        uint32_t sec = clus2datsec(fat, curr_clus);
        for(int i = 0; i < fat->sec_per_cluster; i++) {
            buf_t *b = bread(fat->dev, sec + i);
            for(int j = 0; j < fat->bytes_per_sec/sizeof(struct dir_item); j++) {
                struct dir_item *item = (struct dir_item *)(b->data) + j;
                if(item->name[0] == FAT_NAME_END) {
                    brelse(b);
                    return FR_ERR;
                } else if(item->name[0] == FAT_NAME_FREE) {
                    // JUST a free entry
                } else {
                    if(FAT_IS_LFN(item->attr)) { // 长文件名标识
                        // nothing to do
                        // struct dir_slot *slot = (struct dir_slot *)item;
                        // debug("slot id: %d eof: %d", FAT_LFN_ID(slot->id), FAT_LFN_END(slot->id));
                    } else { // 短文件
                        if(strncmp(short_name, (char *)item->name, FAT_SFN_LENGTH) == 0) {
                            *ret_item = *item;
                            brelse(b);
                            return FR_OK;
                        }
                    }
                }
            }
            brelse(b);
        }
        curr_clus = fat_next_cluster(fat, curr_clus);
    }
    // not found
    return FR_ERR;
}






