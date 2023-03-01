
#ifndef _LINUX_RMAP_H
#define _LINUX_RMAP_H

#include "spinlock.h"
#include "list.h"
#include "types.h"

#ifdef RMAP
/* 参考了linux 2.6.0的reverse mappping方案，这个方案感觉十分占用内存 */

#define NRPTE 2^2-1

/*
 * next_and_idx encodes both the address of the next pte_chain and the
 * offset of the highest-index used pte in ptes[].
 * next和index指针复用，因为NRPTE占两位，所以要保证next不会用到最后2bit。slob分配的地址有对齐...
 * ptes从NRPTE-1到0，倒着放进入，idx表示下一个放置的位置加上1，数值上等于当前pte_chain还可以存放的数量。
 */
struct pte_chain {
	uint64_t next_and_idx;
	pte_addr_t ptes[NRPTE];
	/* 下面两个filed供回收页的时候清除tlb使用， 不是必须的，记录一个pte属于哪一个pagetable的哪一个地址va，
		可以更加精确的清除tlb，不用为此刷掉整个tlb，但是同时也增加了内存开销。 */
	/* 所以另一个方案是，清除一个项时，遍历当前页表，判断这个映射是不是属于当前页表，如果是，对应哪个虚拟地址？清除对应的tlb */
	// uint64_t pagetable[NRPTE]; 
	// uint64_t	va[NRPTE];
};

typedef struct pte_chain pte_chain_t;


#endif	/* _LINUX_RMAP_H */

#endif