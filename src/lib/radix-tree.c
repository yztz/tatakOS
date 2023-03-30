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
#include "fs/fcntl.h"
#include "fs/file.h"
#include "fs/fs.h"
#include "fs/stat.h"
#include "fs/mpage.h"
#include "kernel/proc.h"
#include "mm/vm.h"
#include "riscv.h"
#include "utils.h"
#include "radix-tree.h"
#include "bitops.h"

#define __MODULE_NAME__ RADIX_TREE
#include "debug.h"


static struct radix_tree_node *
radix_tree_node_alloc(){
	struct radix_tree_node *ret;
	ret = kzalloc(sizeof(struct radix_tree_node));
	if(ret == NULL)
		panic("rdt alloc");
	
	/* 这里需要初始化一下内容，否则似乎里面不为0 */
	ret->count = 0;
	for(int i = 0; i < RADIX_TREE_MAP_SIZE; i++)
		ret->slots[i] = NULL;
	return ret;
}

static inline void radix_tree_node_free(struct radix_tree_node *node){
	kfree((void *)node);
}

static inline int root_tag_get(radix_tree_root_t *root, uint tag){
	return test_bit(tag, &root->tags);
}


static inline void root_tag_set(radix_tree_root_t *root, uint tag){
	return set_bit(tag, &root->tags);
}

static inline void root_tag_clear(radix_tree_root_t *root, uint tag){
	return clear_bit(tag, &root->tags);
}

static inline void tag_set(radix_tree_node_t *node, uint32_t tag_type, int offset){
	set_bit(offset, node->tags[tag_type]);
}

static inline void tag_clear(struct radix_tree_node *node, unsigned int tag,
		int offset)
{
	clear_bit(offset, node->tags[tag]);
}

static inline int tag_get(struct radix_tree_node *node, unsigned int tag, int offset)
{
	return test_bit(offset, node->tags[tag]);
}

/*
 * Returns 1 if any slot in the node has this tag set.
 * Otherwise returns 0.
 */
static inline int any_tag_set(struct radix_tree_node *node, unsigned int tag)
{
	int idx;
	for (idx = 0; idx < RADIX_TREE_TAG_LONGS; idx++) {
		if (node->tags[tag][idx])
			return 1;
	}
	return 0;
}

uint64_t
radix_tree_maxindex(uint height){
	if(height > RADIX_TREE_MAX_PATH)
		ER();
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
	/* 当要查的页号大于当前rdt所能表示的最大页号，返回null，在写时可能会出现 */
	if (index > radix_tree_maxindex(height))
		return NULL;

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

  /* we get a pointer to struct page or null. */
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
	uint32_t height, shift;
	// int error;


	/* make sure the tree is high enough */
	if(index > radix_tree_maxindex(root->height)){
		if(radix_tree_extend(root, index))	
			ER();
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
	/* slot should be null 当打印之后，*slot就不为0了，很奇怪, 莫非分配节点时应该把指针初始化为NULL？*/
	if(*slot != NULL)
		panic("rdt insert 3");
	if(node)
		node->count++;

	*slot = item;	
	
	return 0;
}


// /**
//  * @brief Set the extend node tags object
//  * the child is parent->slots[0] when extend a tree.
//  * 
//  * @param parent 
//  * @param child 
//  */
// static void set_extend_node_tags(radix_tree_node_t *parent, radix_tree_node_t *child){
// 	for(int i = 0; i < RADIX_TREE_MAX_TAGS; i++){
// 		if(any_tag_set(child, i))
// 			parent->tags[i][0] = 1;
// 	}
// }
// static void set_extend_node_tags(radix_tree_node_t *parent, radix_tree_node_t *child){
// 	for(int i = 0; i < RADIX_TREE_MAX_TAGS; i++){
// 		for(int j = 0; j < RADIX_TREE_TAG_LONGS; j++){
// 			if(child->tags[i][j] > 0){
// 				parent->tags[i][0] = 1;
// 				break;
// 			}
// 		}
// 	}	
// }


/*
 *	Extend a radix tree so it can store key @index.
 */
static int radix_tree_extend(struct radix_tree_root *root, unsigned long index)
{
	struct radix_tree_node *node;
	unsigned int height;
	int tag;

	/* Figure out what the height should be.  */
	height = root->height + 1;
	while (index > radix_tree_maxindex(height))
		height++;

	/* 如果root->rnode为NULL，在插入函数的while循环中会分配 */
	if(root->rnode == NULL){
		root->height = height;
		goto out;
	}
	
	do {
		if (!(node = radix_tree_node_alloc()))
			ER();

		/* Increase the height.  */
		node->slots[0] = root->rnode;

		/* Propagate the aggregated tag info into the new root */
		for (tag = 0; tag < RADIX_TREE_MAX_TAGS; tag++) {
			if (root_tag_get(root, tag))
				tag_set(node, tag, 0);
		}

		node->count = 1;
		root->rnode = node;
		root->height++;
	} while (height > root->height);
	

out:
	return 0;
}



void radix_tree_tag_set(radix_tree_root_t *root, uint64_t pg_id, uint tag_type){
	uint height, shift;
	radix_tree_node_t *slot;

	height = root->height;
	if(pg_id > radix_tree_maxindex(height))
		panic("rdt tag set");

	shift = (height-1)*RADIX_TREE_MAP_SHIFT;
	slot = root->rnode;
	
	while (height > 0)
	{
		int slot_num;
		if(slot == NULL)
			panic("rdt tag set");
			// return;
		
		slot_num = (pg_id >> shift) & RADIX_TREE_MAP_MASK;
		tag_set(slot, tag_type, slot_num);
		slot = (radix_tree_node_t *)(slot->slots[slot_num]);
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}

	/* the index of page to set is empty */
	// if(slot == NULL)
		// panic("rdt tag set 2");
	if(slot && !root_tag_get(root, tag_type))
		root_tag_set(root, tag_type);

	return;
}


void radix_tree_tag_clear(radix_tree_root_t *root, uint64_t pg_id, uint tag_type){
	/*
	 * The radix tree path needs to be one longer than the maximum path
	 * since the "list" is null terminated.
	 * path[0]作为结束符？
	 */
	radix_tree_path_t path[RADIX_TREE_MAX_PATH + 1], *pathp = path;
	radix_tree_node_t *slot = NULL;
	uint32_t height, shift;

	height = root->height;
	if(pg_id > radix_tree_maxindex(height))
		panic("rdt tag clear");

	shift = (height - 1) * RADIX_TREE_MAP_SHIFT;
	pathp->node = NULL;
	slot = root->rnode;

	while (height > 0)
	{
		int offset;

		if(slot == NULL)
			goto out;
		
		offset = (pg_id >> shift) & RADIX_TREE_MAP_MASK;
		pathp[1].offset = offset;
		pathp[1].node = slot;

		slot = slot->slots[offset];
		pathp++;
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}

	if (slot == NULL)
	{
		goto out;
	}
	
	while(pathp->node) {
		if(!tag_get(pathp->node, tag_type, pathp->offset))
			goto out;
		tag_clear(pathp->node, tag_type, pathp->offset);
		if(any_tag_set(pathp->node, tag_type))
			goto out;
		pathp--;
	}

	/* clear the root's tag bit */
	if (root_tag_get(root, tag_type))
		root_tag_clear(root, tag_type);

out:
	return;
}

/**
 * @brief recursive lookup tagged page
 * 
 */
void lookup_tag(radix_tree_node_t *node, uint32_t tag, rw_page_list_t *pg_list, int height, uint64_t pg_id_base){
	int i, shift;
	shift = (height-1)*RADIX_TREE_MAP_SHIFT;
	page_t *ppage;

	if(height < 1)
		ER();

	for(i=0; i<RADIX_TREE_MAP_SIZE; i++){
		if(tag_get(node, tag, i)){
			if(height == 1){
				rw_page_t *page = kzalloc(sizeof(rw_page_t));

				ppage = (page_t *)node->slots[i];
				page->pa = PG_TO_ADDR(ppage);
				page->pg_id = pg_id_base + i;

				if(pg_list->head == NULL && pg_list->tail == NULL){
					pg_list->head = page;
					pg_list->tail = page;
				}
				else{
					pg_list->tail->next = page;
					pg_list->tail = page;
				}

				pg_list->nr_pages++;
			}
			else{

				lookup_tag((radix_tree_node_t *)node->slots[i], tag, pg_list, height - 1, pg_id_base + i * (1<<shift));
			}
		}
	}
}



/**
 * @brief find all pages with tag in the rdt
 */
rw_page_list_t *
radix_tree_find_tags(radix_tree_root_t *root, uint32_t tag, rw_page_list_t *pg_list){
	if(root->height == 0){
		rw_page_t *page = kzalloc(sizeof(rw_page_t));
		page_t *ppage = (page_t *)root->rnode;
		page->pa = PG_TO_ADDR(ppage);
		page->pg_id = 0;

		pg_list->head = page;
		pg_list->tail = page;
	}
	else
		lookup_tag(root->rnode, tag, pg_list, root->height, 0);
	return pg_list;
}

/**
 *	radix_tree_shrink    -    shrink height of a radix tree to minimal
 *	@root		radix tree root
 *  可用更少的节点表示rdt，删去多余的节点。
 */
static inline void radix_tree_shrink(struct radix_tree_root *root)
{
	/* try to shrink tree height */
	while (root->height > 0) {
		struct radix_tree_node *to_free = root->rnode;
		void *newptr;

		/*
		 * The candidate node has more than one child, or its child
		 * is not at the leftmost slot, we cannot shrink.
		 */
		if (to_free->count != 1)
			break;
		if (!to_free->slots[0])
			break;

		newptr = to_free->slots[0];
		root->rnode = newptr;
		root->height--;
		radix_tree_node_free(to_free);
	}
}

/**
 *	radix_tree_delete    -    delete an item from a radix tree
 *	@root:		radix tree root
 *	@index:		index key
 *
 *	Remove the item at @index from the radix tree rooted at @root.
 *
 *	Returns the address of the deleted item, or NULL if it was not present.
 *  删除节点，并适当缩小树。
 */
void *radix_tree_delete(struct radix_tree_root *root, unsigned long index)
{
	struct radix_tree_path path[RADIX_TREE_MAX_PATH + 1], *pathp = path;
	struct radix_tree_node *slot = NULL;
	struct radix_tree_node *to_free;
	unsigned int height, shift;
	int tag;
	int offset;

	height = root->height;
	if(index > radix_tree_maxindex(height))
		goto out;
	
	slot = root->rnode;
	if(height == 0){
		root->rnode = NULL;
		goto out;
	}

	shift = (height - 1) * RADIX_TREE_MAP_SHIFT;
	pathp->node = NULL;

	/* path[0] 不使用 */
	do{
		if(slot == NULL)
			goto out;

		pathp++;
		offset = (index >> shift) & RADIX_TREE_MAP_MASK;
		pathp->offset = offset;
		pathp->node = slot;
		slot = slot->slots[offset];
		shift -= RADIX_TREE_MAP_SHIFT;
		height--;
	}while(height > 0);

	if(slot == NULL)
		goto out;
	
	/*
	 * Clear all tags associated with the just-deleted item
	 */
	for (tag = 0; tag < RADIX_TREE_MAX_TAGS; tag++) {
		if (tag_get(pathp->node, tag, pathp->offset))
			radix_tree_tag_clear(root, index, tag);
	}

	to_free = NULL;
	/* Now free the nodes we do not need anymore */
	while(pathp->node){
		pathp->node->slots[pathp->offset] = NULL;
		pathp->node->count--;
		/*
		 * Queue the node for deferred freeing after the
		 * last reference to it disappears (set NULL, above).
		 */
		if (to_free)
			radix_tree_node_free(to_free);
		if(pathp->node->count) {
			if(pathp->node == root->rnode)
				radix_tree_shrink(root);
			goto out;
		}

		to_free = pathp->node;
		pathp--;
	}

	root->height = 0;
	root->rnode = NULL;
	if (to_free)
		radix_tree_node_free(to_free);

out:
	return slot;
}