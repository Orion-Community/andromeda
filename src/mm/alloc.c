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
	for (node = heapBase; node < heapBase+heapSize-sizeof(memNode_t); node+=ALLOC_MAX+sizeof(memNode_t))
	{
		node = initHdr(tmp, ALLOC_MAX);
		if(node != heapBase)
		{
			node->previous = lastNode;
			lastNode->next = node;
		}
		lastNode = node;
	}
}

void* alloc (size_t size, boolean pageAlligned)
{
	panic ("Memory allocation hasn't been completed");
}

int free (void* ptr)
{
	panic ("Memory returning hasn't been completed");
}