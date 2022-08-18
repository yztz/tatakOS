#include "common.h"
#include "fs/file.h"
#include "ioctl.h"
#include "mm/vm.h"
#include "sys/termios.h"

extern struct termios term;


int ioctl(file_t *fp, uint64_t request, uint64_t arg0) {
    int pgrp = 0;
    struct termios newterm;
    switch(request) {
        case TCGETS:
            if(copy_to_user(arg0, &term, sizeof(term)) < 0)
                return -1;
            break;
        case TCSETS:
            if(copy_from_user(&newterm, arg0, sizeof(newterm)) < 0)
                return -1;
            term = newterm;
            break;
        case TIOCGPGRP:
            
            if(copy_to_user(arg0, &pgrp, sizeof(int)) < 0)
                return -1;
            break;
        case TIOCSPGRP:
        
            break;
        case TIOCGWINSZ:
            if (copy_to_user(arg0, &(struct winsize){
                .ws_row = 24,
                .ws_col = 80,
            }, sizeof(struct winsize)) < 0)
                return -1;
            break;
        /* for hwclock */
        case 0xffffffff80247009:
            break;
        default:
            return 0;
    }

    return 0;
}