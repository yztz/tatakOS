#ifndef _H_UTILS_
#define _H_UTILS_

#include "printf.h"
#include "sbi.h"
#include "bio.h"

// #include "debug.h"

#define LOOP() {while(1) continue;}
#define DIVIDER() {printf("\n---------------------------------\n");}

#define ALIGN(a, align) ((a + (align - 1)) & ~(align - 1))

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define _section(name) __attribute__((section(#name)))
#define _always_inline __attribute__((always_inline)) inline

#define weak_alias(name, aliasname) _mweak_alias(name, aliasname)
#define _weak_alias(name, aliasname) \
extern __typeof(name) aliasname __attribute__((weak, alias(#name)));

static inline void set_bit(volatile uint32_t *bits, uint32_t mask, uint32_t value)
{
    uint32_t org = (*bits) & ~mask;
    *bits = org | (value & mask);
}

static inline void set_bit_offset(volatile uint32_t *bits, uint32_t mask, size_t offset, uint32_t value)
{
    set_bit(bits, mask << offset, value << offset);
}

static inline void set_gpio_bit(volatile uint32_t *bits, size_t offset, uint32_t value)
{
    set_bit_offset(bits, 1, offset, value);
}

static inline uint32_t get_bit(volatile uint32_t *bits, uint32_t mask, size_t offset)
{
    return ((*bits) & (mask << offset)) >> offset;
}

static inline uint32_t get_gpio_bit(volatile uint32_t *bits, size_t offset)
{
    return get_bit(bits, 1, offset);
}


#define configASSERT(x)                                 \
    if ((x) == 0)                                       \
    {                                                   \
        printf("(%s:%d) %s\r\n", __FILE__, __LINE__, #x); \
        for (;;)                                        \
            ;                                           \
    }

struct dir_item;
struct proc;
struct zone;


void backtrace();
void print_sbiret(sbiret_t ret);
int  luaO_log2 (unsigned int x);
void print_page(int pgnum);
void print_dir_item(struct dir_item *item);
void print_block(uint8_t *b) ;

void TD(char *info, char *s, const char *s1, int d);
// void print_all_vma(mm_struct_t *mm);
// void printf_radix_tree(struct radix_tree_root *root);
void print_bio_vec(struct bio *);
void print_page_contents(char* pa);
void __attribute__((noreturn)) error(char *info, char *s, const char *s1, int d);
void print_argv(char **argv);
void print_zone_list_info(struct zone *zone);
struct pages_be_found_head;
void print_pages_be_found(struct pages_be_found_head *pg_head);

#define todo(str)  TD(str, __FILE__, __func__, __LINE__)
#define get_order(x) luaO_log2(x)

#define abs(n) ((n)>=0?(n):-(n))


#define grn(str) 	"\e[32;1m"str"\e[0m"
#define ylw(str) 	"\e[33;1m"str"\e[0m"
#define rd(str) 	"\e[31;1m"str"\e[0m"
#define bl(str) 	"\e[34;1m"str"\e[0m"


#define ROUNDUP(size, align_size) (((uint64_t)size + align_size-1) & ~(align_size - 1))
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define KB (1024)
#define MB (1024 * KB)

#endif