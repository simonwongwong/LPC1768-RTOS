#include "queue.h"

void enqueue(queue *q, TCB_t *task, uint32_t *qv)
{
	if (q->head == 0)
	{ // empty queue
		q->head = task;
		q->tail = task;
		*qv |= (1 << task->prio); // signal non-empty in bit_vector
	}
	else
	{
		q->tail->next_task = task;
		q->tail = task;
	}
}

TCB_t *dequeue(queue *q, uint32_t *qv)
{
	TCB_t *next_task;
	if (q->head == q->tail)
	{ // last in queue
		next_task = q->head;
		*qv &= ~(1 << q->head->prio); // signal empty in bit_vector
		q->head = 0;
		q->tail = 0;
	}
	else
	{
		TCB_t *temp = q->head->next_task;
		next_task = q->head;
		q->head->next_task = 0;
		q->head = temp;
	}
	return (next_task);
}
