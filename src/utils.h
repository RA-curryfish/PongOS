#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

size_t strlen(const char *s);
bool strcmp(const char *s1, const char *s2);
bool strcmp_cmd(const char *user_cmd, const char *cmd);
void memcpy(void *dest, void *source, size_t len);