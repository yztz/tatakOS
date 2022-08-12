#include "mm/mmap.h"
#include "mm/alloc.h"
#include "fs/file.h"

vma_t *vma_new() {
    return (vma_t *)kzalloc(sizeof(vma_t));
}

void vma_free(vma_t **vma) {
    kfree_safe(vma);
}

void vma_print(vma_t *vma) {
    char *filename = "";

    if(vma == NULL) {
        printf("vma: NULL\n");
    } else {
        char perm[5];
        perm[4] = '\0';
        perm[0] = vma->prot & PROT_READ ? 'r' : '-';
        perm[1] = vma->prot & PROT_WRITE ? 'w' : '-';
        perm[2] = vma->prot & PROT_EXEC ? 'x' : '-';
        perm[3] = vma->prot & PROT_USER ? 'u' : '-';
        if(vma->map_file) {
            filename = vma->map_file->ep->name;
        }
        printf("%#lx vma: %#lx--%#lx len: %#lx %s %s\n", vma, vma->addr, vma->addr + vma->len, vma->len, perm, filename);
    }
}