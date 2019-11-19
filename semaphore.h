#ifndef RTOS_SEMAPHORE_H
#define RTOS_SEMAPHORE_H
#include "queue.h"

extern TCB_t *running_task;
extern queue queue_list[];

typedef struct
{
	int32_t count;
	queue waitlist;
} Semaphore;

void init_semaphore(Semaphore *s, int32_t count);
void wait_semaphore(Semaphore *s);
void signal_semaphore(Semaphore *s);

#endif
