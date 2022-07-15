#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "generated/syscall.h"
#include "syscall.h"
#include "stddef.h"


typedef struct
{
    int a_type;
    union {
        long a_val;
        void *a_ptr;
        void (*a_fnc)();
    } a_un;
} auxv_t;



int main(uint64 argc, uint64 **argv) {
    printf("args list:\n");
    int i;
    for(i = 0; i < argc; i++) {
        printf("arg %d: %s\n", i, argv[i]);
    }
    printf("envs list:\n");
    char **envp = argv + argc + 1;
    for(i = 0; envp[i]; i++) {
        printf("env %d: %s\n", i, envp[i]);
    }
    printf("auxs list:\n");
    auxv_t *aux = (auxv_t *)(envp + i + 1);
    while(aux->a_type != 0) {
        printf("aux type %d: 0x%x\n", aux->a_type, aux->a_un);
        aux++;
    }

    return 0;
}