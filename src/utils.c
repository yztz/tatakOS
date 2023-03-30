#include "utils.h"
#include "mm/page.h"
#include "fs/fat.h"
#include "str.h"
#include "kernel/proc.h"
#include "debug.h"
#include "bio.h"

#include "radix-tree.h"

void backtrace() {
    uint64_t fp, top;
    fp = r_fp();
    top = PGROUNDUP(fp);
    while (fp < top) {
        kprintf("%p\n", *(uint64_t*)(fp - 8));
        fp = *(uint64_t*)(fp - 16);
    }
}

void backtrace_fp(uint64_t fp) {
    uint64_t top;
    top = PGROUNDUP(fp);
    while (fp < top) {
        kprintf("%p\n", *(uint64_t*)(fp - 8));
        fp = *(uint64_t*)(fp - 16);
    }
}

extern proc_t proc[NPROC];
void backtrace_proc(int pid) {
    for (proc_t *p = &proc[0]; p < &proc[NPROC]; p++) {
        if (p->pid == pid) {
            backtrace_fp(p->context.s0);
            break;
        }
    }
}

void print_block(uint8_t* b) {
    for (int i = 0; i < BSIZE; i++) {
        if (i != 0 && i % 16 == 0) kprintf("\n");
        kprintf("%02x ", b[i]);
    }
}



void print_sbiret(sbiret_t ret) {
    kprintf("sbiret{err: %d, val: %d}\n", ret.error, ret.value);
}


void print_dir_item(struct dir_item* item) {
    if (!item) {
        kprintf("bad item\n");
        return;
    }
    char name[FAT_SFN_LENGTH + 1];
    name[FAT_SFN_LENGTH] = '\0';
    strncpy(name, (char*)item->name, FAT_SFN_LENGTH);
    kprintf("[file: %s] attr: %b filesize: %d cluster: %d\n", name, item->attr, item->size, FAT_FETCH_CLUS(item));
}

// ref: lua fast get order 打表+快速移位x8
int luaO_log2(unsigned int x) {
    static const unsigned char log_2[256] = {
      0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
      6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    };

    int l = -1;
    while (x >= 256) { l += 8; x >>= 8; }
    return l + log_2[x];
}




void error(char* info, char* s, const char* s1, int d) {
    kprintf(rd("ERROR INFO:\n"));
    kprintf(rd("%s\n"), info);
    kprintf(rd("file: %s\t func: %s\t line: %d\r\n"), s, s1, d);
    enable_sum();
    for (;;);
}

/**
 * @brief 给出一段连续的va，打印出pa
 *
 */
void print_pa(pagetable_t pagetable, uint64_t start, uint64_t end) {
    for (int i = start; i < end; i += PGSIZE) {
        pte_t* pte;
        pte = walk(pagetable, i, 1);
        uint64_t pa = PTE2PA(*pte);
        kprintf("va: %p\tpa: %p\n", i, pa);
    }

}

void _kprintf_radix_tree(struct radix_tree_node* node, uint8_t height, uint8_t c_h) {
    kprintf(bl("count: %-3d, dirty tags: %5p\n"), node->count, node->tags[0][0]);
    for (int i = 0; i < (1 << RADIX_TREE_MAP_SHIFT); i++) {
        if (node->slots[i] != NULL) {
            for (int j = 0; j < c_h; j++) {
                kprintf(".. ");
            }
            /* the leaf node */
            if (c_h == height) {
                kprintf("%-3d  "grn("pa: %p\n"), i, node->slots[i]);
            } else {
                kprintf("%-3d  ", i);
                // kprintf(bl("count: %-3d, dirty tags: %5p\n"), node->count, node->tags[0][0]);
                _kprintf_radix_tree((struct radix_tree_node*)node->slots[i], height, c_h + 1);
            }
        }
    }
}

void kprintf_radix_tree(struct radix_tree_root* root) {
    kprintf(rd("root:  "));
    // kprintf(bl("count: %-3d, dirty tags: %5p"), root->rnode->count, root->rnode->tags[0][0]);
    kprintf("\n");
    _kprintf_radix_tree(root->rnode, root->height, 1);
    kprintf("\n");
}


void print_bio_vec(struct bio* cur_bio) {
    struct bio_vec* cur_bio_vec = cur_bio->bi_io_vec;
    while (cur_bio_vec != NULL) {
        kprintf(ylw("bio_vec start: %d, bio_vec counts: %d\n"),
            cur_bio_vec->bv_start_num, cur_bio_vec->bv_count);

        cur_bio_vec = cur_bio_vec->bv_next;
    }
}

/* 打印一个页的内容, 以块为单位*/
void print_page_contents(char* pa) {
    for (int i = 0; i < PGSIZE; i++) {
        if (i % BSIZE == 0) {
            kprintf(rd("\n%d\n"), i);
        }
        kprintf(grn("%c"), *pa);
        pa++;
    }
    kprintf("\n");
}

/**
 * @brief 在exec.c中的exec中使用，打印参数。
 *
 */
void print_argv(char** argv) {
    int i = 0;
    while (argv[i] != NULL) {
        kprintf(ylw("argv[%d]: %s\n"), i, argv[i]);
        i++;
    }
}

/**
 * 打印页信息
 */
void print_page_info(page_t* page) {
    kprintf("refcnt: %d\tpgnum: %d\taddr: 0x%x\tflags: 0x%x\tpg_pointer: 0x%x\n",
        page->refcnt, page - pages, NR_TO_ADDR(page - pages), page->flags, page);
}

/**
 * @brief 打印出从kernel end的位置开始，非空闲的页(buddy管理的页），用来检测内存泄漏。
 *
 */
void print_not_freed_pages() {
    // int pgnum;
    // uint64_t p;

    kprintf(rd("pages not be freed:\n"));
    // for(p = (uint64_t)end; p < MEM_END; p += PGSIZE){
      // pgnum = PG_TO_NR(p);
      // if(pages[pgnum].refcnt > 0)
        // kprintf("pgnum: %d\taddr: %p\n", pgnum, p);
    // }
    for (int i = 0; i < PAGE_NUMS; i++) {
        if (page_refcnt(&pages[i]) > 0)
            print_page_info(&pages[i]);
    }
}


void debug_pages(uint64_t imapping) {
    for (int i = 0; i < PAGE_NUMS; i++) {
        if ((uint64_t)pages[i].mapping == imapping)
            print_page_info(&pages[i]);
    }
}



void print_zone_list(list_head_t* head) {
    page_t* page;
    list_for_each_entry(page, head, lru) {
        print_page_info(page);
    }
}


void print_rw_page_list(struct rw_page_list* pg_list) {
    rw_page_t* pg = pg_list->head;
    while (pg) {
        kprintf(bl("\npa:%p\tpg_id:%d\n"), pg->pa, pg->pg_id);
        // print_page_contents((char*)pg->pa);
        pg = pg->next;
    }
}

void print_chars(char* c, int n) {
    for (int i = 0; i < n; i++) {
        kprintf(grn("%c"), *c);
        c++;
        kprintf("\n");
    }
}

// Empty implentation
uint64_t __sys_dummy(void) {
    return 0;
} 