#include "mm/trapframe.h"
#include "common.h"
#include "mm/alloc.h"

tf_t *tf_new() {
    return (tf_t*)kzalloc(PGSIZE);
}

void tf_free(tf_t **pptf) {
    kfree_safe(pptf);
}


void tf_print(tf_t *tf) {
    printf("trapframe {\n");
    printf("    sp: %lx\n", tf->sp);
    printf("    fp: %lx\n", tf->s0);
    printf("    pc: %lx\n", tf->epc);
    printf("    ra: %lx\n", tf->ra);
    printf("}\n");
}