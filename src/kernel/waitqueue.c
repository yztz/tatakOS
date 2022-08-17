#include "kernel/waitqueue.h"



// int wq_is_empty(wq_t *self) {
//     return list_empty(&self->head);
// }

// void *__wq_poll(wq_t *self) {
//     if(wq_is_empty(self)) 
//         return NULL;
// }