#include <stdint.h>
#include "../src/libf.h"

void foo2();

void foo()
{
	uint8_t a=10;
	foo2();
	printf("ok");
}

void foo2()
{
	printf("nonsense");
}
