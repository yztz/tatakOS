#include <stdio.h>
#include <string.h>

#define FAT_SFN_LENGTH 11
#define MAXFILENAME 255
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

void generate_shortname(char *shortname, char *name) {
    memset(shortname, ' ', FAT_SFN_LENGTH);
    char *ext = NULL;
    
    
    for(int i = strlen(name) - 1; i >= 0; i--) {
        if(*(name + i) == '.') {
            *(name + i) = '\0';
            ext = name + i + 1;
            break;
        }
    }
    // 处理文件名
    int len = strlen(name);
    if(len > 8) {
        strncpy(shortname, name, 6);
        *(shortname + 6) = '~';
        *(shortname + 7) = '1'; // todo: 暂时先不检查重名了..
    } else {
        strncpy(shortname, name, len);
    }
    // 拷贝扩展名
    if(ext) {
        strncpy(shortname + 8, ext, min(strlen(ext), 3));
    }

    shortname[FAT_SFN_LENGTH] = '\0';
}

int main() {
    char input[MAXFILENAME];
    char short_name[FAT_SFN_LENGTH + 1];
    while(1) {
        gets(input);
        generate_shortname(short_name, input);
        printf("%s\n", short_name);
    }

    return 0;
}