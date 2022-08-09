//
// Console input and output, to the uart.
// Reads are line at a time.
// Implements special input characters:
//   newline -- end of line
//   control-h -- backspace
//   control-u -- kill line
//   control-d -- end of file
//   control-p -- print process list
//

// #include <stdarg.h>

#include "common.h"
#include "atomic/spinlock.h"
#include "atomic/sleeplock.h"
#include "fs/fs.h"
#include "fs/file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "kernel/proc.h"
#include "platform.h"
#include "driver/plic.h"
#include "mm/vm.h"

#define __MODULE_NAME__ CONSOLE
#include "debug.h"

#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x

/* used by printf */
void _putchar(char c) 
{
  sbi_putchar(c);
}

//
// send one character to the uart.
// called by printf, and to echo input characters,
// but not from write().
//
void
consputc(int c)
{
  if(c == BACKSPACE){
    // if the user typed backspace, overwrite with a space.
    sbi_putchar('\b'); sbi_putchar(' '); sbi_putchar('\b');
  } else {
    sbi_putchar(c);
  }
}

struct {
  struct spinlock lock;
  
  // input
#define INPUT_BUF 128
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} cons;

//
// user write()s to the console go here.
//
int
consolewrite(int user_src, uint64 src, int n)
{
  int i;

  // enable_sum();
  // if(src == 0x60000000){
  //   printf(grn("src: %p \n"), src);
  //   // for(;;);
  //   char c;
  //   for(int i=0; i < 50; i++)
  //     printf(rd("char: %c\n"), c);
  //     // printf(grn("%c"), *(char*)((void*)0x60000000 + i));
  //   // printf(grn("pa content: %c\n"), *(char*)va);
  //   printf("\n");
  // }
  // disable_sum();
  for(i = 0; i < n; i++){
    char c;
    if(either_copyin(&c, user_src, src+i, 1) == -1)
      break;
    consputc(c);
  }

  return i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dist indicates whether dst is a user
// or kernel address.
//
int
consoleread(int user_dst, uint64 dst, int n)
{
  uint target;
  int c;
  char cbuf;

  target = n;
  acquire(&cons.lock);
  while(n > 0){
    // wait until interrupt handler has put some
    // input into cons.buffer.
    while(cons.r == cons.w){
      if(myproc()->killed){
        release(&cons.lock);
        return -1;
      }
      sleep(&cons.r, &cons.lock);
    }

    c = cons.buf[cons.r++ % INPUT_BUF];

    if(c == C('D')){  // end-of-file
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      break;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    if(either_copyout(user_dst, dst, &cbuf, 1) == -1)
      break;

    dst++;
    --n;

    if(c == '\n'){
      // a whole line has arrived, return to
      // the user-level read().
      break;
    }
  }
  release(&cons.lock);

  return target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
// 基于中断，用于字符回显，以及存储字符到缓存
//
extern void buddy_print_free();
void
consoleintr(char c)
{
  acquire(&cons.lock);
  switch(c){
  case C('P'):  // Print process list.
    procdump();
    buddy_print_free();
    break;
  case C('U'):  // Kill line.
    while(cons.e != cons.w &&
          cons.buf[(cons.e-1) % INPUT_BUF] != '\n'){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  case C('H'): // Backspace
  case '\x7f':
    if(cons.e != cons.w){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  default:
    if(c != 0 && cons.e-cons.r < INPUT_BUF){
      #ifndef QEMU // refer xv6-k210
			if (c == '\r') break;
			#else
			c = (c == '\r') ? '\n' : c;
			#endif


      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % INPUT_BUF] = c;

      if(c == '\n' || c == C('D') || cons.e == cons.r+INPUT_BUF){
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    break;
  }
  
  release(&cons.lock);
}

int cons_irq_callback(void *ctx) {
  char c = sbi_getchar();
  if (c < 0) return 0;
  consoleintr(c);
  return 0;
}


void
consoleinit(void)
{
  initlock(&cons.lock, "cons");
  plic_register_handler(UART_IRQ, cons_irq_callback, NULL);
  // console_init(consoleintr, NULL);
  cons.e = cons.w = cons.r = 0;
  // connect read and write system calls
  // to consoleread and consolewrite.
  devs[CONSOLE].read = consoleread;
  devs[CONSOLE].write = consolewrite;

  debug("console init success!\n");
}
