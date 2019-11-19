#include <LPC17xx.h>
#include <stdint.h>
#include <stdio.h>
#include "rtos_queue.h"
#include "rtos_semaphore.h"
#include "rtos_mutex.h"

typedef void (*rtosTaskFunc_t)(void *args);

void osCreateTask(rtosTaskFunc_t func, void *args, uint8_t prio);
void osKernelInitialize(void);
void osKernelStart(void);
void osThreadExit(void);
void SysTick_Handler(void);
__asm void PendSV_Handler(void);

