#ifndef _H_GCC_ATTR_
#define _H_GCC_ATTR_

#define _section(name) __attribute__((section(#name)))
#define _always_inline __attribute__((always_inline)) inline

#define weak_alias(name, aliasname) _mweak_alias(name, aliasname)
#define _weak_alias(name, aliasname) \
extern __typeof(name) aliasname __attribute__((weak, alias(#name)));

#define _unused __attribute__((__unused__))
#define unused(x) x _unused
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define _noret __attribute__((noreturn))

#endif