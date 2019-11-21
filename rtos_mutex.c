#include "rtos_mutex.h"
#define max_of(x,y) ((x) >= (y)) ? (x) : (y)

void init_mutex(Mutex *m) 
{
	init_semaphore(&(m->sem), 1);
	m->owner = 0;
	m->highest_prio = 0;
}

void acquire_mutex(Mutex *m) 
{
	__disable_irq();
	if (m->sem.count == 1) {
		// set owner of mutex if count is 1 before acquire
		m->owner = running_task;
	}
	m->highest_prio = max_of(m->highest_prio, running_task->prio);
	
	wait_semaphore(&(m->sem));
	__enable_irq();
	
}

bool release_mutex(Mutex *m) 
{
	bool success;
	__disable_irq();
	if (m->owner == running_task) {
		m->owner = m->sem.waitlist.head;
		m->highest_prio = 0;
		// find next highest prio. could be faster using a heap
		// but i'm not going to write one in C :)
		TCB_t *node = m->owner;
		while(node) {
			m->highest_prio = max_of(m->highest_prio, node->prio);
			node = node->next_task;
		}
		
		signal_semaphore(&(m->sem));
		success = true;
	} else {
		success = false;
	}
	return success;
}
