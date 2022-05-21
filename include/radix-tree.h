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

struct radix_tree_root
{
  uint8 height;
  struct radix_tree_node *rnode;
};

void *radix_tree_lookup(struct radix_tree_root *root, unsigned long index);
int radix_tree_insert(struct radix_tree_root *root, unsigned long index, void *item);
static int radix_tree_extend(struct radix_tree_root *root, unsigned long index);
#endif