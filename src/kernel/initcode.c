#include "usys.h"
#include "stdarg.h"

void printf(const char *fmt, ...);

__attribute__((section(".startup"))) 
void main() {
    memuse();
    int status;
    char *argv[4];
    argv[0] = "busybox";
    // argv[1] = "du";
    argv[1] = "sh";
    // argv[1] = "var";
    // argv[2] = "var";
    argv[2] = "busybox_testcode.sh";
    // argv[2] = "-lh";
    // argv[2] = 0;
    argv[3] = 0;
    
    // argv[2] = 0;

    int npid = fork();
    if(npid < 0) {
        printf("fork failed");
        for(;;);
    }
    if (npid == 0) { //子进程
        int ret = exec(argv[0], argv);
        printf("exec fail with %d\n", ret);
    } else {          // 父进程
        wait(&status);
        printf("child exit with %d\n", status);
    }
    
    memuse();
    halt();
    for(;;);
}



// // #include "usys.h"
// // #include "stdarg.h"
// #include "fs/fcntl.h"

// void printf(const char *fmt, ...);
// int strncmp(const char *p, const char *q, uint n);
// #define MAX_LINE 70

// #define assert(cond, fmt, ...) if(!(cond)) {printf(fmt"\n", ##__VA_ARGS__);for(;;);}

// int fd = -1;
// char proc_name[40];
// char line[MAX_LINE];
// char *argv[] = {"./runtest.exe", "-w", NULL, proc_name, NULL};
// int foffset = 0;

// #define COND (i == 1)

// void run(char *casename, char *entryname);

// __attribute__((section(".startup")))
// int main() {
//     memuse();
    
//     // run("run-static.sh", "entry-test");

//     run("run-static.sh", "entry-static.exe");
//     // run("run-dynamic.sh", "entry-dynamic.exe");
    
//     memuse(); 
//     // philosophy();
//     printf("\n\e[32;1mALL TEST DONE!\e[0m\n");

//     halt();
//     return 0;
// }

// int read_test_name() {
//     off_t loffset = 0;
//     int i;
//     assert(fd > 0, "bad fd2");
//     lseek(fd, foffset, SEEK_SET);
//     int cnt = read(fd, line, MAX_LINE);
//     if(cnt <= 0) 
//       return -1;
//     // skip space
//     for (i = 0; i < 3; i++) {
//         for (; line[loffset] != ' '; loffset++);
//         loffset++;
//     }
    
//     for (i = 0; line[loffset] != '\n'; i++, loffset++)
//         proc_name[i] = line[loffset];
//     *(proc_name + i) = '\0';
//     loffset++;
//     foffset+=loffset;
//     return 0;
// }

// void run(char *casename, char *entryname) {
//   if(fd > 0) close(fd);
//   fd = -1;
//   foffset = 0;
//   fd = openat(AT_FDCWD, casename, O_RDONLY);
//   assert(fd > 0, "bad fd");
//   argv[2] = entryname;

//   int i = 0;
//   while(read_test_name() != -1) {
//     i++;
//     // filters //
//     // if(!COND) continue;
//     if(strncmp(proc_name, "scanf_long", 10) == 0) continue;

//     printf("\nReady To Run Test-%d %s\n", i, proc_name);
//     int npid = fork();
//     assert(npid >= 0, "bad fork");
//     if (npid == 0) { // child
//         exec(argv[0], argv);
//         printf("exec fail\n");
//         for(;;);
//     } else { // parent
//         int status;
//         wait(&status);
//         printf("child exit with %d\n", status);
//     }
//   }
// }


///////////utils/////////////

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

int
strncmp(const char *p, const char *q, uint n)
{
  while(n > 0 && *p && *p == *q)
    n--, p++, q++;
  if(n == 0)
    return 0;
  return (uchar)*p - (uchar)*q;
}

///////////////////////
// #include "usys.h"
// #include "stdarg.h"

// void printf(const char *fmt, ...);
// void read_test();

// // FS
// char *fs_testcase[] = { "mkdir_","openat", "dup2","close", "unlink", "getcwd", "getdents",
//                       "chdir", "dup", "pipe", "open", "read", "write", "fstat",
//                       "mount", "umount", "test_echo"};
// //
// char *proc_testcase[] = { "getppid", "getpid",
//                        "clone", "wait", "waitpid",
//                       "yield", "fork",  "execve", "exit", "sleep"};

// char *mm_testcase[] = {"brk", "mmap", "munmap"};

// char *other_testcase[] = {"gettimeofday", "times", "uname"};
// // //  单项测试
// char* prog_name[] = { "bigwrite" };

// void run(char *testcases[], int cnt);
// #define run(cases) run(cases, sizeof(cases)/sizeof(cases[0]))
// __attribute__((section(".startup"))) 
// void main() {
//     memuse();
//     // run(fs_testcase);
//     // run(proc_testcase);
//     // run(mm_testcase);
//     // run(other_testcase);
//     run(prog_name);
//     memuse();
//   for(;;);
// }
// #undef run

// void run(char *testcases[], int cnt) {
//   // char *argv[4];
//   // argv[1] = "ash";
//   // argv[2] = "./busybox_cmd.txt";
//   // argv[2] = "busybox_testcode.sh";
//   // argv[3] = 0;
//   char *argv[2];
//   argv[1] = 0;
//   for (int t = 0; t < cnt; t++) {
//       printf("ready to run %s\n", testcases[t]);
//       int npid = fork();
//       if(npid < 0) {
//           printf("fork failed");
//           for(;;);
//       }
//       if (npid == 0) { //子进程
//           argv[0] = testcases[t];
//           int ret = exec(argv[0], argv);
//           printf("exec fail with %d\n", ret);
//       } else {          // 父进程
//           int status;
//           wait(&status);
//           printf("child exit with %d\n", status);
//       }
//   }
// }
