/* 
    此模块的设计思想在于，尽可能地减少与上层的耦合，尽可能的独立，为后续VFS加入提供可能性。
    此外，许多接口函数的参数使用的都是基本的数据类型，且尽可能地仅与fat32协议相关，提供了移植的可能性。
*/
#include "fs/fs.h"
#include "fs/blk_device.h"
#include "mm/alloc.h"
#include "mm/vm.h"
#include "common.h"
#include "str.h"

// #define QUIET
#define __MODULE_NAME__ FAT
#include "debug.h"

#define FAT_CLUS_END 0x0FFFFFFF // 簇结束标识
#define FAT_CLUS_FREE 0x0       // 空闲簇
#define __FAT_LFN_ATTR 0xF    // 长文件名标识
#define FAT_IS_LFN(attr) ((attr) == __FAT_LFN_ATTR)

#define FAT_NAME_END 0x0
#define FAT_NAME_FREE 0xe5
#define FAT_NAME_SPEC 0x2e

#define FAT_LFN_END_MASK 0x40
#define FAT_LFN_ID_MASK (0x20 - 1)
#define FAT_LFN_ID(id) ((id) & FAT_LFN_ID_MASK)
#define FAT_LFN_END(id) (((id) & FAT_LFN_END_MASK) > 0)


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


typedef struct travs_meta {
    int item_no;
    int nitem;
    buf_t *buf;
    uint32_t offset;
} travs_meta_t;


/* 遍历目录项处理接口，不能在其中释放buf，最多可附带两个参数 */
typedef FR_t (*entry_handler_t)(dir_item_t *item, travs_meta_t *meta, void *param, void *ret);

static FR_t fat_travs_dir(fat32_t *fat, uint32_t dir_clus, 
                          entry_handler_t handler, 
                          int alloc, uint32_t *offset,
                          void *p1, void *p2);


typedef struct lookup_param {
    char *longname; // 长文件名
    char *top; // 栈顶
    char *p; // 名称栈指针（满栈）
    uint8_t checksum; // 当前校验和
    int next; // 下一个序号

    void *state; // 其他状态信息
} lp_t;

typedef struct alloc_param {
    char *longname;
    int length;
    dir_item_t *item;
    uint8_t checksum;
    int entry_num;
} ap_t;


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

    printf("start sector   is %d\n", fat->fat_start_sector);
    printf("fat sectors    is %d\n", fat->fat_tbl_sectors);
    printf("sec per clus   is %d\n", fat->sec_per_cluster);
    printf("bytes per clus is %d\n", fat->bytes_per_sec);
    printf("fat table num  is %d\n", fat->fat_tbl_num);
    printf("root cluster   is %d\n", fat->root_cluster);
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
    // memset(buffer->data, 0, 256);
    // bwrite(buffer);
    brelse(buffer);
    // for(;;);
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
    // debug("read cluster %d", cclus);
    if(cclus == 0 || n == 0)
        return 0;
    // debug("now off is %d", off)
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

FR_t fat_update(fat32_t *fat, uint32_t dir_clus, int offset, dir_item_t *item) {
    if(dir_clus == 0) {
        debug("dir clus 0?");
        return FR_ERR;
    } 
    debug("offset is %d\n", offset)
    fat_write(fat, dir_clus, 0, (uint64_t)item, offset, sizeof(dir_item_t));
    return FR_OK;
}

int fat_write(fat32_t *fat, uint32_t cclus, int user, uint64_t buffer, int off, int n) {
    if(cclus == 0 || n == 0) ///todo:空文件怎么办？
        return 0;
    debug("cclus is %d", cclus);
    int alloc_num = 1; // 防止频繁alloc
    // 计算起始簇
    while(off >= BPC(fat)) {
        cclus = fat_next_cluster(fat, cclus);
        off -= BPC(fat);
        if(cclus == FAT_CLUS_END) {
            debug("alloc ?")
            uint32_t prev = cclus;
            if(fat_alloc_cluster(fat, &cclus, alloc_num) == FR_ERR) {
                debug("fat_write: alloc cluster fail");
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
        // if(sect == clus2datsec(fat, 72)) panic("alloc data clus?");
        uint32_t *entry = (uint32_t *)b->data;
        for(int j = 0; j < entry_per_sect; j++) { // 遍历FAT项
            if(i == 0 && j < 2) continue; // 跳过第0,1号簇
            if(*(entry + j) == FAT_CLUS_FREE) { // 找到标记之
                uint32_t clus_num = i * entry_per_sect + j;
                if(next == 0) { // 如果是最后一个簇，则标记为END
                    debug("mark clus %d as end", clus_num);
                    *(entry + j) = FAT_CLUS_END;
                } else { // 如果是前面的簇，则标记为下一个簇的簇号
                    *(entry + j) = next;
                    debug("mark clus %d as head", clus_num);
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
        // debug("cur bno is %d clus is %d", clus2fatsec(fat, cclus), cclus);
        pclus = (uint32_t *)(b->data + clus2offset(fat, cclus));
        cclus = *pclus; // 得到下一个簇
        if(cclus == FAT_CLUS_FREE) {
            panic("release free?");
        }
        if(idx == 0 && keepfirst) {
            *pclus = FAT_CLUS_END;
        } else {
            *pclus = FAT_CLUS_FREE;
        }
        bwrite(b);
        brelse(b);
        idx++;
    }

    return FR_OK;
}



/**
 * 寻找短文件名下一序号：|1|2|3|4|5|6|~|?|E|X|T| 
 * @startwith 要比较的前缀
 * @ret 返回的序号，传入的时候务必确保初始化为1
 */
static FR_t get_next_shortname_order(dir_item_t *item, travs_meta_t *meta, void *startwith, void *UNUSED(ret)) {
    int *order = (int *) ret;
    if(item->name[0] == FAT_NAME_END){ // 没找到了就返回
        return FR_OK;
    } else if(item->name[0] == FAT_NAME_FREE) {
        return FR_CONTINUE;
    } else {
        if(strncmp((char *)startwith, (char *)item->name, 7) == 0)
            (*order)++;
        return FR_CONTINUE;
    }
}

/**
 * 假设扩展名是合法的，否则将被截断
 * @param shortname: 生成的短文件名
 * @param name     : 输入原始文件名
 * @return 若文件名是合法短文件名则返回1，否则返回0
 */
int generate_shortname(fat32_t *fat, uint32_t dir_clus, char *shortname, char *name) {
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
        int id = 1;
        strncpy(shortname, name, 6);
        *(shortname + 6) = '~';
        fat_travs_dir(fat, dir_clus, get_next_shortname_order, 0 , NULL, shortname, &id);
        if(id > 5)
            panic("sn: id > 5");
        *(shortname + 7) = '0' + id;
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





/**
 * 用于遍历目录的通用方法 
 */
static FR_t fat_travs_dir(fat32_t *fat, uint32_t dir_clus, 
                          entry_handler_t handler, 
                          int alloc, uint32_t *offset,
                          void *p1, void *p2) 
{
    const int item_per_sec = fat->bytes_per_sec/sizeof(dir_item_t);
    uint32_t curr_clus = dir_clus;
    uint32_t clus_cnt = 0; // 簇计数器
    while(1) {
        // 簇起始扇区
        uint32_t sec = clus2datsec(fat, curr_clus);
        for(int i = 0; i < fat->sec_per_cluster; i++) {
            buf_t *b = bread(fat->dev, sec + i);
            for(int j = 0; j < item_per_sec; j++) {
                FR_t res;
                uint32_t ofst = (clus_cnt * SPC(fat) + i) * BPS(fat) + j * sizeof(dir_item_t);
                dir_item_t *item = (dir_item_t *)(b->data) + j;
                travs_meta_t meta = {.buf = b, .item_no = j, .nitem = item_per_sec,.offset = ofst};
                if((res = handler(item, &meta, p1, p2)) != FR_CONTINUE) {
                    brelse(b);
                    if(offset) // 计算当前目录项在当前目录数据簇中的偏移量
                        *offset = ofst;
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



FR_t fat_trunc(fat32_t *fat, uint32_t dir_clus, int offset, dir_item_t *item) { 
    item->size = 0;
    // 将文件大小设置为0
    fat_update(fat, dir_clus, offset, item);
    debug("file size zeroed");
    // 回收簇
    fat_destory_clus_chain(fat, FAT_FETCH_CLUS(item), 0);
    debug("cluster recycled");
    return FR_OK;
}

static FR_t unlink_handler(dir_item_t *item, travs_meta_t *meta, void *checkson, void *ofs) {
    uint8_t checksum = *(uint8_t *)checkson;
    int offset = *(int *)ofs;

    if(meta->offset > offset) return FR_ERR;

    if(item->name[0] == FAT_NAME_END) {
        return FR_OK; // 找不到了
    } else if(item->name[0] == FAT_NAME_FREE) {
        // JUST a free entry
    } else {
        if(FAT_IS_LFN(item->attr)) { // 长目录项
            if(((dir_slot_t *)item)->alias_checksum == checksum) {
                item->name[0] = FAT_NAME_FREE;
                bwrite(meta->buf);
            }
        } else { // 短目录项
            if(meta->offset == offset) {
                item->name[0] = FAT_NAME_FREE;
                bwrite(meta->buf);
                return FR_OK;
            }
        }
        
    }
    return FR_CONTINUE;
}

FR_t fat_unlink(fat32_t *fat, uint32_t dir_clus, int offset, dir_item_t *item) {
    uint32_t checksum = cal_checksum((char *)item->name);
    debug_if(FAT_IS_DIR(item->attr), "warning! You are trying del a dir");
    // 截断文件
    fat_trunc(fat, dir_clus, offset, item);
    // 删除目录项
    return fat_travs_dir(fat, dir_clus, unlink_handler, 0, NULL, &checksum, &offset);
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

/* 目前仅仅支持英文 */
// static void unicode2char(char *buf, uint16_t *wchar, int len) {
//     for(int i = 0; i < len; i++)
//         buf[i] = (char)wchar[i];
// }

static int extractname(dir_slot_t *slot, char *buf) {
    uint8_t *n = slot->name0_4;
    int p = 0;
    int nul = 0;
    while(!nul && p < 5) {
        buf[p] = n[2*p];
        if(buf[p] == '\0') nul = 1;
        p++;
    }
    n = slot->name5_10;
    while(!nul && p < 11) {
        buf[p] = n[2*(p - 5)];
        if(buf[p] == '\0') nul = 1;
        p++;
    }
    n = slot->name11_12;
    while(!nul && p < 13) {
        buf[p] = n[2*(p - 11)];
        if(buf[p] == '\0') nul = 1;
        p++;
    }
    // buf[p] = '\0';
    return p;
}

static void fillname(dir_slot_t *slot, char *buf, int len) {
    static char fill[] = {0xff, 0xff, 0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    uint8_t *n;
    int p = 0;
    uint8_t fillch = 0;
    refill:
    // 存在非对齐访问问题
    while(p < len && p < 5) {
        n = slot->name0_4;
        n[p*2] = buf[p];
        n[p*2+1] = fillch;
        p++;
    }
    while(p < len && p < 11) {
        n = slot->name5_10;
        n[(p - 5)*2] = buf[p];
        n[(p - 5)*2+1] = fillch;
        p++;
    }
    while(p < len && p < 13) {
        n = slot->name11_12;
        n[(p - 11)*2] = buf[p];
        n[(p - 11)*2+1] = fillch;
        p++;
    }
    if(len < 13) {
        len = 13;
        buf = fill;
        fillch = 0xff;
        goto refill;
    }
}


// static void char2unicode(uint16_t *buf, uint8_t *ch, int len, int rlen) {
//     int i;
//     for(i = 0; i < len; i++)
//         buf[i] = ch[i];
//     while(i < rlen) {
//         buf[i] = 0xffff;
//         i++;
//     }
// }


static FR_t entry_alloc_handler(dir_item_t *item, travs_meta_t *meta, void *p1, void *p2) {
    ap_t *ap = (ap_t *)p1;
    int *cnt = (int *)p2;
    // 我们在设计上强行要求长文件名目录项必须创建一个扇区上
    // 主要原因是怕出现下面情况(假设我们需要的entry数目为5)
    // F: free  E: end  U: used
    // |  sect0  |   sect1  |
    // |UUUUUUFFF|FFFFEEEEEE|
    // 这样可能会出现跨扇区写入的问题

    // 当前扇区剩下的item(假设都是free的)小于剩余所需的item数
    if(meta->nitem - meta->item_no < ap->entry_num - *cnt) {
        debug("No space, skipped entry");
        debug("item_no: %d all: %d", meta->item_no, meta->nitem);
        debug("entry num: %d, found slot(exclude curr): %d", ap->entry_num, *cnt);
        debug("need: %d actual: %d", ap->entry_num - *cnt, meta->nitem - meta->item_no);
        debug("");
        *cnt = 0;
        if(item->name[0] == FAT_NAME_END) {
            item->name[0] = FAT_NAME_FREE;
            bwrite(meta->buf);
        }
        
        return FR_CONTINUE;
    }

    // 一个空的目录项
    if(item->name[0] == FAT_NAME_END || item->name[0] == FAT_NAME_FREE) {
        (*cnt)++;
        if(*cnt == ap->entry_num) { // 满足我们的写入需求了
            int end = meta->item_no - ap->entry_num + 1;
            dir_slot_t *slot_end = (dir_slot_t *)item - ap->entry_num + 1;
            if(end < 0) panic("entry_alloc: item no");
            assert((uint64)slot_end >= (uint64)meta->buf->data);
            // 复制短目录项
            debug("copy short item");
            *item = *ap->item; 
            // 复制长目录项
            char *p = (char *)ap->longname;
            int id = 1;
            int len = ap->length;
            for(dir_slot_t *slot = (dir_slot_t *)item - 1; slot >= slot_end; slot--) {
                debug("copy long item id: %d rest text: %s", id, p);
                slot->alias_checksum = ap->checksum;
                slot->id = id++;
                slot->attr = 0xf;
                if(slot == slot_end) { // 最后一个
                    slot->id |= FAT_LFN_END_MASK;
                    fillname(slot, p, len);
                } else {
                    fillname(slot, p, 13);
                    p+=13;
                    len-=13;
                }
            }
            if(meta->offset == 0)
                panic("something happened");
            bwrite(meta->buf);
            debug("offset is %d", meta->offset);
            return FR_OK;
        } else {
            return FR_CONTINUE;
        }
    } else {
        *cnt = 0;
        return FR_CONTINUE;
    }
}

// todo: 不是本函数的TODO！主要是上层函数调用时，可能还要增加父entry的文件大小？
/* 在指定目录簇下创建一个新的（空的）目录项，无论长短文件名，都将创建长目录项*/
FR_t fat_alloc_entry(fat32_t *fat, uint32_t dir_clus, const char *cname, uint8_t attr, dir_item_t *item, uint32_t *offset) {
    char name[MAX_FILE_NAME];
    uint32_t clus;
    int len = strlen(cname);
    int entry_num = (len + 1 + FAT_LFN_LENGTH) / FAT_LFN_LENGTH + 1; // 包括'\0'
    strncpy(name, cname, MAX_FILE_NAME);
    int UNUSED(snflag) = generate_shortname(fat, dir_clus, (char *)item->name, name); // 结束符'\0'的溢出并不重要
    debug("short name gened: %s", item->name);
    // debug("gen %s", (char *)item->name);
    // 预分配一个簇
    if(fat_alloc_cluster(fat, &clus, 1) == FR_ERR)
        panic("fat_alloc_entry: alloc clus fail");

    debug("cluster allocated clus id is %d", clus);
    // 构造目录项
    item->attr = attr;
    item->startl = clus & FAT_CLUS_LOW_MASK;
    item->starth = clus >> 16;
    item->size = 0;

    int cnt = 0;
    ap_t ap = {.checksum = cal_checksum((char *)item->name), .entry_num = entry_num, .item = item, .longname = (char *)cname, .length = len + 1};

    if(fat_travs_dir(fat, dir_clus, entry_alloc_handler, 1, offset, &ap, &cnt) == FR_ERR)
        panic("fat_alloc_entry: fail");
    debug("alloc success");
    if(attr == FAT_ATTR_DIR) { // 如果是目录，创建'.'与'..'
        generate_dot(fat, dir_clus, clus);
    }
    // debug("gen end...");
    return FR_OK;
}



static int match_long_name(lp_t *lp, dir_slot_t *slot) {
    char buf[14];
    char *p = lp->p;

    int pos = extractname(slot, buf) - 1;

    while(p >= lp->longname && pos >= 0) {
        if(buf[pos] != *p) {
            return 0;
        }
        pos--;
        p--; 
    }
    // 全部匹配完了
    if(pos == -1) {
        lp->p = p;
        return 1;
    }

    return 0;
}
    

static FR_t lookup_handler(dir_item_t *item, travs_meta_t *meta, void *param, void *ret) {
    lp_t *lp = (lp_t *)param;
    if(item->name[0] == FAT_NAME_END) {
        return FR_ERR; // 找不到了
    } else if(item->name[0] == FAT_NAME_FREE) {
        // JUST a free entry
    } else {
        if(FAT_IS_LFN(item->attr)) { // 长文件名标识
            dir_slot_t *slot = (dir_slot_t *)item;
            if(FAT_LFN_END(slot->id)) { // 找到最后一段了
                if(match_long_name(lp, slot)) { // 名称匹配
                    lp->next = FAT_LFN_ID(slot->id) - 1;
                    lp->checksum = slot->alias_checksum;
                }
            } else { // 不是最后一段
                if(lp->next != FAT_LFN_ID(slot->id) || lp->checksum != slot->alias_checksum) { // 不匹配
                    goto orphan;
                }

                if(match_long_name(lp, slot)) { // 名称匹配
                    lp->next--;
                    return FR_CONTINUE;
                }
                orphan:
                lp->p = lp->top;
                lp->checksum = 0;
                lp->next = 0;
                return FR_CONTINUE;
            }
           
            // print_slot_name(slot);
            // debug("slot id: %d eof: %d checksum: %d", FAT_LFN_ID(slot->id), FAT_LFN_END(slot->id), slot->alias_checksum);
        } else { // 短文件
            // print_dir_item(item);
            // debug("lpcheckis: %d, mychecksum is: %d", lp->checksum,cal_checksum((char *)item->name));
            if(lp->next != 0) { // 短文件名没有紧随？文件系统错误了
                debug("error: long entry not continous current next is %d", lp->next);
            }

            if(lp->checksum == 0) { // 之前没有碰到长文件目录项
                if(strncmp(lp->longname, (char *)item->name, FAT_SFN_LENGTH) == 0) {
                    goto found;
                }
            }

            if(lp->p == lp->longname - 1) { // 长文件名匹配完毕
                if(lp->checksum == cal_checksum((char *)item->name)) { // 确认校验和
                    goto found;
                }
            }

            lp->p = lp->top;
            lp->checksum = 0;
            lp->next = 0;
            return FR_CONTINUE;

            found:
            *(dir_item_t *)ret = *item;
            debug("find target %s start cluster %d", item->name, FAT_FETCH_CLUS(item));
            return FR_OK;
        }
    }
    return FR_CONTINUE;
}

/* 在指定目录簇下寻找名为name的目录项 */
FR_t fat_dirlookup(fat32_t *fat, uint32_t dir_clus, const char *cname, dir_item_t *ret, uint32_t *offset) {
    char *name = (char *)cname;
    int len = strlen(name);
    lp_t lp = {.longname = name, .p = name + len, .top = name + len, .next = 0, .checksum = 0};

    return fat_travs_dir(fat, dir_clus, lookup_handler, 0, offset, &lp, ret);
}

/* 用于目录遍历（加了一层封装） */
static FR_t travs_handler(dir_item_t *item, travs_meta_t *meta, void *p1, void *p2) {
    travs_handler_t *handler = (travs_handler_t *)p1;
    lp_t *lp = (lp_t *)p2;
    // 结束遍历 
    if(item->name[0] == FAT_NAME_END) {
        return FR_OK;
    }
    if(item->name[0] == FAT_NAME_FREE) {
        return FR_CONTINUE;
    }
    if(FAT_IS_LFN(item->attr)) { // 长文件名标识
        dir_slot_t *slot = (dir_slot_t *)item;
        int id = FAT_LFN_ID(slot->id);
        if(FAT_LFN_END(slot->id)) { // 找到最后一段了
            lp->next = id - 1;
            lp->checksum = slot->alias_checksum;
            char *buf = lp->longname + lp->next * FAT_LFN_LENGTH;
            lp->p = buf + extractname(slot, buf); //  使p指向文件名末尾
        } else { // 不是最后一段
            if(lp->next != FAT_LFN_ID(slot->id) || lp->checksum != slot->alias_checksum) { // 不匹配
                lp->p = lp->longname;
                lp->next = 0;
                lp->checksum = 0;
                return FR_CONTINUE;
            }
            lp->next--;
            extractname(slot, lp->longname + lp->next * FAT_LFN_LENGTH);
        }
        
        return FR_CONTINUE;
    } else { // 短文件
        if(lp->next != 0) { // 短文件名没有紧随？文件系统错误了
            debug("error: long entry not continous current next is %d", lp->next);
        }

        if(lp->checksum == 0) { // 之前没有碰到长文件目录项
            strncpy(lp->longname, (char *)item->name, FAT_SFN_LENGTH);
            lp->longname[FAT_SFN_LENGTH] = '\0';
        } else {
            assert(lp->p);
            *(lp->p) = '\0';
        }

        lp->p = lp->longname;
        lp->checksum = 0;
        lp->next = 0;
        
        return (*handler)(item, lp->longname, meta->offset, lp->state);
    }

}

/* 这里的state是用来保存handler的状态的 */
FR_t fat_traverse_dir(fat32_t *fat, uint32_t dir_clus, travs_handler_t handler, void *state) {
    char buf[MAX_FILE_NAME];
    // 复用一下lp
    lp_t lp = {.checksum = 0, .longname = buf, .p = buf, .state = state};
    return fat_travs_dir(fat, dir_clus, travs_handler, 0, NULL, &handler, &lp);
}


