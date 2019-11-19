#include "rtos_semaphore.h"
#include <stdbool.h>
extern TCB_t *running_task;

typedef struct {
    Semaphore sem;
    TCB_t *owner;
} Mutex;

void init_mutex(Mutex *m);
void acquire_mutex(Mutex *m);
bool release_mutex(Mutex *m);
