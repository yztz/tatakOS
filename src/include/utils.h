#ifndef _H_UTILS_
#define _H_UTILS_

#include "sbi.h"
#include "bio.h"
#include "str.h"
#include "types.h"
#include "gcc_attr.h"

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


struct dir_item;
struct proc;
struct zone;
struct kmap;
struct radix_tree_root;


void backtrace();
void backtrace_fp(uint64_t fp);
void print_sbiret(sbiret_t ret);
int  luaO_log2 (unsigned int x);
void print_dir_item(struct dir_item *item);
void print_block(uint8_t *b) ;
void printf_radix_tree(struct radix_tree_root *root);
void print_bio_vec(struct bio *);
void print_page_contents(char* pa);
void __attribute__((noreturn)) error(char *info, char *s, const char *s1, int d);
void print_argv(char **argv);
void print_pagetable(pagetable_t pagetable);
void print_mm_free();


#define grn(str) 	"\e[32;1m"str"\e[0m"
#define ylw(str) 	"\e[33;1m"str"\e[0m"
#define rd(str) 	"\e[31;1m"str"\e[0m"
#define bl(str) 	"\e[34;1m"str"\e[0m"

extern int kprintf(const char *format, ...);
extern void panic(char *s) _noret;

#endif