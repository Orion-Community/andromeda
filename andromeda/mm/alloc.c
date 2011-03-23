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

memNode_t* blocks = NULL; // Head pointer of the linked list maintaining the heap


// Some random headers used later in the code
boolean useBlock(memNode_t* block);
void returnBlock(memNode_t* block);
memNode_t* split(memNode_t* block, size_t size);
memNode_t* splitMul(memNode_t* block, size_t size, boolean pageAlligned);
memNode_t* merge(memNode_t* alpha, memNode_t* beta);

#ifdef TESTA
// Debugging function used to examine the heap (duh ...)
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

// This initialises the heap to hold a block of the maximum possible size.
// In the case of the compressed kernel that's 128 MB, which is huge, since
// allocmax = 4KB
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


// Finds a block on the heap, which is free and which is large enough.
// In the case that pageAlligned is enabled the block also has to hold
// page alligned data (usefull for the page directory).
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
				// If the pointer should be page alligned, do some magic.
				// Needs to be rewritten to be readable
				#ifdef X86
				unsigned int offset = PAGEBOUNDARY-((int)carrige+sizeof(memNode_t))%PAGEBOUNDARY;
				#else
				unsigned int offset = PAGEBOUNDARY-(long long)carrige+sizeof(memNode_t)%PAGEBOUNDARY;
				#endif
				offset %= PAGEBOUNDARY;
				unsigned int blockSize = offset+size;
				#ifdef TESTA
				printf("BlockSize:\t");
				printhex(blockSize); putc('\n');
				printf("Offset:\t");
				printhex(offset); putc('\n');
				printf("Size:\t");
				printhex(size); putc('\n');
				printf("Carrige size:\t");
				printhex(carrige->size); putc('\n');
				printf("Ifresult:\t");
				printhex(carrige->size-blockSize); putc('\n');
				#endif
				if (carrige->size >= blockSize)
				{
					#ifdef TESTA
					printf("I get reached2\n");
					#endif
					memNode_t* ret = splitMul(carrige, size, TRUE);
					useBlock(ret);
					#ifdef TESTA
					printf("Size of block\n");
					printhex(ret->size); putc('\n');
					#endif
// 					#ifdef X86
// 					if (((int)((void*)ret+sizeof(memNode_t)))%PAGEBOUNDARY != 0)
// 					#else
// 					if (((long long)((void*)ret+sizeof(memNode_t)))%PAGEBOUNDARY != 0)
// 					#endif
// 					{
// 						return NULL;
// 					}
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
			// If the block is the right size or too small to hold 2 separate blocks,
			// In which one of them is the size allocated, then allocate the entire block.
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
			
			// The block is too large, it needs to be split.
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
		if (carrige->next == NULL || carrige->next == carrige)
		{
			break; 	// If we haven't found anything but we're at the end of the list
				// or heap corruption occured we break out of the loop and return
				// the default pointer (which is NULL).
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
	
	// Try to put the block back into the list of free nodes,
	// Actually claim it's free and then merge it into the others if possible.
	// This code is littered with debugging code.
	
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
			memNode_t* test = merge(block, carrige);
			if (test == NULL)
			{
				printf("Merge failed\n");
				#ifdef TESTA
				printf("After\n");
				examineHeap();
				printf("\n");
				#endif
				return -1;
			}
			else
			{
				block = test;
				carrige = test;
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
	// This code actually just sets the block to used.
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
	// This code should set the block to unused, but needs rewriting.
	block->used = FALSE;
	memNode_t* carrige;
	if ((void*)block < (void*)blocks && (void*)block >= (void*)heapBase)
	{
		block -> next = blocks;
		block -> previous = NULL;
		blocks = block;
	}
	for (carrige=blocks; carrige!=NULL; carrige=carrige->next)
	{
		if ((void*)carrige+sizeof(memNode_t)+carrige->size == block)
		{
			block -> next = carrige -> next;
			block -> previous = carrige;
			carrige -> next = block;
			return;
		}
		else if ((void*)block+sizeof(memNode_t)+block->size == carrige)
		{
			block -> next = carrige;
			block -> previous = carrige -> previous;
			carrige -> previous = block;
			return;
		}
		else if (carrige->next == NULL)
		{
			carrige -> next = block;
			block -> previous = carrige;
			return;
		}
	}
}
memNode_t* split(memNode_t* block, size_t size)
{
	// This code splits the block into two parts, the lower of which is returned
	// to alloc.
	memNode_t* second = (memNode_t*)((void*)(block)+size+sizeof(memNode_t));
	initHdr(second, block->size-size-sizeof(memNode_t));
	second->previous = block;
	second->next = block->next;
	block->next = second;
	block->size = size;
	return block;
}
memNode_t* splitMul(memNode_t* block, size_t size, boolean pageAlligned)
{
	// This code needs to be rewritten
	if (pageAlligned)
	{
		#ifdef X86
		if (((int)((void*)block+sizeof(memNode_t)))%PAGEBOUNDARY == 0)
		#else
		if (((long long)((void*)block+sizeof(memNode_t)))%PAGEBOUNDARY == 0)
		#endif
		{
			#ifdef TESTA
			printf("Simple split\n");
			#endif
			// If this block gets reached the block is at the offset in memory.
			return split (block, size);
		}
		#ifdef X86
		else if ((int)((void*)block+sizeof(memNode_t))%PAGEBOUNDARY != 0)
		#else
		else if ((long long)((void*)block+sizeof(memNode_t))%PAGEBOUNDARY != 0)
		#endif
		{
			#ifdef TESTA
			printf("Complex split\n");
			#endif
			// If we get here the base address of the block isn't alligned with the offset.
			// Split the block and then use split on the higher block so the middle is
			// pageAlligned.
			#ifdef X86
			unsigned int secondAddr;
			unsigned int base = (unsigned int)((void*)block+2*sizeof(memNode_t));
			unsigned int offset = PAGEBOUNDARY-(base%PAGEBOUNDARY);
			secondAddr = (unsigned int)((void*)block+sizeof(memNode_t));
			secondAddr += offset;
			memNode_t* second = (void*)secondAddr;
			#else
			unsigned long long secondAddr;
			unsigned long long base = (unsigned long long)((void*)block+2*sizeof(memNode_t));
			unsigned long long offset = PAGEBOUNDARY-(base%PAGEBOUNDARY);
			secondAddr = (unsigned long long)((void*)block+sizeof(memNode_t));
			secondAddr += offset;
			memNode_t* second = (void*)secondAddr;
			#endif
			memNode_t* next = block->next;
			int secondSize = block->size - ((void*)second - (void*)block);
			initHdr(second, secondSize);
			block->size = (void*)second-((void*)block+sizeof(memNode_t));
			block->next = second;
			second->previous = block;
			second->next = next;
			if (second->size>size+sizeof(memNode_t))
			{
				return split(second, size);
			}
			else
			{
				return second;
			}
		}
	}
	else
	{
		return split(block, size);
	}
}
memNode_t* merge(memNode_t* alpha, memNode_t* beta)
{
	// This code actually merges the blocks together to form a larger block, which is easier when it comes to allocating,
	// This way the blocks don't need to be merged when allocated, but they can now just be used.
	// Does some magic and needs more clarification.
	if (alpha->hdrMagic != HDRMAGIC || beta->hdrMagic != HDRMAGIC)
	{
		#ifdef TESTA
		printf("HDR error\n");
		#endif
		return NULL;
	}
	if ((alpha->next != beta) && (beta->next != alpha))
	{
		#ifdef TESTA
		printf("WARNING!!!\n");
		
		printf("Alpha->next:\t");
		printhex((int)(void*)alpha->next); putc('\n');
		printf("Beta->next:\t");
		printhex((int)(void*)beta->next); putc('\n');
		printf("\nAlpha:\t");
		printhex((int)(void*)alpha); putc('\n');
		printf("Alpha:\t");
		printhex((int)(void*)beta); putc('\n');
		#endif
		return NULL;
	}
	memNode_t* tmp;
	if ((void*)alpha+alpha->size+sizeof(memNode_t) == (void*)beta)
	{
		tmp = alpha;
		alpha = beta;
		beta = tmp;
		#ifdef TESTALLOC
		printf("Alpha\n");
		#endif
	}
	beta->size = beta->size+alpha->size+sizeof(memNode_t);
	beta->next = alpha->next;
	beta->used = FALSE;
	return beta;
}