#ifndef _H_STR_
#define _H_STR_

int             memcmp(const void*, const void*, uint);
void*           memmove(void* dst, const void* src, uint n);
void*           memcpy(void* dst, const void* src, uint n);
void*           memset(void* addr, int val, uint n);
char*           safestrcpy(char* dst, const char* src, int n);
int             strlen(const char*);
int             strncmp(const char*, const char*, uint);
char*           strncpy(char*, const char*, int);
void            to_upper(char *str);
int             endwith(const char *s, const char *tail);

#endif