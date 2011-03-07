#include <mm/memory.h>

void paging ()
{
}

void memset(char* offset, char value, int size)
{
	char *end = offset+size;
	for (;offset < end; offset++)
	{
		*offset=value;
	}
}
