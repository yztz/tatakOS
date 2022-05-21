#include "types.h"
#include "atomic/sleeplock.h"
#include "atomic/atomic.h"
#include "hlist.h"
#include "fs/fat.h"
#include "fs/stat.h"
#include "param.h"
#include "radix-tree.h"

//基数树(radix tree, rdt)相关

/*
 * Radix tree node definition.
 */
#define RADIX_TREE_MAP_SHIFT  6
#define RADIX_TREE_MAP_SIZE  (1UL << RADIX_TREE_MAP_SHIFT)
#define RADIX_TREE_MAP_MASK  (RADIX_TREE_MAP_SIZE-1)

// struct radix_tree_node {
//   uint8 height;
//   uint8 count;
//   void *slots[RADIX_TREE_MAP_SIZE];
//   uint64 tags[RADIX_TREE_MAX_TAGS];
// };

struct radix_tree_node {
	unsigned int	count;
	void		*slots[RADIX_TREE_MAP_SIZE];
};

//根据rdt的高度返回其最大的id值。
uint64 
radix_tree_maxindex(uint height){
  return 1<<(height * RADIX_TREE_MAP_SHIFT) - 1;
}
/**
 *	radix_tree_lookup    -    perform lookup operation on a radix tree
 *	@root:		radix tree root
 *	@index:		index key
 *
 *	Lookup them item at the position @index in the radix tree @root.
 */
void *radix_tree_lookup(struct radix_tree_root *root, unsigned long index)
{
	unsigned int height, shift;
	struct radix_tree_node **slot;

	height = root->height;
	if (index > radix_tree_maxindex(height))
    panic("radix tree lookup 1");
		// return NULL;

	shift = (height-1) * RADIX_TREE_MAP_SHIFT;
	slot = &root->rnode;

	while (height > 0) {
		if (*slot == NULL)
			return NULL;

		slot = (struct radix_tree_node **)
			((*slot)->slots + ((index >> shift) & RADIX_TREE_MAP_MASK));
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}

  // we get a pointer to struct page or null.
	return (void *) *slot;
}