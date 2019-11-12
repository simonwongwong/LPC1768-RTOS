/*
 * Default main.c for rtos lab.
 * @author Andrew Morton, 2018
 */
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
	uint8_t task_id;
	uint32_t stack_pointer;
} TCB_t;

TCB_t TCB_struct[2];

uint32_t *vector_table = 0x0;
uint32_t curr_sp;
uint32_t next_sp;

void osKernelInitialize() {
	printf("main stack base pointer: %x\n", vector_table[0]);
	// initialize TCB stack pointers
	for(int i = 0; i < 2; i++) {
		
		TCB_struct[1-i].stack_pointer = vector_table[0] - 2048 - 1024 * i;
		printf("TCB[%d] stack pointer address: %x\n", (1-i), TCB_struct[1-i].stack_pointer);
	}
	curr_sp = TCB_struct[0].stack_pointer;
	next_sp = TCB_struct[1].stack_pointer;
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
	// create 2 TCBs with stack pointers
	// set curr_sp and next_sp to TCB[0] and TCB[1], respectively
	osKernelInitialize();
	PendSV_Handler();
		
}
