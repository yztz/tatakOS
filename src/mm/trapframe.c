#include "mm/trapframe.h"
#include "common.h"
#include "mm/alloc.h"

#define BACKUP_TF(tf) ((utf_t *)((uint64_t)tf + sizeof(utf_t)))

utf_t *tf_new(struct proc *p) {
    utf_t *tf = (utf_t*)kzalloc(PGSIZE);
    if(tf == NULL) {
        return NULL;
    }
    
    tf->proc = (uint64_t)p;
    tf->sigtf = NULL;
    return tf;
}


utf_t *tf_new_clone(struct proc *p, utf_t *old) {
    utf_t *tf = (utf_t*)kzalloc(PGSIZE);
    if(tf == NULL) {
        return NULL;
    }

    // inlcudes backup
    memmove(tf, old, 2 * sizeof(utf_t));

    tf->proc = (uint64_t)p;
    
    if (tf->sigtf) {
        tf->sigtf = BACKUP_TF(tf);
        tf->sigtf->proc = (uint64_t)p;
    }

    return tf;
}

void tf_backup(utf_t *self) {
    utf_t *backup = BACKUP_TF(self);
    *backup = *self;
    self->sigtf = backup;
}

void tf_restore(utf_t *self) {
    if(self->sigtf == NULL)
        panic("whereis tf?");
    *self = *self->sigtf;
    self->sigtf = NULL;
}

void tf_reset(utf_t *self, uint64_t pc, uint64_t sp) {
    uint64_t proc = self->proc;
    memset(self, 0, sizeof(utf_t));
    self->proc = proc;
    self->sp = sp;
    self->epc = pc;
}

void tf_free(utf_t **pptf) {
    kfree_safe(pptf);
}


void tf_flstore(utf_t *self) {
    __asm__ __volatile__ (
        "fsd f0,296(%0)\n\t"
        "fsd f1,304(%0)\n\t"
        "fsd f2,312(%0)\n\t"
        "fsd f3,320(%0)\n\t"
        "fsd f4,328(%0)\n\t"
        "fsd f5,336(%0)\n\t"
        "fsd f6,344(%0)\n\t"
        "fsd f7,352(%0)\n\t"
        "fsd f8,360(%0)\n\t"
        "fsd f9,368(%0)\n\t"
        "fsd f10,376(%0)\n\t"
        "fsd f11,384(%0)\n\t"
        "fsd f12,392(%0)\n\t"
        "fsd f13,400(%0)\n\t"
        "fsd f14,408(%0)\n\t"
        "fsd f15,416(%0)\n\t"
        "fsd f16,424(%0)\n\t"
        "fsd f17,432(%0)\n\t"
        "fsd f18,440(%0)\n\t"
        "fsd f19,448(%0)\n\t"
        "fsd f20,456(%0)\n\t"
        "fsd f21,464(%0)\n\t"
        "fsd f22,472(%0)\n\t"
        "fsd f23,480(%0)\n\t"
        "fsd f24,488(%0)\n\t"
        "fsd f25,496(%0)\n\t"
        "fsd f26,504(%0)\n\t"
        "fsd f27,512(%0)\n\t"
        "fsd f28,520(%0)\n\t"
        "fsd f29,528(%0)\n\t"
        "fsd f30,536(%0)\n\t"
        "fsd f31,544(%0)\n\t"
        "frcsr t0\n\t"
        "sd t0,552(%0)\n\t"
        :
        : "r"(self)
        : "t0"
    );
}
void tf_flrestore(utf_t *self) {
    __asm__ __volatile__ (
        "fld f0,296(%0)\n\t"
        "fld f1,304(%0)\n\t"
        "fld f2,312(%0)\n\t"
        "fld f3,320(%0)\n\t"
        "fld f4,328(%0)\n\t"
        "fld f5,336(%0)\n\t"
        "fld f6,344(%0)\n\t"
        "fld f7,352(%0)\n\t"
        "fld f8,360(%0)\n\t"
        "fld f9,368(%0)\n\t"
        "fld f10,376(%0)\n\t"
        "fld f11,384(%0)\n\t"
        "fld f12,392(%0)\n\t"
        "fld f13,400(%0)\n\t"
        "fld f14,408(%0)\n\t"
        "fld f15,416(%0)\n\t"
        "fld f16,424(%0)\n\t"
        "fld f17,432(%0)\n\t"
        "fld f18,440(%0)\n\t"
        "fld f19,448(%0)\n\t"
        "fld f20,456(%0)\n\t"
        "fld f21,464(%0)\n\t"
        "fld f22,472(%0)\n\t"
        "fld f23,480(%0)\n\t"
        "fld f24,488(%0)\n\t"
        "fld f25,496(%0)\n\t"
        "fld f26,504(%0)\n\t"
        "fld f27,512(%0)\n\t"
        "fld f28,520(%0)\n\t"
        "fld f29,528(%0)\n\t"
        "fld f30,536(%0)\n\t"
        "fld f31,544(%0)\n\t"
        "ld t0,552(%0)\n\t"
        "fscsr t0\n\t"
        :
        : "r"(self)
        : "t0"
    );
}


void tf_print(utf_t *tf) {
    kprintf("Trapframe {\n");
    kprintf("    sp: %lx\n", tf->sp);
    kprintf("    fp: %lx\n", tf->s0);
    kprintf("    pc: %lx\n", tf->epc);
    kprintf("    ra: %lx\n", tf->ra);
    kprintf("    a0: %lx\n", tf->a0);
    kprintf("    a1: %lx\n", tf->a1);
    kprintf("    a2: %lx\n", tf->a2);
    kprintf("    a3: %lx\n", tf->a3);
    kprintf("    a4: %lx\n", tf->a4);
    kprintf("    a5: %lx\n", tf->a5);
    kprintf("    s3: %lx\n", tf->s3);
    kprintf("}\n");
}