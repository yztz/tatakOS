#ifndef _H_LIST_
#define _H_LIST_

#define offset_of(type, member) ((size_t)(&(((type *)0)->member)))
#define containter_of(ptr, type, member) {\
    const typeof(((type *)0)->member)* _pmem = (ptr);\
    (type *)((char *)_pmem-offset_of(type, member));}

typedef struct _list_head_t {
    struct _list_head_t *pre, *next;
} list_head_t;

#define list_entry(ptr, type, member) \
    containter_of(ptr, type, member)

#define list_for_each_entry(pos, head, member) \
    for(pos=list_entry((head)->next), typeof(*pos), member); \
        &(pos->member) != (head); \
        pos=list_entry(pos->member.next, typeof(*pos), member))

static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}



#endif