/**
 * @file mmap.c
 * @author YangZongzhen
 * @brief about mmap
 * @note
 * We use `mm_t` (Memory Layout) to represent the process memory space
 * and the memory region is divided as `vma_t` (virtual memory area).
 * It should be emphasized that the memory region we are talking about doesn't
 * include the kernel space(I/O remap, Kernel identity map...) which will be decribed
 * in `public_map`
 * 
 * @version 0.1
 * @date 2023-03-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "mm/mmap.h"
#include "mm/vm.h"

#define QUIET
#define __MODULE_NAME__ MMAP
#include "debug.h"

/* 
#define check_range(va, len, limit) \
  (((uint64_t)(va) + (uint64_t)(len) > (uint64_t)(va)) && \
  (((uint64_t)(va) + (uint64_t)(len) <= (uint64_t)(limit))))
*/

/* vma */
extern int vma_resize(vma_t *vma, uint64_t newsize);
extern vma_t *vma_new(mm_t *mm,
        struct file *fp, 
        off_t foff, 
        uint64_t addr, 
        uint64_t len, 
        int flags, 
        int prot, 
        int page_spec);
extern void vma_free(vma_t **vma);
extern vma_t *vma_clone(mm_t *newmm, vma_t *vma);
extern void vma_print(vma_t *vma);



vma_t *__vma_find_greater(mm_t *mm, uint64_t addr) {
    vma_t *ans;
    list_for_each_entry(ans, &mm->vma_head, head) {
        if(addr < ans->addr + ans->len) {
            return ans;
        }
    }
    return NULL;
}

vma_t *__vma_find_less(mm_t *mm, uint64_t addr) {
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

vma_t *__vma_find_strict(mm_t *mm, uint64_t addr) {
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

static vma_t *__mmap_map(
        mm_t *mm, 
        struct file *fp, 
        off_t foff, 
        uint64_t addr, 
        uint64_t len, 
        int flags, 
        int prot,
        int pg_spec) 
{
    vma_t *vma = vma_new(mm, fp, foff, addr, len, flags, prot, pg_spec);
    if (vma) {
        vma_insert(mm, vma);
        return vma;
    } else {
        return NULL;
    }
}


static vma_t *__mmap_map_fixed(
    mm_t *mm, 
    struct file *fp, 
    off_t foff, 
    uint64_t addr, 
    uint64_t len, 
    int flags, 
    int prot,
    int pg_spec,
    vma_t *inter_vma) 
{
    // No overlay vma
    if (inter_vma == NULL) {
        return __mmap_map(mm, fp, foff, addr, len, flags, prot, pg_spec);
    }

    len = PGROUNDUP(len);
    // IMPROVE ME: This part is hardcode for busybox, 
    // so it is not completed, just like shit.
    if(addr + len != inter_vma->addr + inter_vma->len || addr < inter_vma->addr) {
        debug("fixed map must be mapped inner an existed map: addr %#lx len %#lx prot %#b", addr, len, prot);
        release(&mm->mm_lock);
        return NULL;
    }
    inter_vma->len = addr - inter_vma->addr;

    return inter_vma;
}


vma_t *mmap_map(
    mm_t *mm, 
    struct file *fp, 
    off_t off, 
    uint64_t addr, 
    uint64_t len, 
    int flags, 
    int prot) 
{
    vma_t *vma, *ret;
    
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
        ret = __mmap_map_fixed(mm, fp, off, addr, len, flags, prot, PGSPEC_NORMAL, vma);
        if (ret == NULL) {
            goto release_fail;
        }
    } else if (vma == NULL){
        ret = __mmap_map(mm, fp, off, addr, len, flags, prot, PGSPEC_NORMAL);
        if (ret == NULL) {
            goto release_fail;
        }
    } else {
        debug("vma remmapped");
        goto release_fail;
    }
    
    release(&mm->mm_lock);

    return ret;

  release_fail:
    release(&mm->mm_lock);
  fail:
    return NULL;
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


static vma_t *__mmap_map_io(
        mm_t *mm, 
        void *pa, 
        uint64_t addr, 
        uint64_t len, 
        int flags, 
        int prot,
        int pg_spec) 
{
    vma_t *vma = vma_new(mm, NULL, 0, addr, len, flags, prot, pg_spec);
    if (vma) {
        vma->io_addr = pa;
        vma_insert(mm, vma);
        return vma;
    } else {
        return NULL;
    }
}


vma_t *mmap_map_io(mm_t *mm, uint64_t addr, uint64_t len, uint64_t pa, int prot, int pg_spec) {
    vma_t *ret;
    uint64_t rlen = PGROUNDUP_SPEC(len, pg_spec);

    acquire(&mm->mm_lock);

    if (vma_inter(mm, addr, len)) {
        goto fail;
    }

    ret = __mmap_map_io(mm, (void *)pa, addr, rlen, 0, prot, pg_spec);
    if (ret == NULL) {
        goto fail;
    }

    if (__map_pages(mm->pagetable, addr, rlen, pa, prot, pg_spec) == -1) {
        __unmap_pages(mm->pagetable, addr, rlen, 0, pg_spec);
        goto fail;
    }
    
    release(&mm->mm_lock);

    return ret;

  fail:
    release(&mm->mm_lock);
    return NULL;
}

static void unmap_vmas(mm_t *mm) {
    vma_t *vma, *next;
    list_for_each_entry_safe(vma, next, &mm->vma_head, head) {   
        vma_remove(mm, vma);
        vma_free(&vma);
    }
}



mm_t *__mmap_new() {
    mm_t *mm = (mm_t *)kzalloc(sizeof(mm_t));
    if (mm == NULL) {
        return NULL;
    }
    mm->pagetable = kzalloc(PGSIZE);
    if (mm->pagetable == NULL) {
        kfree(mm);
        return NULL;
    }
    initlock(&mm->mm_lock, "mmlock");
    INIT_LIST_HEAD(&mm->vma_head);

    return mm;
}

mm_t *mmap_new() {
    mm_t *mm = __mmap_new();

    if(mm == NULL) 
        return NULL;

    if(setupkvm(mm->pagetable) == -1) {
        kfree_safe(&mm->pagetable);
        kfree_safe(&mm);
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

vma_t *mmap_map_heap(mm_t *mm) {
#define HEAP_OFFSET 0x200000UL // 2MB
#define HEAP_INIT_SIZE 0x800000UL // 8MB
    assert(mm->ustack == NULL);
    assert(mm->uheap == NULL);
    vma_t *last = list_last_entry(&mm->vma_head, vma_t, head);
    uint64_t heap_start = last->addr + last->len + HEAP_OFFSET;
    mm->uheap = mmap_map(mm, NULL, 0, heap_start, HEAP_INIT_SIZE, MAP_ANONYMOUS, PROT_READ|PROT_WRITE|PROT_USER);
    if (mm->uheap == NULL) {
        return NULL;
    }

    return mm->uheap;
}

vma_t *mmap_map_stack(mm_t *mm, uint64_t stacksize) {
    assert(mm->ustack == NULL);
    mm->ustack = mmap_map(mm, NULL, 0, USERSPACE_END - stacksize, stacksize, MAP_STACK, PROT_READ|PROT_WRITE|PROT_EXEC|PROT_USER);
    if(mm->ustack == NULL) {
        return NULL;
    }

    return mm->ustack;
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
        kprintf("%d. ", id++);
        vma_print(vma);
    }
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
                // kprintf("%s %-3d: pte[LEAF] %p pa %p\n", indent, i, pte, PTE2PA(pte));
                kprintf("%s %-3d[LEAF]: ", indent, i);
                pte_print(&pte);
            } else {// 打印下级页表地址 
                kprintf("%s %-3d: ", indent, i);
                pte_print(&pte);
                __print_pagetable((pagetable_t) PTE2PA(pte), level - 1, ignore_level);
            }
        }
    }
}


void print_pagetable(pagetable_t pagetable) {
  kprintf("page table %p\n", pagetable);
  __print_pagetable(pagetable, 2, -1);
}

