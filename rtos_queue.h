#ifndef RTOS_QUEUE_H
#define RTOS_QUEUE_H

#include <stdint.h>
typedef struct TCB_t TCB_t;
typedef enum state {INACTIVE, TERMINATED, BLOCKED, READY, RUNNING} thread_state_t;

struct TCB_t
{
	uint8_t task_id;
	uint32_t stack_pointer;
	thread_state_t state; // 0: inactive, 1: terminated, 2: blocked, 3: ready, 4: running
	uint8_t prio;  // priorities 0 to 7, with 0 being lowest (IDLE task)
	TCB_t *next_task;
};

typedef struct
{
	TCB_t *head;
	TCB_t *tail;
} queue;

void enqueue_ready(queue *q, TCB_t *task);
TCB_t *dequeue_ready(queue *q);
void enqueue_waitlist(queue *q, TCB_t *task);
TCB_t *dequeue_waitlist(queue *q);

#endif
