#include "cmds.h"
#include "libf.h"
#include "drivers/vfs.h"
#include "phy_mem_mgr.h"
#include "virt_mem_mgr.h"
#include "task.h"
#include "load_proc.h"

pcb* KERNEL_TASK;

void help(const char* cmd_list[], uint8_t sz)
{
    printf("Command List\n");
    for(uint8_t i=0;i<sz;i++) {
        printf("%s\n", cmd_list[i]);
    }
}

void text()
{
    // code to create a txt file? or open another file
}

void pong()
{
    file_t* f = (file_t*)ph_malloc(sizeof(file_t));
	vas_t* vas = (vas_t*)ph_malloc(sizeof(vas_t)); 
	f->type = DEVICE;

    load(f,vas);

    pcb* task = (pcb*)ph_malloc(sizeof(pcb));
	create_task(task,KERNEL_TASK,0, (void (*)())vas->code_begin, vas->stack_begin);

    KERNEL_TASK->next_task = task;
    switch_task(KERNEL_TASK,task);
	
    ph_free((uintptr_t)f);
	ph_free((uintptr_t)task);
    printf("outside??");
}

void cmd_initialize(void* kernel_task)
{
    KERNEL_TASK = (pcb*)kernel_task;
}