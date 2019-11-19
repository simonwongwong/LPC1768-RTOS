#include "rtos_mutex.h"

void init_mutex(Mutex *m) 
{
	init_semaphore(&(m->sem), 1);
	m->owner = 0;
}

void acquire_mutex(Mutex *m) 
{
	__disable_irq();
	if (m->sem.count == 1) {
		// set owner of mutex if count is 1 before acquire
		m->owner = running_task;
	}
	wait_semaphore(&(m->sem));
	__enable_irq();
	
}

bool release_mutex(Mutex *m) 
{
	bool success;
	__disable_irq();
	if (m->owner == running_task) 
	{
		m->owner = m->sem.waitlist.head;
		signal_semaphore(&(m->sem));
		success = true;
	} 
	else 
	{
		success = false;
	}
	return success;
}
