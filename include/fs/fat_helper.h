#ifndef _H_FAT_HELPER_
#define _H_FAT_HELPER_

#include "types.h"
#include "fs/fat.h"


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
    int length;
    dir_item_t *item;
    uint8_t checksum;
    const int entry_need;
    int entry_cnt;
} alloc_helper_t;



#define DEFINE_LOOKUP_HELPER(NAME, F_NAME, CHECKSUM, P_ITEM, P_OFF) \
    lookup_helper_t NAME = {.name=F_NAME,.item=P_ITEM,.offset=P_OFF,.checksum=CHECKSUM}

#define DEFINE_ALLOC_HELPER(NAME, LFN, LFN_LEN, ENT_NEED, P_ITEM, CHECKSUM) \
    alloc_helper_t NAME = {.entry_cnt=0,.longname=LFN,.length=LFN_LEN,.entry_need=ENT_NEED,.item=P_ITEM,.checksum=CHECKSUM}

#define DEFINE_TRAVS_HELPER(NAME, CALLBACK, PARAM) \
    travs_helper_t NAME = {.checksum=0,.p=(NAME).buf,.param=PARAM,.callback=CALLBACK,.next=0,.is_shortname=0}

/**
 * @brief extract name from a given dir slot
 * 
 * @param slot 
 * @param buf 
 * @return int the end index of the name
 */
static int extractname(dir_slot_t *slot, char *buf) {
    uint8_t *n = slot->name0_4;
    int p = 0;
 
    while(p < 5) {
        buf[p] = n[2*p];
        if(buf[p] == '\0') return p - 1;
        p++;
    }
    n = slot->name5_10;
    while(p < 11) {
        buf[p] = n[2*(p - 5)];
        if(buf[p] == '\0') return p - 1;
        p++;
    }
    n = slot->name11_12;
    while(p < 13) {
        buf[p] = n[2*(p - 11)];
        if(buf[p] == '\0') return p - 1;
        p++;
    }

    return p - 1;
}


static inline void travs_helper_reset(travs_helper_t *helper) {
    helper->p = helper->buf;
    helper->next = 0;
    helper->checksum = 0;
}


static inline uint8_t cal_checksum(char* shortname) {
    if (shortname == NULL)
        return -1;
    uint8_t sum = 0;
    for (int i = FAT_SFN_LENGTH; i != 0; i--) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *shortname++;
    }
    return sum;
}


#endif