#include "task.h"
#include <stddef.h>

void init_kernel_task(pcb* task)
{
    __asm__ __volatile__ ("mov %%esp, %0" : "=r" (task->kernel_sp)); // necessary? filled during switch
    __asm__ __volatile__ ("mov %%cr3, %0" : "=r" (task->pd));
    task->next_task = NULL;
    task->task_state = RUNNING;
}

void create_task(pcb* task, uint32_t* pd)
{
    task->kernel_sp = 0x402000+16; // hardcode
    task->pd = pd;
    task->next_task = NULL;
    task->task_state = STOPPED;
}