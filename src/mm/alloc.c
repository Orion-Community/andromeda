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

// This code still needs improvement.
/*
 * Right now the code assumes all of the memory is available, as in reality
 * it isn't. The memory needs to be limited, to what is available.
 * This requires growing the heap dynamically to it's maximum size, if possible.
 * We need the multi-boot-header for that though, and that requires a little
 * more investigation.
 */

#include <stdlib.h>
#include <thread.h>

#include "kern/cpu.h"

#define HDRMAGIC 0xAF00BEA8
#define PAGEBOUNDARY 0x1000

volatile memNode_t* blocks; // Head pointer of the linked list maintaining the heap

volatile mutex_t prot;


// Some random headers used later in the code
inline static boolean useBlock(volatile memNode_t* block);
inline static void returnBlock(volatile memNode_t* block);
inline static volatile memNode_t* split(volatile memNode_t* block, size_t size);
inline static volatile memNode_t* splitMul(volatile memNode_t* block, size_t size, boolean pageAlligned);
inline static volatile memNode_t* merge(volatile memNode_t* alpha, volatile memNode_t* beta);

// Debugging function used to examine the heap
void examineHeap()
{
	printf("Head\n0x%X\n", (int) blocks);
	volatile memNode_t* carige;
	for (carige = blocks; carige!=NULL; carige=carige->next)
	{
		printf("node: 0x%X\tsize: 0x%X\n", (int)carige, carige->size);
	}
}

// This code is called whenever a new block header needs to be created.
// It initialises the header to a good position and simplifies the code a bit.
void initHdr(volatile memNode_t* block, size_t size)
{
	block->size = size;
	block->previous = NULL;
	block->next = NULL;
	block->used = FALSE;
	block->hdrMagic = HDRMAGIC;
}

void *realloc(void* ptr, size_t size)
{
  void* new = alloc(size, FALSE);
  volatile memNode_t* ptrInfo = ptr - sizeof(memNode_t);
  size_t currentSize = ptrInfo->size;
  memcpy(new, ptr, (size > currentSize) ? currentSize : size);
  return new;
}

void* nalloc(size_t size)
{
  void* tmp = alloc(size, FALSE);
  if (tmp != NULL);
    memset(tmp, 0, size);
  return tmp;
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
	mutexEnter(prot);
	volatile memNode_t* carige;
	for(carige = blocks; carige!=NULL; carige=carige->next)
	{
		if (pageAlligned == TRUE)
		{
			if (!carige->used)
			{
				/* 
				 * If the block isn't used and the block should be alligned with the page boundary
				 * this block is called. The ifdef below is just to get the syntax right for different
				 * kind of architectures.
				 *
				 * The code figures out the required offset for the block to be able to hold the desired
				 * block.
				 */
				unsigned long offset = PAGEBOUNDARY-((long)carige+sizeof(memNode_t))%PAGEBOUNDARY;
				offset %= PAGEBOUNDARY;
				unsigned long blockSize = offset+size;
				
				if (carige->size >= blockSize) // if the size is large enough to be split into
								// page alligned blocks, then do it.
				{
					volatile memNode_t* ret = splitMul(carige, size, TRUE); // Split the block
					useBlock(ret); // Mark the block as used

					//return the desired block
					mutexRelease(prot);
					return (void*)ret+sizeof(memNode_t);
				}
				else
				{
					// The block isn't the right size so find another one.
					continue;
				}
			}
			else
			{
				// The block is used, which may be the case in multi threaded environments.
				// This means the other thread isn't done with this block and we need to
				// leave it alone. This code still isn't thread safe, but it's a start.
				continue;
			}
		}
		else if (carige->size >= size && carige->size < size+sizeof(memNode_t))
		{
			if (useBlock(carige) == TRUE) // check the usage of the block
			{
				continue;
			}
			// If the block is the right size or too small to hold 2 separate blocks,
			// In which one of them is the size allocated, then allocate the entire block.
			mutexRelease(prot);
			return (void*)carige+sizeof(memNode_t);
		}
		else if(carige->size >= size+sizeof(memNode_t)) // the block is too large
		{
			if(carige->used!=FALSE) // assert that the block isn't used
			{
				continue;
			}
			
			volatile memNode_t* tmp = split(carige, size);  // split the block
			if(useBlock(tmp) == TRUE) // mark the block used.
			{
				continue;
			}
			mutexRelease(prot);
			return (void*)tmp+sizeof(memNode_t);
		}
		if (carige->next == NULL || carige->next == carige)
		{
			printf("Allocation at end of list!\nblocks: %X\tCarrige: %X\tnext: %X\n", (int)blocks, (int)carige, (int)carige->next);
			if (carige->next == carige)
			  printf("Loop in list!\n");
			break; 	// If we haven't found anything but we're at the end of the list
				// or heap corruption occured we break out of the loop and return
				// the default pointer (which is NULL).
		}
	}
	mutexRelease(prot);
	return NULL;
}

int free (void* ptr)
{
	#ifdef MMTEST
	printf("Free!!!\n");
	#endif
	if (ptr == NULL)
	  return -1;
	mutexEnter(prot);
	volatile memNode_t* block = (void*)ptr-sizeof(memNode_t);
	volatile memNode_t* carige;
	if (block->hdrMagic != HDRMAGIC)
	{
		mutexRelease(prot);
		return -1;
	}
	
	// Try to put the block back into the list of free nodes,
	// Actually claim it's free and then merge it into the others if possible.
	// This code is littered with debugging code.
	
	// Debugging code
	#ifdef MMTEST
	printf("Before:\n");
	examineHeap();
	printf("\n");
	#endif
	returnBlock(block); // actually mark the block unused.
	// more debugging code
	#ifdef MMTEST
	printf("During:");
	examineHeap();
	printf("\n");
	#endif
	// Now find a place for the block to fit into the heap list
	for (carige = blocks; carige!=NULL && carige->next != carige && 
       carige->next != blocks; carige=carige->next) // Loop through the heap list
	{

		// if we found the right spot, merge the lot.
		if ((void*)block+block->size+sizeof(memNode_t) == (void*)carige || (void*)carige+carige->size+sizeof(memNode_t) == (void*)block)
		{
			volatile memNode_t* test = merge(block, carige); // merging code
			if (test == NULL) // if the merge failed
			{
				printf("Merge failed\n");
				#ifdef MMTEST
				printf("After\n");
				examineHeap();
				printf("\n");
				wait();
				#endif
				mutexRelease(prot);
				return -1;
			}
			else
			{
    		block = test;
				carige = test;
				// We can now continue trying to merge the rest of the list, which might be possible.
			}
		}
	}
	// Even more debugging code
	#ifdef MMTEST
	printf("After\n");
	examineHeap();
	printf("\n");
	#endif
	mutexRelease(prot);
	return 0; // Return success
}

inline static boolean useBlock(volatile memNode_t* block)
{
	// mark the block as used and remove it from the heap list
	if(block->used == FALSE)
	{
		block->used = TRUE;
		if (block->previous!=NULL) // if we're not at the top of the list
		{
			block->previous->next = block->next; // set the previous block to hold the next block
		}
		else if (blocks == block) // if we are at the top of the list, move the top of the list to the next block
		{
			blocks = block->next;
		}
		if (block->next!=NULL) // if we're not at the end of the list
		{
			block->next->previous = block->previous; // set the next block to hold the previous block
		}
		// Over here the block should be removed from the heap lists.
		return FALSE; // return that the block wasn't used.
	}
	else
	{
		return TRUE;
	}
}
inline static void returnBlock(volatile memNode_t* block)
{
	// This code marks the block as unused and puts it back in the list.
	if (block->hdrMagic != HDRMAGIC) // Make sure we're not corrupting the heap
	{
		#ifdef MMTEST
		printf("WARNING\n");
		#endif
		return;
	}
	block->used = FALSE;
	volatile memNode_t* carige;
	if ((void*)block < (void*)blocks)
	{// if we're at the top of the heap list add the block there.
		blocks -> previous = block;
		block -> next = blocks;
		block -> previous = NULL;
		blocks = block;
		return;
	}
	// We're apparently not at the top of the list
	for (carige=blocks; carige!=NULL; carige=carige->next) // Loop through the heap list.
	{
		if ((void*)carige+sizeof(memNode_t)+carige->size <= (void*)block) // if the carige connects to the bottom of our block
		{
			block -> next = carige -> next;
			block -> previous = carige;
			carige -> next = block;

			return; // add the block to the list after the carige
		}
		else if ((void*)block+sizeof(memNode_t)+block->size <= (void*)carige) // if the block connects to the bottom of the carige
		{
			block -> next = carige;
			block -> previous = carige -> previous;
			carige -> previous = block;
			return; // add the block to the list before the carige
		}
		else if (carige->next == NULL)
		{
			carige -> next = block;
			block -> previous = carige;
      carige->next->next = NULL;
			return; // if we have gotten to the end of the heap we must add the block here
		}
	}
}
inline static volatile memNode_t* split(volatile memNode_t* block, size_t size)
{
	// This code splits the block into two parts, the lower of which is returned
	// to the caller.

	volatile memNode_t* second = (volatile memNode_t*)((void*)(block)+size+sizeof(memNode_t)); // find out what the address of the upper block should be

	initHdr(second, block->size-size-sizeof(memNode_t)); // initialise the second block to the right size

	second->previous = block; // fix the heap lists
	second->next = block->next;

	block->next = second;
	block->size = size;
	return block; // return the bottom block
}

inline static volatile memNode_t* splitMul(volatile memNode_t* block, size_t size, boolean pageAlligned)
{
	// if the block should be pageAlligned
	if (pageAlligned)
	{
		// figure out whether or not the block is at the right place
		if (((long)((void*)block+sizeof(memNode_t)))%PAGEBOUNDARY == 0)
		{	// if so we can manage with a simple split
			#ifdef MMTEST
			printf("Simple split\n");
			#endif
			// If this block gets reached the block is at the offset in memory.
			return split (block, size);
		}
		else if ((long)((void*)block+sizeof(memNode_t))%PAGEBOUNDARY != 0)
		{	// if not we must do a bit more complex
			#ifdef MMTEST
			printf("Complex split\n");
			#endif
			// If we get here the base address of the block isn't alligned with the offset.
			// Split the block and then use split on the higher block so the middle is
			// pageAlligned.
			// Below we figure out where the second block should start using some algorithms
			// of which it isn't if a shame a beginner doesn't fully get it.
			unsigned long secondAddr;
			unsigned long base = (unsigned int)((void*)block+2*sizeof(memNode_t)); // the base address is put in an int with some header
											      // sizes because the calculation requires them.
			unsigned long offset = PAGEBOUNDARY-(base%PAGEBOUNDARY); // the addrress is used to figure out the offset to the page boundary
			secondAddr = (unsigned long)((void*)block+sizeof(memNode_t)); // put the base address into second
			secondAddr += offset; // add the offset to second
			volatile memNode_t* second = (void*)secondAddr; // put the actual address in second
			volatile memNode_t* next = block->next; // Temporarilly store next
			
			int secondSize = block->size - ((void*)second - (void*)block); // second's temporary size gets calculated.
			initHdr(second, secondSize); // init the second block with the temporary size
			block->size = (void*)second-((void*)block+sizeof(memNode_t)); // fix the original block size as it isn't correct anymore.
			block->next = second; // fix the heap lists to make a split or return possible
			second->previous = block;
			second->next = next;
			if (second->size>size+sizeof(memNode_t))
			{
				#ifdef MMTEST
				printf("Split in three\n");
				#endif
				volatile memNode_t *ret = split(second, size);
				#ifdef MMTEST
				printf("Split successful\n");
				#endif
				return ret; // if the second block still is too large do a normal split because this will return the
							    // right address anyways.
			}
			else
			{
				#ifdef MMTEST
				printf("Split in two\n");
				#endif
				return second; // the size is right and at the right address, what more could we want.
			}
		}
	}
	else
	{
		// here we can just do a normal block split because there is no address requirement.
		return split(block, size);
	}
}

static volatile memNode_t* merge(volatile memNode_t* alpha, volatile memNode_t* beta)
{
	// First we check for possible corruption
	if (alpha->hdrMagic != HDRMAGIC || beta->hdrMagic != HDRMAGIC)
	{
		#ifdef MMTEST
		printf("HDR error\n"); // debugging code
		#endif
		return NULL; // return error
	}
	if ((alpha->next != beta) && (beta->next != alpha))
	{ // if the pointers don't match, we should not proceed.
		return NULL; // return error
	}
	volatile memNode_t* tmp;
	if (beta->next == alpha)
	{	// if the blocks are in reversed order, put them in the right order
		tmp = alpha;
		alpha = beta;
		beta = tmp;
	}
        
        alpha->size += beta->size+sizeof(memNode_t);
        alpha->next = beta->next;
        alpha->used = FALSE;
        return alpha;
}
