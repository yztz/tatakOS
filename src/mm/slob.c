#include "atomic/spinlock.h"
#include "common.h"
#include "mm/page.h"

#define QUIET
#define __MODULE_NAME__ SLOB
#include "debug.h"

/* This value specifies the max number of pages that slob could hold. */
#define MAX_KEEP_HOLD_PAGE 16

// 16bits for an unit
typedef int16_t slobidx_t;

typedef struct slob_block {
	/*
	< 0 means only one page available
    > 0 means next block records offset and this block records units num
	*/
	slobidx_t units;
} slob_t;

// two units for page desc
typedef struct slob_page { 	
	slobidx_t units;
	slob_t *freelist;
	struct slob_page *next, *pre;
} sp_t;

extern void *__alloc_pages(int pgnum);

static atomic_t page_cnt = INIT_ATOMIC();

static inline void *__alloc_one_page() {

	debug("alloc new page");
	void *new = __alloc_pages(1);
    if (new) {
        atomic_inc(&page_cnt);
    }
	return new;
}

static inline void __free_one_page(void *addr) {
	debug("page freed");
    atomic_dec(&page_cnt);
	put_page((uint64_t)addr);
}


#define SLOB_UNIT sizeof(slob_t) // 16bit
#define SLOB_UNITS(size) (((size) + SLOB_UNIT - 1)/SLOB_UNIT)
#define SMALL_BREAK 256
#define LARGE_BREAK 1024
// In theory, ((4096 - 32) + 2 - 1) / 2 = 2032 units per page, 2048 in all.
#define SLOB_PAGE_UNITS SLOB_UNITS(PGSIZE - sizeof(struct slob_page))

#define SLOB_INIT_LIST(name) \
	sp_t name = {.units = 0, .next = &name, .pre = &name};

SLOB_INIT_LIST(small_list);
SLOB_INIT_LIST(medium_list);
SLOB_INIT_LIST(large_list);

static struct spinlock slob_lock;

void slob_init() {
    initlock(&slob_lock, "slob_lock");
}

static inline void set_slob(slob_t *s, slobidx_t size, slob_t *next) {
	slob_t *base = (slob_t *)((uint64_t)s & PGMASK);
	slobidx_t offset = next - base;

	if (size > 1) {
		s[0].units = size;
		s[1].units = offset;
	} else
		s[0].units = -offset;
}

static inline slob_t *slob_next(slob_t *s) {
	slob_t *base = (slob_t *)((uint64_t)s & PGMASK);
	slobidx_t next;

	if (s[0].units < 0)
		next = -s[0].units;
	else
		next = s[1].units;
	return base+next;
}

static inline slobidx_t slob_units(slob_t *s) {
	return s->units > 0 ? s->units : 1;
}

static int slob_last(slob_t *s) {
	return !((uint64_t)slob_next(s) & ~PGMASK);
}

static void print_slob(sp_t *sp) {
	// #ifdef __log__slob
	slob_t *b = sp->freelist;
	debug("sp@%p slob_page units: %d", sp, sp->units);
	if(b) {
		debug("slob@%p {units: %d, next: %p}", b, slob_units(b), slob_next(b));
		while(!slob_last(b)) {
			b = slob_next(b);
			debug("slob@%p {units: %d, next: %p}", b, slob_units(b), slob_next(b));
		}
	}
	// #endif
}

static void *slob_page_alloc(sp_t *sp, size_t size, size_t align_offset, int align) {
	slob_t *prev, *cur, *aligned = 0;
	int delta = 0, units = SLOB_UNITS(size);
	debug("size: %d request units: %d", size, units);
    // iterate slob list
	for (prev = NULL, cur = sp->freelist; ; prev = cur, cur = slob_next(cur)) {
		slobidx_t avail = slob_units(cur);

		if (align) { // 要求对齐？
			/* 	align_offset 是额外的对齐开销
				slob将align_offset用于存储对象size
				这里将cur+align_offset先做对齐然后再减去align_offset后做为对齐单元
				外部将返回单元+align_offset后，得到的对象将是符合对齐要求的
			*/
			aligned = (slob_t *)(ALIGN((uint64_t)cur + align_offset, align) - align_offset); // 向上对齐
			delta = aligned - cur;
		}
		if (avail >= units + delta) { /* room enough? */
			slob_t *next;

			if (delta) { /* need to fragment head to align? */
				next = slob_next(cur);
				set_slob(aligned, avail - delta, next);
				set_slob(cur, delta, aligned);
				prev = cur;
				cur = aligned;
				avail = slob_units(cur);
			}

			next = slob_next(cur);
			if (avail == units) { /* exact fit? unlink. */
				if (prev)
					set_slob(prev, slob_units(prev), next);
				else
					sp->freelist = next;
			} else { /* fragment */
				if (prev)
					set_slob(prev, slob_units(prev), cur + units);
				else
					sp->freelist = cur + units;
				set_slob(cur + units, avail - units, next);
			}

			sp->units -= units;
			if (!sp->units) { // 如果链表空了，就将freelist置NULL
				sp->freelist = NULL;
			}

			return cur;
		}
		// 到末尾了还是找不到，算了
		if (slob_last(cur)){
			debug("no fit zone");
			return NULL;
		}
	}
}

#define SLOB_PAGE(addr) ((sp_t *)PGROUNDDOWN(addr))

// 释放slob_page
static void slob_free_page(sp_t *sp) {
	sp->pre->next = sp->next;
	sp->next->pre = sp->pre;
	__free_one_page(sp);
}

static inline void slob_init_page(sp_t *sp) {
    slob_t *b = (slob_t *)((uint64_t)sp + sizeof(struct slob_page));
    sp->units = SLOB_PAGE_UNITS;
	sp->freelist = b;
    set_slob(b, SLOB_PAGE_UNITS, (slob_t*)((uint64_t)(b + SLOB_PAGE_UNITS) & PGMASK));
}

// 申请一页新的slob_page
static sp_t *slob_new_page(sp_t *sp) {
	sp_t *new;

	new = (sp_t *)__alloc_one_page();
	if(!new) 
		return NULL;
	
    slob_init_page(new);

	new->next = sp->next;
	new->next->pre = new;
	new->pre = sp;
	new->pre->next = new;

	return new;
}

static void do_slob_free(void *block, int size) {
	sp_t *sp;
	slobidx_t units;
	slob_t *b = (slob_t *)block;

	if(!size)
		panic("slob_free: size == 0");

	if(!block)
		return;
	
	sp = SLOB_PAGE(block);
	units = SLOB_UNITS(size);
	debug("free block@%p size: %d units: %d", block, size, units);
	acquire(&slob_lock);
	if(sp->units + units == SLOB_PAGE_UNITS) {
        if (atomic_get(&page_cnt) > MAX_KEEP_HOLD_PAGE) {
            slob_free_page(sp);
        } else {
            slob_init_page(sp);
        }
		
		goto ret;
	}

	if(sp->units == 0) {
		sp->units = units;
		sp->freelist = b;
		set_slob(b, units, (slob_t*)((uint64_t)(b + SLOB_PAGE_UNITS) & PGMASK));
		goto ret;
	} else {
		slob_t *prev, *next;
		sp->units += units;
		prev = sp->freelist;
		next = slob_next(prev);
		while (b > next) {
			prev = next;
			next = slob_next(prev);
		}
		if (!slob_last(prev) && b + units == next) {
			// merge next
			units += slob_units(next);
			set_slob(b, units, slob_next(next));
		} else {
			set_slob(b, units, next);
		}

		if (prev + slob_units(prev) == b) {
			// merge prev
			units = slob_units(b) + slob_units(prev);
			set_slob(prev, units, slob_next(b));
		} else {
			set_slob(prev, slob_units(prev), b);
		}
	}

	ret:
	release(&slob_lock);
}

void slob_free(void* addr) {
	if(!addr)
		return;
	int minalign = sizeof(size_t);
	size_t* m = (size_t *)((uint64_t)addr - minalign);
	debug("free: size is %d", *m);
	do_slob_free(m, *m + minalign);
}

/* 入口函数 */
void *slob_alloc(size_t size) {
	sp_t *sp, *sp0;
	void *ret = NULL;
	// minimum 8 bytes alignment
	int minalign = sizeof(size_t);
	int align = minalign;
	// the number of slob units required.
	slobidx_t units = SLOB_UNITS(size);
	// more than one page.
	if(unlikely(size >= PGSIZE)) 
		panic("slob_alloc: unfit size");

	// exceeds the actual allocatable page size.
	if(units + minalign >= SLOB_PAGE_UNITS) {
		return __alloc_one_page();
	}

	// select suitable slob page list
	if(size <= SMALL_BREAK)
		sp0 = &small_list;
	else if(size <= LARGE_BREAK)
		sp0 = &medium_list;
	else 
		sp0 = &large_list;

	// natural alignment (pow of 2). ref: linux slob.c
	if(is_pow2(size))
		align = max((size_t)align, size);
	
	debug("alloc size: %d minalign: %d align: %d", size, minalign, align);
	
	acquire(&slob_lock);

	for(sp = sp0->next; sp != sp0; sp = sp->next) {
		if(sp->units == 0 || sp->units < units) {
            continue;
        }
		// find a suitable slob page!
		ret = slob_page_alloc(sp, size + minalign, minalign, align);
        // bad alloc, go on.
		if(!ret) {
			continue; 
        }
		break;
	}
    // no suitable page found
	if(!ret) { 
		// alloc a new page
		sp = slob_new_page(sp0);
		if(sp) { 
            // do alloc in new page
			ret = slob_page_alloc(sp, size + minalign, minalign, align);
		}
	}
	release(&slob_lock);
	if(ret) {
		// save the size info in the begining
		*(size_t *)ret = size;
        // add size offset
		ret += minalign;
	} 
	return ret;
}

/* Below is alloc interface implementation */

void __alloc_frag_init() {
    slob_init();
}

void *__alloc_frags(size_t size) {
	return slob_alloc(size);
}

void __free_frags(void *addr) {
	slob_free(addr);
}