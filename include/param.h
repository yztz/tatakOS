#ifndef _H_PARAM_
#define _H_PARAM_

#define NPROC        64  // maximum number of processes
#define NOFILE       16  // open files per process
#define NFILE       30  // open files per system
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       1000  // size of file system in blocks
#define MAXPATH      128   // maximum file path name
#define MAX_MAP      15    // maximum io map

#define BSIZE 512
#define NENTRY 40
#define MAX_FILE_NAME 20


/////////////MM//////////////
#define BUDDY
#define MAX_ORDER 10

////////////COW//////////////
#define COW


////////////DEBUG////////////


#endif