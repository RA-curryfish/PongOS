#include "libf.h"
#include "drivers/terminal.h"

size_t strlen(const char *s)
{
	size_t len=0;
	while(s[len++]);
	return len-1;
}

void strrev(char *s)
{
	size_t len = strlen(s);
	int8_t l=0, r=len-1;
	while(l<=r) {
		char tmp = s[l];
		s[l] = s[r];
		s[r] = tmp;
		l++;r--;
	}
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
		if(user_cmd[len2] != ' ' && user_cmd[len2] != '\n') return false; // user cmd must end or have param after space
		// for(size_t i=len2;i<len1;i++) {
		// 	if(user_cmd[i] != ' ' || user_cmd[i] != '\n') return false;
		// }
		return true;
	}
	return false; // unreachable
}

void memset(void *dest, char c, size_t len)
{
	char *d = (char*)dest;
	while(len) {
		*(d+len-1) = c;
		len--;
	}
}

void memcpy(void *dest, void *source, size_t len)
{
	// todo: need to handle \0
	char *d = (char*)dest;
	char *s = (char*)source;
	while(len) {
		*(d+len-1) = *(s+len-1);
		len--;
	}
}

uint32_t bin_search(void* arr, uint8_t item_disp, uint32_t item, uint32_t low, uint32_t high, uint8_t elem_size, bool desc)
{
	// todo: fix this returning 2047???
	uint32_t l = low, r = high;
	uint32_t ans = high+1;
	// change the base to item you're looking for
	arr += item_disp;
	while (l<=r) {
		uint32_t m = l + (r-l)/2;

		if(*(uint32_t*)(arr+m*elem_size) == item) return m;
		
		// todo: cleaner code here
		if(!desc) {
			if(*(uint32_t*)(arr+m*elem_size) > item) {
				r = m-1;
				ans = r; // best fit that is higher than searched item
			} 
			else l = m+1;
		}
		else {
			if(*(uint32_t*)(arr+m*elem_size) < item) {
				r = m-1;
			} 
			else {
				l = m+1;
				ans = l;
			}
		}
	}
	return ans;
}

void itoa(char* buf, char b, int val)
{
	uint8_t base = (b=='d')?10:((b=='x')?16:10); // default to base 10 for now
	int tmp = val; 
	if (tmp==0) {
		buf[1] = '\0'; buf[0]='0';
		return;
	}
	char char_map[16] = "0123456789ABCDEF";
	uint8_t idx=0;
	while(tmp) {
		buf[idx] = char_map[tmp%base];
		tmp /= base;
		idx++;
	}
	if(b=='x') {
		buf[idx++]='x';
		buf[idx++]='0';
	}
	strrev(buf);
	buf[idx]='\0';
}

void printf(const char *format, ...)
{
	char **arg = (char **) &format;
	int c;
	char buf[20];
	memset(buf,'\0',20);
	arg++;

	while ((c = *format++) != 0) {
    	if (c != '%') printchar(c);
      	else {
			char *p, *p2;
			int pad0 = 0, pad = 0;
			c = *format++;

			if (c == '0') {
				pad0 = 1;
				c = *format++;
			}
			if (c >= '0' && c <= '9') {
				pad = c - '0';
				c = *format++;
			}
			switch (c) {
				case 'd':
				case 'u':
				case 'x':
				  itoa (buf, c, *((int *) arg++));
				  p = buf;
				  goto string;
				  break;
				case 's':
					p = *arg++;
					if (! p)
					p = "(null)";
				string:
					for (p2 = p; *p2; p2++);
					for (; p2 < p + pad; p2++) printchar(pad0 ? '0' : ' ');
					while (*p) printchar(*p++);
					break;
				default: // %c
					printchar(*((int *) arg++));
					break;
			}
		}
    }
}