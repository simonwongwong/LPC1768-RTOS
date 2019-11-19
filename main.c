/*
 * Default main.c for rtos lab.
 * @author Andrew Morton, 2018
 */
#include "rtos.h"
#include <stdint.h>
#include <stdio.h>


Mutex print_mutex;

uint8_t tasknum[] = {1, 2, 3, 4, 5};
uint32_t test_sums[] = {0, 0, 0, 0, 0, 0};


void test_task(void *arg)
{
	uint8_t task = *(uint8_t *)arg;

	acquire_mutex(&print_mutex);
	for (int i = 0; i < 10; i++)
	{
		printf("running task %d\n", task);
		//test_sums[task]++;
	}
	bool released = release_mutex(&print_mutex);
	osThreadExit();
	while(1);
}

void release_task(void *arg)
{
	bool released = release_mutex(&print_mutex);
	if (released)
	{
		printf("successfully stole un-owned mutex");
	} 
	else 
	{
		printf("\ncould  not release mutex, wrong owner\n");
	}
	osThreadExit();
	while(1);
}


int main(void)
{
	printf("\nStarting setup...\n\n");

	osKernelInitialize();
	osCreateTask(test_task, &tasknum[3], 4);
	osCreateTask(release_task, &tasknum[4], 4);

	init_mutex(&print_mutex);
	printf("\nfinishing setup\n\n");

	osKernelStart();
}
