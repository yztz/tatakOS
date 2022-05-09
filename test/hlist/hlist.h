#ifndef _H_HLIST_
#define _H_HLIST_

typedef struct hlist_node {
    struct hlist_node *next, **pre;
} hlist_node_t;

typedef struct hlist_head {
    hlist_node_t *next;
} hlist_head_t;



#endif