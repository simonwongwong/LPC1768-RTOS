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
	if (m->sem.count < 1 && running_task->prio > m->highest_prio) {
		// if another task owns the mutex with a lower prio, promote owner
		m->highest_prio = running_task->prio;
		enqueue_ready(&queue_list[running_task->prio], m->owner);
	}

	wait_semaphore(&(m->sem));
	m->owner = running_task;
	m->highest_prio = max_of(running_task->prio, m->highest_prio);
	running_task->mutex_prio = &(m->highest_prio);
	

	__enable_irq();
	
}

bool release_mutex(Mutex *m) 
{
	__disable_irq();
	bool success;
	
	if (m->owner == running_task) {
		m->owner = m->sem.waitlist.head;
		running_task->mutex_prio = 0;
		
		// if running_task was not highest prio, highest prio doesn't change
		if (running_task->prio == m->highest_prio) {
			m->highest_prio = 0;
			// find next highest prio. could be faster using a heap
			// but i'm not going to write one in C :)
			TCB_t *node = m->owner;
			while(node) {
				m->highest_prio = max_of(m->highest_prio, node->prio);
				node = node->next_task;
			}
		}
		
		signal_semaphore(&(m->sem));
		success = true;
	} else {
		success = false;
	}
	__enable_irq();
	return success;
}
