#include "fs/fs.h"
#include "fs/blk_device.h"
#include "mm/alloc.h"
#include "mm/vm.h"
#include "common.h"
#include "str.h"

#define QUIET
#define __MODULE_NAME__ FAT
#include "debug.h"

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
#define BPC(fat) (fat->bytes_per_cluster)

#define SPFAT(fat) (fat->fat_tbl_sectors)

// 将簇号转为fat表中对应的扇区号
#define clus2fatsec(fat, clus) (((clus)*4)/BPS(fat)+FAT_SEC(fat))
// 簇号在扇区内的偏移量
#define clus2offset(fat, clus) (((clus)*4)%BPS(fat))
// 簇号对应在数据区内的扇区号
#define clus2datsec(fat, clus) (((clus)-2)*SPC(fat)+FAT_SEC(fat)+FAT_NUMS(fat)*SPFAT(fat))

typedef struct buf buf_t;


char *DOT = ".          ";
char *DOTDOT = "..         ";

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
    // 簇字节数
    fat->bytes_per_cluster = fat->sec_per_cluster * fat->bytes_per_sec;
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
    root->ref = 1;
    root->fat = fat;
    root->nlink = 1;
    root->parent = NULL;
    root->raw.attr = FAT_ATTR_DIR;
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
    if(cclus == FAT_CLUS_END || cclus == FAT_CLUS_FREE) return cclus;

    buf_t *buf = bread(fat->dev, clus2fatsec(fat, cclus));
    uint32_t offset = clus2offset(fat, cclus);
    uint32_t next = *(uint32_t *)(buf->data + offset);
    brelse(buf);

    return next;
}


FR_t fat_append_cluster(fat32_t *fat, uint32_t prev, uint32_t new) {
    if(new == FAT_CLUS_FREE) {
        panic("fat_append_cluster: new is free");
    }

    buf_t *buf = bread(fat->dev, clus2fatsec(fat, prev));
    uint32_t offset = clus2offset(fat, prev);
    uint32_t next = *(uint32_t *)(buf->data + offset);
    if(FAT_CLUS_END != next) {
        panic("fat_append_cluster:prev is not the end");
    }
    *(uint32_t *)(buf->data + offset) = new;
    brelse(buf);

    return FR_OK;
}

/* 读取簇 */
int fat_read(fat32_t *fat, uint32_t cclus, int user, uint64_t buffer, int off, int n) {
    debug("read cluster %d", cclus);
    if(cclus == 0 || n == 0)
        return 0;
    debug("now off is %d", off)
    // 计算起始簇
    while(off >= BPC(fat)) {
        cclus = fat_next_cluster(fat, cclus);
        off -= BPC(fat);
        if(cclus == FAT_CLUS_END) {
            return 0;
        }
    }
    int rest = n;
    // 计算簇内起始扇区号
    uint32_t sect = clus2datsec(fat, cclus) + off / BPS(fat);
    // 扇区内偏移
    off %= BPS(fat);
    debug("start sect is %d off in sec is %d", sect, off);
    while(cclus != FAT_CLUS_END && rest > 0) {
        while(sect < sect + SPC(fat) && rest > 0) {
            // 计算本扇区内需要读取的字节数（取剩余读取字节数与扇区内剩余字节数的较小值）
            int len = min(rest, BPS(fat) - off);
            buf_t *b = bread(fat->dev, sect);
            either_copyout(user, buffer, b->data + off, len);
            brelse(b);
            rest -= len;
            buffer += len;

            off = 0; // 偏移量以及被抵消了，恒置为0
            sect++;
        }
        cclus = fat_next_cluster(fat, cclus);
        sect = clus2datsec(fat, cclus);
    }

    return n - rest;
}

int fat_write(fat32_t *fat, uint32_t cclus, int user, uint64_t buffer, int off, int n) {
    if(cclus == 0 || n == 0) ///todo:空文件怎么办？
        return 0;
    
    int alloc_num = 1; // 防止频繁alloc
    // 计算起始簇
    while(off >= BPC(fat)) {
        cclus = fat_next_cluster(fat, cclus);
        off -= BPC(fat);
        if(cclus == FAT_CLUS_END) {
            uint32_t prev = cclus;
            if(fat_alloc_cluster(fat, &cclus, alloc_num) == FR_ERR) {
                debug("fat_write: alloc cluster f");
                return 0;
            }
            fat_append_cluster(fat, prev, cclus);
            alloc_num++;
        }
    }
    int rest = n;
    // 计算簇内起始扇区号
    uint32_t sect = clus2datsec(fat, cclus) + off / BPS(fat);
    // 扇区内偏移
    off %= BPS(fat);
    debug("fat_write: sect is %d off is %d n is %d", sect, off, rest);
    while(rest > 0) {
        while(sect < sect + SPC(fat) && rest > 0) {
            // 计算本扇区内需要写入的字节数（取剩余读取字节数与扇区内剩余字节数的较小值）
            int len = min(rest, BPS(fat) - off);
            buf_t *b = bread(fat->dev, sect);
            either_copyin(b->data + off, user, buffer, len);
            bwrite(b);
            brelse(b);
            rest -= len;
            buffer += len;
            
            off = 0; // 偏移量以及被抵消了，恒置为0
            sect++;
        }
        if(rest == 0) break;
        cclus = fat_next_cluster(fat, cclus);
        if(cclus == FAT_CLUS_END) {
            uint32_t prev = cclus;
            if(fat_alloc_cluster(fat, &cclus, alloc_num) == FR_ERR) {
                debug("fat_write: alloc cluster f");
                return 0;
            }
            fat_append_cluster(fat, prev, cclus);
            alloc_num++;
        }
        sect = clus2datsec(fat, cclus);
    }

    return n - rest;
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
                cnt--;
                if(cnt < 0) {
                    *news = clus_num;
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

/* 释放cclus簇以及接下来的簇 置位keepfirst将会保留首节点置为END*/
FR_t fat_destory_clus_chain(fat32_t *fat, uint32_t clus, int keepfirst) {
    if(clus == FAT_CLUS_FREE) return FR_ERR;

    uint32 cclus = clus;
    uint32 idx = 0;

    while(cclus != FAT_CLUS_END) {
        uint32_t *pclus;

        buf_t *b = bread(fat->dev, clus2fatsec(fat, cclus));
        pclus = (uint32_t *)(b->data + clus2offset(fat, cclus));
        cclus = *pclus; // 得到下一个簇
        if(idx == 0 && keepfirst)
            *pclus = FAT_CLUS_END;
        else
            *pclus = FAT_CLUS_FREE;

        bwrite(b);
        brelse(b);
        idx++;
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

// static void print_slot_name(dir_slot_t *slot) {
//     char name[14];
//     name[13] = '\0';
//     int p = 0;
//     for(int i = 0; i < 10; i+=2) {
//         name[p++] = slot->name0_4[i];
//     }
//     for(int i = 0; i < 12; i+=2) {
//         name[p++] = slot->name5_10[i];
//     }
//     for(int i = 0; i < 4; i+=2) {
//         name[p++] = slot->name11_12[i];
//     }
//     debug("slot name: %s", name);
// }

/* 遍历目录项处理接口，不能在其中释放buf，ret既可传参也可以作为返回值*/
typedef FR_t (*entry_handler_t)(dir_item_t *item, buf_t *b, void *param, void *ret);

/**
 * 用于遍历目录的通用方法 
 */
static FR_t fat_travs_dir(fat32_t *fat, uint32_t dir_clus, 
                          entry_handler_t handler, 
                          int alloc, uint32_t *offset,
                          void *param, void *ret) 
{
    uint32_t curr_clus = dir_clus;
    uint32_t clus_cnt = 0; // 簇计数器
    while(1) {
        // 簇起始扇区
        uint32_t sec = clus2datsec(fat, curr_clus);
        for(int i = 0; i < fat->sec_per_cluster; i++) {
            buf_t *b = bread(fat->dev, sec + i);
            for(int j = 0; j < fat->bytes_per_sec/sizeof(dir_item_t); j++) {
                dir_item_t *item = (dir_item_t *)(b->data) + j;
                FR_t res;
                if((res = handler(item, b, param, ret)) != FR_CONTINUE) {
                    brelse(b);
                    if(offset) // 计算当前目录项在当前目录数据簇中的偏移量
                        *offset = (clus_cnt * SPC(fat) + i) * BPS(fat) + j * sizeof(dir_item_t);
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
        clus_cnt++;
    }

    return FR_ERR;
}


static FR_t file_trunc_handler(dir_item_t *item, buf_t *b, void *fat, void *name) {
    if(item->name[0] == FAT_NAME_END) {
        panic("trunc: no diritem");
    } else if(item->name[0] != FAT_NAME_FREE) {
        // found
        if(strncmp((char *)item->name, (char *)name, FAT_SFN_LENGTH) == 0) {
            item->size = 0;
            // 销毁fat链，但保留一个簇大小来减少分配频率
            fat_destory_clus_chain((fat32_t *)fat, FAT_FETCH_CLUS(item), 1);
            bwrite(b);
            return FR_OK;
        }
    }

    return FR_CONTINUE;
}

/* 
    若item为文件，设置filesize为0，此外解除链
*/
FR_t fat_trunc(fat32_t *fat, uint32_t dir_clus, dir_item_t *item) { // only for file
    if(item->attr == FAT_ATTR_DIR) {
        return FR_ERR;
    } else {
        return fat_travs_dir(fat, dir_clus, file_trunc_handler, 0, NULL, fat, item->name);
    }
}


static FR_t entry_alloc_handler(dir_item_t *item, buf_t *b, void *new, void *ret) {
    // 一个空的目录项
    if(item->name[0] == FAT_NAME_END || item->name[0] == FAT_NAME_FREE) {
        print_dir_item((dir_item_t *)new);
        *item = *(dir_item_t *)new;
        bwrite(b);
        return FR_OK;
    } else {
        return FR_CONTINUE;
    }
}

static void zero_clus(fat32_t *fat, uint32_t clus) {
    uint32_t start_sec = clus2datsec(fat, clus);
    for(int i = 0; i < SPC(fat); i++) {
        buf_t *b = bread(fat->dev, start_sec + i);
        memset(b->data, 0, BPS(fat));
        bwrite(b);
        brelse(b);
    }
}

static void generate_dot(fat32_t *fat, uint32_t parent_clus, uint32_t curr_clus) {
    dir_item_t item;
    uint32_t sect = clus2datsec(fat, curr_clus);
    item.attr = FAT_ATTR_DIR;
    // Note: .与..中只保存了簇号
    buf_t *b = bread(fat->dev, sect);
    strncpy((char *)item.name, DOT, FAT_SFN_LENGTH);
    item.startl = curr_clus & FAT_CLUS_LOW_MASK;
    item.starth = curr_clus >> 16;
    *(dir_item_t *)(b->data) = item;

    strncpy((char *)item.name, DOTDOT, FAT_SFN_LENGTH);
    item.startl = parent_clus & FAT_CLUS_LOW_MASK;
    item.starth = parent_clus >> 16;
    *(dir_item_t *)(b->data + sizeof(dir_item_t)) = item;
    brelse(b);
}

/* 在指定目录簇下创建一个新的（空的）目录项 */
FR_t fat_alloc_entry(fat32_t *fat, uint32_t dir_clus, const char *cname, uint8_t attr, dir_item_t *item, uint32_t *offset) {
    char name[MAX_FILE_NAME];
    uint32_t clus;
    // dir_slot_t slot;

    strncpy(name, cname, MAX_FILE_NAME);
    // todo:为长文件名建立目录项
    int UNUSED(snflag) = generate_shortname((char *)item->name, name); // 结束符'\0'的溢出并不重要
    // debug("gen %s", (char *)item->name);
    // 预分配一个簇
    if(fat_alloc_cluster(fat, &clus, 1) == FR_ERR)
        panic("fat_alloc_entry: alloc clus fail");

    // 构造目录项
    item->attr = attr;
    item->startl = clus & FAT_CLUS_LOW_MASK;
    item->starth = clus >> 16;
    item->size = 0;
    if(fat_travs_dir(fat, dir_clus, entry_alloc_handler, 1, offset, item, NULL) == FR_ERR)
        panic("fat_alloc_entry: fail");

    if(attr == FAT_ATTR_DIR) { // 如果是目录，创建'.'与'..'
        generate_dot(fat, dir_clus, clus);
    }
    // debug("gen end...");
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
            // dir_slot_t *slot = (dir_slot_t *)item;
            // print_slot_name(slot);
            // debug("slot id: %d eof: %d checksum: %d", FAT_LFN_ID(slot->id), FAT_LFN_END(slot->id), slot->alias_checksum);
        } else { // 短文件
            // print_dir_item(item);
            // debug("mychecksum is: %d", cal_checksum((char *)item->name));
            if(strncmp((char *)param, (char *)item->name, FAT_SFN_LENGTH) == 0) {
                *(dir_item_t *)ret = *item;
                debug("find target %s start cluster %d", item->name, FAT_FETCH_CLUS(item));
                return FR_OK;
            }
        }
    }
    return FR_CONTINUE;
}

/* 在指定目录簇下寻找名为name的目录项 */
FR_t fat_dirlookup(fat32_t *fat, uint32_t dir_clus, const char *cname, dir_item_t *ret, uint32_t *offset) {
    // 生成对应的短文件名
    char short_name[FAT_SFN_LENGTH + 1];
    char name[MAX_FILE_NAME];
    strncpy(name, cname, MAX_FILE_NAME);
    generate_shortname(short_name, name);
    // debug("the shorname is: %s", short_name);

    return fat_travs_dir(fat, dir_clus, lookup_handler, 0, offset,short_name,ret);
}



