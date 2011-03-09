#include <mm/memory.h>

void paging ()
{
	panic("Paging wasn't initialised!");
}

void memset(int* offset, int value, int size)
{
	int i = 0;
	for (; i <= size; i++)
	{
		offset[i] = value;
	}
}
