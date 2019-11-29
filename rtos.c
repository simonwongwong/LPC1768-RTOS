#include "rtos.h"
#define max_of(x,y) ((x) >= (y)) ? (x) : (y)

TCB_t *running_task;
queue queue_list[8];
TCB_t TCB_array[6];
uint32_t *curr_sp;
uint32_t *next_sp;
uint32_t queue_vector = 1;
uint8_t num_tasks = 1;

void SysTick_Handler(void)
{
	// requeue running task if state is ready or running
	if (running_task->state >= READY)
	{
		uint8_t task_prio = (running_task->mutex_prio) ? *running_task->mutex_prio : running_task->prio;
		enqueue_ready(&queue_list[task_prio], running_task);
	}
	
	// determine next task from queue
	uint32_t next_queue;
	TCB_t *next_task;
	do {
		next_queue = 31 - (uint8_t)__clz(queue_vector);
		next_task = dequeue_ready(&queue_list[next_queue]);
	} while (next_task->state < READY);
	
	next_sp = &(next_task->stack_pointer);
	

	// update running task
	running_task = next_task;
	running_task->state = RUNNING;

	// invoke PendSV exception
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}


void osCreateTask(rtosTaskFunc_t func, void *args, uint8_t prio)
{
	// assign TCB
	TCB_t *task_tcb = &TCB_array[num_tasks++];

	// assign priority
	task_tcb->prio = prio;
	task_tcb->mutex_prio = 0;
	// set addr of PSR to base of stack
	uint32_t *PSR = (uint32_t *)task_tcb->stack_pointer;

	// set stack_pointer to point to R4 (top of stack)
	task_tcb->stack_pointer = (uint32_t)(PSR - 15);

	// set *PSR default value
	*PSR = 0x01000000;
	// set PC value
	*(PSR - 1) = (uint32_t)func;
	// set R0 value
	*(PSR - 7) = (uint32_t)args;
	task_tcb->next_task = 0;

	// add to queue
	enqueue_ready(&queue_list[prio], task_tcb);
}

void osKernelInitialize(void)
{
	uint32_t *vector_table = 0x0;
	uint8_t task_id;
	// initialize TCB stack pointers
	for (int i = 0; i < 6; i++)
	{
		task_id = 5 - i;
		TCB_array[task_id].task_id = task_id;
		TCB_array[task_id].stack_pointer = vector_table[0] - 2048 - 1024 * i;
		TCB_array[task_id].state = INACTIVE;
		TCB_array[task_id].next_task = 0;
	}
	// initialize queue pointers
	for (int i = 0; i < 8; i++)
	{
		queue_list[i].head = 0;
		queue_list[i].tail = 0;
	}
}

void osKernelStart(void)
{
	uint32_t *vector_table = 0x0;
	// set MSP to main stack base addr
	__set_MSP(vector_table[0]);
	// get Control Register and set bit 1 to 1
	__set_CONTROL(__get_CONTROL() | CONTROL_SPSEL_Msk);

	// set PSP to point to idle task (task 0)
	curr_sp = &TCB_array[0].stack_pointer;
	__set_PSP(*curr_sp);

	// first task should be IDLE task
	running_task = &TCB_array[0];

	// set interrupt priorities for SysTick_Handler and PendSV_Handler
	NVIC_SetPriority(SysTick_IRQn, 0x00);
	NVIC_SetPriority(PendSV_IRQn, 0xff);

	// configure SysTick_Handler to invoke every 100ms
	SysTick_Config(SystemCoreClock / 10);

	// transform into idle task (0)
	while (1)
	{
		//printf("in idle task\n");
	}
}

void osThreadExit(void)
{
	__disable_irq();
	// set state to 1 so it won't be requeued anywhere, effectively terminated
	running_task->state = TERMINATED;
	__enable_irq();
}

void osThreadYield(void)
{
	__disable_irq();
	running_task->state = READY;
	__enable_irq();

	// finish timeslice
	while(running_task->state < RUNNING);
	
}


__asm void PendSV_Handler(void)
{
	CPSID i // disable interrupts
	MRS R0, MSP // store MSP into R0
	MRS R1, PSP // store PSP into R1
	MOV R13, R1	// move PSP into SP
	PUSH {R4 - R11} // Push registers onto current stack
	LDR R1, = __cpp(&curr_sp) // load address of curr_sp global variable into R1
	LDR R5,	[R1] 
	STR R13, [R5] // store stack pointer
	LDR R2, = __cpp(&next_sp) // load address of next_sp into R1
	LDR R3, [R2] 
	LDR R13, [R3] // load next_sp (stack pointer) into R13
	STR R3, [R1] // change curr_sp to point to new task
	POP {R4 - R11} // pop registers from next stack
	MOV R1,	R13
	MSR PSP, R1 // save PSP after POP
	MSR MSP, R0 // restore MSP from R0
	CPSIE i // enable interrupts
	BX LR
}
