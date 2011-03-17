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
	 printf("Test\n");
	 int i;
	 memBlock_t *frame;
	 int baseAddr = heapBase;
	 for (i = 0; i < ALLOCSIZES; i++)
	 {
		frame = &memory[i];
		frame->size=find_index(i)+4;
	 }
	 println("checkpoint 0");
	 for (i=0; i < heapSize/(find_index(ALLOCSIZES)); i++)
	 {
		//println("checkpoint 1");
		memNode_t* tmp = (memNode_t*)baseAddr;
		tmp -> used = FALSE;
		
		//println("checkpoint 2");
		
		if (tmp == (memNode_t*)heapBase)
		{
			frame = &memory[find_index(ALLOC_MAX)];
			frame -> head = tmp;
		//	println("checkpoint 3");
		}
		else
		{
			mmapAdd(ALLOC_MAX, tmp);
		//	println("checkpoint 4");
		}
		printf("index\taddress\n");
		printhex(i); putc('\t');
		printhex((int)tmp); printf("\n");
		baseAddr += ALLOC_MAX;
		int x = 0;
	 }
	 
}

void* alloc (size_t size, boolean pageAlligned)
{
	if (heapBase == 0 || heapSize == 0 || &memory[1] == NULL)
	{
		panic("Heap not initialised");
	}
	// Allocates memory from heap.
	
	/*
	 * Step 1: Find a block the right or a multiplication of the right size.
	 * Step 2: If the block is a multiplication of the right size, devide it into
	   a right size block and a remnant block.
	 * Step 3: Correct the linked lists as they have been broken.
	 * Step 4: Mark the right size block as used and return it's data pointer.
	 */
}

int free (void* ptr)
{
	// Return the memory to the heap.
	/*
	 * Step 1: Mark the block unused.
	 * Step 2: Find the pre-ceeding and succeeding blocks.
	 * Step 3: Merge the blocks, if free.
	 * Step 4: return success.
	 */
}