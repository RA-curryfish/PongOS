#include "vfs.h"
#include "floppy.h"
#include "../libf.h"

void open(file_t* file)
{
    if(file->type == FILE) {
        // assign file related read/write operations
    }
    else if(file->type == DEVICE) {
        // only floppy for now
        file->op.readfile = &fpc_read;
        file->op.writefile = &fpc_write;
    }
}

int read(file_t* file, char** buf, size_t offset, size_t len)
{
    int error = file->op.readfile(buf,offset,len);
    if(error<0) {
        printf("File read error\n");
    }
    return error; // num of bytes read
}

int write(file_t* file, char* buf, size_t offset, size_t len)
{
    int error = file->op.writefile(buf,offset,len);
    if(error<0) {
        printf("File write error\n");
    }
    return error; // num of bytes written
}