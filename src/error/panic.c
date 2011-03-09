#include <error/panic.h>
#include <text.h>

void panic(char* msg)
{
	println("Orion panic");
	println(msg);
	for (;;);
}
