#ifndef MPAGE_H
#define MPAGE_H

#include "common.h"
#include "fs/fs.h"

// int read_one_page(entry_t *entry, uint64_t buff, uint32_t intdex);
bio_t *get_rw_pages_bio(entry_t *entry, uint64_t buff, uint32_t pg_id, int pg_cnt, int rw);
// int write_one_page(entry_t *entry, uint64_t buff, uint32_t index);
// int write_pages(entry_t *entry, rw_page_list_t *pg_head);
int rw_pages(entry_t *entry, rw_page_list_t *pg_list, int rw);
int rw_one_page(entry_t *entry, uint64_t buff, uint32_t index, int rw);

#define read_pages(entry, pg_list) rw_pages(entry, pg_list, BIO_READ)
#define write_pages(entry, pg_list) rw_pages(entry, pg_list, BIO_WRITE)

#define read_one_page(entry, buff, index) rw_one_page(entry, buff, index, BIO_READ)
#define write_one_page(entry, buff, index) rw_one_page(entry, buff, index, BIO_WRITE)


#endif