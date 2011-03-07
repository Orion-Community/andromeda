#include <boot/mboot.h>
#include <kern/sched.h>
#include <text.h>

void announce()
{
	println("Starting the kernel");
}

int kmain ()
{
	announce();
	paging();
	// Start decompression
	// Start initial (kernel) process
	sched();
	for (;;);
}
