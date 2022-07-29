#include "mm/trapframe.h"
#include "common.h"
#include "mm/alloc.h"

tf_t *tf_new() {
    tf_t *tf = (tf_t*)kzalloc(PGSIZE);
    if(tf == NULL)
        return NULL;
    tf->sigtf = NULL;
    return tf;
}

void tf_backup(tf_t *self) {
    tf_t *backup = (tf_t *)((uint64_t)self + sizeof(tf_t));
    *backup = *self;
    self->sigtf = backup;
}

void tf_restore(tf_t *self) {
    if(self->sigtf == NULL)
        panic("whereis tf?");
    *self = *self->sigtf;
    self->sigtf = NULL;
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