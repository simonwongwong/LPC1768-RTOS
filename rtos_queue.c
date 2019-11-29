#include "rtos_queue.h"

extern uint32_t queue_vector;

void enqueue_ready(queue *q, TCB_t *task)
{
	// function to enqueue TCB to ready queue
	uint8_t prio = (task->mutex_prio) ? *task->mutex_prio : task->prio;
	task->state = READY; // set state to ready
	if (q->head == 0)
	{ // enqueue on empty queue
		q->head = task;
		q->tail = task;
		queue_vector |= (1 << prio); // signal non-empty in bit_vector
	}
	else
	{
		q->tail->next_task = task;
		q->tail = task;
	}
}

TCB_t *dequeue_ready(queue *q)
{
	TCB_t *task = q->head;
	uint8_t prio = (task->mutex_prio) ? *task->mutex_prio : task->prio;
	
	if (q->head == q->tail)
	{ // last in queue
		queue_vector &= ~(1 << prio); // signal empty in bit_vector
		q->head = 0;
		q->tail = 0;
	}
	else
	{
		TCB_t *temp = task->next_task;
		task->next_task = 0;
		q->head = temp;
	}
	return (task);
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
