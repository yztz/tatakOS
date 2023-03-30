/**
 * @file fat32.c
 * @author YangZongzhen
 * @brief FAT32 Implementation.
 * 
 *        The main body is the directory traversal, we use functional programming method.
 *        The whole traversal is divided into three layers:
 * 
 *                  1. abstract function
 *                            ↓
 *                  2. logical traversal
 *                            ↓
 *                  3. physical traversal
 *        
 *        Abstract function is about `dirlookup`, `readdent`...which are base on logical directory traversal.
 * 
 *        Logical traversal implements the logical directory entry traversal, and ignores the details 
 *        about fat32 entry like LFN, SFN...
 * 
 *        Physical traversal is the most basic traversal, it traverses at the granularity of FAT32 directory entries,
 *        that means it will show the details about SFN, LFN, FAT32 entry flags...
 * 
 *        To do that, I set some handler to carry on the function of the lower layer, such as `entry_handler_t`,
 *        `travs_handler_t` which will help us improve the abstract level.
 * 
 * 
 * @version 0.1
 * @date 2023-03-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/* 
    此模块的设计原则，尽可能地减少与上层的耦合，尽可能的独立，为后续VFS加入提供便捷性。
*/
#include "fs/fs.h"
#include "fs/blk_device.h"
#include "fs/fat_helper.h"
#include "mm/alloc.h"
#include "mm/vm.h"
#include "common.h"
#include "str.h"
#include "bio.h"

#include "fs/mpage.h"
#include "utils.h"

#define QUIET
#define __MODULE_NAME__ FAT
#include "debug.h"

#define IS_FAT_CLUS_END(clus) ((clus) >= 0x0FFFFFF8) // 簇结束标识
#define FAT_CLUS_END 0x0FFFFFFF // 簇结束标识
#define FAT_CLUS_FREE 0x0       // 空闲簇

#define FAT_ATTR_LONG_NAME  (FAT_ATTR_READ_ONLY |   \
                             FAT_ATTR_HIDDEN |      \
                             FAT_ATTR_SYSTEM |      \
                             FAT_ATTR_VOLUME_ID)

#define FAT_ATTR_LONG_NAME_MASK (FAT_ATTR_READ_ONLY |   \
                             FAT_ATTR_HIDDEN |          \
                             FAT_ATTR_SYSTEM |          \
                             FAT_ATTR_VOLUME_ID |       \
                             FAT_ATTR_DIRECTORY |       \
                             FAT_ATTR_ARCHIVE)

#define FAT_IS_LFN(attr) (((attr) & FAT_ATTR_LONG_NAME_MASK) == FAT_ATTR_LONG_NAME)

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
// 下一个空闲簇所在的fat扇区
#define nextclussec(fat) clus2fatsec(fat, (fat)->fsinfo.next_free_cluster)

/// @brief metadata for a round of traversal
typedef struct travs_meta {
    /// @brief the number of the directory entry in the current sector
    int item_no;
    /// @brief total number of directory entries in a sector
    int nitem;
    /// @brief the block buffer of sector
    blk_buf_t *buf;
    /// @brief the offset of the current directory entry within the directory
    uint32_t offset;
} travs_meta_t;


/**
 * @brief The handler to handle fat entry.
 *        There are 4 params given:
 *          1. item  - raw dir entry item
 *          2. meta  - metadata
 *          3. param - custom param
 *        Returning FR_OK/FR_ERR means end, or FR_CONTINUE to continue.
 */
typedef FR_t (*entry_handler_t)(dir_item_t *item, travs_meta_t *meta, void *param);

/**
 * @brief The utility to traverse the directory
 * 
 * @param fat fat obj
 * @param dir_clus the begining cluster of directory
 * @param dir_offset the offset of the directory entry to start traversing
 * @param handler the directory entry handler
 * @param alloc whether to alloc for a new cluster when traversing to the end
 * @param offset [OUT] the offset of the last traversed directory entry
 * @param param param to be passed to handler
 * @return FR_t 
 */
static FR_t fat_travs_phy_dir(fat32_t *fat, uint32_t dir_clus, 
                          uint32_t dir_offset,
                          int alloc, uint32_t *offset,
                          entry_handler_t handler, 
                          void *param);

static inline uint8_t cal_checksum(char* shortname) {
    if (shortname == NULL)
        return -1;
    uint8_t sum = 0;
    for (int i = FAT_SFN_LENGTH; i != 0; i--) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *shortname++;
    }
    return sum;
}

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
    debug("bytes per sec  is %d", fat->bytes_per_sec);
    debug("bytes per cluster is %d", fat->bytes_per_cluster);
    debug("fat table num  is %d", fat->fat_tbl_num);
    debug("root cluster   is %d", fat->root_cluster);

    fat->fsinfo.next_free_cluster = -1;
    assert(fat->bytes_per_sec == 512);
}

static entry_t *get_root(fat32_t *fat) {
    entry_t *root = (entry_t *)kzalloc(sizeof(entry_t));
    if(!root) {
        panic("get_root: alloc fail");
    }
    root->ref = 1;
    root->fat = fat;
    root->nlink = 1;
    root->parent = NULL;
    root->raw.attr = FAT_ATTR_DIRECTORY;
    root->name[0] = '/';
    root->clus_start = fat->root_cluster;

    address_space_t *tmp = kzalloc(sizeof(address_space_t));
    root->i_mapping = tmp;
    
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
    fat->lock = INIT_SPINLOCK(fat_lock);
    blk_buf_t *buffer = bread(dev, 0);
    // 解析fatDBR
    fat_parse_hdr(fat, (struct fat_boot_sector*)buffer->data);
    
    brelse(buffer);
    fat->root = get_root(fat);

    *ppfat = fat;

    /* Don't forget to initialize the list */
    INIT_LIST_HEAD(&fat->fat_dirty);
    return FR_OK;
}

/**
 * @brief follow the next cluster
 * 
 * @param fat 
 * @param cclus curent cluster
 * @return uint32_t next cluster
 */
static uint32_t fat_next_cluster(fat32_t *fat, uint32_t cclus) {
    if(IS_FAT_CLUS_END(cclus) || cclus == FAT_CLUS_FREE) return cclus;

    blk_buf_t *buf = bread(fat->dev, clus2fatsec(fat, cclus));
    uint32_t offset = clus2offset(fat, cclus);
    uint32_t next = *(uint32_t *)(buf->data + offset);
    brelse(buf);

    return next;
}

/**
 * @brief join two clusters
 * 
 * @param fat 
 * @param prev 
 * @param new 
 * @return FR_t 
 */
static FR_t fat_append_cluster(fat32_t *fat, uint32_t prev, uint32_t new) {
    if(new == FAT_CLUS_FREE) {
        panic("fat_append_cluster: new is free");
    }

    blk_buf_t *buf = bread(fat->dev, clus2fatsec(fat, prev));
    uint32_t offset = clus2offset(fat, prev);
    uint32_t next = *(uint32_t *)(buf->data + offset);
    if(!IS_FAT_CLUS_END(next)) {
        panic("fat_append_cluster:prev is not the end");
    }
    *(uint32_t *)(buf->data + offset) = new;
    bwrite(buf);
    brelse(buf);

    return FR_OK;
}

int fat_read(fat32_t *fat, uint32_t cclus, int user, uint64_t buffer, off_t off, int n) {
    if(cclus == 0 || n == 0)
        return 0;

    // 计算起始簇
    int rest = n;
    while(!IS_FAT_CLUS_END(cclus) && rest > 0) {
        if(off >= BPC(fat)) {
            off -= BPC(fat);
            goto next_clus;
        }
        // 计算簇内起始扇区号
        int nth_sect = off / BPS(fat);
        uint32_t sect = clus2datsec(fat, cclus);
        // 扇区内偏移
        off %= BPS(fat);
        for(int i = nth_sect; i < SPC(fat) && rest > 0; i++) {
            // 计算本扇区内需要写入的字节数（取剩余读取字节数与扇区内剩余字节数的较小值）
            int len = min((off_t)rest, BPS(fat) - off);
            blk_buf_t *b = bread(fat->dev, sect + i);
            either_copyout(user, buffer, b->data + off, len);
            brelse(b);
            rest -= len;
            buffer += len;

            off = 0; // 偏移量已经被抵消了，恒置为0
        }
        if(rest == 0) break;
      next_clus:
        cclus = fat_next_cluster(fat, cclus);
    }

    return n - rest;
}

FR_t fat_update(fat32_t *fat, uint32_t dir_clus, uint32_t dir_offset, dir_item_t *item) {
    if(dir_clus == 0) {
        debug("dir clus 0?");
        return FR_ERR;
    } 
    // debug("offset is %d\n", offset)
    fat_write(fat, dir_clus, 0, (uint64_t)item, dir_offset, sizeof(dir_item_t));
    return FR_OK;
}

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


int fat_write(fat32_t *fat, uint32_t cclus, int user, uint64_t buffer, off_t off, int n) {
    if(cclus == 0 || n == 0) // todo:空文件怎么办？
        return 0;
    debug("cclus is %d", cclus);
    uint32_t prev_clus;
    int alloc_num = 1; // 防止频繁alloc
    int rest = n;

    while(rest > 0) {
        if(off >= BPC(fat)) {
            off -= BPC(fat);
            goto next_clus;
        }
        // 计算簇内起始扇区号
        int nth_sect = off / BPS(fat);
        uint32_t sect = clus2datsec(fat, cclus);
        // 扇区内偏移
        off %= BPS(fat);
        for(int i = nth_sect; i < SPC(fat) && rest > 0; i++) {
            // 计算本扇区内需要写入的字节数（取剩余读取字节数与扇区内剩余字节数的较小值）
            int len = min((off_t)rest, BPS(fat) - off);
            blk_buf_t *b = bread(fat->dev, sect + i);
            if(either_copyin(b->data + off, user, buffer, len) < 0) {
                brelse(b);
                return n - rest;
            }
            bwrite(b);
            brelse(b);
            rest -= len;
            buffer += len;

            off = 0; // 偏移量以及被抵消了，恒置为0
        } 
        if(rest == 0) break;
    next_clus:
        prev_clus = cclus;
        cclus = fat_next_cluster(fat, cclus);
        if(IS_FAT_CLUS_END(cclus)) {
            if(fat_alloc_cluster(fat, &cclus, alloc_num) == FR_ERR) {
                debug("fat_write: alloc fail");
                return 0;
            }
            fat_append_cluster(fat, prev_clus, cclus);
            alloc_num++;
        }
    }

    return n - rest;
}

/**
 * 反向分配簇
 * 注意：反向分配不利于发挥页面缓存的性能以及多块连读的策略 
 * 但是在算法实现上会比较简洁明了且高效
 */
FR_t __fat_alloc_cluster_reversed_order(fat32_t *fat, uint32_t *news, int n) {
    const int entry_per_sect = BPS(fat) / 4;
    uint32_t sect = fat->fat_start_sector;
    int cnt = n - 1;
    uint32_t next = 0;

    for(int i = 0; i < fat->fat_tbl_sectors; i++, sect++) {
        int changed = 0; // 用于标记当前的块是否被写
        blk_buf_t *b = bread(fat->dev, sect);
        uint32_t *entry = (uint32_t *)b->data;
        for(int j = 0; j < entry_per_sect; j++) { // 遍历FAT项
            if(i == 0 && j < 2) continue; // 跳过第0,1号簇
            if(*(entry + j) == FAT_CLUS_FREE) { // 找到标记之
                uint32_t clus_num = i * entry_per_sect + j;

                if(next == 0) { // 如果是最后一个簇，则标记为END
                    debug("mark clus %d[sect: %d] as end", clus_num, clus2datsec(fat, clus_num));
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

static inline void update_next_freeclus(fat32_t *fat, uint32_t freeclus) {
    acquire(&fat->lock);
    fat->fsinfo.next_free_cluster = freeclus;
    release(&fat->lock); 
}

/**
 * 正向分配簇
 * 注意：正向分配有利于发挥页面缓存的性能以及适应多块连读的策略 
 * 但是在算法实现上会比较繁琐（有时候需要同时持有两个块的锁）
 */
FR_t __fat_alloc_cluster_order(fat32_t *fat, uint32_t *news, int n) {
    assert(n > 0);
    const int entry_per_sect = BPS(fat) / 4;
    // 将下个空闲簇号作为参考值来减少遍历时间
    uint32_t sect = fat->fsinfo.next_free_cluster == -1 ? fat->fat_start_sector : nextclussec(fat);
    uint32_t i = sect - fat->fat_start_sector;

    int cnt = n;

    blk_buf_t *pb = NULL;    // previous block
    uint32_t *pe = NULL; // previous entry

    // debug("i %d sect %d next freeclus %d", i, sect, fat->fsinfo.next_free_cluster);

    for(; i < fat->fat_tbl_sectors; i++, sect++) {
        blk_buf_t *b = bread(fat->dev, sect);
        uint32_t *entry = (uint32_t *)b->data;
        for(int j = 0; j < entry_per_sect; j++) {   // 遍历FAT项
            if(i == 0 && j < 2) continue;           // 跳过第0,1号簇
            if(*(entry + j) == FAT_CLUS_FREE) {     // 找到标记之
                uint32_t clus_no = i * entry_per_sect + j;
                if(pe) {     // 非起始分配簇
                    *pe = clus_no;
                    if(pb) { // 如果存在pb，那么pe必定指向上个pb的内容，所以接下来写回释放即可
                        bwrite(pb);
                        brelse(pb);
                        pb = NULL;
                    }
                } else {
                    *news = clus_no;
                }

                if(--cnt == 0) { // 已经是所需的最后一块了
                    debug("mark clus %d[sect: %d] as end", clus_no, clus2datsec(fat, clus_no));
                    *(entry + j) = FAT_CLUS_END;
                    bwrite(b);
                    brelse(b);
                    update_next_freeclus(fat, clus_no + 1);
                    return FR_OK;
                }
                // 记录当前的表项指针
                // 当找到下一空闲块时需要修改当前块，使其与下一块空闲块链接
                pe = entry + j;
            } 
        }
        if(pb || !pe) { 
            // 如果pb不为空，说明pe仍然指向pb的内存区域，所以可以直接释放当前块
            // 如果pe为空，说明还不存在对上一块的引用，那自然可以释放当前块
            brelse(b);
        } else {
            pb = b;
        }
    }

    return FR_ERR;
}

/* 释放cclus簇以及接下来的簇 置位keepfirst将会保留首节点置为END*/
FR_t fat_destory_clus_chain(fat32_t *fat, uint32_t clus, int keepfirst) {
    if(clus == FAT_CLUS_FREE) return FR_ERR;

    uint32_t cclus = clus;
    uint32_t idx = 0;

    while(!IS_FAT_CLUS_END(cclus)) {
        uint32_t *pclus;

        blk_buf_t *b = bread(fat->dev, clus2fatsec(fat, cclus));
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
 * @brief 寻找短文件名下一序号：|1|2|3|4|5|6|~|?|E|X|T| 
 * 
 * @param item 
 * @param meta 
 * @param startwith 要比较的前缀
 * @param ret 返回的序号，传入的时候务必确保初始化为1
 * @return FR_t 
 */
static FR_t get_next_shortname_order(dir_item_t *item, travs_meta_t *meta, void *__short_name) {
    char *order = (char *) __short_name + 7;
    
    if(item->name[0] == FAT_NAME_END){ // 没找到了就返回
        return FR_OK;
    } else if(item->name[0] == FAT_NAME_FREE) {
        return FR_CONTINUE;
    } else {
        if(strncmp((char *)__short_name, (char *)item->name, 7) == 0)
            (*order)++;
                   // 目前还没有设计好后续ID生成策略
        if(*order > '5')
            panic("sn: id > 5");
        return FR_CONTINUE;
    }
}

/**
 * @brief generate shortname for the given name
 * 
 * @note 假设扩展名是合法的，否则将被截断
 * @param shortname 生成的短文件名
 * @param name  输入原始文件名
 * @return 若文件名是合法短文件名则返回1，否则返回0
 */
int generate_shortname(fat32_t *fat, uint32_t dir_clus, char *dst, const char *src) {
    const char *ext = NULL;
    int ret = 1;
    const int slen = strlen(src);

    char buf[FAT_SFN_LENGTH];
    
    // find extention name
    for(int i = slen - 1; i >= 0; i--) {
        if(*(src + i) == '.') {
            ext = src + i + 1;
            break;
        }
    }
    // length of name/ext
    const int nlen = ext ? ext - src - 1 : slen;
    const int elen = ext ? strlen(ext) : 0;
    
    // copy name
    for (int i = 0; i < nlen; i++) {
        buf[i] = toupper(src[i]);
    }
    if(nlen > 8) {
        *(buf + 6) = '~';
        *(buf + 7) = '1';
        fat_travs_phy_dir(fat, dir_clus, 0, 0, NULL, get_next_shortname_order, buf);
        ret = 0;
    } else {
        memset(buf + nlen, ' ', 8 - nlen);
    }

    // copy extension
    for (int i = 0; i < min(3, elen); i++) {
        buf[i + 8] = toupper(ext[i]);
    }
    if(elen < 3) {
        memset(buf + 8 + elen, ' ', 3 - elen);
    }

    return ret;
}



static FR_t fat_travs_phy_dir(fat32_t *fat, uint32_t dir_clus, uint32_t dir_offset,
                          int alloc, uint32_t *offset,
                          entry_handler_t handler, 
                          void *param) 
{
    const int item_size = sizeof(dir_item_t);
    const int item_per_sec = fat->bytes_per_sec/item_size;
    uint32_t curr_clus = dir_clus;
    uint32_t prev_clus;
    uint32_t clus_cnt = 0; // 簇计数器
    if(dir_offset % item_size) {
        panic("illegal dir offset");
    }
    while(1) { // 遍历簇
        if(dir_offset > BPC(fat)) {
            dir_offset -= BPC(fat);
            goto next_clus;
        }
        // 簇起始扇区
        int i, j;
        uint32_t sec = clus2datsec(fat, curr_clus);
        for(i = dir_offset / BPS(fat), dir_offset %= BPS(fat);  // 遍历簇内扇区
                i < fat->sec_per_cluster;   i++  ) 
        { 
            blk_buf_t *b = bread(fat->dev, sec + i);
            for(j = dir_offset / item_size, dir_offset = 0;     // 遍历扇区内目录项
                j < item_per_sec;   j++  ) 
            { 
                FR_t res;
                // 生成目录项元信息
                uint32_t ofst = (clus_cnt * SPC(fat) + i) * BPS(fat) + j * item_size;
                dir_item_t *item = (dir_item_t *)(b->data) + j;
                travs_meta_t meta = {.buf = b, .item_no = j, .nitem = item_per_sec,.offset = ofst};
                if((res = handler(item, &meta, param)) != FR_CONTINUE) {
                    brelse(b);
                    if(offset) // 计算当前目录项在当前目录数据簇中的偏移量
                        *offset = ofst;
                    return res; 
                }
            }
            brelse(b);
        }
    next_clus:
        prev_clus = curr_clus;
        curr_clus = fat_next_cluster(fat, curr_clus);
        // 触底了
        if(IS_FAT_CLUS_END(curr_clus)) {
            if(alloc) {
                if(fat_alloc_cluster(fat, &curr_clus, 1) == FR_ERR)
                    return FR_ERR;
                fat_append_cluster(fat, prev_clus, curr_clus);
            } else {
                break;
            }
        }
        debug("next clus %d", curr_clus);
        clus_cnt++;
    }

    return FR_ERR;
}



FR_t fat_trunc(fat32_t *fat, uint32_t dir_clus, uint32_t dir_offset, dir_item_t *item) { 
    item->size = 0;
    // 将文件大小设置为0
    fat_update(fat, dir_clus, dir_offset, item);
    debug("file size zeroed");
    // 回收簇
    fat_destory_clus_chain(fat, FAT_FETCH_CLUS(item), 0);
    debug("cluster recycled");
    return FR_OK;
}

static void generate_dot(fat32_t *fat, uint32_t parent_clus, uint32_t curr_clus) {
    dir_item_t item;
    uint32_t sect = clus2datsec(fat, curr_clus);
    item.attr = FAT_ATTR_DIRECTORY;
    // Note: .与..中只保存了簇号
    blk_buf_t *b = bread(fat->dev, sect);
    strncpy((char *)item.name, FAT_DOT, FAT_SFN_LENGTH);
    item.startl = curr_clus & FAT_CLUS_LOW_MASK;
    item.starth = curr_clus >> 16;
    *(dir_item_t *)(b->data) = item;

    strncpy((char *)item.name, FAT_DOTDOT, FAT_SFN_LENGTH);
    item.startl = parent_clus & FAT_CLUS_LOW_MASK;
    item.starth = parent_clus >> 16;
    *(dir_item_t *)(b->data + sizeof(dir_item_t)) = item;
    brelse(b);
}


static FR_t entry_alloc_handler(dir_item_t *item, travs_meta_t *meta, void *__helper) {
    alloc_helper_t *helper = (alloc_helper_t *)__helper;
    const int need = helper->entry_need;
    int *cnt = &helper->entry_cnt;
    // 我们在设计上强行要求长文件名目录项必须创建一个扇区上
    // 主要原因是怕出现下面情况(假设我们需要的entry数目为5)
    // F: free  E: end  U: used
    // |  sect0  |   sect1  |
    // |UUUUUUFFF|FFFFEEEEEE|
    // 这样可能会出现跨扇区写入的问题

    // 当前扇区剩下的item(假设都是free的)小于剩余所需的item数
    if(meta->nitem - meta->item_no < need - *cnt) {
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
    if (item->name[0] == FAT_NAME_END || item->name[0] == FAT_NAME_FREE) {
        (*cnt)++;
        if(*cnt == need) { // 满足我们的写入需求了
            int end = meta->item_no - need + 1;
            dir_slot_t *slot_end = (dir_slot_t *)item - need + 1;
            if(end < 0) panic("entry_alloc: item no");
            assert((uint64_t)slot_end >= (uint64_t)meta->buf->data);
            // 复制短目录项
            debug("copy short item");
            *item = *helper->item; 
            // 复制长目录项
            int id = 1;
            for(dir_slot_t *slot = (dir_slot_t *)item - 1; slot >= slot_end; slot--, id++) {
                debug("copy long item id: %d rest text: %s", id, p);
                slot->alias_checksum = helper->checksum;
                slot->id = id;
                slot->attr = 0xf;
                if(slot == slot_end) { // 最后一个
                    slot->id |= FAT_LFN_END_MASK;
                } 
                fillname(slot, helper);
            }
            if(meta->offset == 0)
                panic("something happened");
            bwrite(meta->buf);

            return FR_OK;
        } else {
            return FR_CONTINUE;
        }
    } else {
        *cnt = 0;
        return FR_CONTINUE;
    }
}

static FR_t fat_alloc_entry(fat32_t *fat, uint32_t dir_clus, const char *cname, dir_item_t *item, uint32_t *offset) {
    char name[MAX_FILE_NAME];
    
    int len = strlen(cname) + 1;
    int entry_need = (len + FAT_LFN_LENGTH) / FAT_LFN_LENGTH + 1;
    // strncpy(name, cname, MAX_FILE_NAME);
    int unused(snflag) = 
        generate_shortname(fat, dir_clus, (char *)item->name, name);
    debug("short name gened: %s", item->name);
    uint8_t checksum = cal_checksum((char *)item->name);
    
    DEFINE_ALLOC_HELPER(helper, cname, entry_need, item, checksum);

    if(fat_travs_phy_dir(fat, dir_clus, 0, 1, offset, entry_alloc_handler, &helper) == FR_ERR)
        panic("fat_alloc_entry: fail");

    return FR_OK;
}

static FR_t entry_free_handler(dir_item_t *item, travs_meta_t *meta, void *__helper) {
    lookup_helper_t *helper = (lookup_helper_t *)__helper;
    uint32_t offset = *(helper->offset);
    uint32_t checksum = helper->checksum;

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

static FR_t fat_free_entry(fat32_t *fat, uint32_t dir_clus, uint32_t dir_offset, dir_item_t *item) {
    char *name = (char *)item->name;
    uint8_t checksum = cal_checksum(name);
    DEFINE_LOOKUP_HELPER(helper, name, checksum, item, &dir_offset);
    return fat_travs_phy_dir(fat, dir_clus, 0, 0, NULL, entry_free_handler, &helper);
}

FR_t fat_unlink(fat32_t *fat, uint32_t dir_clus, uint32_t dir_offset, dir_item_t *item) {
    debug_if(FAT_IS_DIR(item->attr), "warning! You are trying del a dir");
    // 截断文件
    fat_trunc(fat, dir_clus, dir_offset, item);
    // 删除目录项
    return fat_free_entry(fat, dir_clus, dir_offset, item);
}

FR_t fat_create_entry(fat32_t *fat, uint32_t dir_clus, const char *cname, uint8_t attr, dir_item_t *item, uint32_t *offset) {
    uint32_t clus;
    // 预分配一个簇
    if(fat_alloc_cluster(fat, &clus, 1) == FR_ERR)
        panic("fat_alloc_entry: alloc clus fail");
    debug("cluster allocated clus id is %d", clus);

    // 构造目录项
    item->attr = attr;
    item->startl = clus & FAT_CLUS_LOW_MASK;
    item->starth = clus >> 16;
    item->size = 0;

    FR_t ret = fat_alloc_entry(fat, dir_clus, cname, item, offset);

    if(ret == FR_OK && FAT_IS_DIR(attr)) { // 如果是目录，创建'.'与'..'
        generate_dot(fat, dir_clus, clus);
    }

    return ret;
}

FR_t fat_rename(fat32_t *fat, uint32_t dir_clus, uint32_t dir_offset, dir_item_t* item, const char *newname, uint32_t *offset) {
    if(fat_free_entry(fat, dir_clus, dir_offset, item) != FR_OK) panic("free fail");
    return fat_alloc_entry(fat, dir_clus, newname, item, offset);
}


static void copy_shortname(char *dst, dir_item_t *item) {
    uint8_t lcase = item->lcase;
    const char *item_name = (const char *)item->name;
    int body_lcase = lcase & FAT_NAME_BODY_L_CASE;
    int ext_lcase = lcase & FAT_NAME_EXT_L_CASE;
    
    for (int i = 0; i < 8; i++) {
        char c = *item_name++;
        if (c == ' ') continue;
        *dst++ = body_lcase ? tolower(c) : c;
    }
    if (*item_name != ' ') *dst++ = '.';
    for (int i = 0; i < 3; i++) {
        char c = *item_name++;
        if (c == ' ') break;
        *dst++ = ext_lcase ? tolower(c) : c;
    }
    *dst = '\0';
}

static FR_t travs_handler(dir_item_t *item, travs_meta_t *meta, void *__helper) {
    travs_helper_t *helper = (travs_helper_t *)__helper;
    // 结束遍历 
    if (item->name[0] == FAT_NAME_END) {
        return FR_ERR;
    }
    if (item->name[0] == FAT_NAME_FREE) {
        return FR_CONTINUE;
    }
    if (FAT_IS_LFN(item->attr)) { // 长文件名标识
        dir_slot_t *slot = (dir_slot_t *)item;
        int id = FAT_LFN_ID(slot->id);
        if(FAT_LFN_END(slot->id)) { // 找到最后一段了
            helper->next = id - 1;
            helper->checksum = slot->alias_checksum;
            char *buf = helper->buf + helper->next * FAT_LFN_LENGTH;
            helper->p = buf + extractname(slot, buf) + 1; //  使p指向文件名末尾
        } else { // 不是最后一段
            if(helper->next != FAT_LFN_ID(slot->id) || helper->checksum != slot->alias_checksum) { // 不匹配
                travs_helper_reset(helper);
                return FR_CONTINUE;
            }
            helper->next--;
            extractname(slot, helper->buf + helper->next * FAT_LFN_LENGTH);
        }
        
        return FR_CONTINUE;
    } else { // 短文件
        if(helper->next != 0) { // 短文件名没有紧随？文件系统错误了
            warn("long entry not continous current next is %d", helper->next);
        }

        if(helper->checksum == 0) { // 之前没有碰到长文件目录项
            copy_shortname(helper->buf, item);
            helper->is_shortname = 1;
        } else {
            assert(helper->p);
            *(helper->p) = '\0';
        }

        travs_helper_reset(helper);
        
        return (helper->callback)(item, helper->buf, meta->offset, helper->param);
    }

}


FR_t fat_travs_logical_dir(fat32_t *fat, uint32_t dir_clus, uint32_t dir_offset, travs_handler_t handler, void *state) {
    DEFINE_TRAVS_HELPER(helper, handler, state);
    return fat_travs_phy_dir(fat, dir_clus, dir_offset, 0, NULL, travs_handler, &helper);
}

static FR_t lookup_handler(dir_item_t *item, const char *name, off_t offset, void *__helper) {
    lookup_helper_t *helper = (lookup_helper_t *)__helper;
    if (strncmp(name, helper->name, MAX_FILE_NAME) == 0) {
        if (helper->item)
            *helper->item = *item;
        if (helper->offset)
            *helper->offset = offset;
        return FR_OK;
    }

    return FR_CONTINUE;
}

/* 在指定目录簇下寻找名为name的目录项 */
FR_t fat_dirlookup(fat32_t *fat, uint32_t dir_clus, const char *cname, dir_item_t *ret, uint32_t *offset) {
    DEFINE_LOOKUP_HELPER(helper, cname, 0, ret, offset);
    return fat_travs_logical_dir(fat, dir_clus, 0, lookup_handler, &helper);
}



bio_vec_t *fat_get_sectors(fat32_t *fat, uint32_t cclus, int off, int n) {
    bio_vec_t *first_bio_vec = NULL, *cur_bio_vec = NULL;
    /* sector counts in a cluster */
    uint32_t spc = SPC(fat);
    /* the total number of sectors to get, need up align*/
    uint32_t sec_total_num = (n + BPS(fat)-1) / BPS(fat);
    uint32_t sec_off_num;
    uint32_t sect;


    if(off & ~PGMASK)
        panic("fat_readpage: offset not page aligned!");
    
    if(cclus == 0 || n == 0)
        return 0;
    // 计算起始簇
    while(off >= BPC(fat)) {
        cclus = fat_next_cluster(fat, cclus);
        off -= BPC(fat);
        /* 如果在把page写回disk之前，没有append足够的cluster，会出现这种情况 */
        if(IS_FAT_CLUS_END(cclus)) {
            ER();
        }
    }
    
    /* the initial offset number of a sector in a cluster */
    sec_off_num = off / BPS(fat);
    // 计算簇内起始扇区号
    sect = clus2datsec(fat, cclus) + sec_off_num % spc;

    while(!IS_FAT_CLUS_END(cclus) && sec_total_num > 0) {
        if(cur_bio_vec == NULL){
            cur_bio_vec = kzalloc(sizeof(bio_vec_t));
            first_bio_vec = cur_bio_vec;
        } else if(cur_bio_vec->bv_start_num + cur_bio_vec->bv_count != sect) {
            /** 
             * 新的簇的第一个扇区号和上一个簇的最后一个扇区号不连续，新建一个结构体存放这个段，这里的链表
             * 操作是进程私有的，似乎不用加锁。
            */
            struct bio_vec *new_bio_vec = kzalloc(sizeof(struct bio_vec));
            cur_bio_vec->bv_next = new_bio_vec;
            cur_bio_vec = new_bio_vec;
        }

        uint32_t sect_nums = min(spc - sec_off_num, sec_total_num);
        /* the bio_vec is new allocated in this loop (we use kzalloc) */
        if(cur_bio_vec->bv_start_num == 0){
            cur_bio_vec->bv_start_num = sect;
            cur_bio_vec->bv_count = min(spc - sec_off_num, sec_total_num);
        }
        else{
            /* the sectors in this cluster is continuous with last cluster */
            cur_bio_vec->bv_count += min(spc - sec_off_num, sec_total_num);
        }

        sec_off_num = (sec_off_num + sect_nums) % spc;
        sec_total_num -= sect_nums;

        cclus = fat_next_cluster(fat, cclus);

        /* 如果簇没了，但是还没有写完，error */
        if(IS_FAT_CLUS_END(cclus) && sec_total_num > 0)
            ER();
        sect = clus2datsec(fat, cclus);
    }

    return first_bio_vec;
}

uint32_t get_clus_end(fat32_t *fat, uint32_t cur_clus){
   uint32_t next_clus;

   while(!IS_FAT_CLUS_END(cur_clus)){
        next_clus = fat_next_cluster(fat, cur_clus);
        if(next_clus == 0)
            ERROR("next_clus = 0, the entry's cluster may be reclaimed.");
        if(IS_FAT_CLUS_END(next_clus))
            return cur_clus;
        cur_clus = next_clus;
   } 
   return 0;
}

uint32_t get_clus_cnt(fat32_t *fat, uint32_t cur_clus){
    uint64_t clus_cnt = 0;

    while(!IS_FAT_CLUS_END(cur_clus)){
        clus_cnt++;
        cur_clus = fat_next_cluster(fat, cur_clus);
    }
    return clus_cnt;
}

FR_t fat_alloc_append_clusters(fat32_t *fat, uint32_t clus_start, uint32_t *clus_end, uint64_t *clus_cnt, uint32_t size_in_mem){
    /**
     * 在eget中将这两个值赋为0，如果这两个值都为0，说明这个entry对应的file自从
     * 读入打开以来，还没有调用过此函数。
     * 
     */
    
    uint32_t bpc = BPC(fat);
    uint64_t size = ROUNDUP(size_in_mem, bpc);
    int alloc_num;
    uint32_t new_clus;

    if(*clus_end == 0 || *clus_cnt == 0){
        *clus_end = get_clus_end(fat, clus_start);
        *clus_cnt = get_clus_cnt(fat, clus_start);
    }

    alloc_num = size/bpc - *clus_cnt;
    
    if(alloc_num == 0)
        return FR_OK;

    // 分配簇
    if(fat_alloc_cluster(fat, &new_clus, alloc_num) == FR_ERR)
        panic("fat enlarge file 2");
    fat_append_cluster(fat, *clus_end, new_clus);

    /* update clus_end and clus_cnt */
    if(alloc_num == 1)
        *clus_end = new_clus; 
    else
        *clus_end = get_clus_end(fat, *clus_end);

    *clus_cnt += alloc_num;

    return FR_OK; 
}
