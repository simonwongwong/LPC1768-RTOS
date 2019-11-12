#include <stdint.h>
#include <stdio.h>

typedef struct {
	uint32_t stack_pointer;
} TCB_t;

TCB_t TCB_struct[6];
uint32_t *vector_table = 0x0;


int main(void) {
	printf("main stack base pointer: %x\n", vector_table[0]);
	// initialize TCB stack pointers
	for(int i = 0; i < 6; i++) {
		TCB_struct[5-i].stack_pointer = vector_table[0] - 2048 - 1024 * i;
		printf("TCB[%d] stack pointer address: %x\n", (5-i), TCB_struct[5-i].stack_pointer);
	}
	
}
