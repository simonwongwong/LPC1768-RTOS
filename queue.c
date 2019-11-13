#include "queue.h"

void enqueue(queue *q, TCB_t *task, uint32_t *bv) {
	if(q->head == 0) { // empty queue
		q->head = task;
		q->tail = task;
		*bv |= (1 << task->prio); // signal non-empty in bit_vector
	} else {
		q->tail->next_task = task;
		q->tail = task;
	}
}

uint8_t dequeue(queue *q, uint32_t *bv){
	uint8_t task_id;
	if (q->head == q->tail) { // last in queue
		task_id = q->head->task_id;
		*bv &= ~(1 << q->head->prio); // signal empty in bit_vector
		q->head = 0;
		q->tail = 0;
	} else {
		TCB_t *temp = q->head->next_task;
		task_id = q->head->task_id;
		q->head->next_task = 0;
		q->head = temp;
	}
	return(task_id);
}



