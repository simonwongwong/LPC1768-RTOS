#include "queue.h"

extern uint32_t queue_vector;

void enqueue(queue *q, TCB_t *task)
{
	if (q->head == 0)
	{ // enqueue on empty queue
		q->head = task;
		q->tail = task;
		queue_vector |= (1 << task->prio); // signal non-empty in bit_vector
	}
	else
	{
		q->tail->next_task = task;
		q->tail = task;
	}
}

TCB_t *dequeue(queue *q)
{
	TCB_t *next_task;
	if (q->head == q->tail)
	{ // last in queue
		next_task = q->head;
		queue_vector &= ~(1 << q->head->prio); // signal empty in bit_vector
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

void enqueue_waitlist(queue *q, TCB_t *task)
{
	if (q->head == 0)
	{ // enqueue on empty queue
		q->head = task;
		q->tail = task;
	}
	else
	{
		q->tail->next_task = task;
		q->tail = task;
	}
}

TCB_t *dequeue_waitlist(queue *q)
{
	TCB_t *next_task;
	if (q->head == q->tail)
	{ // last in queue
		next_task = q->head;
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
