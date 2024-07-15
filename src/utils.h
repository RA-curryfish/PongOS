#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

size_t strlen(const char *s)
{
	size_t len=0;
	while(s[len++]);
	return len-1;
}

bool strcmp(const char *s1, const char *s2)
{
	size_t len1 = strlen(s1); size_t len2 = strlen(s2);
	if(len1 != len2) return false;
	for(size_t i=0;i<len1;i++) {
		if(s1[i] != s2[i]) return false;
	}
	return true;
}

bool strcmp_cmd(const char *user_cmd, const char *cmd)
{
	size_t len1 = strlen(user_cmd); size_t len2 = strlen(cmd);
	if(len1 < len2) { return false;}
	else if(len1 == len2) { return strcmp(user_cmd,cmd);}
	else {
		for(size_t i=0;i<len2;i++) {
			if(user_cmd[i] != cmd[i]) return false;
		}
		for(size_t i=len2;i<len1;i++) {
			if(user_cmd[i] != ' ' || user_cmd[i] != '\n') return false;
		}
		return true;
	}
	return false; // unreachable
}

void memcpy(void *dest, void *source, size_t len)
{
	// todo: need to handle \0
	char *d = (char*)dest;
	char *s = (char*)source;
	while(len)
	{
		*(d+len-1) = *(s+len-1);
		len--;
	}
}