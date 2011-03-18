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

#define ALLOC_MAX 4*1024*1024
#define ALLOCSIZES ALLOC_MAX/4

#define find_index(a) (a/4)
#define find_index_size(a) (a*4)

memBlock_t memory[ALLOCSIZES];

void addToList(memNode_t*,memNode_t*);

void mmapAdd(int index, memNode_t *node)
{
	memNode_t* listIndex = memory[find_index(index)].head;
	addToList(listIndex, node);
}

void initHdr(memNode_t* block, size_t size)
{
	block->size = size;
	block->previous = NULL;
	block->next = NULL;
	block->used = FALSE;
}

void initBlockMap ()
{
	int i;
	memBlock_t *frame;
	int baseAddr = heapBase;
	for (i = 0; i < ALLOCSIZES; i++)
	{
		frame = &memory[i];
		frame->size=find_index_size(i);
	}
	
	for (i=0; heapSize+heapBase > (baseAddr + ALLOC_MAX + sizeof(memNode_t)); i++)
	{
		memNode_t* tmp = (memNode_t*)baseAddr;
		initHdr(tmp, ALLOC_MAX);
		tmp -> used = FALSE;
		
		
		if (tmp == (memNode_t*)heapBase)
		{
			frame = &memory[find_index(ALLOC_MAX)];
			frame -> head = tmp;
		}
		else
		{
			mmapAdd(ALLOC_MAX, tmp);
		}
		#if DBG==1
			printf("index\taddress\n");
			printhex(i); putc('\t');
			printhex((int)tmp); printf("\n");
		#endif
		baseAddr += ALLOC_MAX+sizeof(memNode_t);
	}
	#if DBG==1
		printf("Size of header:\t");
		printhex(sizeof(memNode_t)); putc('\n');
	#endif
	 
}

void addToList(memNode_t* head, memNode_t* block)
{
	memNode_t* carrige;
	for (carrige = head; carrige != NULL; carrige=carrige->next)
	{
		if (carrige->next == NULL)
		{
			carrige->next = block;
			block->previous = carrige;
			break;
		}
	}
}

memNode_t *split(memNode_t* block, size_t size)
{
	if (block->size == size)
	{
		return block;
	}
	else
	{
		size_t tmpSize;
		memNode_t* first = block;
		memNode_t* second = block+size+sizeof(memNode_t);
		
		tmpSize = first->size;
		
		first->previous->next = first->next;
		first->next->previous = first->previous;
		
		initHdr(first, size);
		
		addToList(memory[find_index(size)].head, first);
		
		initHdr(second,tmpSize-size-sizeof(memNode_t));
		addToList(memory[find_index(second->size)].head, second);
		return first;
	}
}

void* alloc (size_t size, boolean pageAlligned)
{
	if (heapBase == 0 || heapSize == 0 || &memory[1] == NULL)
	{
		panic("Heap not initialised");
	}
	// Allocates memory from heap.
	printf("WARNING: Allocating memory without working free function!\n");
	void* ret = NULL;
	int i;
	memNode_t* block;
	
	/*
	 * The loop below looks for the right size block. If it is there is a right size block
	 * we return it. Else we continue looking.
	 */
	
	if (memory[find_index(size)].head != NULL)
	{
		for (block = memory[find_index(i*size)].head; block != NULL; block = block->next)
		{
			if (block->used == FALSE)
			{
				return (void*)(block+sizeof(memNode_t));
			}
		}
	}
	/*
	 * Let's scan every block which is at least twice the size required, or larger.
	 */
	for (i = size+size+sizeof(memNode_t); ret==NULL; i++)
	{
		if (i > ALLOC_MAX)
		{
			break;
		}
		for (block = memory[find_index(i)].head; block != NULL; block = block->next)
		{
			if (block->used == FALSE)
			{
				return (void*)(split(block, size)+sizeof(memNode_t));
			}
		}
	}
	/*
	 * This step might be considered redundant, but I personally think it should be there.
	 * This step will result in internal fragmentation, so it should be used as little as
	 * possible.
	 */
	for(i = size; i < size+size+sizeof(memNode_t); i++)
	{
		for (block = memory[find_index(i)].head; block != NULL; block = block->next)
		{
			if (block->used == FALSE)
			{
				return (void*)(block+sizeof(memNode_t));
			}
		}
	}
	// If we reach this point there is no block of memory large enough to be allocated.
	return NULL;
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