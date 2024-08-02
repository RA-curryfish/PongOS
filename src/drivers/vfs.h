#pragma once
#include <stdint.h>
#include <stddef.h>
typedef enum file_type {
    FILE,
    FOLDER,
    DEVICE
} ftype;

typedef struct operations {
    int (*readfile)(char**,size_t,size_t);
    int (*writefile)(char*,size_t,size_t);

} operations_t;

typedef struct file
{
    char* fname;
    uint16_t offset;
    uint8_t flags;
    ftype type;
    uint16_t size;
    operations_t op;
} file_t;

typedef struct fat12_hdr {

} fat12_hdr_t;

// returns num of bytes read/written or error
int read(file_t* file, char** buf, size_t offset, size_t len);
int write(file_t* file, char* buf, size_t offset, size_t len);
void mount();
// same functionality as initialize
void open(file_t* file);