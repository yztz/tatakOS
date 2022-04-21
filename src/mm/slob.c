#include "atomic/spinlock.h"
#include "common.h"

typedef struct slob_block {
	uint64 units;
	struct slob_block *next;
} slob_t;

#define SLOB_UNIT sizeof(slob_t) // 16bit
#define SLOB_UNITS(size) (((size) + SLOB_UNIT - 1)/SLOB_UNIT)
// #define SLOB_ALIGN L1_CACHE_BYTES

typedef struct bigblock {
	int order;
	void *pages;
	struct bigblock *next;
} bigblock_t;

static slob_t arena = { .next = &arena, .units = 1 };
static slob_t *slobfree = &arena;
static bigblock_t *bigblocks;

static struct spinlock slob_lock;
static struct spinlock block_lock;

void slob_init(void) {
    initlock(&slob_lock, "slob_lock");
    initlock(&block_lock, "block_lock");
}

static void slob_free(void *b, int size);

static void *slob_alloc(size_t size, int align)
{
	slob_t *prev, *cur, *aligned = 0;
	int delta = 0, units = SLOB_UNITS(size); // 计算需要多少单元（单元对齐）

	acquire(&slob_lock);
	prev = slobfree; // 链表头，起始只是一个占一个单元的头结点 
    // 遍历slob链表
	for (cur = prev->next; ; prev = cur, cur = cur->next) {
		if (align) { // 要求对齐？
			aligned = (slob_t *)ALIGN((unsigned long)cur, align); // 向上对齐
			delta = aligned - cur;
		}
		if (cur->units >= units + delta) { /* room enough? */
			if (delta) { /* need to fragment head to align? */
				aligned->units = cur->units - delta; // 将对齐后的unit的units设置为对齐前的units-delta个unit
				aligned->next = cur->next;
				cur->next = aligned;
				cur->units = delta;
				prev = cur;
				cur = aligned;
			}

			if (cur->units == units) /* exact fit? */
				prev->next = cur->next; /* unlink */
			else { /* fragment */
				prev->next = cur + units;
				prev->next->units = cur->units - units;
				prev->next->next = cur->next;
				cur->units = units;
			}

			slobfree = prev;
			spin_unlock_irqrestore(&slob_lock, flags);
			return cur;
		}
		if (cur == slobfree) { // 没有合适的空间
			spin_unlock_irqrestore(&slob_lock, flags);

			if (size == PAGE_SIZE) /* trying to shrink arena? */
				return 0;

			cur = (slob_t *)__get_free_page(gfp);
			if (!cur)
				return 0;

			slob_free(cur, PAGE_SIZE);
			spin_lock_irqsave(&slob_lock, flags);
			cur = slobfree;
		}
	}
}

static void slob_free(void *block, int size)
{
	slob_t *cur, *b = (slob_t *)block;
	unsigned long flags;

	if (!block)
		return;

	if (size)
		b->units = SLOB_UNITS(size);

	/* Find reinsertion point */
	spin_lock_irqsave(&slob_lock, flags);
	for (cur = slobfree; !(b > cur && b < cur->next); cur = cur->next)
		if (cur >= cur->next && (b > cur || b < cur->next))
			break;

	if (b + b->units == cur->next) {
		b->units += cur->next->units;
		b->next = cur->next->next;
	} else
		b->next = cur->next;

	if (cur + cur->units == b) {
		cur->units += b->units;
		cur->next = b->next;
	} else
		cur->next = b;

	slobfree = cur;

	spin_unlock_irqrestore(&slob_lock, flags);
}

static int FASTCALL(find_order(int size));
static int fastcall find_order(int size)
{
	int order = 0;
	for ( ; size > 4096 ; size >>=1)
		order++;
	return order;
}

void *kmalloc(size_t size, gfp_t gfp)
{
	slob_t *m;
	bigblock_t *bb;
	unsigned long flags;

	if (size < PAGE_SIZE - SLOB_UNIT) { // 如果size小于一页
		m = slob_alloc(size + SLOB_UNIT, gfp, 0);
		return m ? (void *)(m + 1) : 0;
	}

	bb = slob_alloc(sizeof(bigblock_t), gfp, 0); // 申请大块？
	if (!bb)
		return 0;

	bb->order = find_order(size);
	bb->pages = (void *)__get_free_pages(gfp, bb->order);

	if (bb->pages) {
		spin_lock_irqsave(&block_lock, flags);
		bb->next = bigblocks;
		bigblocks = bb;
		spin_unlock_irqrestore(&block_lock, flags);
		return bb->pages;
	}

	slob_free(bb, sizeof(bigblock_t));
	return 0;
}

EXPORT_SYMBOL(kmalloc);

void *kzalloc(size_t size, gfp_t flags)
{
	void *ret = kmalloc(size, flags);
	if (ret)
		memset(ret, 0, size);
	return ret;
}

EXPORT_SYMBOL(kzalloc);

void kfree(const void *block)
{
	bigblock_t *bb, **last = &bigblocks;
	unsigned long flags;

	if (!block)
		return;

	if (!((unsigned int)block & (PAGE_SIZE-1))) {
		/* might be on the big block list */
		spin_lock_irqsave(&block_lock, flags);
		for (bb = bigblocks; bb; last = &bb->next, bb = bb->next) {
			if (bb->pages == block) {
				*last = bb->next;
				spin_unlock_irqrestore(&block_lock, flags);
				free_pages((unsigned long)block, bb->order);
				slob_free(bb, sizeof(bigblock_t));
				return;
			}
		}
		spin_unlock_irqrestore(&block_lock, flags);
	}

	slob_free((slob_t *)block - 1, 0);
	return;
}