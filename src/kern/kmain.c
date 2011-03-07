#include <boot/mboot.h>
#include <text.h>

void announce()
{
	println("Starting the kernel");
}

int kmain ()
{
	announce();
}
