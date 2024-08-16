#include "../src/libf.h"

void __attribute__((naked)) main()
{
	__asm__ volatile ("push %ebp\n\t" 
	"mov %esp, %ebp");
	printf("hello");
	__asm__ volatile ("mov %ebp, %esp\n\t"
	"pop %ebp\n\t ret");
}
