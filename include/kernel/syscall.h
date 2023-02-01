#ifndef _K_SYSCALL_H_
#define _K_SYSCALL_H_

#include "types.h"

/**
 * @brief Fetch the nth 32-bit system call argument.
 * 
 * @return int 
 */
int  argint(int, int*);

/**
 * @brief Fetch the nth word-sized system call argument as a null-terminated string.
 *        Copies into buf, at most max.
 * 
 * @return int string length if OK (including nul), -1 if error.
 */
int  argstr(int, char*, int);


/**
 * @brief Retrieve an argument as a pointer.
 *        Doesn't check for legality, since copyin/copyout will do that.
 * 
 * @return int 
 */
int  argaddr(int, uint64 *);


/**
 * @brief Fetch the nul-terminated string at addr from the current process.
 * 
 * @return int length of string, not including nul, or -1 for error.
 */
int  fetchstr(uint64, char*, int);

/**
 * @brief Fetch the uint64 at addr from the current process.
 * 
 * @return int 
 */
int  fetchaddr(uint64, uint64*);

void syscall();


#endif