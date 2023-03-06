#include "mm/mmap.h"
#include "mm/vm.h"

#define __MODULE_NAME__ MMAP
#define QUIET
#include "debug.h"

/* 
#define check_range(va, len, limit) \
  (((uint64_t)(va) + (uint64_t)(len) > (uint64_t)(va)) && \
  (((uint64_t)(va) + (uint64_t)(len) <= (uint64_t)(limit))))
*/

extern int vma_resize(vma_t *vma, uint64 newsize);

vma_t *__vma_find_greater(mm_t *mm, uint64 addr) {
    vma_t *ans;
    list_for_each_entry(ans, &mm->vma_head, head) {
        if(addr < ans->addr + ans->len) {
            return ans;
        }
    }
    return NULL;
}

vma_t *__vma_find_less(mm_t *mm, uint64 addr) {
    vma_t *ans = NULL;
    vma_t *vma;
    list_for_each_entry(vma, &mm->vma_head, head) {
        if(addr < vma->addr) {
            break;
        }
        ans = vma;
    }
    return ans;
}

vma_t *__vma_find_strict(mm_t *mm, uint64 addr) {
    vma_t *ans = __vma_find_greater(mm, addr);
    return ans && ans->addr <= addr ? ans : NULL;
}

static inline void vma_remove(mm_t *mm, vma_t *vma) {
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
    vma_t *ans = __vma_find_strict(mm, addr);
    if(ans && addr - ans->addr <= ans->len - len) {
        return ans;
    }

    return NULL;
}

/**
 * @brief Find the intersecting vma.
 * 
 * @param mm 
 * @param addr 
 * @param len 
 * @return vma_t* 
 */
static vma_t *vma_inter(mm_t *mm, uint64_t addr, uint64_t len) {
    uint64_t end = addr + len;
    vma_t *vma = __vma_find_greater(mm, addr);
    // vma[...end
    if(vma && vma->addr < end) { // 当存在相交vma
        return vma;
    } else {
        return NULL;
    }
}

static uint64_t find_free_maparea(mm_t *mm, uint64_t len) {
    uint64_t va = MMAP_BASE;
    vma_t *vma = __vma_find_greater(mm, va);

    while(vma && vma->addr < va + len) {
        va = PGROUNDUP(vma->addr + vma->len) + PGSIZE;
        vma = __vma_find_greater(mm, va);
    }

    return va;
}

static int __mmap_map(
        mm_t *mm, 
        struct file *fp, 
        off_t foff, 
        uint64_t addr, 
        uint64_t len, 
        int flags, 
        int prot) 
{
    vma_t *vma = vma_new(mm, fp, foff, addr, len, flags, prot);
    if (vma) {
        vma_insert(mm, vma);
        return 0;
    } else {
        return -1;
    }
}


static inline int __mmap_map_fixed(
    mm_t *mm, 
    struct file *fp, 
    off_t foff, 
    uint64_t addr, 
    uint64_t len, 
    int flags, 
    int prot,
    vma_t *inter_vma) 
{
    // No overlay vma
    if (inter_vma == NULL) {
        return __mmap_map(mm, fp, foff, addr, len, flags, prot);
    }

    len = PGROUNDUP(len);
    // IMPROVE ME: This part is hardcode for busybox, 
    // so it is not completed or like shit.
    if(addr + len != inter_vma->addr + inter_vma->len || addr < inter_vma->addr) {
        debug("fixed map must be mapped inner an existed map: addr %#lx len %#lx prot %#b", addr, len, prot);
        release(&mm->mm_lock);
        return -1;
    }
    inter_vma->len = addr - inter_vma->addr;

    return 0;
}


uint64_t mmap_map(
    mm_t *mm, 
    struct file *fp, 
    off_t off, 
    uint64_t addr, 
    uint64_t len, 
    int flags, 
    int prot) 
{
    vma_t *vma;
    
    // Set MAP_ANONYMOUS flag when fp is not specified.
    flags = fp ? (flags & (~MAP_ANONYMOUS)) : (flags | MAP_ANONYMOUS);

    // Zero length is not allowed.
    if (len == 0) {
        goto fail;
    }

    // Use default map address.
    if(addr == 0) {
        addr = find_free_maparea(mm, len);
    }

    // Address should be page-aligned.
    uint64_t addr_align = PGROUNDDOWN(addr);

    acquire(&mm->mm_lock);

    vma = vma_inter(mm, addr_align, len);

    if(flags & MAP_FIXED) {
        // Check Alignment
        if(addr !=  addr_align) {
            goto release_fail;
        }
        if (__mmap_map_fixed(mm, fp, off, addr, len, flags, prot, vma) == -1) {
            goto release_fail;
        }
    } else if (vma == NULL){
        if (__mmap_map(mm, fp, off, addr, len, flags, prot) == -1) {
            goto release_fail;
        }
    } else {
        debug("vma remmapped");
        goto release_fail;
    }
    
    release(&mm->mm_lock);

    return addr;

  release_fail:
    release(&mm->mm_lock);
  fail:
    return -1;
}   

void mmap_unmap(mm_t *mm, uint64_t addr) {
    vma_t *vma = __vma_find_strict(mm, addr);

    if (vma) {
        acquire(&mm->mm_lock);
        vma_remove(mm, vma);
        release(&mm->mm_lock);
        vma_free(&vma);
    }
    
}

uint64_t mmap_map_alloc(mm_t *mm, uint64_t addr, uint64_t len, int flags, int prot) {
    char *mem;
    uint64_t va;

    if(mmap_map(mm, NULL, 0, addr, len, flags, prot) == -1) {
        goto bad;
    }

    for(va = PGROUNDDOWN(addr); va < addr + len; va += PGSIZE) {
        mem = kzalloc(PGSIZE);
        if (mem == 0) {
            // Alloc failure
            goto freebad;
        }

        if (mappages(mm->pagetable, va, PGSIZE, (uint64)mem, prot) != 0) {
            // Page map failure
            kfree(mem);
            goto freebad;
        }
    }

    return addr;

  freebad:
    mmap_unmap(mm, addr);
  bad:
    return -1;
}

static void unmap_vmas(mm_t *mm) {
    vma_t *vma, *next;
    list_for_each_entry_safe(vma, next, &mm->vma_head, head) {   
        vma_remove(mm, vma);
        vma_free(&vma);
    }

}



static int mmap_init(mm_t *mm) {
    initlock(&mm->mm_lock, "mmlock");
    mm->pagetable = kzalloc(PGSIZE);
    INIT_LIST_HEAD(&mm->vma_head);
    if(mm->pagetable == NULL) {
        return -1;
    }
    
    if(setupkvm(mm->pagetable) == -1) {
        kfree_safe(&mm->pagetable);
        return -1;
    }
    
    return 0;
}

mm_t *mmap_new() {
    mm_t *mm = (mm_t *)kzalloc(sizeof(mm_t));
    if(mm == NULL) 
        return NULL;
    if(mmap_init(mm) == -1) {
        kfree(mm);
        return NULL;
    }
    return mm;
}

void mmap_free(mm_t **pself) {
    if(pself == NULL || *pself == NULL) 
        return;
    mm_t *self = *pself;

    if(self->ref == 0)
        panic("ref");

    mmap_deref(self);
    if(self->ref > 0)
        return;

    unmap_vmas(self);
    erasekvm(self->pagetable);
    freewalk(self->pagetable);
    kfree_safe(pself);
}

static int __mmap_clone(mm_t *mm, mm_t *newmm) {
    vma_t *vma;
    list_for_each_entry(vma, &mm->vma_head, head) {
        vma_t *dup = vma_clone(newmm, vma);
        if(dup == NULL) {
            goto fail;
        }

        // Add cloned vma to the vma list
        vma_insert(newmm, dup);

        // Record the special vma
        if(mm->ustack == vma)
            newmm->ustack = dup;
        if(mm->uheap == vma)
            newmm->uheap = dup;
    }

    return 0;

  fail:
    unmap_vmas(newmm);
    return -1;
}

mm_t *mmap_clone(mm_t *mm) {
    mm_t *newmm = mmap_new();
    if(newmm == NULL) 
        return NULL;

    acquire(&mm->mm_lock);

    if(__mmap_clone(mm, newmm) == -1) {
        mmap_free(&newmm);
        return NULL;
    }

    release(&mm->mm_lock);

    return newmm;
}


// called after load
int mmap_map_stack(mm_t *mm, uint64_t stacksize) {
    // brk_addr = PGROUNDUP(brk_addr);
    // if(mmap_map(mm, NULL, 0, brk_addr, heapsize, 0, PROT_READ|PROT_WRITE|PROT_EXEC|PROT_USER) == -1) {
    //     return -1;
    // }
    mm->uheap = list_last_entry(&mm->vma_head, vma_t, head);

    if(mmap_map(mm, NULL, 0, USERSPACE_END - stacksize, stacksize, MAP_STACK, PROT_READ|PROT_WRITE|PROT_EXEC|PROT_USER) == -1) {
        return -1;
    }

    mm->ustack = __vma_find_greater(mm, USERSPACE_END - stacksize);
    assert(mm->uheap != mm->ustack);
    return 0;
}

int mmap_ext_heap(mm_t *mm, uint64_t newbreak) {
    if(mm->uheap == NULL) return -1;
    if(newbreak <= mm->uheap->addr) return -1;

    uint64_t newsize = newbreak - mm->uheap->addr;

    acquire(&mm->mm_lock);
    int ret = vma_resize(mm->uheap, newsize);
    release(&mm->mm_lock);

    return ret;
}

int mmap_ext_stack(mm_t *mm, uint64_t newsize) {
    if(mm->ustack == NULL) return -1;

    acquire(&mm->mm_lock);
    int ret = vma_resize(mm->ustack, newsize);
    release(&mm->mm_lock);

    return ret;
}


void mmap_ref(mm_t *self) {
    acquire(&self->mm_lock);
    self->ref++;
    release(&self->mm_lock);
}

void mmap_deref(mm_t *self) {
    acquire(&self->mm_lock);
    self->ref--;
    release(&self->mm_lock);
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

  set_pgtbl(mm->pagetable);
  sfence_vma();
}

extern pagetable_t kernel_pagetable;
void switchkvm() {
  set_pgtbl(kernel_pagetable);
//   sfence_vma();
}


static void __print_pagetable(pagetable_t pagetable, int level, int ignore_level) {
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
                __print_pagetable((pagetable_t) PTE2PA(pte), level - 1, ignore_level);
            }
        }
    }
}


void print_pagetable(pagetable_t pagetable) {
  printf("page table %p\n", pagetable);
  __print_pagetable(pagetable, 2, -1);
}

