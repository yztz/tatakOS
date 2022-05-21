#include "types.h"
#include "atomic/sleeplock.h"
#include "atomic/atomic.h"
#include "hlist.h"
#include "fs/fat.h"
#include "fs/stat.h"
#include "param.h"
#include "radix-tree.h"
#include "mm/alloc.h"

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

/**
 * @brief 为了节省空间，我们尽量不在页描述符page_t结构体中加入更多字段来表示page所对应的的物理地址，
 * 在linux中，如果有缓存，叶子节点的slot指向页描述符；我们直接在叶子节点的slot保存缓存的物理地址。
 * 
 */
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
	int error;

	/* make sure the tree is high enough*/
	if(index > radix_tree_maxindex(root->height)){
		if(radix_tree_extend(root, index))	
			panic("rdt insert 1");
	}
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
	while (index > radix_tree_maxindex(height))
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