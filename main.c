/*
 * Default main.c for rtos lab.
 * @author Andrew Morton, 2018
 */
#include "rtos.h"
#include <stdint.h>
#include <stdio.h>


Semaphore test_sem;

uint8_t tasknum[] = {1, 2, 3, 4, 5};
uint32_t test_sums[] = {0, 0, 0, 0, 0, 0};


void test_task(void *arg)
{
	uint8_t task = *(uint8_t *)arg;

	wait_semaphore(&test_sem);
	for (int i = 0; i < 10; i++)
	{
		printf("running task %d\n", task);
		//test_sums[task]++;
	}
	
	signal_semaphore(&test_sem);
	osThreadExit();
	while(1);
}


int main(void)
{
	printf("\nStarting setup...\n\n");

	osKernelInitialize();
	osCreateTask(test_task, &tasknum[0], 1);
	osCreateTask(test_task, &tasknum[1], 3);
	osCreateTask(test_task, &tasknum[2], 3);
	osCreateTask(test_task, &tasknum[3], 4);
	osCreateTask(test_task, &tasknum[4], 4);
	init_semaphore(&test_sem, 1);
	printf("\nfinishing setup\n\n");

	osKernelStart();
}
