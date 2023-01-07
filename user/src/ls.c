#include "unistd.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#pragma GCC optimize ("O0")

#define NUM (17 * 4096)
static int data[NUM];

void
ls(char *path)
{

  char buf[512];
  int fd;
  struct linux_dirent64 *de = (struct linux_dirent64 *)buf;

  if((fd = open(path, 0)) < 0){
    printf("ls: cannot open %s\n", path);
    return;
  }
  int nread;

  while(1) {
    if((nread = getdents(fd, (struct linux_dirent64 *)buf, 512)) == -1) {
      printf("ls: getdentes fail\n");
      break;
    }

    if(nread == 0) break;

    int pos = 0;

    while(pos < nread) {
      de = (struct linux_dirent64 *)&buf[pos];
      printf("%s\n", de->d_name);
      pos += de->d_reclen;
    }
  }
  int flag = 1;
  for(int i = 0; i < NUM; i++) {
    if(data[i]) {
      flag = 0;
      break;
    }
  }
  if(flag)
    printf("ok!!\n");
  else 
    printf("panic!!\n");

  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
