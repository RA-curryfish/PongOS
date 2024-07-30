#pragma once
#include "isr.h"
#include <stdint.h>

typedef void (*irq_handler)(registers* regs); // func ptrs to irq

void irq_initialize();
void irq_register_handler(uint8_t irq, irq_handler handler);