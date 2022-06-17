#ifndef _H_PARAM_
#define _H_PARAM_

#define NPROC        32  // maximum number of processes
#define NOFILE       5  // open files per process
#define NFILE        30  // open files per system
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments

#define NBUF         (30)  // size of disk block cache
#define MAXPATH      128   // maximum file path name
#define MAX_MAP      15    // maximum io map

#define BSIZE 512
#define NENTRY 40
#define MAX_FILE_NAME 20

#define USTACKSIZE 8192


/////////////MM//////////////
#if defined K210
#define MAX_ORDER 10
#elif defined QEMU
#define MAX_ORDER 15
#endif
////////////COW//////////////
#define COW


////////////DEBUG////////////


#endif