#include <error/panic.h>
#include <text.h>

void panic()
{
	println("Orion panic");
	for (;;);
}
