// #include "types.h"
// #include "atomic/sleeplock.h"
// #include "atomic/atomic.h"
// #include "fs/fat.h"
// #include "fs/stat.h"
// #include "param.h"
// #include "radix-tree.h"
// #include "mm/alloc.h"

// #include "debug.h"
#include "atomic/sleeplock.h"
#include "atomic/spinlock.h"
#include "defs.h"
#include "fs/fcntl.h"
#include "fs/file.h"
#include "fs/fs.h"
#include "fs/stat.h"
#include "kernel/proc.h"
#include "mm/vm.h"
#include "param.h"
#include "riscv.h"
#include "types.h"
#include "debug.h"
#include "utils.h"
#include "mm/mm.h"
#include "radix-tree.h"
#include "debug.h"

//基数树(radix tree, rdt)相关

// struct radix_tree_node {
//   uint8 height;
//   uint8 count;
//   void *slots[RADIX_TREE_MAP_SIZE];
//   uint64 tags[RADIX_TREE_MAX_TAGS];
// };



/** 根据rdt的高度返回其最大的id值。注意height为0，最大id不为0，
 * id为0，则至少有一个节点，高度至少为1。所以
 * return (1<<(height * RADIX_TREE_MAP_SHIFT)) - 1;
 * 的表达是错的(要排除掉height为0的情况）。前面使用这个表达式，把pa存在了rdt root指向node的指针中
 * 歪打正着也能读出来。
 * 如果height为0选择返回一个负值，还要考虑有符号数和无符号数比较时的转化，所以我们选择特殊处理height=0
 * 的情况。
 */
uint64
radix_tree_maxindex(uint height){
	if(height == 0)
		return 0;
		// panic("rdt maxindex: height is 0!");
		// return -1;
  return (1<<(height * RADIX_TREE_MAP_SHIFT)) - 1;
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

	// printf(rd("index: %d height: %d\n"), index, root->height);
	
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

/**
 *	radix_tree_insert    -    insert into a radix tree
 *	@root:		radix tree root
 *	@index:		index key
 *	@item:		item to insert
 *
 *	Insert an item into the radix tree at position @index.
 */
int radix_tree_insert(struct radix_tree_root *root, unsigned long index, void *item)
{
	struct radix_tree_node *node = NULL, *tmp, **slot;
	uint32 height, shift;
	// int error;

	// printf(rd("insert0: index: %d height: %d\n"), index, root->height);

	// printf(rd("insert: index: %d maxindex: %d\n"), index, radix_tree_maxindex(root->height));
	/* make sure the tree is high enough*/
	if(root->height == 0 || index > radix_tree_maxindex(root->height)){
	// printf(rd("insert1: index: %d height: %d\n"), index, root->height);
		if(radix_tree_extend(root, index))	
			panic("rdt insert 1");
	}
	// printf(rd("insert2: index: %d height: %d\n"), index, root->height);
	slot = &root->rnode;
	height = root->height;
	shift = (height-1) * RADIX_TREE_MAP_SHIFT;	

	while (height > 0)
	{
		if(*slot == NULL){
			/* add a child node */
			if(!(tmp = radix_tree_node_alloc())){
				panic("rdt insert 2");
			}
			*slot = tmp;
			if(node)
				node->count++;
		}

		/* Go a level down. */
		node = *slot;
		slot = (struct radix_tree_node **)
						(node->slots + ((index >> shift) & RADIX_TREE_MAP_MASK));
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}
	/* slot should be null*/
	if(*slot != NULL)
		panic("rdt insert 3");
	if(node)
		node->count++;

	*slot = item;	
	
	return 0;
}

/*
 *	Extend a radix tree so it can store key @index.
 */
static int radix_tree_extend(struct radix_tree_root *root, unsigned long index)
{
	struct radix_tree_node *node;
	unsigned int height;

	/* Figure out what the height should be.  */
	height = root->height + 1;
	while (height == 0 || index > radix_tree_maxindex(height))
		height++;
	
	if (root->rnode) {
		do {
			if (!(node = radix_tree_node_alloc()))
				panic("rdt extend");
				// return -ENOMEM;

			/* Increase the height.  */
			node->slots[0] = root->rnode;
			node->count = 1;
			root->rnode = node;
			root->height++;
		} while (height > root->height);
	} else 
		root->height = height;

	return 0;
}

static struct radix_tree_node *
radix_tree_node_alloc(){
	struct radix_tree_node *ret;
	ret = kmalloc(sizeof(struct radix_tree_node));
	if(ret == NULL)
		panic("rdt alloc");
	return ret;
}