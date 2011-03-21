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
#define PAGEBOUNDARY 0x1000

memNode_t* blocks = NULL;

boolean useBlock(memNode_t* block);
void returnBlock(memNode_t* block);
memNode_t* split(memNode_t* block, size_t size);
memNode_t* splitMiddle(memNode_t* block, size_t size, void* base);
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
	block->offset = 0;
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
		if (pageAlligned == TRUE)
		{
			panic("Page alligned allocation hasn't been implemented");
			if (!carrige->used)
			{
				#if DBG==1
				printf("I get reached!\n");
				#endif
				void* ptr = (void*) carrige;
				#ifdef X86
				ptr = (void*)((int)ptr%PAGEBOUNDARY);
				#else
				ptr = (void*)((long long)ptr%PAGEBOUNDARY);
				#endif
				if (carrige->size >= (int)((void*)ptr+size+2*sizeof(memNode_t)))
				{
					memNode_t* ret = splitMiddle(carrige, size, ptr);
				}
			}
		}
		else if (carrige->size >= size && carrige->size < size+sizeof(memNode_t))
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
			if(carrige->used!=FALSE)
			{
				continue;
			}
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
		#if DBG==1
		printf("Head:\t"); printhex(blocks); putc('\n');
		printf("Block:\t"); printhex(block); putc('\n');
		#endif
		block->used = TRUE;
		if (block->previous!=NULL)
		{
			block->previous->next = block->next;
		}
		else if (blocks == block)
		{
			blocks = block->next;
		}
		#if DBG==1
		printf("Head:\t"); printhex(blocks); putc('\n');
		printf("Block:\t"); printhex(block); putc('\n');
		#endif
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
memNode_t* splitMiddle(memNode_t* block, size_t size, void* base)
{
	memNode_t* first = block;
	memNode_t* second = NULL;
	memNode_t* third = NULL;
	
	memNode_t* previous = block->previous;
	memNode_t* next = block->next;
	
	if (base-sizeof(memNode_t) == (void*)block)
	{
		split(block, size);
		return block;
	}
	else if ((void*)(base-sizeof(memNode_t))>(void*)block && (void*)base-sizeof(memNode_t) <= (void*)block+sizeof(memNode_t))
	{
		memNode_t* previous = block->previous;
		memNode_t* next = block->next;
		first = base-sizeof(memNode_t);
		initHdr(first, size);
		first->offset = base - (void*)block;
		second = (void*)first+size+sizeof(memNode_t);
		initHdr(second, (((void*)block+size+sizeof(memNode_t))-((void*)second)));
		first->next = second;
		second->previous = first;
		first->previous = previous;
		second->next = next;
		next->previous = second;
		previous->next = first;
		return first;
	}
	else
	{
		int tmpSize = first->size;
		second = (void*)base-sizeof(memNode_t);
		first->size = ((void*)first+sizeof(memNode_t))-((void*)second+sizeof(memNode_t));
		initHdr(second, size);
		third = (void*)second+size+sizeof(memNode_t);
		int thirdSize = ((void*)first+tmpSize+sizeof(memNode_t))-((void*)third+sizeof(memNode_t));
		initHdr(third, thirdSize);
		
		first->previous = previous;
		first->next = second;
		second->previous = first;
		second->next = third;
		third->previous = second;
		third->next = next;
		
		return second;
	}
}
memNode_t* merge(memNode_t* alpha, memNode_t* beta)
{
	if (alpha->hdrMagic != HDRMAGIC || beta->hdrMagic != HDRMAGIC)
	{
		return NULL;
	}
	memNode_t* tmp;
	if ((void*)alpha+alpha->size+sizeof(memNode_t) == (void*)beta-beta->offset)
	{
		tmp = alpha;
		alpha = beta;
		beta = tmp;
	}
	if (alpha->offset != 0)
	{
		memNode_t* previous = alpha->previous;
		memNode_t* next = alpha->next;
		tmp = (void*) alpha - alpha->offset;
		initHdr(tmp, alpha->size+alpha->offset);
		tmp->previous = previous;
		tmp->next = next;
		alpha = tmp;
	}
	else if(beta->offset != 0)
	{
		memNode_t* previous = beta->previous;
		memNode_t* next = beta->next;
		tmp = (void*) beta - beta->offset;
		initHdr(tmp, beta->size+beta->offset);
		tmp->previous = previous;
		tmp->next = next;
		beta = tmp;
	}
	beta->size = beta->size+alpha->size+sizeof(memNode_t);
	beta->next = alpha->next;
	beta->used = FALSE;
	#if DBG==1
	printf("Beta\n");
	#endif
	return beta;
}