#include "common.h"
#include "fs/file.h"
#include "mm/alloc.h"
#include "mm/mmap.h"
#include "mm/vm.h"
#include "common.h"

vma_t *vma_new(mm_t *mm,
    struct file *fp,
    off_t foff,
    uint64_t addr,
    uint64_t len,
    int flags,
    int prot,
    int page_spec) 
{
    vma_t *vma = (vma_t *)kzalloc(sizeof(vma_t));

    if (vma) {
        uint64_t addr_align = PGROUNDDOWN(addr);
        uint64_t end = addr + len;
        // Do we need to add reference count for mm?
        vma->mm = mm;
        vma->addr = addr_align;
        vma->len = PGROUNDUP(end - addr_align);
        vma->raddr = addr;
        vma->rlen = len;
        vma->flags = flags;
        if (fp) {
            filedup(fp);
        }
        vma->page_spec = page_spec;
        vma->map_file = fp;
        vma->prot = prot;
        vma->offset = foff;
    }
    
    return vma;
}

void vma_free(vma_t **_vma) {
    if (*_vma) {
        vma_t *vma = *_vma;
        mm_t *mm = vma->mm;
        // Unmap area
        __unmap_pages(mm->pagetable, vma->addr, vma->len, 1, vma->page_spec);
        // Close file map
        if (vma->map_file) {
            fileclose(vma->map_file);
        }
    }
    kfree_safe(_vma);
}


vma_t *vma_clone(mm_t *newmm, vma_t *vma) {
    vma_t *dup;
    if ((dup = (vma_t *)kzalloc(sizeof(vma_t))) == NULL) {
        return NULL;
    }

    if (vma->map_file) {
        filedup(vma->map_file);
    }

    memcpy(dup, vma, sizeof(vma_t));

    dup->mm = newmm;

    // Copy memory (with cow)
    if (uvmcopy(vma->mm->pagetable, newmm->pagetable, vma) == -1) {
        vma_free(&dup);
    }

    return dup;
}



static int __vma_resize_reverse(vma_t *vma, uint64_t newsize) {
    uint64_t cursize = vma->len;
    mm_t *mm = vma->mm;
    newsize = PGROUNDUP_SPEC(vma->raddr + newsize, vma->page_spec) - vma->addr;
    int64_t delta = newsize - cursize;
    if (delta == 0) {
        return 0;
    } else if (delta < 0) {
        __unmap_pages(mm->pagetable, vma->addr, -delta, 1, vma->page_spec);
    } else {
        if (__vma_find_greater(mm, vma->addr - delta) != vma) {
            return -1;
        }
    }
    vma->addr = vma->raddr = vma->addr - delta;
    vma->len = newsize;
    return 0;
}

static int __vma_resize(vma_t *vma, uint64_t newsize) {
    uint64_t cursize = vma->len;
    mm_t *mm = vma->mm;
    newsize = PGROUNDUP_SPEC(vma->raddr + newsize, vma->page_spec) - vma->addr;
    if (cursize == newsize) {
        return 0;
    } else if (cursize > newsize) {
        __unmap_pages(mm->pagetable, vma->addr + newsize, cursize - newsize, 1, vma->page_spec);
    } else {
        if (__vma_find_less(mm, vma->addr + newsize) != vma) {
            return -1;
        }
    }
    vma->len = newsize;
    return 0;
}


int vma_resize(vma_t *vma, uint64_t newsize) {
    if (vma->flags & MAP_STACK) {
        return __vma_resize_reverse(vma, newsize);
    } else {
        return __vma_resize(vma, newsize);
    }

}


void vma_protect(uint prot) {
    
}

void vma_print(vma_t *vma) {
    char *filename = "";

    if (vma == NULL) {
        kprintf("vma: NULL\n");
    } else {
        char perm[5];
        perm[4] = '\0';
        perm[0] = vma->prot & PROT_READ ? 'r' : '-';
        perm[1] = vma->prot & PROT_WRITE ? 'w' : '-';
        perm[2] = vma->prot & PROT_EXEC ? 'x' : '-';
        perm[3] = vma->prot & PROT_USER ? 'u' : '-';
        if (vma->map_file) {
            filename = vma->map_file->ep->name;
            kprintf("vma@%#lx: %#lx--%#lx len: %#lx %s %s\n", vma, vma->addr, vma->addr + vma->len, vma->len, perm, filename);
        } else if (vma->io_addr){
            kprintf("vma@%#lx: %#lx--%#lx len: %#lx %s io@%lx\n", vma, vma->addr, vma->addr + vma->len, vma->len, perm, vma->io_addr);
        } else {
            kprintf("vma@%#lx: %#lx--%#lx len: %#lx %s anonymous\n", vma, vma->addr, vma->addr + vma->len, vma->len, perm);
        }
    }
}
