#ifndef _H_UTILS_
#define _H_UTILS_

#include "printf.h"
#include "sbi.h"
#include "bio.h"
#include "str.h"

#define LOOP() {while(1) continue;}
#define ALIGN(a, align) ((a + (align - 1)) & ~(align - 1))
#define ROUNDUP(size, align_size) (((uint64_t)size + align_size-1) & ~(align_size - 1))
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

/* Math */
#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);          \
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })



#define abs(n) ((n)>=0?(n):-(n))

#define pow2(n) (1 << n)

#define is_pow2(n) ((n & (n - 1)) == 0)

#define log2(x) luaO_log2(x)


#define _section(name) __attribute__((section(#name)))
#define _always_inline __attribute__((always_inline)) inline

#define weak_alias(name, aliasname) _mweak_alias(name, aliasname)
#define _weak_alias(name, aliasname) \
extern __typeof(name) aliasname __attribute__((weak, alias(#name)));

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

#define __STR(name) #name
#define STR(name) __STR(name)

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#define unused(x) x __attribute__((__unused__))
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)


struct dir_item;
struct proc;
struct zone;
struct kmap;
struct radix_tree_root;
struct pages_be_found_head;


void backtrace();
void backtrace_fp(uint64 fp);
void print_sbiret(sbiret_t ret);
int  luaO_log2 (unsigned int x);
void print_page(int pgnum);
void print_dir_item(struct dir_item *item);
void print_block(uint8_t *b) ;
void printf_radix_tree(struct radix_tree_root *root);
void print_bio_vec(struct bio *);
void print_page_contents(char* pa);
void __attribute__((noreturn)) error(char *info, char *s, const char *s1, int d);
void print_argv(char **argv);
void print_zone_list_info(struct zone *zone);
void print_pages_be_found(struct pages_be_found_head *pg_head);
void print_kmap(struct kmap map);
void print_pagetable(pagetable_t pagetable);


void TD(char *info, char *s, const char *s1, int d);
#define todo(str)  TD(str, __FILE__, __func__, __LINE__)

#define grn(str) 	"\e[32;1m"str"\e[0m"
#define ylw(str) 	"\e[33;1m"str"\e[0m"
#define rd(str) 	"\e[31;1m"str"\e[0m"
#define bl(str) 	"\e[34;1m"str"\e[0m"


#endif