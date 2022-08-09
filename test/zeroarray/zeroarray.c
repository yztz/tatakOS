#include <stdio.h>
#include <stdlib.h>


struct array {
    int size;
    int fixed[5];
    int more[];
};

typedef struct array array_t;


array_t *new_array(int sz) {
    if(sz <= 5)
}

void print_array(array_t *array) {
    for(int i = 0; i < array->size; i++) {
        printf("%d ", array->fixed[i]);
    }
}

void expand_array(array_t *array, int sz) {
    if(sz < array->size)
        return;
    
}

int main() {
    array_t *array = malloc(sizeof(array_t));
    array->size = 5
    
}