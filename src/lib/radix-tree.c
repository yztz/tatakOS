#include "types.h"
#include "atomic/sleeplock.h"
#include "atomic/atomic.h"
#include "hlist.h"
#include "fs/fat.h"
#include "fs/stat.h"
#include "param.h"
#include "radix-tree.h"

#define RADIX_TREE_MAP_SIZE 64

struct radix_tree_node {
  uint8 height;
  uint8 count;
  void *slots[RADIX_TREE_MAP_SIZE];
  uint64 tags[RADIX_TREE_MAX_TAGS];
};