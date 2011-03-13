#include <mm/memory.h>
#include <types.h>

int requestPage(int i)
{
	return -1;
}

void initPaging (int *heapPtr, int size)
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

int *alloc (int size, boolean pageAlligned)
{
	// Allocates memory from heap.
}

int free (void* ptr)
{
	// Return the memory to the heap.
}
