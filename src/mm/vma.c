#include "mm/mmap.h"
#include "mm/alloc.h"

vma_t *vma_new() {
    return (vma_t *)kzalloc(sizeof(vma_t));
}

void vma_free(vma_t **vma) {
    kfree_safe(vma);
}

void vma_print(vma_t *vma) {
    if(vma == NULL) {
        printf("vma: NULL\n");
    } else {
        char perm[5];
        perm[4] = '\0';
        perm[0] = vma->prot & MAP_PROT_READ ? 'r' : '-';
        perm[1] = vma->prot & MAP_PROT_WRITE ? 'w' : '-';
        perm[2] = vma->prot & MAP_PROT_EXEC ? 'x' : '-';
        perm[3] = vma->prot & MAP_PROT_USER ? 'u' : '-';
        
        printf("vma: %lx----%lx len: 0x%lx %s\n", vma->addr, vma->addr + vma->len, vma->len, perm);
    }
}