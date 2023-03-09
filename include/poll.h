#ifndef _H_POLL_
#define _H_POLL_

/* Event types that can be polled for.  These bits may be set in `events'
   to indicate the interesting event types; they will appear in `revents'
   to indicate the status of the file descriptor.  */
#define POLLIN		0x001		/* There is data to read.  */
#define POLLPRI		0x002		/* There is urgent data to read.  */
#define POLLOUT		0x004		/* Writing now will not block.  */

#if defined __USE_XOPEN || defined __USE_XOPEN2K8
/* These values are defined in XPG4.2.  */
# define POLLRDNORM	0x040		/* Normal data may be read.  */
# define POLLRDBAND	0x080		/* Priority data may be read.  */
# define POLLWRNORM	0x100		/* Writing now will not block.  */
# define POLLWRBAND	0x200		/* Priority data may be written.  */
#endif

#ifdef __USE_GNU
/* These are extensions for Linux.  */
# define POLLMSG	0x400
# define POLLREMOVE	0x1000
# define POLLRDHUP	0x2000
#endif

/* Event types always implicitly polled for.  These bits need not be set in
   `events', but they will appear in `revents' to indicate the status of
   the file descriptor.  */
#define POLLERR		0x008		/* Error condition.  */
#define POLLHUP		0x010		/* Hung up.  */
#define POLLNVAL	0x020		/* Invalid polling request.  */

#include "bitops.h"

struct pollfd {
    /// @brief file descriptor
    int   fd;
    /// @brief requested events
    short events;
    /// @brief returned events
    short revents;
};

typedef struct pollfd pollfd_t;

#define FD_SETSIZE 1024

/// @brief file descriptor set
struct fdset {
    /// @brief each set bit means a fd
    unsigned long fds_bits[FD_SETSIZE / (8 * sizeof(long))];
};

typedef struct fdset fdset_t;


static inline void fdset_set(struct fdset* set, int nr) {
  set_bit(nr, (volatile unsigned long *)set);
} 

static inline void fdset_clear(struct fdset* set, int nr) {
  clear_bit(nr, (volatile unsigned long *)set);
} 

static inline int fdset_test(struct fdset* set, int nr) {
  return test_bit(nr, (volatile unsigned long *)set);
} 

#endif