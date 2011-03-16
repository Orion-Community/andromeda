/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <mm/memory.h>
#include <mm/heap.h>
#include <error/panic.h>
#include <text.h>

#define ALLOCSIZES 0xFFFFF
#define ALLOC_MAX ALLOCSIZES*4
#define find_index(a) (a*4)

memBlock_t memory[ALLOCSIZES];

void mmapAdd(int index, memNode_t *node)
{
	memNode_t* listIndex = memory[find_index(index)].head;
}

void initBlockMap ()
{
	 int i;
	 memBlock_t *frame;
	 int baseAddr = heapBase;
	 for (i = 0; i < ALLOCSIZES; i++)
	 {
		frame = &memory[i];
		frame->size=i*4+4;
	 }
	 for (i=0; i < heapSize/find_index(ALLOCSIZES); i++)
	 {
		memNode_t* tmp = (memNode_t*)baseAddr;
		tmp -> used = FALSE;
		
		if (tmp = (memNode_t*)heapBase)
		{
			frame = &memory[find_index(ALLOC_MAX)];
			frame -> head = tmp;
		}
		else
		{
			mmapAdd(ALLOC_MAX, tmp);
		}
		printf("index\taddress\n");
		printhex(i); putc('\n');
		printhex((int)tmp); printf("\n\n");
		baseAddr += ALLOC_MAX;
	 }
	 
}

void* alloc (size_t size, boolean pageAlligned)
{
	if (heapBase == 0 || heapSize == 0)
	{
		panic("Heap not initialised");
	}
	// Allocates memory from heap.
}

int free (void* ptr)
{
	// Return the memory to the heap.
}