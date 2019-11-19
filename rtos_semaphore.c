#include <stdint.h>
#include "rtos_semaphore.h"

void init_semaphore(Semaphore *s, int32_t count)
{
	s->count = count;
	s->waitlist.head = 0;
	s->waitlist.tail = 0;
}

void wait_semaphore(Semaphore *s)
{
	__disable_irq();
	s->count--;
	if (s->count < 0) {
		running_task->state = 2; // blocked
		enqueue_waitlist(&(s->waitlist), running_task);
		__enable_irq();
		while(running_task->state < 3); // prevent from continuing prematurely
	}
	__enable_irq();
}

void signal_semaphore(Semaphore *s)
{
	__disable_irq();
	s->count++;
	if (s->count <= 0)
	{
		TCB_t *unblock = dequeue_waitlist(&(s->waitlist));
		unblock->state = 3; // ready
		enqueue(&queue_list[unblock->prio], unblock); // requeue
	}
	__enable_irq();
}
