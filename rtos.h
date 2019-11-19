#include <LPC17xx.h>
#include <stdint.h>
#include <stdio.h>
#include "rtos_queue.h"
#include "rtos_semaphore.h"

typedef void (*rtosTaskFunc_t)(void *args);

void osCreateTask(rtosTaskFunc_t func, void *args, uint8_t prio);
void osKernelInitialize(void);
void SysTick_Handler(void);
void osKernelStart(void);
__asm void PendSV_Handler(void);
