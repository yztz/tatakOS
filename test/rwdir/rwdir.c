#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

int main(){
  int fd;
  fd = open("./dir",  O_DIRECTORY) ;
  printf("fd: %d\n", fd);
  char a[123];
  int ret = read(fd, a, 120);
  a[121] = '\0';
  printf("%s\n%d\n", a, ret);
  return 0;
  // const char *b = "123";
  // write(fd, b, 1);
}