#ifndef _RADIX_TREE_H
#define _RADIX_TREE_H

#include "types.h"
#include "atomic/sleeplock.h"
#include "atomic/atomic.h"
#include "fs/fat.h"

/*** radix-tree API starts here ***/

#define RADIX_TREE_MAX_TAGS 2
/*
 * Radix tree node definition.
 */
#define RADIX_TREE_MAP_SHIFT  6
#define RADIX_TREE_MAP_SIZE  (1UL << RADIX_TREE_MAP_SHIFT)
#define RADIX_TREE_MAP_MASK  (RADIX_TREE_MAP_SIZE-1)
#define RADIX_TREE_TAG_LONGS	\
	((RADIX_TREE_MAP_SIZE + BITS_PER_LONG - 1) / BITS_PER_LONG)

#define RADIX_TREE_INDEX_BITS  (8 /* CHAR_BIT */ * sizeof(unsigned long))
 /* 64 / 6上取整 = 11, 这么设置是因为页号index只能是个64bit的数 */
#define RADIX_TREE_MAX_PATH (DIV_ROUND_UP(RADIX_TREE_INDEX_BITS, \
					  RADIX_TREE_MAP_SHIFT))

/**
 * @brief 当高度为0且node != NULL，表示里面存储的是一个页指针(index 为0)，否则为一个rdt node指针。
 * 
 */
typedef struct radix_tree_root {
    unsigned int height;
    struct radix_tree_node *rnode;
    /* 因为现在只有index为0的页时直接存入rnode，所以设置了一个tags位 */
    uint64_t tags;
} radix_tree_root_t;

/**
 * @brief 为了节省空间，我们尽量不在页描述符page_t结构体中加入更多字段来表示page所对应的的物理地址，
 * 在linux 2.6中，如果有缓存，叶子节点的slot指向页描述符；我们直接在叶子节点的slot保存缓存的物理地址。
 *
 */
typedef struct radix_tree_node {
    unsigned int	count;
    /* 虽然是rdt node的指针，但是叶子指向具体的页地址 */
    struct radix_tree_node *slots[RADIX_TREE_MAP_SIZE];
    uint64_t tags[RADIX_TREE_MAX_TAGS][RADIX_TREE_TAG_LONGS];
} radix_tree_node_t;

/// @brief record the path frome root to leaf, seems only used in tag clear
typedef struct radix_tree_path {
    struct radix_tree_node *node;
    int offset;
} radix_tree_path_t;

/**
 * @brief 这是一个辅助结构体，用于函数radix_tree_lookup_tag，
 * 返回一颗rdt中所有具有tag的页的物理地址并其index，以链表的形式。
 *
 * 一次要读/写的页
 */
typedef struct rw_page {
    uint64_t pa;
    uint64_t pg_id;
    struct rw_page *next;
} rw_page_t;


/**
 * @brief 上述结构体的头，记录链表数量, 头结点和尾节点指针
 *
 */
typedef struct rw_page_list {
    rw_page_t *head;
    rw_page_t *tail;
    uint64_t nr_pages;
} rw_page_list_t;

void *radix_tree_lookup(struct radix_tree_root *root, unsigned long index);
int radix_tree_insert(struct radix_tree_root *root, unsigned long index, void *item);
static int radix_tree_extend(struct radix_tree_root *root, unsigned long index);
uint64_t radix_tree_maxindex(uint height);
static struct radix_tree_node *radix_tree_node_alloc();
void radix_tree_tag_set(radix_tree_root_t *root, uint64_t pg_id, uint tag_type);
void radix_tree_tag_clear(radix_tree_root_t *root, uint64_t pg_id, uint tag_type);
rw_page_list_t *radix_tree_find_tags(radix_tree_root_t *root, uint32_t tag, rw_page_list_t *pg_head);



/* functions defined in write-pageback.c */
// void set_pg_rdt_dirty(uint64_t pa, radix_tree_root_t *root, uint64_t pg_id, uint tag_type);
void set_pg_rdt_dirty(page_t *page, radix_tree_root_t *root, uint64_t pg_id, uint tag_type);


#endif
