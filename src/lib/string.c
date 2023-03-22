#include "types.h"
#include "str.h"

void *memset(void *dst, int c, uint n) {
  uint8_t *cdst = (uint8_t *) dst;
  int i;
  for(i = 0; i < n; i++){
    cdst[i] = c;
  }
  return dst;
}

int memcmp(const void *v1, const void *v2, uint n) {
  const uint8_t *s1, *s2;

  s1 = v1;
  s2 = v2;
  while(n-- > 0){
    if(*s1 != *s2)
      return *s1 - *s2;
    s1++, s2++;
  }

  return 0;
}

void *memmove(void *dst, const void *src, uint n) {
  const uint8_t *s;
  uint8_t *d;

  if(n == 0)
    return dst;
  
  s = src;
  d = dst;
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
      *d++ = *s++;

  return dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *memcpy(void *dst, const void *src, uint n) {
  return memmove(dst, src, n);
}

int strncmp(const char *p, const char *q, uint n) {
  while(n > 0 && *p && *p == *q)
    n--, p++, q++;
  if(n == 0)
    return 0;
  return (uchar)*p - (uchar)*q;
}

char *strncpy(char *s, const char *t, int n) {
  char *os;

  os = s;
  while(n-- > 0 && (*s++ = *t++) != 0)
    ;
  while(n-- > 0)
    *s++ = 0;
  return os;
}

// Like strncpy but guaranteed to NUL-terminate.
char *safestrcpy(char *s, const char *t, int n) {
  char *os;

  os = s;
  if(n <= 0)
    return os;
  while(--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  return os;
}

int strlen(const char *s) {
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void to_upper(char *str) {
  while(*str != '\0') {
      *str = toupper(*str);
      str++;
  }
}

int endwith(const char *s, const char *tail) {
  if(s == NULL || tail == NULL)
    return 0;

  int len = strlen(s);
  int tail_len = strlen(tail);

  if(tail_len > len)
    return 0;
  
  s = s + len - tail_len;

  for(int i = 0; i < tail_len; i++) {
    if(s[i] != tail[i])
      return 0;
  }

  return 1;
}

int strcasencmp(const char *s1, const char *s2, int n) {
    unsigned char c1, c2;

    do {
        c1 = *s1++;
        c2 = *s2++;
        if (c1 != c2) {
            c1 = toupper(c1);
            c2 = toupper(c2);
            if (c1 != c2)
                return (int)c1 - (int)c2;
        }
    } while (c1 != 0 && --n > 0);
    return 0;
}

char *strchr(const char* str1, const char str2) {
	while (*str1 != '\0'&& *str1 !=(char) str2) {
		++str1;
	}
	if (*str1 == (char)str2) {
		return (char*)str1;
	}
	return NULL;
}

int startswith(const char *str, const char *target) {
    int len = strlen(target);
	if (len <= 0)
		return 0;
	if (strncmp(str, target, len) == 0)
		return 1;

	return 0;
}