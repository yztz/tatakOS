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
#include "kernel/proc.h"
#include "platform.h"
#include "driver/plic.h"
#include "driver/console.h"
#include "mm/vm.h"
#include "termios.h"

#define __MODULE_NAME__ CONSOLE
#include "debug.h"

// static int __wrapper_sbi_getchar() {
//     return sbi_getchar();
// }

// static void __wrapper_sbi_putchar(char c) {
//     sbi_putchar(c);
// }

// static console_io_op_t sbi_ioop = {
//     .console_getchar = __wrapper_sbi_getchar,
//     .console_putchar_sync = __wrapper_sbi_putchar,
//     .console_putchar = __wrapper_sbi_putchar
// };

static console_io_op_t *ioop = NULL;

void console_register(console_io_op_t *console) {
    ioop = console;
}

struct termios term = {
    .c_iflag = ICRNL,
    .c_oflag = OPOST,
    .c_cflag = 0,
    .c_lflag = ECHO | ICANON,
    .c_line = 0,
    .c_cc = {0},
};


#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x

static inline void putchar(char c) {
    if (panicked) {
        LOOP();
    }
    ioop->console_putchar(c);
}


//
// send one character to the uart.
// called by printf, and to echo input characters,
// but not from write().
//
void consputc(int c) {
    if (c == BACKSPACE) {
        // if the user typed backspace, overwrite with a space.
        putchar('\b'); putchar(' '); putchar('\b');
    } else {
        putchar(c);
    }
}

struct console {
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
int consolewrite(int user_src, uint64 src, int n) {
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
    for (i = 0; i < n; i++) {
        char c;
        if (either_copyin(&c, user_src, src + i, 1) == -1)
            break;
        consputc(c);
    }

    return i;
}


int consoleready() {
    acquire(&cons.lock);
    int ready = cons.w - cons.r;
    release(&cons.lock);
    return ready;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dist indicates whether dst is a user
// or kernel address.
//
int consoleread(int user_dst, uint64 dst, int n) {
    uint target;
    char c;

    uint lflag = term.c_lflag;
    uint8 vmin = term.c_cc[VMIN];

    // debug("reading %d bytes", n);
    // debug("lflag is %d", lflag);

    target = n;
    acquire(&cons.lock);
    while (n > 0) {
        // wait until interrupt handler has put some
        // input into cons.buffer.
        while (cons.r == cons.w) {
            if ((lflag & ICANON) == 0 && vmin == 0) break;
            if (current->killed) {
                release(&cons.lock);
                return -1;
            }
            sleep(&cons.r, &cons.lock);
        }

        c = cons.buf[cons.r++ % INPUT_BUF];

        if ((lflag & ICANON) == 0) {
            if (either_copyout(user_dst, dst, &c, 1) == -1)
                break;
            dst++;--n;
            if (target - n == vmin) break;
        } else {
            if (c == C('D')) {  // end-of-file
                if (n < target) {
                    // Save ^D for next time, to make sure
                    // caller gets a 0-byte result.
                    cons.r--;
                }
                break;
            }

            // copy the input byte to the user-space buffer.
            if (either_copyout(user_dst, dst, &c, 1) == -1)
                break;
            dst++;--n;

            if (c == '\n') {
                // a whole line has arrived, return to
                // the user-level read().
                break;
            }
        }

    }
    release(&cons.lock);

    return target - n;
}

extern void buddy_print_free();

static void interactive_debug_info(char c) {
    // printf("%d\n", c);
    switch (c) {
    case C('P'):  // Print process list.
        procdump();
        buddy_print_free();
        break;
    }
}


//
// the console input interrupt handler.
// uartintr() calls this for input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
// 基于中断，用于字符回显，以及存储字符到缓存
//
void console_intr_callback(char c) {
    uint16_t iflag = term.c_iflag;
    uint16_t lflag = term.c_lflag;
    acquire(&cons.lock);

    //   #ifdef DEBUG
    interactive_debug_info(c);
    //   #endif

      // not cookmode
    if ((lflag & ICANON) == 0) {
        if (c != 0 && cons.e - cons.r < INPUT_BUF) {

            // echo back to the user.
            if (lflag & ECHO) consputc(c);

            // store for consumption by consoleread().
            cons.buf[cons.e++ % INPUT_BUF] = c;
            cons.w = cons.e;
            wakeup(&cons.r);
        }
        release(&cons.lock);
        return;
    }

    switch (c) {
    case C('U'):  // Kill line.
        while (cons.e != cons.w &&
            cons.buf[(cons.e - 1) % INPUT_BUF] != '\n') {
            cons.e--;
            consputc(BACKSPACE);
        }
        break;
    case C('H'): // Backspace
    case '\x7f':
        if (cons.e != cons.w) {
            cons.e--;
            consputc(BACKSPACE);
        }
        break;
    default:
        if (c != 0 && cons.e - cons.r < INPUT_BUF) {

            if (iflag & ICRNL)
                c = (c == '\r') ? '\n' : c; // 回车转换行


            // echo back to the user.
            if (lflag & ECHO) consputc(c);

            // store for consumption by consoleread().
            cons.buf[cons.e++ % INPUT_BUF] = c;

            if (c == '\n' || c == C('D') || cons.e == cons.r + INPUT_BUF) {
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


void consoleinit(void) {
    initlock(&cons.lock, "cons");
    // plic_register_handler(UART_IRQ, cons_irq_callback, NULL);
    // console_init(consoleintr, NULL);
    cons.e = cons.w = cons.r = 0;
    // connect read and write system calls
    // to consoleread and consolewrite.
    devs[CONSOLE].read = consoleread;
    devs[CONSOLE].write = consolewrite;

    debug("console init success!\n");
}
