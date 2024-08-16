#include "task.h"
#include <stddef.h>
#include "libf.h"
#include "virt_mem_mgr.h" 

uint32_t* CUR_TASK_ADDR;

void task_end()
{
    printf("TASK END\n");
    while(1);
}

void task_begin(uint32_t* task)
{
    printf("TASK BEGIN\n");
    *CUR_TASK_ADDR = task;   
}

void init_kernel_task(uint32_t* cur_task_addr, pcb* k_task)
{
    __asm__ __volatile__ ("mov %%esp, %0" : "=r" (k_task->kernel_sp)); // necessary? filled during switch
    __asm__ __volatile__ ("mov %%cr3, %0" : "=r" (k_task->pd));
    k_task->next_task = NULL;
    k_task->task_state = RUNNING;
    CUR_TASK_ADDR = cur_task_addr;
}

void setup_kstack(pcb* task, void(*task_entry)(), uint8_t* stack_begin)
{
    uint32_t* stack_top = (uint32_t*)stack_begin;

    stack_top -= 1; *stack_top = (uint32_t)task; // param1 to task_begin
    stack_top -= 1; *stack_top = (uint32_t)task_end; // return address to task_end
    stack_top -= 1; *stack_top = (uint32_t)task_entry; // return address to task
    stack_top -= 1; *stack_top = (uint32_t)task_begin; // return address to exch tasks
    stack_top -= 1; __asm__ __volatile__ ("mov %%ebx, %0" : "=r" (*stack_top)); // ebx
    stack_top -= 1; __asm__ __volatile__ ("mov %%esi, %0" : "=r" (*stack_top)); // esi
    stack_top -= 1; __asm__ __volatile__ ("mov %%edi, %0" : "=r" (*stack_top)); // edi
    stack_top -= 1; __asm__ __volatile__ ("mov %%ebp, %0" : "=r" (*stack_top)); // ebp

    task->kernel_sp = stack_top;
}

void create_task(pcb* new_task, pcb* next_task, uint32_t* pd, void(*func)(), uint8_t* stack_begin)
{
    setup_kstack(new_task, func, stack_begin);
    new_task->pd = pd;
    new_task->next_task = next_task;
    new_task->task_state = STOPPED;
}