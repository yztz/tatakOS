#include "mm/mmap.h"
#include "fs/fs.h"
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
    vma_t *pre;
    list_for_each_entry(pre, &mm->vma_head, head) {
        if(vma->addr >= pre->addr + pre->len) {
            list_add(&vma->head, &pre->head);
            return;
        }
    }
    debug("no entry found, insert directly");
    list_add(&vma->head, &mm->vma_head);
}

int __vmaS_merge(mm_t *mm, vma_t *start, vma_t *end) {
    if(start == NULL || end == NULL) return 0;
    vma_t *tmp = start;
    // 合并检查
    while(tmp != end) {
        vma_t *next = list_next_entry(tmp, head);
        if(!check_prot(tmp->prot, next->prot)) return -1;
        if(!check_flags(tmp->flags, next->flags)) return -1;
        tmp = next;
    }
    // 执行合并
    while(1) {
        vma_t *next = list_next_entry(tmp, head);
        start->len = next->addr + next->len - start->addr;
        vma_remove(mm, next);
        kfree(next);
        if(next == end) {
            break;
        }
    }
    return 0;
}


int __do_mmap(mm_t *mm, struct file *fp, int file_offset, uint64_t addr, uint64_t len, int flags, int prot) {
    uint64_t end = addr + len;
    // 寻找到第一个结束地址大于addr的vma（可以不包含）
    vma_t *vma = __vma_find(mm, addr);
    // 寻找到第一个结束地址大于end的vma
    vma_t *vma_end = __vma_find(mm, end);\
    if(vma_end && vma_end != vma && end < vma_end->addr) {
        vma_end = vma_previous(mm, vma_end);
    }
    // 经过上述处理完，找到的vma布局如下所示：
    // (addr) vma0] [vma1] [vma2 (end)
    // 处理边界问题
    if(vma) { // 当存在相交vma
        if(check_flags(vma->flags, flags) == 0 || check_prot(vma->prot, prot) == 0) return -1;
        if(__vmaS_merge(mm, vma, vma_end) == -1) return -1;
        if(vma->addr > addr) {
            vma->addr = PGROUNDDOWN(addr);
        }
        if(vma->addr + vma->len < end) {
            vma->len = end - vma->addr;
        }
    } else { // 不存在相交vma
        vma = (vma_t *)kzalloc(sizeof(vma_t));
        vma->addr = PGROUNDDOWN(addr);
        vma->len = len;
        vma->flags = flags;
        vma->map_file = fp;
        vma->prot = prot;
        vma->offset = file_offset;
        vma_insert(mm, vma);
    }
    return 0;
}

// int do_file_mmap(mm_t *mm, struct file *fp, int offset, uint64_t addr, uint64_t len, int flags, int prot) {
//     if(fp == NULL) return -1;
//     if(__do_mmap(mm, fp, offset, addr, len, flags, prot) == -1) return -1;
// }

int do_mmap(mm_t *mm, struct file *fp, uint64_t addr, uint64_t len, int flags, int prot) {
    flags |= (fp ? 0 : MAP_ANONYMOUS);
    if(__do_mmap(mm, NULL, 0, addr, len, flags, prot) == -1)
        return -1;
    
    return 0;
}

void do_unmap(mm_t *mm, uint64_t addr, int do_free) {
    vma_t *vma = __vma_find(mm, addr);
    if(vma) {
        // TODO: file map
        uvmunmap(mm->pagetable, vma->addr, ROUND_COUNT(vma->len), do_free);
        vma_remove(mm, vma);
        kfree(vma);
    }
}

int do_mmap_alloc(mm_t *mm, struct file *fp, uint64_t addr, uint64_t len, int flags, int prot) {
    char *mem;
    uint64_t a;

    if(do_mmap(mm, fp, addr, len, flags, prot) == -1) {
        return -1;
    }

    for(a = PGROUNDDOWN(addr); a < addr + len; a += PGSIZE){
        mem = kzalloc(PGSIZE);
        if(mem == 0){
            goto bad;
        }
        if(mappages(mm->pagetable, a, PGSIZE, (uint64)mem, prot) != 0){
            kfree(mem);
            goto bad;
        }
    }

    
    return 0;

  bad:
    do_unmap(mm, addr, 0);
    for(uint64_t i = PGROUNDDOWN(addr); i < a; i += PGSIZE) {
        uvmunmap(mm->pagetable, i, 1, 1);
    }
    return -1;
}

static int map_kstack(mm_t *mm) {
    if((mm->kstack = (uint64)kmalloc(KSTACK_SZ)) == 0) {
        return -1;
    }
    return 0;
}

static void unmap_kstack(mm_t *mm) {
    if(mm->kstack) {
        kfree((void *)mm->kstack);
        mm->kstack = 0;
    }
}

static void unmap_vmas(mm_t *mm) {
    vma_t *vma, *next;
    list_for_each_entry_safe(vma, next, &mm->vma_head, head) {
        uvmunmap(mm->pagetable, vma->addr, ROUND_COUNT(vma->len), 1);
        vma_remove(mm, vma);
        kfree(vma);
    }
}


static int map_trapframe(mm_t *mm) {
    if((mm->trapframe = (uint64)kmalloc(PGSIZE)) == 0) {
        return -1;
    }
    return 0;
}

static void unmap_trapframe(mm_t *mm) {
    if(mm->trapframe) {
        kfree((void *)mm->trapframe);
        mm->trapframe = 0;
    }
}

int mmap_init(mm_t *mm, mm_t *old) {
    mm->pagetable = kzalloc(PGSIZE);
    INIT_LIST_HEAD(&mm->vma_head);
    if(mm->pagetable == NULL) {
        debug("stub1");
    }
    
    if(map_kstack(mm) == -1) {
        kfree(mm->pagetable);
        return -1;
    }
    if(map_trapframe(mm) == -1) {
        unmap_kstack(mm);
        kfree(mm->pagetable);
        return -1;
    }

    if(setupkvm(mm->pagetable) == -1) {
        unmap_kstack(mm);
        unmap_trapframe(mm);
        kfree(mm->pagetable);
        return -1;
    }

    // for exec
    if(old) {
        memcpy((void *)mm->kstack, (void *)old->kstack, KSTACK_SZ);
    }
    
    return 0;
}

void mmap_free(mm_t **pmm) {
    mm_t *mm = *pmm;
    if(mm == NULL) 
        panic("nullpointer");

    unmap_vmas(mm);
    unmap_kstack(mm);
    unmap_trapframe(mm);
    erasekvm(mm->pagetable);
    kfree(mm->pagetable);
    kfree(mm);
    *pmm = NULL;
}

static vma_t *vma_dup(vma_t *vma) {
    vma_t *dup;
    if((dup = (vma_t *)kmalloc(sizeof(vma_t))) == NULL) {
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
        if(uvmcopy(oldm->pagetable, newm->pagetable, vma->addr, vma->len) == -1) {
            kfree(dup);
            unmap_vmas(newm);
            return -1;
        }
        vma_insert(newm, dup);
    }
    memcpy((void *)newm->kstack, (void *)oldm->kstack, KSTACK_SZ);
    memcpy((void *)newm->trapframe, (void *)oldm->trapframe, PGSIZE);

    return 0;
}

int mmap_ext_heap(mm_t *mm, int newsize) {
    if(mm->uheap == NULL) return -1;
    int cursize = mm->uheap->len;
    if(cursize == newsize) {
        return 0;
    } else if(cursize < newsize) {
        uvmunmap(mm->pagetable, 
            PGROUNDUP(mm->uheap->addr + newsize), 
            ROUND_COUNT(mm->uheap->addr + mm->uheap->len) - ROUND_COUNT(mm->uheap->addr + newsize), 1);
    } else {
        if(__vma_find_strict(mm, mm->uheap->addr + newsize)) {
            return -1;
        }
    }
    mm->uheap->len = newsize;
    return 0;
}

int mmap_ext_stack(mm_t *mm) {
    // TODO:
    return -1;
}


struct trapframe *get_trapframe(mm_t *mm) {
    return (struct trapframe *)mm->trapframe;
}

uint64_t get_kstack(mm_t *mm) {
    return mm->kstack;
}

vma_t *vma_exist(mm_t *mm, uint64_t addr, uint64_t len) {
    // if(!check_range(addr, len, USERSPACE_END)) return NULL;
    vma_t *ans = __vma_find_strict(mm, addr);
    if(ans && addr -  ans->addr <= ans->len - len) {
        return ans;
    }

    return NULL;
}

void mmap_print_vma(vma_t *vma) {
    if(vma == NULL) {
        printf("vma: NULL\n");
    } else {
        char perm[5];
        perm[4] = '\0';
        perm[0] = vma->prot & MAP_PROT_READ ? 'r' : '-';
        perm[1] = vma->prot & MAP_PROT_WRITE ? 'w' : '-';
        perm[2] = vma->prot & MAP_PROT_EXEC ? 'x' : '-';
        perm[3] = vma->prot & MAP_PROT_USER ? 'u' : '-';
        
        printf("vma: %lx----%lx len: %ld %s\n", vma->addr, vma->addr + vma->len, vma->len, perm);
    }
}

void mmap_print_vmas(mm_t *mm) {
    vma_t *vma;
    int id = 1;
    list_for_each_entry(vma, &mm->vma_head, head) {
        printf("%d. ", id++);
        mmap_print_vma(vma);
    }
}


void switchuvm(mm_t *mm) {
  if(mm->kstack == 0)
    panic("switchuvm: no kstack");
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
