#include "utils.h"
#include "mm/vm.h"
#include "mm/mmap.h"
#include "platform.h"
#include "kernel/proc.h"

#define __MODULE_NAME__ COPY
#include "debug.h"


int memset_user(uint64_t dstva, int val, uint64_t len) {
    vma_t *vma;
    proc_t *p = current;
    if (!p)
        panic("copyout: no process ctx");

    // 1. 首先确定目标段是否存在
    if ((vma = vma_exist(p->mm, (uint64_t)dstva, len)) == NULL) {
        return -1;
    }
    // 2. 是否是用户段
    if ((vma->prot & PROT_USER) == 0) {
        return -1;
    }
    // 3. 复制

    enable_sum();

    char *dst = (char *)dstva;
    while (len > 0) {
        *dst++ = val;
        len--;
    }

    disable_sum();

    return 0;
}

int copy_to_user(uint64_t to, void *from, uint64_t len) {
    vma_t *vma;
    proc_t *p = current;
    if (!p)
        panic("copyout: no process ctx");

    // 1. 首先确定目标段是否存在
    if ((vma = vma_exist(p->mm, (uint64_t)to, len)) == NULL) {
        return -1;
    }
    // 2. 是否是用户段
    if ((vma->prot & PROT_USER) == 0) {
        return -1;
    }
    // 3. 直接拷贝 
    enable_sum();

    char *s = (char *)from;
    char *d = (char *)to;
    if (s < d && s + len > d) {
        s += len;
        d += len;
        while (len-- > 0)
            *--d = *--s;
    } else
        while (len-- > 0)
            *d++ = *s++;

    disable_sum();

    return 0;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
int copyout(uint64_t dstva, char *src, uint64_t len) {
    return copy_to_user(dstva, src, len);
}

/* We trust that kernel address is legal... */
int copy_from_user(void *to, uint64_t from, size_t n) {
    proc_t *p = current;
    if (!p)
        panic("copy_from_user: no process ctx");

    // todo: more checks, such as: guard pages, **mmap**...
    if (vma_exist(p->mm, (uint64_t)from, n) == NULL) {
        panic("not exist");
        return -1;
    }


    enable_sum();

    char *s = (char *)from;
    char *d = (char *)to;
    if (s < d && s + n > d) {
        s += n;
        d += n;
        while (n-- > 0)
            *--d = *--s;
    } else {
        while (n-- > 0)
            *d++ = *s++;
    }


    disable_sum();

    return 0;
}

int copyin(void *dst, uint64_t srcva, uint64_t len) {
    return copy_from_user(dst, srcva, len);
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
int copyinstr(char *dst, uint64_t srcva, uint64_t max) {
    int got_null = 0;
    proc_t *proc = current;
    vma_t *vma;
    if (!proc)
        panic("copyinstr: no process context");
    if ((vma = __vma_find_strict(proc->mm, srcva)) == NULL) {
        return -1;
    }
    max = min(max, vma->len - (srcva - vma->addr));

    enable_sum();
    char *p = (char *)srcva;
    // no consider wrap
    while (max > 0) {
        *dst = *p;
        if (*p == '\0') {
            got_null = 1;
            break;
        }
        max--;
        p++;
        dst++;
    }
    disable_sum();
    return (got_null ? 0 : -1);
}



// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
int either_copyin(void *dst, int user_src, uint64_t src, uint64_t len) {
    if (unlikely(len == 0))
        return 0;
    if (user_src) {
        return copyin(dst, src, len);
    } else {
        memmove(dst, (char *)src, len);
        return 0;
    }
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
int either_copyout(int user_dst, uint64_t dst, void *src, uint64_t len) {
    if (user_dst) {
        return copyout(dst, src, len);
    } else {
        memmove((char *)dst, src, len);
        return 0;
    }
}