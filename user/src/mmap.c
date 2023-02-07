#include "unistd.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

/*
 * 测试成功时输出：
 * "  Hello, mmap success"
 * 测试失败时输出：
 * "mmap error."
 */
// static struct kstat kst;
// void test_mmap(void){
//     
//     char *array;
//     const char *str = "  Hello, mmap successfully!";
//     int fd;

//     fd = open("test_mmap.txt", O_RDWR | O_CREATE);
//     write(fd, str, strlen(str));
//     fstat(fd, &kst);
//     printf("file len: %d\n", kst.st_size);
//     array = mmap(NULL, kst.st_size, PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
//     //printf("return array: %x\n", array);

//     if (array == MAP_FAILED) {
// 	printf("mmap error.\n");
//     }else{
// 	printf("mmap content: %s\n", array);
// 	//printf("%s\n", str);

// 	munmap(array, kst.st_size);
//     }

//     close(fd);

//     
// }

// int main(void){
//     test_mmap();
//     return 0;
// }
static struct kstat kst;
void test_mmap(void){
    
    char *array1;
    char *array2;
    char *array3;
    const char *str = "Hello, mmap successfully!";
    int fd;

    // printf(rd("maptest begin !!\n"));
    fd = open("test_mmap.txt", O_RDWR | O_CREATE);
    write(fd, str, strlen(str));
    fstat(fd, &kst);
    printf(bl("file len: %d\n"), kst.st_size);

    /* 先关闭再打开一下，否则文件的偏移为末尾(write?)，读不出来 */
    close(fd);
    fd = open("test_mmap.txt", O_RDWR | O_CREATE);
    char buf[50];
    read(fd, buf, 24);
    printf(grn("buf: %s\n"), buf);
    printf(grn("buf: %p\n"), buf);
    /* 这里n不能指定为1，要2才行，有bug */
    read(fd, buf, 1);
    printf(grn("buf2: %s\n"), buf);


    array1 = mmap(NULL, kst.st_size, PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    array2 = mmap(NULL, 0x111 , PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    array3 = mmap(NULL, 0x1001, PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    // array4 = mmap(NULL, 0x2000, PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    // array5 = mmap(NULL, 0x200 , PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);

	printf("mmap content: %s\n", array1);
	printf("mmap content: %s\n", array2);
	printf("mmap content: %s\n", array3);
	// printf("mmap content: %s\n", array4);
	// printf("mmap content: %s\n", array5);




    // if (array == MAP_FAILED) {
	// printf("mmap error.\n");
    // }else{
	// printf("mmap content: %s\n", array);
	// //printf("%s\n", str);

	// munmap(array, kst.st_size);
    // }

//   for(;;);
  close(fd);
//   for(;;);
    
}

int main(void){
    // printf("==============maptest begin================\n");
    test_mmap();
    // printf("==============maptest end================\n");
    // for(;;);
    return 0;
}