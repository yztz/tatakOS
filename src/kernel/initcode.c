#include "usys.h"
#include "stdarg.h"

void printf(const char *fmt, ...);
void*memcpy(void* dst, const void* src, uint n);
void __run(char *argv[]);

#define run(name, ...) {char *__cmd[] = {name, ##__VA_ARGS__, 0};__run(__cmd);}
#define shell(...) {char *__cmd[] = {"busybox", "sh", ##__VA_ARGS__, 0};__run(__cmd);}
#define lua(...) {char *__cmd[] = {"lua", ##__VA_ARGS__, 0};__run(__cmd);}
#define lmbench(...) {char *__cmd[] = {"lmbench_all", ##__VA_ARGS__, 0};__run(__cmd);}


__attribute__((section(".startup"))) 
void main() {

    mkdirat(-100, "tmp");
    mkdirat(-100, "proc");
    mkdirat(-100, "proc/mounts");

    memuse();
    shell();
    // printf("Hello World\n");
    // run("qjs");
    memuse();
    halt();
    for(;;);
}

#define WEXITSTATUS(s) (((s) & 0xff00) >> 8)
void __run(char *argv[]) {
    int npid = fork();
    if(npid < 0) {
        printf("fork failed");
        for(;;);
    }
    if (npid == 0) { //子进程
        int ret = exec(argv[0], argv);
        printf("exec %s fail with %d\n", argv[0], ret);
    } else {          // 父进程
        int status;
        wait(&status);
        printf("child %s exit with %d\n", argv[0], WEXITSTATUS(status));
    }
}


///////////utils/////////////

void*memcpy(void *dst, const void *src, uint n) {
  const char *s;
  char *d;

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

static char digits[] = "0123456789ABCDEF";

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(int fd, uint64 x) {
  int i;
  putc(fd, '0');
  putc(fd, 'x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void vprintf(int fd, const char *fmt, va_list ap) {
  char *s;
  int c, i, state;

  state = 0;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, va_arg(ap, int), 10, 1);
      } else if(c == 'l') {
        printint(fd, va_arg(ap, uint64), 10, 0);
      } else if(c == 'x') {
        printint(fd, va_arg(ap, int), 16, 0);
      } else if(c == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } else if(c == 's'){
        s = va_arg(ap, char*);
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
        putc(fd, va_arg(ap, uint));
      } else if(c == '%'){
        putc(fd, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
    }
  }
}

void fprintf(int fd, const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

void printf(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}

int strncmp(const char *p, const char *q, uint n) {
  while(n > 0 && *p && *p == *q)
    n--, p++, q++;
  if(n == 0)
    return 0;
  return (uchar)*p - (uchar)*q;
}