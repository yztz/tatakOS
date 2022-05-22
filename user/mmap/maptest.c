#include "types.h"
#include "fs/stat.h"
#include "user.h"
#include "fs/fcntl.h"
#include "../src/include/debug.h"
// #include "unistd.h"


/*
 * 测试成功时输出：
 * "  Hello, mmap success"
 * 测试失败时输出：
 * "mmap error."
 */
static struct kstat kst;
void test_mmap(void){
    // TEST_START(__func__);
    char *array1;
    char *array2;
    char *array3;
    char *array4;
    char *array5;
    const char *str = "  Hello, mmap successfully!";
    int fd;

    // printf(rd("maptest begin !!\n"));
    fd = open("test_mmap.txt", O_RDWR | O_CREATE);
    write(fd, str, strlen(str));
    fstat(fd, &kst);
    printf("file len: %d\n", kst.st_size);
    array1 = mmap(NULL, kst.st_size, PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    array2 = mmap(NULL, 0x111 , PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    array3 = mmap(NULL, 0x1001, PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    array4 = mmap(NULL, 0x2000, PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    array5 = mmap(NULL, 0x200 , PROT_WRITE | PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);

	printf("mmap content: %s\n", array1);
	printf("mmap content: %s\n", array2);
	printf("mmap content: %s\n", array3);
	printf("mmap content: %s\n", array4);
	printf("mmap content: %s\n", array5);


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
    // TEST_END(__func__);
}

int main(void){
    test_mmap();
    return 0;
}