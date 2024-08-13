#include "task.h"
#include <stddef.h>
#include "libf.h"

void foo()
{
    printf("TESTNG\n");
    while(true);
}

void init_kernel_task(pcb* task)
{
    __asm__ __volatile__ ("mov %%esp, %0" : "=r" (task->kernel_sp)); // necessary? filled during switch
    __asm__ __volatile__ ("mov %%cr3, %0" : "=r" (task->pd));
    task->next_task = NULL;
    task->task_state = RUNNING;
}

void setup_kstack(pcb* task)
{
    uint32_t* stack_bottom = 0x403000; // 4th frame, hardcode
    uint32_t* stack_top = 0x404000;

    stack_top -= 1; *stack_top = foo; // return address
    stack_top -= 1; *stack_top = 0x1; // ebx
    stack_top -= 1; *stack_top = 0x2; // esi
    stack_top -= 1; *stack_top = 0x3; // edi
    stack_top -= 1; *stack_top = 0x4; // ebp

    task->kernel_sp = stack_top;
}

void create_task(pcb* task, uint32_t* pd)
{
    setup_kstack(task);
    task->pd = pd;
    task->next_task = NULL;
    task->task_state = STOPPED;
}