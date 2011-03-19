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

memNode_t* blocks = NULL;

int useBlock(memNode_t* block);
void returnBlock(memNode_t* block);
memNode_t* split(memNode_t* block, size_t size);
memNode_t* merge(memNode_t* alpha, memNode_t* beta);

void initHdr(memNode_t* block, size_t size)
{
	block->size = size;
	block->previous = NULL;
	block->next = NULL;
	block->used = FALSE;
}

void initBlockMap ()
{
	memNode_t* node;
	memNode_t* lastNode;
// 	#if sizeof(void*) == 4
	for (node = (memNode_t*)heapBase; node < (memNode_t*)heapBase+heapSize-sizeof(memNode_t); node+=(ALLOC_MAX+sizeof(memNode_t)))
// 	#endif
	{
		initHdr(node, ALLOC_MAX);
		if(node != (memNode_t*)heapBase)
		{
			node->previous = lastNode;
			lastNode->next = node;
		}
		else
		{
			blocks=node;
		}
		lastNode = node;
	}
}

void* alloc (size_t size, boolean pageAlligned)
{
	panic ("Memory allocation hasn't been completed");
	
	memNode_t* carrige;
	for(carrige = blocks; carrige->next!=NULL; carrige=carrige->next)
	{
		if (carrige->size >= size || carrige->size < 2*size+sizeof(memNode_t))
		{
			if (useBlock(carrige) == 1)
			{
				continue;
			}
			return (void*)(carrige+sizeof(memNode_t));
		}
		else if(carrige->size >= 2*size+sizeof(memNode_t))
		{
			memNode_t* tmp = split(carrige, size);
			if(useBlock(tmp) == 1)
			{
				continue;
			}
			return (void*)(tmp+sizeof(memNode_t));
		}
	}
	
	return NULL;
}

int free (void* ptr)
{
	panic ("Memory returning hasn't been completed");
}

int useBlock(memNode_t* block)
{
	if(!block->used)
	{
		block->used = TRUE;
		if (block->previous!=NULL)
		{
			block->previous->next = block->next;
		}
		if (block->next!=NULL)
		{
			block->next->previous = block->previous;
		}
		return 0;
	}
	else
	{
		return -1;
	}
}
void returnBlock(memNode_t* block)
{
	block->used = FALSE;
	memNode_t* carrige;
	memNode_t* tmp;
	for (carrige = blocks; carrige!=NULL; carrige=carrige->next)
	{
		if (carrige->next == NULL)
		{
			block->previous = carrige;
			block->next = NULL;
			carrige->next = block;
			return;
		}
		if (carrige > block)
		{
			continue;
		}
		tmp = carrige->previous;
		block->next = carrige;
		carrige->previous=block;
		tmp->next = block;
		block->previous=tmp;
		
	}
}
memNode_t* split(memNode_t* block, size_t size)
{
	memNode_t* second = block+size+sizeof(memNode_t);
	initHdr(second, block->size-size-sizeof(memNode_t));
	second->previous = block;
	second->next = block->next;
	block->next = second;
	block->size = size;
	return block;
}