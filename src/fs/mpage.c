#include "common.h"
#include "platform.h"
#include "mm/alloc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "debug.h"
#include "mm/page.h"
#include "radix-tree.h"
#include "bio.h"
#include "mm/mmap.h"

#include "fs/mpage.h"
/**
 * 这个文件里定义了关于页page的操作函数
 * linux mpage.c
 */

/**
 * @brief 从entry中读取或写回一个page到buff。
 * 得到bio结构体，记录了此次读写操作涉及到的sector信息。
 *
 * linux readpage。
 *
 * @param entry the entry of the file to read
 * @param buff the physical address of the page to store the result
 * @param index the index of page in the file
 * @return int
 */
int rw_one_page(entry_t *entry, uint64_t buff, uint32_t index, int rw) {
    bio_t *bio = get_rw_pages_bio(entry, buff, index, 1, rw);
    if (bio)
        submit_bio(bio);

    return 0;
}

// int read_one_page(entry_t *entry, uint64_t buff, uint32_t index){ 

//   bio_t *bio = get_rw_pages_bio(entry, buff, index, 1, READ);
//   if(bio)
//     submit_bio(bio);

//   return 0;
// }

// /**
//  * 把一个页写回disk。
//  */
// int write_one_page(entry_t *entry, uint64_t buff, uint32_t index){

//   bio_t *bio = get_rw_pages_bio(entry, buff, index, 1, WRITE);
//   if(bio)
//     submit_bio(bio);

//   return 0;
// }
/**
 * @brief give a list of bio_vec and a start address buff, assign to
 * bio_vec buff with the value of "buff", and plus offset for it.
 *
 * @param first_bio_vec
 * @param buff
 */
void bio_vec_buff_assignment(bio_vec_t *first_bio_vec, uint64_t buff, uint32_t bps) {
    bio_vec_t *cur_bio_vec = first_bio_vec;
    while (cur_bio_vec != NULL) {
        cur_bio_vec->bv_buff = (void *)buff;
        buff += cur_bio_vec->bv_count * bps;

        cur_bio_vec = cur_bio_vec->bv_next;
    }
}

// /**
//  * @brief 找出所有要读的磁盘块号，记录在bio中，把一个页(page)按照扇区(sector)连续的原则分为段(segment)
//  * 
//  */
// bio_t *do_readpage(entry_t *entry, uint64_t buff, uint32_t index){
//   bio_t *bio = kzalloc(sizeof(bio_t));
//   struct bio_vec *first_bio_vec;
//   /* 用来统计扇区总数是否符合条件 */
//   // int sect_num = 0;
//   uint32_t bps = entry->fat->bytes_per_sec;

//   first_bio_vec = fat_get_sectors(entry->fat, entry->clus_start, index*PGSIZE, PGSIZE);
//   // cur_bio_vec = first_bio_vec;
//   // while(cur_bio_vec != NULL){
//   //   cur_bio_vec->bv_buff = (void *)buff;
//   //   buff += cur_bio_vec->bv_count * bps;

//   //   cur_bio_vec = cur_bio_vec->bv_next;
//   // }
//   bio_vec_buff_assignment(first_bio_vec, buff, bps);

//   // if(PGSIZE / bps != sect_num)
//   //   panic("do_readpage: sector num is wrong!");
//   bio->bi_io_vec = first_bio_vec; 
//   bio->bi_rw = READ;
//   bio->bi_dev = entry->fat->dev;
//   // print_bio_vec(bio);
//   return bio;
// }


/**
 * @brief Get the rw pages bio object
 *  找出所有要读的磁盘块号，记录在bio中，把一个页(page)按照扇区(sector)连续的原则分为段(segment)
 * @param entry
 * @param buff
 * @param pg_id 页号
 * @param pg_cnt 页数
 * @param rw
 * @return bio_t*
 */
bio_t *get_rw_pages_bio(entry_t *entry, uint64_t buff, uint32_t pg_id, int pg_cnt, int rw) {
    bio_t *bio = kzalloc(sizeof(bio_t));
    struct bio_vec *first_bio_vec;
    /* 用来统计扇区总数是否符合条件 */
    // int sect_num = 0;
    uint32_t bps = entry->fat->bytes_per_sec;

    first_bio_vec = fat_get_sectors(entry->fat, entry->clus_start, pg_id * PGSIZE, pg_cnt * PGSIZE);
    // cur_bio_vec = first_bio_vec;
    // while(cur_bio_vec != NULL){
    //   cur_bio_vec->bv_buff = (void *)buff;
    //   buff += cur_bio_vec->bv_count * bps;

    //   cur_bio_vec = cur_bio_vec->bv_next;
    // }
    bio_vec_buff_assignment(first_bio_vec, buff, bps);

    // if(PGSIZE / bps != sect_num)
    //   panic("do_readpage: sector num is wrong!");
    bio->bi_io_vec = first_bio_vec;
    bio->bi_rw = rw;
    bio->bi_dev = entry->fat->dev;

    // print_bio_vec(bio);
    // printf("\n");

    return bio;
}

void free_rw_page_list(rw_page_list_t *pg_list) {
    rw_page_t *pg = pg_list->head;
    while (pg) {
        rw_page_t *tmp = pg->next;
        kfree((void *)pg);
        pg = tmp;
    }
    kfree(pg_list);
}

// /**
//  * @brief get dirty pages from mapping, and get corresponding sectors,
//  * write back to disk.
//  * 
//  */
// int mpage_writepages(address_space_t *mapping){
//   entry_t *entry = mapping->host;
//   pages_be_found_head_t *pg_list;
//   pages_be_found_t *cur_page, *next_page;
//   uint32_t nr_continuous_pages;


//   // printf_radix_tree(&mapping->page_tree);

//   pg_list = find_pages_tag(mapping, PAGECACHE_TAG_DIRTY);

//   // print_pages_be_found(pg_list);

//   /* no page in mapping is dirty */
//   if(pg_list == NULL)
//     return 0;
//   // if(pg_list->head == NULL){
//     /* free the pg_list !!!*/
//     // kfree(pg_list);
//     // return 0;
//   // }
//   /** 
//    * 合并pg_id连续的页, 一批连续的页调用一次get_sectors, 这样可以使得得到的一个bio_vec
//    * 包含的sectors尽可能多。因为在lookup_tag递归查询时，是按照页index递增的顺序查询的，所以
//    * 在pg_head链表中的页index是递增的。
//    * 
//    * 补充：合并的要求比较严格，不仅要求页的pg_id是连续的，还要要求页的pa是连续的才行，要求比较严格。
//    */
//   cur_page = pg_list->head;
//   while(cur_page){
//     nr_continuous_pages = 1;

//     /* find max counts of continuous page */
//     next_page = cur_page->next;
//     pages_be_found_t *tmp = cur_page;
//     #ifdef TODO
//     todo("modify the allocated page address, make the later allocated one bigger than the former one");
//     #endif
//     // printf("start merge\n");
//     while(next_page){
//       if((next_page->pg_id == tmp->pg_id+1) && (next_page->pa == tmp->pa + PGSIZE)){
//         nr_continuous_pages++;
//         next_page = next_page->next;
//         tmp = tmp->next;
//       }
//       else
//         break;
//     }
//     // printf("merge end\n");
//     // printf(rd("cur_page->pa: %p\t cur_page->pg_id: %d\t nr continuous pages: %d\n"), cur_page->pa, cur_page->pg_id, nr_continuous_pages);

//     bio_t *bio = get_rw_pages_bio(entry, cur_page->pa, cur_page->pg_id, nr_continuous_pages, WRITE);
//     // printf("bio get\n");
//     // print_bio_vec(bio);
//     // printf("submit...\n");
//     submit_bio(bio);
//     // printf("submit end...\n");
//     cur_page = next_page;
//   }

//   /* 这里别忘了释放pghead相关的结构体！ */
//   free_rw_page_list(pg_list);
//   // pages_be_found_t *pg = pg_list->head;
//   // while(pg){
//   //   pages_be_found_t *tmp = pg->next;
//   //   kfree((void*)pg);
//   //   pg = tmp;
//   // }
//   // kfree(pg_list);

//   return 0;

// }

/**
 * 将由pg_head串联起来的页写回磁盘。
 * linux: mpage_writepages
 */
int rw_pages(entry_t *entry, rw_page_list_t *pg_list, int rw) {
    rw_page_t *cur_page, *next_page;
    uint32_t nr_continuous_pages;

    /**
     * 合并pg_id连续的页, 一批连续的页调用一次get_sectors, 这样可以使得得到的一个bio_vec
     * 包含的sectors尽可能多。因为在lookup_tag递归查询时，是按照页index递增的顺序查询的，所以
     * 在pg_head链表中的页index是递增的。
     *
     * 补充：合并的要求比较严格，不仅要求页的pg_id是连续的，还要要求页的pa是连续的才行，要求比较严格。
     */
    cur_page = pg_list->head;
    while (cur_page) {
        nr_continuous_pages = 1;

        /* find max counts of continuous page */
        next_page = cur_page->next;
        rw_page_t *tmp = cur_page;
#ifdef TODO
        todo("modify the allocated page address, make the later allocated one bigger than the former one");
#endif
        // printf("start merge\n");
        while (next_page) {
            if ((next_page->pg_id == tmp->pg_id + 1) && (next_page->pa == tmp->pa + PGSIZE)) {
                nr_continuous_pages++;
                next_page = next_page->next;
                tmp = tmp->next;
            } else
                break;
        }
        // printf("merge end\n");
        // printf(rd("cur_page->pa: %p\t cur_page->pg_id: %d\t nr continuous pages: %d\n"), cur_page->pa, cur_page->pg_id, nr_continuous_pages);

        bio_t *bio = get_rw_pages_bio(entry, cur_page->pa, cur_page->pg_id, nr_continuous_pages, rw);
        // printf("bio get\n");
        // print_bio_vec(bio);
        // printf("submit...\n");
        submit_bio(bio);
        // printf("submit end...\n");
        cur_page = next_page;
    }

    /* 这里别忘了释放pghead相关的结构体！ */
    free_rw_page_list(pg_list);
    // pages_be_found_t *pg = pg_list->head;
    // while(pg){
    //   pages_be_found_t *tmp = pg->next;
    //   kfree((void*)pg);
    //   pg = tmp;
    // }
    // kfree(pg_list);

    return 0;
}

void msync(uint64_t addr, uint64_t length, int flags) {
    length = PGROUNDUP(length);
    uint64_t rest_cnts = length >> PGSHIFT;

    while (rest_cnts > 0) {
        struct proc *p = myproc();
        vma_t *vma = __vma_find_strict(p->mm, addr);

        if (!vma)
            ER();

        uint64_t vma_len_pg_cnts = vma->len >> PGSHIFT;
        uint64_t cnts = min(vma_len_pg_cnts, rest_cnts);
        int begin_id = vma->offset >> PGSHIFT;
        rw_page_list_t *rwlist = kzalloc(sizeof(rw_page_list_t));

        for (int i = 0; i < cnts; i++) {
            rw_page_t *rwpage = kzalloc(sizeof(rw_page_t));
            rwpage->pa = addr;
            addr += PGSIZE;
            rwpage->pg_id = begin_id++;

            if (rwlist->head == NULL) {
                rwlist->head = rwpage;
                rwlist->tail = rwpage;
            } else {
                rwlist->tail->next = rwpage;
                rwlist->tail = rwpage;
            }
        }

        write_pages(vma->map_file->ep, rwlist);

        rest_cnts -= cnts;
    }
}
