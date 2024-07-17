#include "hal.h"
#include "idt.h"

void init_hal()
{
    // do something
    idt_initialize();
}