#include <stdint.h>
#include "queue.h"

typedef struct
{
	int32_t count;
	queue *waitlist;
} Semaphore;

void init(Semaphore *s, int32_t count)
{
	s->count = count;
	s->waitlist->head = 0;
	s->waitlist->tail = 0;
}

void wait(Semaphore *s)
{
	__disable_irq();
	s->count--;
	if (s->count <= 0) {
		//enqueue(&(s->waitlist), 
	}
}

void signal(Semaphore *s);