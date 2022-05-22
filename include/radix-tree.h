#ifndef _RADIX_TREE_H
#define _RADIX_TREE_H

#include "types.h"
#include "atomic/sleeplock.h"
#include "atomic/atomic.h"
#include "hlist.h"
#include "fs/fat.h"
#include "fs/stat.h"
#include "param.h"

/*** radix-tree API starts here ***/

#define RADIX_TREE_MAX_TAGS 2
/*
 * Radix tree node definition.
 */
#define RADIX_TREE_MAP_SHIFT  6
#define RADIX_TREE_MAP_SIZE  (1UL << RADIX_TREE_MAP_SHIFT)
#define RADIX_TREE_MAP_MASK  (RADIX_TREE_MAP_SIZE-1)


struct radix_tree_root
{
  uint8 height;
  struct radix_tree_node *rnode;
};

/**
 * @brief 为了节省空间，我们尽量不在页描述符page_t结构体中加入更多字段来表示page所对应的的物理地址，
 * 在linux 2.6中，如果有缓存，叶子节点的slot指向页描述符；我们直接在叶子节点的slot保存缓存的物理地址。
 * 
 */
struct radix_tree_node {
	unsigned int	count;
	void		*slots[RADIX_TREE_MAP_SIZE];
};

void *radix_tree_lookup(struct radix_tree_root *root, unsigned long index);
int radix_tree_insert(struct radix_tree_root *root, unsigned long index, void *item);
static int radix_tree_extend(struct radix_tree_root *root, unsigned long index);

uint64
radix_tree_maxindex(uint height);

static struct radix_tree_node *
radix_tree_node_alloc();
#endif