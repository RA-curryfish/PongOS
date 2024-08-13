#include "task.h"
#include <stddef.h>
#include "libf.h"

void init_kernel_task(pcb* task)
{
    __asm__ __volatile__ ("mov %%esp, %0" : "=r" (task->kernel_sp)); // necessary? filled during switch
    __asm__ __volatile__ ("mov %%cr3, %0" : "=r" (task->pd));
    task->next_task = NULL;
    task->task_state = RUNNING;
}

void setup_kstack(pcb* task, void(*func)())
{
    uint32_t* stack_bottom = 0x403000; // 4th frame, hardcode
    uint32_t* stack_top = 0x404000;

    stack_top -= 1; *stack_top = func; // return address
    stack_top -= 1; __asm__ __volatile__ ("mov %%ebx, %0" : "=r" (*stack_top)); // ebx
    stack_top -= 1; __asm__ __volatile__ ("mov %%esi, %0" : "=r" (*stack_top)); // esi
    stack_top -= 1; __asm__ __volatile__ ("mov %%edi, %0" : "=r" (*stack_top)); // edi
    stack_top -= 1; __asm__ __volatile__ ("mov %%ebp, %0" : "=r" (*stack_top));// ebp

    task->kernel_sp = stack_top;
}

void create_task(pcb* task, uint32_t* pd, void(*func)())
{
    setup_kstack(task, func);
    task->pd = pd;
    task->next_task = NULL;
    task->task_state = STOPPED;
}