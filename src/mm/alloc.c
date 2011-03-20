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

#define HDRMAGIC 0xAF00BEA8

memNode_t* blocks = NULL;

boolean useBlock(memNode_t* block);
void returnBlock(memNode_t* block);
memNode_t* split(memNode_t* block, size_t size);
memNode_t* merge(memNode_t* alpha, memNode_t* beta);

#if DBG==1

void examineHeap()
{
	printf("Head\n");
	printhex(blocks); putc('\n');
	memNode_t* carrige;
	for (carrige = blocks; carrige!=NULL; carrige=carrige->next)
	{
		printf("node: "); printhex((void*)carrige); putc('\n');
	}
}

#endif

void initHdr(memNode_t* block, size_t size)
{
	block->size = size;
	block->previous = NULL;
	block->next = NULL;
	block->used = FALSE;
	block->hdrMagic = HDRMAGIC;
}

void initBlockMap ()
{
	memNode_t* node = (memNode_t*)heapBase;
	initHdr(node, heapSize-sizeof(memNode_t));
	blocks = node;
// 	printhex(node->size); putc('\n');
// 	printhex(blocks); putc('\n');
}

void* alloc (size_t size, boolean pageAlligned)
{
	if(size > ALLOC_MAX)
	{
		return NULL;
	}
	memNode_t* carrige;
	for(carrige = blocks; carrige!=NULL; carrige=carrige->next)
	{
		if (carrige->size >= size && carrige->size < size+sizeof(memNode_t))
		{
			if (useBlock(carrige) == TRUE)
			{
				continue;
			}
			#if DBG==1
			printf("Size of block\n");
			printhex(carrige->size); putc('\n');
			#endif
			return (void*)carrige+sizeof(memNode_t);
		}
		else if(carrige->size >= size+sizeof(memNode_t))
		{
			memNode_t* tmp = split(carrige, size);
			if(useBlock(tmp) == TRUE)
			{
				continue;
			}
			#if DBG==1
			printf("Size of block\n");
			printhex(tmp->size); putc('\n');
			#endif
			return (void*)tmp+sizeof(memNode_t);
		}
		if (carrige->next == NULL)
		{
			break;
		}
	}
	
	return NULL;
}

int free (void* ptr)
{
	memNode_t* block = (void*)ptr-sizeof(memNode_t);
	memNode_t* carrige;
	if (block->hdrMagic != HDRMAGIC)
	{
		return -1;
	}
	
	returnBlock(block);
	#if DBG==1
	printf("Before:\n");
	examineHeap();
	printf("\n");
	#endif
	for (carrige = blocks; carrige!=NULL; carrige=carrige->next)
	{
		if ((void*)block+block->size+sizeof(memNode_t) == (void*)carrige || (void*)carrige+carrige->size+sizeof(memNode_t) == (void*)block)
		{
			if (merge(block, carrige) == NULL)
			{
				printf("Merge failed\n");
			}
		}
	}
	#if DBG==1
	printf("After\n");
	examineHeap();
	printf("\n");
	#endif
	
	// Now return the block to the heap.
}

boolean useBlock(memNode_t* block)
{
	if(block->used == FALSE)
	{
		block->used = TRUE;
		if (block->previous!=NULL)
		{
			block->previous->next = block->next;
		}
		else
		{
			blocks = block->next;
		}
		if (block->next!=NULL)
		{
			block->next->previous = block->previous;
		}
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
void returnBlock(memNode_t* block)
{
	block->used = FALSE;
	memNode_t* carrige;
	memNode_t* tmp;
	for (carrige = blocks; carrige!=NULL; carrige=carrige->next)
	{
		if ((void*)block == (void*)heapBase)
		{
			block->previous = NULL;
			block->next = blocks;
			blocks=block;
			return;
		}
		if ((void*)carrige->next == NULL)
		{
			block->previous = carrige;
			block->next = NULL;
			carrige->next = block;
			return;
		}
		if (carrige < block)
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
	memNode_t* second = (memNode_t*)((void*)(block)+size+sizeof(memNode_t));
	initHdr(second, block->size-size-sizeof(memNode_t));
	second->previous = block;
	second->next = block->next;
	block->next = second;
	block->size = size;
	return block;
}
memNode_t* merge(memNode_t* alpha, memNode_t* beta)
{
	if (alpha->hdrMagic != HDRMAGIC || beta->hdrMagic != HDRMAGIC)
	{
		return NULL;
	}
	if ((void*)alpha+alpha->size+sizeof(memNode_t) == (void*)beta)
	{
		alpha->size = alpha->size+beta->size+sizeof(memNode_t);
		alpha->next = beta->next;
		alpha->used = FALSE;
		#if DBG==1
		printf("Alpha\n");
		#endif
		return alpha;
	}
	else if ((void*)beta+beta->size+sizeof(memNode_t) == (void*)alpha)
	{
		beta->size = beta->size+alpha->size+sizeof(memNode_t);
		beta->next = alpha->next;
		beta->used = FALSE;
		#if DBG==1
		printf("Beta\n");
		#endif
		return beta;
	}
	return NULL;
}