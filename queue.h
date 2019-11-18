#include <stdint.h>
typedef struct TCB_t TCB_t;

struct TCB_t
{
	uint8_t task_id;
	uint32_t stack_pointer;
	uint8_t state; // 0: inactive, 1: terminated, 2: blocked, 3: ready, 4: running
	uint8_t prio;  // priorities 0 to 7, with 0 being lowest (IDLE task)
	TCB_t *next_task;
};

typedef struct
{
	TCB_t *head;
	TCB_t *tail;
} queue;

void enqueue(queue *q, TCB_t *task, uint32_t *queue_vector);
TCB_t *dequeue(queue *q, uint32_t *queue_vector);
