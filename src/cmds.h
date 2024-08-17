#pragma once
#include <stdint.h>
#include <stdarg.h>

void help(const char* cmd_list[], uint8_t sz);
void text();
void pong();
void cmd_initialize(void* kernel_task);