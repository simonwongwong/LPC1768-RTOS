/*
 * Default main.c for rtos lab.
 * @author Andrew Morton, 2018
 */
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef void (*rtosTaskFunc_t)(void *args);

typedef struct {
	uint8_t task_id;
	uint32_t stack_pointer;
	uint8_t state; // 0: inactive, 1: terminated, 2: blocked, 3: ready, 4: running
	uint8_t prio; // priorities 0 to 7, with 0 being lowest (IDLE task)
} TCB_t;

TCB_t TCB_struct[6];

uint32_t *vector_table = 0x0;
uint32_t curr_sp;
uint32_t next_sp;
uint8_t num_tasks = 0;
uint8_t curr_task = 0;

uint32_t msTicks = 0;

void osIdleTask(void *arg) {
	(void) arg;
	for(;;) {}
}

void osCreateTask(rtosTaskFunc_t func, void *args, uint8_t prio){
	// assign TCB
	TCB_t *task_tcb = &TCB_struct[num_tasks++];
	
	// assign priority
	task_tcb->prio = prio;
	
	// set addr of PSR to base of stack
	uint32_t *PSR = (uint32_t *)task_tcb->stack_pointer;
	// set all registers (PSR to R4) to 0
	for (int i = 0; i < 16; i++) {
		*(PSR - i) = 0;
	}
	// set stack_pointer to point to R4 (top of stack)
	task_tcb->stack_pointer = (uint32_t)(PSR - 15);
	
	// set *PSR default value
	*PSR = 0x01000000;
	// set PC value
	*(PSR - 1) = (uint32_t)func;
	// set R0 value
	*(PSR - 7) = (uint32_t)args;
	
	// TODO: add task_id to appropriate ready queue
}

void osKernelInitialize() {
	printf("main stack base pointer: %x\n", vector_table[0]);
	uint8_t task_id;
	// initialize TCB stack pointers
	for(int i = 0; i < 6; i++) {
		task_id = 5 - i;
		TCB_struct[task_id].task_id = task_id;
		TCB_struct[task_id].stack_pointer = vector_table[0] - 2048 - 1024 * i;
		TCB_struct[task_id].state = 0;
		printf("TCB[%d] stack pointer address: %x\n", task_id, TCB_struct[5-i].stack_pointer);
	}
	
	// create idle task at TCB 0
	osCreateTask(osIdleTask, NULL, 0);
	
}

void osKernelStart() {
	// set MSP to main stack base addr
	__set_MSP(vector_table[0]);
	// get Control Register and set bit 1 to 1
	uint32_t control_register = __get_CONTROL() | (1<<1) ;
	__set_CONTROL(control_register);
	
	// set PSP to point to idle task (task 0)
	__set_PSP(TCB_struct[0].stack_pointer);
	// set curr_sp to task 0
	curr_sp = TCB_struct[0].stack_pointer;
	
	// set interrupt priorities for SysTick_Handler and PendSV_Handler
	NVIC_SetPriority(SysTick_IRQn, 0x00);
	NVIC_SetPriority(PendSV_IRQn, 0xff);
	
	// configure SysTick_Handler to invoke every 1s
	SysTick_Config(SystemCoreClock); 
}


void SysTick_Handler(void) {
	// determine next sp
	// set next_sp to next task's stack pointer
	// TODO: use vectors
	
	// invoke PendSV interrupt
	
}

__asm void PendSV_Handler(void) {
  // Push registers onto current stack
	PUSH {R4-R11}
	// load address of curr_sp global variable into R1
	LDR R1, =__cpp(&curr_sp)
	// store stack pointer into curr_sp variable
	STR R13, [R1]
	// load address of next_sp into R1
	LDR R1, =__cpp(&next_sp)
	// load next_sp (stack pointer) into R13
	LDR R13, [R1]
	// pop registers from next stack back into the stack
	POP {R4-R11}
	BX		LR
}

int main(void) {
	
	printf("\nStarting...\n\n");
	
	osKernelInitialize();
	// create threads
	osKernelStart();
		
}
