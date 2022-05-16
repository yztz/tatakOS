#include "usys.h"
#include "stdarg.h"
/* #define syscall(num, arg0, arg1, arg2, arg3) ({ \
    register uint64 a0 asm ("a0") = (uint64)(arg0); \
    register uint64 a1 asm ("a1") = (uint64)(arg1); \
    register uint64 a2 asm ("a2") = (uint64)(arg2); \
    register uint64 a3 asm ("a3") = (uint64)(arg3); \
    register uint64 a7 asm ("a7") = (uint64)(num); \
    asm volatile ("ecall" \
        : "+r" (a0) \
        : "r" (a1), "r" (a2), "r" (a3), "r" (a7) \
        : "memory"); \
    a0; \
}) */

void printf(const char *fmt, ...);

// FS
char *fs_testcase[] = { "mkdir_","openat", "dup2","close", "unlink", "getcwd", "getdents",
                      "chdir", "dup", "pipe", "open", "read", "write", "fstat",
                      "mount", "umount", "test_echo"};
//
char *proc_testcase[] = { "getppid", "getpid",
                       "clone", "wait", "waitpid",
                      "yield", "fork",  "execve", "exit", "sleep"};

char *mm_testcase[] = {"brk", "mmap"};

char *other_testcase[] = {"gettimeofday", "times", "uname"};
//  单项测试
char *prog_name[] = { "gettimeofday",};
void run(char *testcases[], int cnt);
#define run(cases) run(cases, sizeof(cases)/sizeof(cases[0]))
__attribute__((section(".startup"))) 
void main() {
    // printf("hello world %d\n", 123);
    run(fs_testcase);
    run(proc_testcase);
    run(mm_testcase);
    run(other_testcase);
    // run(prog_name);
    for(;;);
}
#undef run

void run(char *testcases[], int cnt) {
  char *argv[2];
  argv[1] = 0;
  for (int t = 0; t < cnt; t++) {
      printf("ready to run %s\n", testcases[t]);
      int npid = fork();
      if(npid < 0) {
          printf("fork failed");
          for(;;);
      }
      if (npid == 0) { //子进程
          argv[0] = testcases[t];
          int ret = exec(argv[0], argv);
          printf("exec fail with %d\n", ret);
      } else {          // 父进程
          int status;
          wait(&status);
          printf("child exit with %d\n", status);
      }
  }
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
void
vprintf(int fd, const char *fmt, va_list ap)
{
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

void
fprintf(int fd, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

void
printf(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
