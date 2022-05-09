#ifndef _H_FS_
#define _H_FS_

#include "types.h"
#include "atomic/sleeplock.h"

#define E_ISDIR(entry) (((entry)->attr & 0b10000) > 0)

//IN RAM
struct fat_entry {
    uint        dev;        /* 所属设备号 */
    uint        ref;        /* 引用数 */

    uint8_t     attr;       /* 文件属性 */

    uint16_t	ctime;		/* 创建时间 */
	uint16_t	cdate;		/* 创建日期 */
	uint16_t	adate;		/* 最后访问日期 */
    uint16_t	mtime;		/* 修改时间 */
	uint16_t	mdate;		/* 修改日期 */

    uint32_t    file_size;  /* 文件大小 */

    uint32_t    clus_start; /* 起始簇号 */

    spinlock_t  lock;       /* 保护对象字段 */
};


#endif