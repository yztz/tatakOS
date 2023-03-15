#ifndef _CONS_H_
#define _CONS_H_

int  consoleready(void);

/**
 * @brief put a char
 * @note may sleep or be implemented with interrupt
 * 
 * @param c 
 */
extern void console_putchar(char c);

/**
 * @brief put a char
 * @brief no sleep, just implemented with polling
 * 
 * @param c 
 */
extern void console_putchar_sync(char c);

/**
 * @brief get a char
 * @brief same with console_putchar(), maybe sleep
 *        a sync getchar() is not necessary
 * 
 * @return int 
 */
extern int console_getchar();

/**
 * @brief getchar() interrupt should callback this
 *        to pass c to the console handler
 * 
 * @param c 
 */
void console_intr_callback(char c);

#endif