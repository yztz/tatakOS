#ifndef _CONS_H_
#define _CONS_H_

void consoleinit(void);
int  consoleready(void);

typedef struct console_io_op {
    void (*console_putchar)(char c);
    void (*console_putchar_sync)(char c);
    int (*console_getchar)();
} console_io_op_t;

void console_register(console_io_op_t *console);
void console_intr_callback(char c);

#endif