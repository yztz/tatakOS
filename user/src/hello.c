#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "stdlib.h"

int main() {
    // The string to be written
    char txt[] = "Hello World!\n";
    // Read Buffer
    char buffer[20];
    // File descriptor & tmp
    int fd, n;

    // Create & Open file
    fd = open("test.txt", O_CREATE|O_RDWR);
    assert(fd >= 0);
    printf("1) Open test.txt fd is %d\n", fd);
    // Write file
    n = write(fd, txt, strlen(txt));
    assert(n >= 0);
    printf("2) Return of write is %d\n", n);
    // Close file
    close(fd);
    // re-open file
    fd = open("test.txt", O_RDONLY);
    assert(fd >= 0);
    printf("3) Open test.txt fd is %d\n", fd);
    // Read file
    n = read(fd, buffer, sizeof(buffer));
    assert(n >= 0);
    printf("4) Return of read is %d\n", n);
    printf("5) Read content is: %s", buffer);
    // Close file
    close(fd);

    return 0;
}