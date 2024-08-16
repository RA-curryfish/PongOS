#include "cmds.h"
#include "libf.h"

void help(const char* cmd_list[], uint8_t sz)
{
    printf("The list of commands are:\n");
    // printf("%d", sz);
    for(uint8_t i=0;i<sz;i++) {
        printf("%s\n", cmd_list[i]);
    }
}

void text()
{
    // code to create a txt file? or open another file
}

void cmd_initialize()
{
	//???
}