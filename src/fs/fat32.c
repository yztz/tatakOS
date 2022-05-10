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
#define FAT_CLUS_FREE 0x0       // 空闲簇
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


FR_t fat_append_cluster(fat32_t *fat, uint32_t prev, uint32_t new) {
    buf_t *buf = bread(fat->dev, clus2fatsec(fat, prev));
    uint32_t offset = clus2offset(fat, prev);
    uint32_t next = *(uint32_t *)(buf->data + offset);
    if(FAT_CLUS_END != next) {
        panic("fat_append_cluster:prev is not the end");
    }
    *(uint32_t *)(buf->data + offset) = new;
    brelse(buf);
}

/* 读取簇 */
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

/* 申请指定数量的簇并将它们串在一起 */
FR_t fat_alloc_cluster(fat32_t *fat, uint32_t *news, int n) {
    const int entry_per_sect = BPS(fat) / 4;
    uint32_t sect = fat->fat_start_sector;
    int cnt = n - 1;
    uint32_t next = 0;

    for(int i = 0; i < fat->fat_tbl_sectors; i++, sect++) {
        int changed = 0; // 用于标记当前的块是否被写
        buf_t *b = bread(fat->dev, sect);
        uint32_t *entry = (uint32_t *)b->data;
        for(int j = 0; j < entry_per_sect; j++) { // 遍历FAT项
            if(i == 0 && j < 2) continue; // 跳过第0,1号簇
            if(*(entry + j) == FAT_CLUS_FREE) { // 找到标记之
                uint32_t clus_num = i * entry_per_sect + j;
                if(next == 0) { // 如果是最后一个簇，则标记为END
                    *entry = FAT_CLUS_END;
                } else { // 如果是前面的簇，则标记为下一个簇的簇号
                    *entry = next;
                }
                next = clus_num;
                changed = 1;
                news[cnt--] = clus_num;
                if(cnt < 0) {
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

// FR_t fat_trunc(fat32_t *fat, ) {

// }


/**
 * 假设扩展名是合法的，否则将被截断
 * @param shortname: 生成的短文件名
 * @param name     : 输入原始文件名
 * @return 若文件名是合法短文件名则返回1，否则返回0
 */
int generate_shortname(char *shortname, char *name) {
    char *ext = NULL;
    int ret = 1;

    memset(shortname, ' ', FAT_SFN_LENGTH);
    
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
        ret = 0;
    } else {
        strncpy(shortname, name, len);
    }
    // 拷贝扩展名
    if(ext) {
        strncpy(shortname + 8, ext, min(strlen(ext), 3));
    }

    shortname[FAT_SFN_LENGTH] = '\0';

    return ret;
}





static uint8_t cal_checksum(char* shortname) {
    uint8_t sum = 0;
    for (int i = FAT_SFN_LENGTH; i != 0; i--) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *shortname++;
    }
    return sum;
}

static void print_slot_name(dir_slot_t *slot) {
    char name[14];
    name[13] = '\0';
    int p = 0;
    for(int i = 0; i < 10; i+=2) {
        name[p++] = slot->name0_4[i];
    }
    for(int i = 0; i < 12; i+=2) {
        name[p++] = slot->name5_10[i];
    }
    for(int i = 0; i < 4; i+=2) {
        name[p++] = slot->name11_12[i];
    }
    debug("slot name: %s", name);
}

/* 遍历目录项处理接口，不能在其中释放buf，ret既可传参也可以作为返回值*/
typedef FR_t (*entry_handler_t)(dir_item_t *item, buf_t *b, void *param, void *ret);

/**
 * 用于遍历目录的通用方法 
 */
static FR_t fat_travs_dir(fat32_t *fat, uint32_t dir_clus, entry_handler_t handler, int alloc, void *param, void *ret) {
    uint32_t curr_clus = dir_clus;
    while(1) {
        // 簇起始扇区
        uint32_t sec = clus2datsec(fat, curr_clus);
        for(int i = 0; i < fat->sec_per_cluster; i++) {
            buf_t *b = bread(fat->dev, sec + i);
            for(int j = 0; j < fat->bytes_per_sec/sizeof(struct dir_item); j++) {
                struct dir_item *item = (struct dir_item *)(b->data) + j;
                FR_t res;
                if((res = handler(item, b, param, ret)) != FR_CONTINUE) {
                    brelse(b);
                    return res; 
                }
            }
            brelse(b);
        }
        uint64_t prev_clus = curr_clus;
        curr_clus = fat_next_cluster(fat, curr_clus);
        // 触底了
        if(curr_clus == FAT_CLUS_END) {
            if(alloc) {
                if(fat_alloc_cluster(fat, &curr_clus, 1) == FR_ERR)
                    return FR_ERR;
                fat_append_cluster(fat, prev_clus, curr_clus);
            } else {
                break;
            }
        }
    }

    return FR_ERR;
}


static FR_t entry_trunc_handler(dir_item_t *item, buf_t *b, void *param, void *ret) {
    if(item->name[0] == FAT_NAME_END) {
        panic("trunc: no diritem");
    } else if(item->name[0] != FAT_NAME_FREE) {
        // found
        if(strncmp(item->name, (char *)param, FAT_SFN_LENGTH) == 0) {
            item->size = 0;
            bwrite(b);
            return FR_OK;
        }
    }

    return FR_CONTINUE;
}

FR_t fat_trunc(fat32_t *fat, uint32_t dir_clus, dir_item_t *item) { // only for file
    if(item->attr == FAT_ATTR_DIR) {
        return FR_ERR;
    }
    return fat_travs_dir(fat, dir_clus, entry_trunc_handler, 0, item->name, NULL);
}



/* 这里的ret既可以做为返回值也可以做为参数来传递 */
static FR_t entry_alloc_handler(dir_item_t *item, buf_t *b, void *param, void *ret) {
    // 一个空的目录项
    if(item->name[0] == FAT_NAME_END || item->name[0] == FAT_NAME_FREE) {
        *item = *(dir_item_t *)param;
        bwrite(b);
    } else {
        return FR_CONTINUE;
    }
}

static void generate_dot(fat32_t *fat, uint32_t parent_clus, uint32_t curr_clus) {
    dir_item_t item;
    uint32_t sect = clus2datsec(fat, curr_clus);
    item.attr = FAT_ATTR_DIR;
    // Note: .与..中只保存了簇号
    buf_t *b = bread(fat->dev, sect);
    strncpy(item.name, ".          ", FAT_SFN_LENGTH);
    item.startl = curr_clus & FAT_CLUS_LOW_MASK;
    item.starth = curr_clus >> 16;
    *(dir_item_t *)(b->data) = item;

    strncpy(item.name, "..         ", FAT_SFN_LENGTH);
    item.startl = parent_clus & FAT_CLUS_LOW_MASK;
    item.starth = parent_clus >> 16;
    *(dir_item_t *)(b->data + sizeof(dir_item_t)) = item;
}

/* 在指定目录簇下创建一个新的（空的）目录项 */
FR_t fat_alloc_entry(fat32_t *fat, uint32_t dir_clus, const char *cname, uint8_t attr, dir_item_t *item) {
    char name[MAX_FILE_NAME];
    uint32_t clus;
    // dir_slot_t slot;

    strncpy(name, cname, MAX_FILE_NAME);
    // todo:为长文件名建立目录项
    int UNUSED(snflag) = generate_shortname(item->name, name); // 结束符'\0'的溢出并不重要
    
    // 预分配一个簇
    if(fat_alloc_cluster(fat, &clus, 1) == FR_ERR)
        panic("fat_alloc_entry: alloc clus fail");

    // 构造目录项
    item->attr = attr;
    item->startl = clus & FAT_CLUS_LOW_MASK;
    item->starth = clus >> 16;
    item->size = 0;
    if(fat_travs_dir(fat, dir_clus, entry_alloc_handler, 1, &item, NULL) == FR_ERR)
        panic("fat_alloc_entry: fail");

    if(attr == FAT_ATTR_DIR) { // 如果是目录，创建'.'与'..'
        generate_dot(fat, dir_clus, clus);
    }

    return FR_OK;
}


static FR_t lookup_handler(dir_item_t *item, buf_t *b, void *param, void *ret) {
    if(item->name[0] == FAT_NAME_END) {
        return FR_ERR; // 找不到了
    } else if(item->name[0] == FAT_NAME_FREE) {
        // JUST a free entry
    } else {
        if(FAT_IS_LFN(item->attr)) { // 长文件名标识
            // nothing to do
            dir_slot_t *slot = (dir_slot_t *)item;
            print_slot_name(slot);
            debug("slot id: %d eof: %d checksum: %d", FAT_LFN_ID(slot->id), FAT_LFN_END(slot->id), slot->alias_checksum);
        } else { // 短文件
            print_dir_item(item);
            debug("mychecksum is: %d", cal_checksum((char *)item->name));
            if(strncmp((char *)param, (char *)item->name, FAT_SFN_LENGTH) == 0) {
                *(dir_item_t *)ret = *item;
                return FR_OK;
            }
        }
    }
    return FR_CONTINUE;
}

/* 在指定目录簇下寻找名为name的目录项 */
FR_t fat_dirlookup(fat32_t *fat, uint32_t dir_clus, const char *cname, struct dir_item *ret) {
    uint32_t curr_clus = dir_clus;
    // 生成对应的短文件名
    char short_name[FAT_SFN_LENGTH + 1];
    char name[MAX_FILE_NAME];
    strncpy(name, cname, MAX_FILE_NAME);
    generate_shortname(short_name, name);
    debug("the shorname is: %s", short_name);

    return fat_travs_dir(fat, dir_clus, lookup_handler, 0, short_name,ret);
}





