#ifndef _H_LINKED_LIST
#define _H_LINKED_LIST


#include"types.h"


#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define LIST_POISON0  ((void *) 0x00100100)
#define LIST_POISON1  ((void *) 0x00200200)

struct list_head{
  struct list_head *next, *prev;
};

typedef struct list_head list_head_t;


#define LIST_HEAD_INIT(name) {&(name), &(name)}

#define LIST_HEAD(name) \
        struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/**
 * list_is_head - tests whether @list is the list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int list_is_head(const struct list_head *list, const struct list_head *head)
{
	return list == head;
}

/*
 * Insert a pnew entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *pnew,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = pnew;
	pnew->next = next;
	pnew->prev = prev;
	prev->next = pnew;
}
/**
 * list_add - add a pnew entry
 * @pnew: pnew entry to be added
 * @head: list head to add it after
 *
 * Insert a pnew entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head *pnew, struct list_head *head)
{
	__list_add(pnew, head, head->next);
}

/**
 * list_add_tail - add a pnew entry
 * @pnew: pnew entry to be added
 * @head: list head to add it before
 *
 * Insert a pnew entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *pnew, struct list_head *head)
{
	__list_add(pnew, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void __list_del_entry(struct list_head *entry)
{
	// if (!__list_del_entry_valid(entry))
	// 	return;

	__list_del(entry->prev, entry->next);
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void list_del(struct list_head *entry)
{
	__list_del_entry(entry);
	// entry->next = LIST_POISON1;
	// entry->prev = LIST_POISON2;
	entry->next = (list_head_t *)NULL;
	entry->prev = (list_head_t *)NULL;
}

/**
 * list_replace - replace old entry by pnew one
 * @old : the element to be replaced
 * @pnew : the pnew element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace(struct list_head *old,
				struct list_head *pnew)
{
	pnew->next = old->next;
	pnew->next->prev = pnew;
	pnew->prev = old->prev;
	pnew->prev->next = pnew;
}


/**
 * list_replace_init - replace old entry by pnew one and initialize the old one
 * @old : the element to be replaced
 * @pnew : the pnew element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace_init(struct list_head *old,
				     struct list_head *pnew)
{
	list_replace(old, pnew);
	INIT_LIST_HEAD(old);
}

/**
 * list_swap - replace entry1 with entry2 and re-add entry1 at entry2's position
 * @entry1: the location to place entry2
 * @entry2: the location to place entry1
 */
static inline void list_swap(struct list_head *entry1,
			     struct list_head *entry2)
{
	struct list_head *pos = entry2->prev;

	list_del(entry2);
	list_replace(entry1, entry2);
	if (pos == entry1)
		pos = entry2;
	list_add(entry1, pos);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void list_del_init(struct list_head *entry)
{
	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move(struct list_head *list, struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add(list, head);
}


/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)



/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)


/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; !list_is_head(pos, (head)); pos = pos->next)

/**
 * list_entry_is_head - test if the entry points to the head of the list
 * @pos:	the type * to cursor
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry_is_head(pos, head, member)				\
	(&pos->member == (head))

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     !list_entry_is_head(pos, head, member);			\
	     pos = list_next_entry(pos, member))

#define list_for_each_entry_condition(pos, head, member, condition)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     (condition) && !list_entry_is_head(pos, head, member);			\
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))

#define list_for_each_entry_safe_condition(pos, n, head, member, condition)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     (condition) && &pos->member != (head);					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_last_entry(head, typeof(*pos), member);		\
	     !list_entry_is_head(pos, head, member); 			\
	     pos = list_prev_entry(pos, member))

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

static inline void __list_splice(struct list_head *list,
				 struct list_head *head)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;
	struct list_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice(struct list_head *list, struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head);
}

// // hash list
// #define HLIST_HEAD_INIT {.first = NULL}
// #define HLIST_HEAD(name) struct hlist_head name = {.first = NULL}
// #define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

// static inline void INIT_HLIST_NODE(struct hlist_node *h){
//   h->next = NULL;
//   h->pprev = NULL;
// }




// /**
//  * hlist_add_head - add a pnew entry at the beginning of the hlist
//  * @n: pnew entry to be added
//  * @h: hlist head to add it after
//  *
//  * Insert a pnew entry after the specified head.
//  * This is good for implementing stacks.
//  */
// static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
// {
// 	struct hlist_node *first = h->first;
// 	n->next = first;
// 	if (first)
// 		first->pprev = &n->next;
// 	h->first = n;
// 	n->pprev = &h->first;
// }

// static __inline__ void __hlist_del(struct hlist_node *n) 
// {
// 	struct hlist_node *next = n->next;
// 	struct hlist_node **pprev = n->pprev;
// 	*pprev = next;  
// 	if (next) 
// 		next->pprev = pprev;
// }  

// /**
//  * hlist_del_rcu - deletes entry from hash list without re-initialization
//  * @n: the element to delete from the hash list.
//  *
//  * Note: list_unhashed() on entry does not return true after this, 
//  * the entry is in an undefined state. It is useful for RCU based
//  * lockfree traversal.
//  *
//  * In particular, it means that we can not poison the forward
//  * pointers that may still be used for walking the hash list.
//  */
// static inline void hlist_del_rcu(struct hlist_node *n)
// {
// 	__hlist_del(n);
// 	n->pprev = LIST_POISON2;
// }


#endif