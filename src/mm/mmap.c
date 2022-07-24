#include "mm/mmap.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "str.h"
#include "mm/vm.h"

#define __MODULE_NAME__ MMAP
#include "debug.h"

#define VMA_LEN(vma) ((vma)->len)
#define VMA_PAGES(vma) (((vma)->len) / PGSIZE)
#define PAGETABLE(mm) ((mm)->owner->pagetable)

#define KSTACK_VMA(mm) (&((mm)->kstack))


#define check_range(va, len, limit) \
  (((uint64_t)(va) + (uint64_t)(len) > (uint64_t)(va)) && \
  (((uint64_t)(va) + (uint64_t)(len) <= (uint64_t)(limit))))


static int check_flags(int flags1, int flags2) {
    return flags1 == flags2;
}

// TODO: 检查flag是否兼容（目前只是简单的比较是否相等）
static int check_prot(int prot1, int prot2) {
    return prot1 == prot2;
}

vma_t *__vma_find(mm_t *mm, uint64 addr) {
    vma_t *ans;
    list_for_each_entry(ans, &mm->vma_head, head) {
        if(addr < ans->addr + ans->len) {
            return ans;
        }
    }
    return NULL;
}

vma_t *__vma_find_strict(mm_t *mm, uint64 addr) {
    vma_t *ans = __vma_find(mm, addr);
    return ans && ans->addr <= addr ? ans : NULL;
}

void vma_remove(mm_t *mm, vma_t *vma) {
    list_del(&vma->head);
}

vma_t *vma_previous(mm_t *mm, vma_t *vma) {
    return list_prev_entry(vma, head);
}

void vma_insert(mm_t *mm, vma_t *vma) {
    list_head_t *pre_head = &mm->vma_head;
    vma_t *cur;
    list_for_each_entry(cur, &mm->vma_head, head) {
        if(vma->addr + vma->len <= cur->addr) {
            break;
        }
        pre_head = &cur->head;
    }
    list_add(&vma->head, pre_head);
}


/* 包含查找 vma[...addr...addr+len...] */
vma_t *vma_exist(mm_t *mm, uint64_t addr, uint64_t len) {
    // if(!check_range(addr, len, USERSPACE_END)) return NULL;
    vma_t *ans = __vma_find_strict(mm, addr);
    if(ans && addr -  ans->addr <= ans->len - len) {
        return ans;
    }

    return NULL;
}
/* 相交查找 addr vma[...addr+len */
vma_t *vma_inter(mm_t *mm, uint64_t addr, uint64_t len) {
    uint64_t end = addr + len;
    vma_t *vma = __vma_find(mm, addr);

    if(vma && vma->addr < end) { // 当存在相交vma
        return vma;
    } else {
        return NULL;
    }
}

// int __vmaS_merge(mm_t *mm, vma_t *start, vma_t *end) {
//     if(start == NULL || end == NULL) return 0;
//     vma_t *tmp = start;
//     // 合并检查
//     while(tmp != end) {
//         vma_t *next = list_next_entry(tmp, head);
//         if(!check_prot(tmp->prot, next->prot)) return -1;
//         if(!check_flags(tmp->flags, next->flags)) return -1;
//         tmp = next;
//     }
//     // 执行合并
//     while(1) {
//         vma_t *next = list_next_entry(tmp, head);
//         start->len = next->addr + next->len - start->addr;
//         vma_remove(mm, next);
//         if(next == end) {
//             vma_free(&next);
//             break;
//         }
//         vma_free(&next);
//     }
//     return 0;
// }


void __do_mmap(mm_t *mm, struct file *fp, off_t foff, uint64_t addr, uint64_t len, int flags, int prot) {
    uint64_t end = addr + len;
    vma_t *vma;

    if(fp)
        filedup(fp);
    // 不存在相交vma
    vma = vma_new();
    vma->raddr = addr;
    vma->addr = PGROUNDDOWN(addr);
    vma->len = end - vma->addr;
    vma->flags = flags;
    vma->map_file = fp;
    vma->prot = prot;
    vma->offset = foff;
    vma_insert(mm, vma);

//   merge:
//     // 经过上述处理完，找到的vma布局如下所示：
//     // (addr) vma0] [vma1] [vma2 (end)
//     debug("merge..");
//     if(check_flags(vma->flags, flags) == 0 || check_prot(vma->prot, prot) == 0) return -1;
//     if(__vmaS_merge(mm, vma, vma_end) == -1) return -1;
//     if(vma->addr > addr) {
//         vma->addr = PGROUNDDOWN(addr);
//     }
//     if(vma->addr + vma->len < end) {
//         vma->len = end - vma->addr;
//     }
//     return 0;
}


static uint64_t find_free_maparea(mm_t *mm, uint64_t len) {
    uint64_t va = MMAP_BASE;
    vma_t *vma = __vma_find(mm, va);
    // TODO: 不是很严谨
    while(vma && vma->addr < va + len) {
        va = PGROUNDUP(vma->addr + vma->len) + PGSIZE;
        vma = __vma_find(mm, va);
    }

    return va;
}


uint64_t do_mmap(mm_t *mm, struct file *fp, off_t off, uint64_t addr, uint64_t len, int flags, int prot) {
    vma_t *vma;
    
    flags = fp ? (flags & (~MAP_ANONYMOUS)) : (flags | MAP_ANONYMOUS);

    if(addr == 0) {
        addr = find_free_maparea(mm, len);
    }

    uint64_t addr_align = PGROUNDDOWN(addr);

    vma = vma_inter(mm, addr_align, len);

    if(flags & MAP_FIXED) {
        if(addr != addr_align) return -1;
        if(vma) {
            if(addr + len != PGROUNDUP(vma->addr + vma->len) || addr < vma->addr) {
                debug("fixed map must be mapped in existed map");
                return -1;
            }
            vma->len = addr - vma->addr;
        }
    } else if(vma != NULL) {
        return -1;
    }

    __do_mmap(mm, fp, off, addr, len, flags, prot);
    
    return addr;
}

void do_unmap(mm_t *mm, uint64_t addr, int do_free) {
    vma_t *vma = __vma_find_strict(mm, addr);
    if(vma) {
        // TODO: file map
        uvmunmap(mm->pagetable, vma->addr, ROUND_COUNT(vma->len), do_free);
        vma_remove(mm, vma);
        if(vma->map_file) {
            fileclose(vma->map_file);
        }
        vma_free(&vma);
    }
}

uint64_t do_mmap_alloc(mm_t *mm, uint64_t addr, uint64_t len, int flags, int prot) {
    char *mem;
    uint64_t a;

    if(do_mmap(mm, NULL, 0, addr, len, flags, prot) == -1) {
        return 0;
    }

    for(a = PGROUNDDOWN(addr); a < addr + len; a += PGSIZE){
        mem = kzalloc(PGSIZE);
        if(mem == 0){
            goto bad;
        }
        if(mappages(mm->pagetable, a, PGSIZE, (uint64)mem, riscv_map_prot(prot)) != 0){
            kfree(mem);
            goto bad;
        }
    }

    
    return addr;

  bad:
    do_unmap(mm, addr, 0);
    for(uint64_t i = PGROUNDDOWN(addr); i < a; i += PGSIZE) {
        uvmunmap(mm->pagetable, i, 1, 1);
    }
    return 0;
}

static void unmap_vmas(mm_t *mm) {
    vma_t *vma, *next;
    list_for_each_entry_safe(vma, next, &mm->vma_head, head) {
        uvmunmap(mm->pagetable, vma->addr, ROUND_COUNT(vma->len), 1);   
        vma_remove(mm, vma);
        if(vma->map_file) {
            fileclose(vma->map_file);
        }
        vma_free(&vma);
    }
}



int mmap_init(mm_t *mm) {
    mm->pagetable = kzalloc(PGSIZE);
    INIT_LIST_HEAD(&mm->vma_head);
    if(mm->pagetable == NULL) {
        debug("stub1");
        return -1;
    }
    
    if(setupkvm(mm->pagetable) == -1) {
        kfree_safe(&mm->pagetable);
        return -1;
    }
    
    return 0;
}

void mmap_free(mm_t **pmm) {
    mm_t *mm = *pmm;
    if(mm == NULL) 
        panic("nullpointer");

    unmap_vmas(mm);
    erasekvm(mm->pagetable);
    freewalk(mm->pagetable);
    kfree_safe(pmm);
}

static vma_t *vma_dup(vma_t *vma) {
    vma_t *dup;
    if((dup = vma_new()) == NULL) {
        return NULL;
    }
    memcpy(dup, vma, sizeof(vma_t));
    return dup;
}

int mmap_dup(mm_t *newm, mm_t *oldm) {
    vma_t *vma;
    //TODO: file
    list_for_each_entry(vma, &oldm->vma_head, head) {
        vma_t *dup = vma_dup(vma);
        if(dup == NULL) {
            unmap_vmas(newm);
            return -1;
        }
        if(uvmcopy(oldm->pagetable, newm->pagetable, vma) == -1) {
            vma_free(&dup);
            unmap_vmas(newm);
            return -1;
        }
        vma_insert(newm, dup);

        if(vma->map_file) {
            filedup(vma->map_file);
        }

        if(oldm->ustack == vma)
            newm->ustack = dup;
        if(oldm->uheap == vma)
            newm->uheap = dup;
    }
    // memcpy((void *)newm->kstack, (void *)oldm->kstack, KSTACK_SZ);
    // memcpy((void *)newm->trapframe, (void *)oldm->trapframe, PGSIZE);

    return 0;
}

// called after load
int mmap_map_stack_heap(mm_t *mm, uint64_t brk_addr, uint64_t stacksize, uint64_t heapsize) {
    brk_addr = PGROUNDUP(brk_addr);
    if(do_mmap(mm, NULL, 0, brk_addr, heapsize, 0, PROT_READ|PROT_WRITE|PROT_EXEC|PROT_USER) == -1) {
        return -1;
    }
    if(do_mmap(mm, NULL, 0, USERSPACE_END - stacksize, stacksize, MAP_STACK, PROT_READ|PROT_WRITE|PROT_USER) == -1) {
        do_unmap(mm, brk_addr, 0);
        return -1;
    }

    mm->ustack = __vma_find(mm, USERSPACE_END - stacksize);
    mm->uheap = __vma_find(mm, brk_addr);

    return 0;
}

int mmap_ext_heap(mm_t *mm, uint64_t newbreak) {
    if(mm->uheap == NULL) return -1;
    uint64_t newsize = newbreak - mm->uheap->addr;
    uint64_t cursize = mm->uheap->len;
    if(cursize == newsize) {
        return 0;
    } else if(cursize > newsize) {
        uvmunmap(mm->pagetable, 
            PGROUNDUP(mm->uheap->addr + newsize), 
            ROUND_COUNT(cursize) - ROUND_COUNT(newsize), 1);
    } else {
        vma_t *vma;
        if((vma = __vma_find_strict(mm, mm->uheap->addr + newsize))) {
            debug("uheap out of range newbreak is %#lx but encounter the vma: ", newbreak);
            #ifdef DEBUG
            vma_print(vma);
            #endif
            return -1;
        }
    }
    // debug("uheap: %d -> %d", cursize, newsize);
    mm->uheap->len = newsize;
    return 0;
}

int mmap_ext_stack(mm_t *mm, uint64_t newsize) {
    if(mm->ustack == NULL) return -1;
    uint64_t cursize = mm->ustack->len;
    if(cursize == newsize) {
        return 0;
    } else if(cursize > newsize) {
        uvmunmap(mm->pagetable, 
            mm->ustack->addr, 
            ROUND_COUNT(cursize) - ROUND_COUNT(newsize), 1);
    } else {
        vma_t *vma = __vma_find(mm, mm->ustack->addr - newsize);
        if(vma != mm->ustack) {
            return -1;
        }
    }
    mm->ustack->addr = PGROUNDDOWN(USERSPACE_END - newsize);
    mm->ustack->len = USERSPACE_END - mm->ustack->addr;
    return 0;
}





void mmap_print(mm_t *mm) {
    vma_t *vma;
    int id = 1;
    list_for_each_entry(vma, &mm->vma_head, head) {
        printf("%d. ", id++);
        vma_print(vma);
    }
}


void switchuvm(mm_t *mm) {
  if(mm->pagetable == 0)
    panic("switchuvm: no pgdir");

  write_csr(satp, MAKE_SATP(mm->pagetable));
  sfence_vma();
}

extern pagetable_t kernel_pagetable;
void switchkvm() {
  write_csr(satp, MAKE_SATP(kernel_pagetable));
  sfence_vma();
}





static void _vmprint(pagetable_t pagetable, int level, int ignore_level) {
    static char* indents[] = {
        ".. .. ..",
        ".. ..",
        "..",
    };
    
    if(level == ignore_level) return;
    char *indent = indents[level];
    for(int i = 0; i < 512; i++){
        pte_t pte = pagetable[i];
        if(pte & PTE_V){  // 存在
            if((pte & (PTE_R|PTE_W|PTE_X)) > 0) { // 打印叶节点
                // printf("%s %-3d: pte[LEAF] %p pa %p\n", indent, i, pte, PTE2PA(pte));
                printf("%s %-3d[LEAF]: ", indent, i);
                pte_print(&pte);
            } else {// 打印下级页表地址 
                printf("%s %-3d: ", indent, i);
                pte_print(&pte);
                _vmprint((pagetable_t) PTE2PA(pte), level - 1, ignore_level);
            }
        }
    }
}


void vmprint(pagetable_t pagetable) {
  printf("page table %p\n", pagetable);
  _vmprint(pagetable, 2, -1);
}

void print_map(kmap_t map) {
  printf("map:%p => %p, size: %#x type: %d\n", map.pa, map.va, map.size, map.pg_spec);
}

uint64_t sys_mprotect(void) {
    return 0;
}


