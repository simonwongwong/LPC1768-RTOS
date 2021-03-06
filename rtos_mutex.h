#include "rtos_semaphore.h"
#include <stdbool.h>
extern TCB_t *running_task;
extern queue queue_list[8];

typedef struct {
    Semaphore sem;
    uint8_t highest_prio;
    TCB_t *owner;
} Mutex;

void init_mutex(Mutex *m);
void acquire_mutex(Mutex *m);
bool release_mutex(Mutex *m);
