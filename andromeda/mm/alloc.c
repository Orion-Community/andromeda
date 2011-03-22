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
memNode_t* splitMul(memNode_t* block, size_t size, int base);
memNode_t* merge(memNode_t* alpha, memNode_t* beta);

#ifdef TESTA

void examineHeap()
{
	printf("Head\n");
	printhex((int)blocks); putc('\n');
	memNode_t* carrige;
	for (carrige = blocks; carrige!=NULL; carrige=carrige->next)
	{
		printf("node: "); printhex((int)carrige); putc('\n');
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
			if (!carrige->used)
			{
				#ifdef TESTA
				printf("I get reached1\n");
				#endif
				#ifdef X86
				int offset = PAGEBOUNDARY-((int)carrige+sizeof(memNode_t))%PAGEBOUNDARY;
				#else
				int offset = PAGEBOUNDARY-(long long)carrige+sizeof(memNode_t)%PAGEBOUNDARY;
				#endif
				offset %= PAGEBOUNDARY;
				int blockSize = offset+size;
				#ifdef TESTA
				printf("BlockSize:\t");
				printhex(blockSize); putc('\n');
				printf("Offset:\t");
				printhex(offset); putc('\n');
				printf("Size:\t");
				printhex(size); putc('\n');
				#endif
				if (carrige->size >= blockSize)
				{
					#ifdef TESTA
					printf("I get reached2\n");
					#endif
					memNode_t* ret = splitMul(carrige, size, offset);
					useBlock(ret);
					#ifdef TESTA
					printf("Size of block\n");
					printhex(ret->size); putc('\n');
					#endif
					return (void*)ret+sizeof(memNode_t);
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}
		else if (carrige->size >= size && carrige->size < size+sizeof(memNode_t))
		{
			if (useBlock(carrige) == TRUE)
			{
				continue;
			}
			#ifdef TESTALLOC
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
			#ifdef TESTALLOC
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
	#ifdef TESTA
	printf("Before:\n");
	examineHeap();
	printf("\n");
	#endif
	returnBlock(block);
	#ifdef TESTA
	printf("During:\n");
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
	#ifdef TESTA
	printf("After\n");
	examineHeap();
	printf("\n");
	#endif
	
	return 0;
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
		else if (blocks == block)
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
			#ifdef TESTA
			printf("Trying to reset head");
			#endif
			block->previous = NULL;
			block->next = blocks;
			blocks=block;
			return;
		}
		if ((void*)carrige->next == NULL)
		{
			#ifdef TESTA
			printf("Inserted block on end of line");
			#endif
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
		break;
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
memNode_t* splitMul(memNode_t* block, size_t size, int base)
{
	memNode_t* first = block;
	memNode_t* second = NULL;
	memNode_t* third = NULL;
	
	memNode_t* previous = block->previous;
	memNode_t* next = block->next;
	
	int offset = 0;
	int oldSize = block->size;
	
	if ((void*)block+base <= (void*)block+sizeof(memNode_t))
	{
		offset = (void*)block+sizeof(memNode_t) - (void*)block+base;
		memNode_t* tmp = (void*)block+offset;
		initHdr(tmp, size);
		first = tmp;
		first->previous = previous;
		if ((void*)block+oldSize+sizeof(memNode_t)<=(void*)first+size+sizeof(memNode_t))
		{
			second = (void*)first+size+sizeof(memNode_t);
			initHdr(second, (void*)block+oldSize-(void*)second);
			first->next = second;
			second->previous = first;
			second->next = next;
		}
		else
		{
			first->next = next;
		}
		return first;
	}
	else
	{
		second = (void*)first+base;
		initHdr(second, size);
		first->size = (void*)second-((void*)first+sizeof(memNode_t));
		first->next=second;
		first->previous = previous;
		if ((void*)block+oldSize >= (void*)second+second->size+sizeof(memNode_t))
		{
			third = (void*)second+second->size+sizeof(memNode_t);
			initHdr(third, ((void*)block+oldSize+sizeof(memNode_t))-((void*)second+second->size+sizeof(memNode_t)));
			third->previous = second;
			second->next = third;
			third->next = next;
		}
		else
		{
			second->next = next;
		}
		return second;
	}
}
memNode_t* merge(memNode_t* alpha, memNode_t* beta)
{
	if (alpha->hdrMagic != HDRMAGIC || beta->hdrMagic != HDRMAGIC)
	{
		return NULL;
	}
	if (alpha->next != beta || beta->next != alpha)
	{
		return NULL;
	}
	memNode_t* tmp;
	if ((void*)alpha+alpha->size+sizeof(memNode_t) == (void*)beta-beta->offset)
	{
		tmp = alpha;
		alpha = beta;
		beta = tmp;
		#ifdef TESTALLOC
		printf("Alpha\n");
		#endif
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
	return beta;
}