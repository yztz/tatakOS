#ifndef WRITEBACK_H
#define WRITEBACK_H


/**
 * @brief A control structure which tells the writeback code what to do.  These are
 * always on the stack, and hence need no locking.  They are always initialised
 * in a manner such that unspecified fields are set to zero.
 * 
 */
struct writeback_control {
    /**
     * @brief Write this many pages, and decrease
	 *        this when each page written
     * 
     */
	long nr_to_write;
	// long pages_skipped;		/* Pages which were not written */

	// /*
	//  * For a_ops->writepages(): is start or end are non-zero then this is
	//  * a hint that the filesystem need only write out the pages inside that
	//  * byterange.  The byte at `end' is included in the writeout request.
	//  */
	// loff_t start;
	// loff_t end;

	// unsigned nonblocking:1;			/* Don't get stuck on request queues */
	// unsigned encountered_congestion:1;	/* An output: a queue is full */
	// unsigned for_kupdate:1;			/* A kupdate writeback */
	// unsigned for_reclaim:1;			/* Invoked from the page allocator */
};

/*
 * fs/fs-writeback.c
 */
void writeback_entrys_and_free_mapping(struct writeback_control *wbc);
// void writeback_single_entry(entry_t *entry, struct writeback_control *wbc);
void writeback_single_entry(entry_t *entry);
void writeback_single_entry_idx(uint64_t idx);

/*
 * mm/page-writeback.c
 */
int wakeup_bdflush(long nr_pages);

#endif