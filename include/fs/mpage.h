#ifndef MPAGE_H
#define MPAGE_H

#include "common.h"
#include "fs/fs.h"

int readpage(entry_t *entry, uint64 buff, uint32 flpgnum);
struct bio *do_readpage(entry_t *entry, uint64 buff, uint32 flpgnum);
int mpage_writepages(address_space_t *mapping);
bio_t *get_rw_pages_bio(entry_t *entry, uint64 buff, uint32 pg_id, int pg_cnt, int rw);

#endif