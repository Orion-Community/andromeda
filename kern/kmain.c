#include <textio.h>
#include <sys/stdlib.h>
#include <sys/io.h>

void kmain(void)
{
	clearscreen();
	print("Kernel loaded!", 1);

	char status = inb(0x60);
	
	if((status & 2) == 2)
	{
		print("This A20 gate is opened.", 2);
	}
	halt();
}



