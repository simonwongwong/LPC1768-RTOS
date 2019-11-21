#include "rtos.h"
#include <stdint.h>
#include <stdio.h>


Mutex demo_mutex;
Mutex demo_mutex2;
Semaphore demo_semaphore;

uint8_t tasknum[] = {0, 1, 2, 3, 4, 5};
uint32_t test_sums[] = {0, 0, 0, 0, 0, 0};

void test_fpp_task(void *arg)
{
	for (int i = 0; i < 30; i++)
	{
		uint8_t tasknum = *(uint8_t *)arg;
		printf("in task %d\n", tasknum);
	}
	osThreadExit();
	while (1);
}

void test_inherit_prio_1(void *arg)
{
	uint8_t task = *(uint8_t *)arg;
	// get mutex
	printf("Task 1: Acquiring mutex\n");
	acquire_mutex(&demo_mutex);
	// release task 2 and 3 by signalling semaphore
	printf("Task 1: releasing Task 2 and Task 3\n");
	__disable_irq();
	signal_semaphore(&demo_semaphore);
	signal_semaphore(&demo_semaphore);
	__enable_irq();
	
	osThreadYield();
	
	printf("Task 1: received prio upgrade!\n");
	for (int i = 0; i < 10; i++)
	{
		printf("running task %d\n", task);
	}
	bool released = release_mutex(&demo_mutex);
	osThreadExit();
	while(1);
}

void test_inherit_prio_2(void *arg)
{
	uint8_t task = *(uint8_t *)arg;
	
	// wait on semaphore to let task 1 run
	printf("Task 2: blocking on Semaphore\n");
	wait_semaphore(&demo_semaphore);
	
	for (int i = 0; i < 30; i++)
	{	
		printf("running task %d\n", task);
	}
	osThreadExit();
	while (1);
}

void test_inherit_prio_3(void *arg)
{
	uint8_t task = *(uint8_t *)arg;

	// wait on semaphore to let task 1 run
	printf("Task 3: blocking on Semaphore\n");
	wait_semaphore(&demo_semaphore);
	
	// block on Semaphore to upgrade Task 1 prio
	printf("Task 3: blocking on Mutex\n");
	acquire_mutex(&demo_mutex);
	
	for (int i = 0; i < 10; i++)
	{
		printf("running task %d\n", task);
	}
	release_mutex(&demo_mutex);
	osThreadExit();
	while(1);
}

void test_mutex_task(void *arg)
{
	uint8_t task = *(uint8_t *)arg;

	acquire_mutex(&demo_mutex);
	for (int i = 0; i < 30; i++)
	{
		printf("running task %d\n", task);
	}
	bool released = release_mutex(&demo_mutex);
	osThreadExit();
	while(1);
}

void release_task(void *arg)
{
	uint8_t task = *(uint8_t *)arg;
	bool released = release_mutex(&demo_mutex);
	printf("\nTask %d: trying to release mutex...\n", task);
	if (released)
	{
		printf("successfully stole un-owned mutex");
	} 
	else 
	{
		printf("Could  not release mutex, wrong owner\n");
	}
	printf("Blocking on mutex owned by task %d\n", demo_mutex.owner->task_id);
	acquire_mutex(&demo_mutex);
	for (int i = 0; i < 30; i++)
	{
		printf("running task %d\n", task);
	}
	osThreadExit();
	while(1);
}

void test_semaphore_task(void *arg)
{
	uint8_t task = *(uint8_t *)arg;

	wait_semaphore(&demo_semaphore);
	for (int i = 0; i < 30; i++)
	{
		printf("running task %d\n", task);
	}
	signal_semaphore(&demo_semaphore);
	osThreadExit();
	while(1);
}


void demo_mutex_ownership(void)
{
	// task 1 will acquire mutex 
	// task 2 will attempt to release mutex owned by task 1 (unsuccessfully)
	// task 2 will then block on mutex
	osCreateTask(test_mutex_task, &tasknum[1], 4);
	osCreateTask(release_task, &tasknum[2], 4);
	init_mutex(&demo_mutex);
}
void demo_fpp(void)
{
	// these tasks will pre-empt each other if same prio
	// task 4 will run, and get pre-empted by task 5
	// when task 4 and 5 are terminated, 2 and 3 will run round-robin
	// when tasks 2 and 3 are terminated, task 1 will run until terminated
	// idle task will run finally
	osCreateTask(test_fpp_task, &tasknum[1], 1);
	osCreateTask(test_fpp_task, &tasknum[2], 3);
	osCreateTask(test_fpp_task, &tasknum[3], 3);
	osCreateTask(test_fpp_task, &tasknum[4], 4);
	osCreateTask(test_fpp_task, &tasknum[5], 4);
}

void demo_semaphore_usage(void)
{
	// three tasks will be created 
	// two will be allowed to run at one time using a semaphore
	// task 3 will not be able to print until task 1 signals
	osCreateTask(test_semaphore_task, &tasknum[1], 4);
	osCreateTask(test_semaphore_task, &tasknum[2], 4);
	osCreateTask(test_semaphore_task, &tasknum[3], 4);
	init_semaphore(&demo_semaphore, 2);
}

void demo_inherit_prio(void)
{
	// task 2 has higher prio than task 1
	// task 2 will be pre-empted by task 1 because task 3 is blocked on task 1
	osCreateTask(test_inherit_prio_1, &tasknum[1], 1);
	osCreateTask(test_inherit_prio_2, &tasknum[2], 2);
	osCreateTask(test_inherit_prio_3, &tasknum[3], 3);
	init_mutex(&demo_mutex);
	init_semaphore(&demo_semaphore, 0);
}
	


int main(void)
{
	// uncomment one demo function to see it work
	printf("\nStarting setup...\n\n");
	osKernelInitialize();
	//demo_mutex_ownership();
	//demo_fpp();
	//demo_semaphore_usage();
	demo_inherit_prio();
	printf("\nfinishing setup\n\n");

	osKernelStart();
}
