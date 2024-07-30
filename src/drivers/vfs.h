#pragma once
#include <stdint.h>

typedef enum file_type {
    FILE,
    FOLDER,
    DEVICE
} ftype;

typedef struct file
{
    char* fname;
    uint16_t offset;
    uint8_t flags;
    ftype type;
    uint16_t size;
} file_t;