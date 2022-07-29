#ifndef _H_PARAM_
#define _H_PARAM_

#define NPROC        32  // maximum number of processes
#define NOFILE       5  // open files per process
#define NFILE        30  // open files per system
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXENV       32  // max exec arguments

#define NBUF         (30)  // size of disk block cache
#define MAXPATH      128   // maximum file path name
#define MAX_MAP      15    // maximum io map

#define MAX_FD       16

#define BSIZE 512
#define NENTRY 10
#define MAX_FILE_NAME 64

#define USTACKSIZE (4096 * 10)
#define UHEAPSIZE 4096


/////////////MM//////////////
#define MAX_ORDER 10



////////////DEBUG////////////


#endif