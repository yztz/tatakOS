#ifndef MPAGE_H
#define MPAGE_H

#include "common.h"
#include "fs/fs.h"

// int read_one_page(entry_t *entry, uint64 buff, uint32 intdex);
bio_t *get_rw_pages_bio(entry_t *entry, uint64 buff, uint32 pg_id, int pg_cnt, int rw);
// int write_one_page(entry_t *entry, uint64_t buff, uint32_t index);
int write_pages(entry_t *entry, pages_be_found_head_t *pg_head);
int rw_one_page(entry_t *entry, uint64 buff, uint32 index, int rw);

#define read_one_page(entry, buff, index) rw_one_page(entry, buff, index, READ)
#define write_one_page(entry, buff, index) rw_one_page(entry, buff, index, WRITE)
#endif