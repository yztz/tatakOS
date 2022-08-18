#ifndef _H_IOCTL_
#define _H_IOCTL_

#include "common.h"
#include "termios.h"

// #define _IOC(a,b,c,d) ( ((a)<<29) | ((b)<<8) | (c) | ((d)<<16) )
// #define _IOC_NONE  1U
// #define _IOC_READ  2U
// #define _IOC_WRITE 4U

// #define _IO(a,b) _IOC(_IOC_NONE,(a),(b),0)
// #define _IOW(a,b,c) _IOC(_IOC_WRITE,(a),(b),sizeof(c))
// #define _IOR(a,b,c) _IOC(_IOC_READ,(a),(b),sizeof(c))
// #define _IOWR(a,b,c) _IOC(_IOC_READ|_IOC_WRITE,(a),(b),sizeof(c))

// #define TIOCSWINSZ	_IOW('t', 103, char[8])
// #define TIOCGWINSZ	_IOR('t', 104, char[8])

#define TCGETS		0x5401
#define TCSETS		0x5402
#define TCSETSW		0x5403
#define TCSETSF		0x5404
#define TCGETA		0x5405
#define TCSETA		0x5406
#define TCSETAW		0x5407
#define TCSETAF		0x5408
#define TCSBRK		0x5409
#define TCXONC		0x540A
#define TCFLSH		0x540B
#define TIOCEXCL	0x540C
#define TIOCNXCL	0x540D
#define TIOCSCTTY	0x540E
#define TIOCGPGRP	0x540F
#define TIOCSPGRP	0x5410
#define TIOCOUTQ	0x5411
#define TIOCSTI		0x5412
#define TIOCGWINSZ	0x5413
#define TIOCSWINSZ	0x5414
#define TIOCMGET	0x5415
#define TIOCMBIS	0x5416
#define TIOCMBIC	0x5417
#define TIOCMSET	0x5418


struct winsize {
    uint16 ws_row;		/* rows， in character */
    uint16 ws_col; 		/* columns, in characters */
    uint16 ws_xpixel;	/* horizontal size, pixels (unused) */
    uint16 ws_ypixel;	/* vertical size, pixels (unused) */
};


int ioctl(file_t *fp, uint64_t request, uint64_t arg0);

#endif