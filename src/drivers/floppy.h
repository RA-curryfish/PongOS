#pragma once
#include <stdint.h>

uint8_t floppy_init();
void floppy_read(void* buf, uint32_t dev_loc, uint32_t sz);
void floppy_write(void* buf, uint32_t dev_loc, uint32_t sz);