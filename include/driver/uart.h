#ifndef _H_UART_
#define _H_UART_

typedef void (* uart_intr_handler_t)(char);

void            uartinit(uart_intr_handler_t handler);
void            uartputc(char);
void            uartputc_sync(char);
int             uartgetc(void);



#endif