#ifndef _LINUX_RMAP_H
#define _LINUX_RMAP_H

#include "spinlock.h"
#include "list.h"
#include "types.h"

/* 参考了linux 2.6.0的reverse mappping方案 */


#define NRPTE 2^2-1
/*
 * next_and_idx encodes both the address of the next pte_chain and the
 * offset of the highest-index used pte in ptes[].
 */
struct pte_chain {
	unsigned long next_and_idx;
	pte_addr_t ptes[NRPTE];
	// void *mm_struct[NRPTE]; //mm_struct
	// uint64_t	va[NRPTE];
};

typedef struct pte_chain pte_chain_t;

static inline struct mm_struct * ptep_to_mm(pte_t * ptep){
	PTE2PA();
}
#endif	/* _LINUX_RMAP_H */