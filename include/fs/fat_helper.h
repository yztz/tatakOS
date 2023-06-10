#ifndef _H_FAT_HELPER_
#define _H_FAT_HELPER_

#include "types.h"
#include "fs/fat.h"
#include "param.h"


typedef struct lookup_helper {
    const char *name;
    uint8_t checksum;
    dir_item_t *item;
    uint32_t *offset;
} lookup_helper_t;


typedef struct travs_helper {
    char *p;
    char buf[MAX_FILE_NAME];
    uint8_t checksum;
    int next;
    int is_shortname;

    travs_handler_t callback;
    void *param;
    
} travs_helper_t;


typedef struct alloc_helper {
    const char *longname;
    const char *p;
    dir_item_t *item;
    uint8_t checksum;
    const int entry_need;
    int entry_cnt;
} alloc_helper_t;



#define DEFINE_LOOKUP_HELPER(NAME, F_NAME, CHECKSUM, P_ITEM, P_OFF) \
    lookup_helper_t NAME = {.name=F_NAME,.item=P_ITEM,.offset=P_OFF,.checksum=CHECKSUM}

#define DEFINE_ALLOC_HELPER(NAME, LFN, ENT_NEED, P_ITEM, CHECKSUM) \
    alloc_helper_t NAME = {.entry_cnt=0,.longname=LFN, .p=(NAME).longname,.entry_need=ENT_NEED,.item=P_ITEM,.checksum=CHECKSUM}

#define DEFINE_TRAVS_HELPER(NAME, CALLBACK, PARAM) \
    travs_helper_t NAME = {.checksum=0,.p=(NAME).buf,.param=PARAM,.callback=CALLBACK,.next=0,.is_shortname=0}


static const int lfnOfs[] = {1, 3, 5, 7, 9,         // Name1
                            14, 16, 18, 20, 22, 24, // Name2
                            28, 30};                // Name3          

/**
 * @brief extract name from a given dir slot
 * 
 * @param slot 
 * @param buf 
 * @return int the end index of the name
 */
static int extractname(dir_slot_t *slot, char *buf) {
    uint8_t *dir = (uint8_t *)slot;
    int nullflag = 0;
    int i;  

    // only ascii suported
    for (i = 0; i < FAT_LFN_LENGTH; i++) {
        if (!nullflag) {
            char c = dir[lfnOfs[i]];
            buf[i] = c;
            nullflag = (c == '\0');
        } else {
            if (dir[lfnOfs[i]] != 0xff || dir[lfnOfs[i] + 1] != 0xff)
                return -1;
        }
    }
    
    return i - 1;
}

static void fillname(dir_slot_t *slot, alloc_helper_t *helper) {
    uint8_t *dir = (uint8_t *)slot;
    int nullflag = 0;

    for (int i = 0; i < FAT_LFN_LENGTH; i++) {
        if (!nullflag) {
            char c = *helper->p;
            dir[lfnOfs[i]] = c;
            dir[lfnOfs[i]+1] = 0;
            helper->p++;
            nullflag = (c == '\0');
        } else {
            dir[lfnOfs[i]] = 0xff;
            dir[lfnOfs[i]+1] = 0xff;
        }
    }
}


static inline void travs_helper_reset(travs_helper_t *helper) {
    helper->p = helper->buf;
    helper->next = 0;
    helper->checksum = 0;
}



#endif