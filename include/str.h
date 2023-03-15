#ifndef _H_STR_
#define _H_STR_

int             memcmp(const void*, const void*, uint);
void*           memmove(void* dst, const void* src, uint n);
void*           memcpy(void* dst, const void* src, uint n);
void*           memset(void* addr, int val, uint n);
char*           safestrcpy(char* dst, const char* src, int n);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);
int             strcasencmp(const char *s1, const char *s2, int n);
char*           strncpy(char*, const char*, int);
void            to_upper(char *str);
int             endwith(const char *s, const char *tail);
char*           strchr(const char* str1, const char str2);
int             startswith(const char *str, const char *target);

#define toupper(c) ({ \
    char __rsv_c = (c); \
    __rsv_c >= 'a' && __rsv_c <= 'z' ? __rsv_c - ('a' - 'A') : __rsv_c; \
})

#define tolower(c) ({ \
    char __rsv_c = (c); \
    __rsv_c >= 'A' && __rsv_c <= 'Z' ? __rsv_c + ('a' - 'A') : __rsv_c; \
})

static inline int isdigit(int c) {
	return (unsigned)c-'0' < 10;
}

static inline int isxdigit(int c) {
	return isdigit(c) || ((unsigned)c|32)-'a' < 6;
}

static inline int isspace(int c) {
	return c == ' ' || (unsigned)c-'\t' < 5;
}

#define snprintf snprintf_
extern int snprintf_(char* buffer, size_t count, const char* format, ...);


#endif