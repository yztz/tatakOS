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
#include "defs.h"
#include "debug.h"
#include "mm/page.h"
#include "radix-tree.h"
#include "mm/mm.h"

#include "fs/mpage.h"


/**
 * @brief set page and the radix tree path to it dirty, for look up dirty
 * pages when write it back to disk
 * 
 */
void set_pg_rdt_dirty(uint64_t pa, radix_tree_root_t *root, uint64_t pg_id, uint tag_type){
  set_page_dirty(pa);
  radix_tree_tag_set(root, pg_id, tag_type);
}


