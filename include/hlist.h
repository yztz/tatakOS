#ifndef _H_HLIST_
#define _H_HLIST_

#ifndef container_of
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif
#define container_of(ptr, type, member) ({ \
		const typeof( ((type *)0)->member ) *__mptr = (ptr); \
		(type *) ( (char *) __mptr - offsetof(type, member) ); })
#endif

struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};

static inline void hlist_add(struct hlist_node *n, struct hlist_head *h)
{
    struct hlist_node *first = h->first;
    n->next = first;
    if (first)
        first->pprev = &n->next;        

    h->first = n;   //将head->first赋值为n,即n现在是头节点
    n->pprev = &h->first;   //该赋值在进行节点删除时有很大作用
}

static inline void hlist_del(struct hlist_node *n)
{
    struct hlist_node *next = n->next;
    struct hlist_node **pprev = n->pprev;

    *pprev = next;
    if (next)
        next->pprev = pprev;
}


#define hlist_entry(ptr, type, member) container_of(ptr,type,member)



#endif