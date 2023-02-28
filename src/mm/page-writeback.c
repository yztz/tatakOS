/**
 * @file page-writeback.c
 * @author DavidZyy (1929772352@qq.com)
 * @brief  Contains functions related to writing back dirty pages at the
 * address_space level.
 * @version 0.1
 * @date 2022-06-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "common.h"
#include "platform.h"
#include "mm/alloc.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "mm/vm.h"
#include "kernel/proc.h"
#include "debug.h"
#include "mm/page.h"
#include "radix-tree.h"

#include "fs/mpage.h"
#include "writeback.h"

/*
 * The maximum number of pages to writeout in a single bdflush/kupdate
 * operation.  We do this so we don't hold I_LOCK against an inode for
 * enormous amounts of time, which would block a userspace task which has
 * been forced to throttle against that inode.  Also, the code reevaluates
 * the dirty each time it has written this many pages.
 */
#define MAX_WRITEBACK_PAGES	1024

extern int pdflush_operation(void (*fn)(uint64_t), unsigned long arg0);
/**
 * @brief set page and the radix tree path to it dirty, for look up dirty
 * pages when write it back to disk
 * 
 */
void set_pg_rdt_dirty(page_t *page, radix_tree_root_t *root, uint64_t pg_id, uint tag_type){
  SetPageDirty(page);
  radix_tree_tag_set(root, pg_id, tag_type);
}

/*
 * writeback at least min_pages, and keep writing until the amount of dirty
 * memory is less than the background threshold, or until we're all clean.
 */
void background_writeout(uint64_t min_pages){
/* linux 2.6.11使用一个writeback_control结构体记录写回信息，这里简单处理了 */
  struct writeback_control wbc = {
    .nr_to_write = 0,
  };

  for(;;){
    wbc.nr_to_write = MAX_WRITEBACK_PAGES;
    writeback_entrys_and_free_mapping(&wbc);
    
    /* 先写一遍就推出 */
    break;
  }
}

/*
 * Start writeback of `nr_pages' pages.  If `nr_pages' is zero, write back
 * the whole world.  Returns 0 if a pdflush thread was dispatched.  Returns
 * -1 if all pdflush threads were busy.
 */
int wakeup_bdflush(long nr_pages)
{
  /* 统计所有的写回页数 */
	if (nr_pages == 0) {
		// struct writeback_state wbs;

		// get_writeback_state(&wbs);
		// nr_pages = wbs.nr_dirty + wbs.nr_unstable;
    ER();
	}
	return pdflush_operation(background_writeout, nr_pages);
}