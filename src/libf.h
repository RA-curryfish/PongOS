#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#define FLAG_SET(x, flag) x |= (flag)
#define FLAG_UNSET(x, flag) x &= ~(flag)
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

size_t strlen(const char *s);
bool strcmp(const char *s1, const char *s2);
bool strcmp_cmd(const char *user_cmd, const char *cmd);
void memset(void *dest, char c, size_t len);
void memcpy(void *dest, void *source, size_t len);
void* malloc(size_t sz);
/*  binary search on structures
    Params:
    arr - array of sorted structs
    item_disp - Displacement of item from base of the struct
    item - item to search
    low - left limit
    high - right limit
    elem_size - size of each structure element
    desc - wheter arr sorted in asc or desc
*/
uint32_t bin_search(void* arr, uint8_t item_disp, uint32_t item, uint32_t low, uint32_t high, uint8_t elem_size, bool desc);
void printf(const char *format, ...);