/*
 * Default main.c for rtos lab.
 * @author Andrew Morton, 2018
 */
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

uint32_t msTicks = 0;

void SysTick_Handler(void) {
    msTicks++;
}

__asm void PendSV_Handler(void) {
    
   	// return from handler
	BX		LR
}

int main(void) {
	SysTick_Config(SystemCoreClock/1000);
	printf("\nStarting...\n\n");
	
	uint32_t period = 1000; // 1s
	uint32_t prev = -period;
	while(true) {
		if((uint32_t)(msTicks - prev) >= period) {
			printf("tick ");
			prev += period;
		}
	}
		
}
