/*
    Andromeda
    Copyright (C) 2011, 2012
    Bart Kuivenhoven, Michel Megens, Steven van der Schoot

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

/**
 * The memory allocation file is here. This code requires the heap to be at the
 * end of the binary image, and just beyond the variables.
 *
 * As of now the heap has a fixed size, which isn't an issue, as long as the
 * memory required is free. As of now, we require about 10 MiB.
 *
 * In the future the heap should be made so it grows according to demand. This
 * can be done through the use of a wilderness(1) block or just growing the heap
 * when if we run out of memory.
 *
 * (1) For the definition of the wilderness block, look up documentation on Doug
 * Lea's malloc (aka. dlmalloc).
 */

#include <stdlib.h>
#include <thread.h>
#include <mm/heap.h>

static boolean
use_memnode_block(volatile memory_node_t* block);
static void
return_memnode_block(volatile memory_node_t*);
static volatile memory_node_t*
split(volatile memory_node_t*, size_t);
static volatile memory_node_t*
splitMul(volatile memory_node_t*, size_t, boolean);
static volatile memory_node_t*
merge_memnode(volatile memory_node_t*, volatile memory_node_t*);

volatile memory_node_t* heap; /* heap pointer */
volatile mutex_t prot = mutex_unlocked;

void
examine_heap()
{
	printf("Head\t0x%X\n", (int) heap);
	if (heap == NULL)
		return;
	volatile memory_node_t* carriage;
	for (carriage = heap; carriage != NULL; carriage = carriage->next)
	{
                printf(
                        "node: 0x%X\tsize: 0x%X\tnext: 0x%X\n",
                       (int)carriage,
                       carriage->size,
                       (int)carriage->next
                );
	}
}

// This code is called whenever a new block header needs to be created.
// It initialises the header to a good position and simplifies the code a bit.

int
initHdr(volatile memory_node_t* block, size_t size)
{
	if (size <= 0)
		return 1;
	block->size = size;
	block->previous = NULL;
	block->next = NULL;
	block->used = FALSE;
	block->hdrMagic = MM_NODE_MAGIC;
	return 0;
}

void*
realloc(void* ptr, size_t size)
{
	void* new = alloc(size, FALSE);
	volatile memory_node_t* ptrInfo = ptr - sizeof (memory_node_t);
	size_t currentSize = ptrInfo->size;
	memcpy(new, ptr, (size > currentSize) ? currentSize : size);
	return new;
}

void*
nalloc(size_t size)
{
	void* tmp = alloc(size, FALSE);
	if (tmp != NULL)
		memset(tmp, 0, size);
	return tmp;
}

// Finds a block on the heap, which is free and which is large enough.
// In the case that pageAlligned is enabled the block also has to hold
// page aligned data (useful for the page directory).

void*
alloc(size_t size, uint16_t pageAlligned)
{
	if (size > ALLOC_MAX)
	{
		return NULL;
	}
	mutex_lock(&prot);
	volatile memory_node_t* carriage = heap;
	for (; carriage != NULL; carriage = carriage->next)
	{
		if (pageAlligned == TRUE)
		{
			if (!carriage->used)
			{
				/**
				 * If the block isn't used and the block should
				 * be alligned with the page boundary, the block
				 * is called.
				 * The code figures out the required offset for
				 * the block to be able to hold the desired
				 * block.
				 */
				addr_t offset = PAGEBOUNDARY - ((addr_t)
								      carriage +
							 sizeof (memory_node_t))
								 % PAGEBOUNDARY;
				offset %= PAGEBOUNDARY;
				addr_t blockSize = offset + size;

				if (carriage->size >= blockSize)
				// if the size is large enough to be split into
				// page alligned blocks, then do it.
				{
					volatile memory_node_t* ret = splitMul
								(carriage, size,
									  TRUE);
					// Split the block
					use_memnode_block(ret);
					// Mark the block as used
					//return the desired block
					mutex_unlock(&prot);
					return (void*) ret + sizeof
								(memory_node_t);
				}
				else
				{
					// The block isn't the right size
					//so find another one.
					continue;
				}
			}
			else
			{

				/**
				 * The block is used, which may be the
				 * case in multithreaded environments.
				 * This means the other thread isn't
				 * done with this block and we need to
				 * leave it as is.
				 */
				continue;
			}
		}
		else if (carriage->size >= size && carriage->size < size +
							sizeof (memory_node_t))
		{
			if (use_memnode_block(carriage))
						 // check the usage of the block
			{
				continue;
			}
			mutex_unlock(&prot);
			return (void*) carriage + sizeof (memory_node_t);
		}
		else if (carriage->size >= size + sizeof (memory_node_t))
		// the block is too large
		{
			if (carriage->used)
			// assert that the block isn't used
			{
				continue;
			}

			volatile memory_node_t* tmp = split(carriage, size);
			// split the block

			use_memnode_block(tmp);
			mutex_unlock(&prot);
			return (void*) tmp + sizeof (memory_node_t);
		}
		if (carriage->next == NULL || carriage->next == carriage)
		{
			printf("Allocation at end of list!\nblocks: %X\tCarrige"
							     ": %X\tsize: %X\n",
						     (int) heap, (int) carriage,
							  (int) carriage->size);
			if (carriage->next == carriage)
				printf("Loop in list!\n");
			break;
			/**
			 * If we haven't found anything but we're at the end of
			 * the list or heap corruption occured, we break out of
			 * the loop and return the default pointer (which is
			 * NULL).
			 */
		}
	}
	mutex_unlock(&prot);
	return NULL;
}

void
free(void* ptr, size_t size)
{
#ifdef MMTEST
	printf("Free!!!\n");
#endif
	if (ptr == NULL)
		return;
	mutex_lock(&prot);
	volatile memory_node_t* block = (void*) ptr - sizeof (memory_node_t);
	volatile memory_node_t* carriage;
	if (block->hdrMagic != MM_NODE_MAGIC)
	{
		mutex_unlock(&prot);
		return;
	}

	/**
	* Try to put the block back into the list of free nodes,
	*Actually claim it's free and then merge it into the others if possible.
	* This code is littered with debugging code.
	*/

	// Debugging code
#ifdef MMTEST
	printf("Before:\n");
	examineHeap();
	printf("\n");
#endif
	return_memnode_block(block); // actually mark the block unused.
	// more debugging code
#ifdef MMTEST
	printf("During:");
	examineHeap();
	printf("\n");
#endif
	// Now find a place for the block to fit into the heap list
	for (carriage = heap; carriage != NULL && carriage->next != carriage &&
			      carriage->next != heap; carriage = carriage->next)
	// Loop through the heap list
	{

		// if we found the right spot, merge the lot.
		if ((void*) block + block->size + sizeof (memory_node_t) ==
								(void*) carriage
					 || (void*) carriage + carriage->size +
					sizeof (memory_node_t) == (void*) block)
		{
		volatile memory_node_t* test = merge_memnode(block, carriage);
		// merging code
			if (test == NULL)
			// if the merge failed
			{
				printf("Merge failed\n");
#ifdef MMTEST
				printf("After\n");
				examineHeap();
				printf("\n");
				wait();
#endif
				continue;
			}
			else
			{
                                break;
// 				block = test;
// 				carriage = test->previous;
				/**
				* We can now continue trying to merge the rest
				* of the list, which might be possible.
				*/
			}
		}
	}
	// Even more debugging code
#ifdef MMTEST
	printf("After\n");
	examineHeap();
	printf("\n");
#endif
	mutex_unlock(&prot);
	return; // Return success
}

static boolean
use_memnode_block(volatile memory_node_t* x)
{
	// mark the block as used and remove it from the heap list
	if (x->used == FALSE)
	{
		x->used = TRUE;
		if (x->previous != NULL)
		// if we're not at the top of the list
			x->previous->next = x->next;
			// set the previous block to hold the next block
		else
			// if we are at the top of the list, move the top of the
			// list to the next block
			heap = x->next;
		if (x->next != NULL)
		// if we're not at the end of the list
			x->next->previous = x->previous;
			// set the next block to hold the previous block
		else
			x->next->previous = NULL;
		// Over here the block should be removed from the heap lists.
		return FALSE; // return that the block wasn't used.
	}
	else
		return TRUE;
}

static void
return_memnode_block(volatile memory_node_t* block)
{
	/* This code marks the block as unused and puts it back in the list. */
	if (block->hdrMagic != MM_NODE_MAGIC)
		return; /* Make sure we're not corrupting the
		* heap
		*/
	block->used = FALSE;
	volatile memory_node_t* carriage;
	if ((void*) block < (void*) heap)
	{
		/* if we're at the top of the heap list add the block there. */
		heap->previous = block;
		block->next = heap;
		block->previous = NULL;
		heap = block;
		return;
	}
	/* We're apparently not at the top of the list */
	for (carriage = heap; carriage != NULL; carriage = carriage->next)
	// Loop through the heap list.
        {
                if ((addr_t)carriage < (addr_t)block &&
                                       (addr_t)(carriage->next) > (addr_t)block)
		{
			block->previous = carriage;
			block->next = carriage->next;
			carriage->next = block;
                        block->next->previous = block;
			return;
		}
		if (carriage->next == NULL)
		{
			/* we are at the end of the list, add the block here */
			carriage -> next = block;
			block -> previous = carriage;
			carriage->next->next = NULL;
                        debug("We're done at %X with block %X of size %X\n",
                              (int)carriage,
                              (int)block,
                              (int)(block->size)
                        );
			return;
		}
	}
}

static volatile memory_node_t*
split(volatile memory_node_t* block, size_t size)
{
	/**
	* This code splits the block into two parts, the lower of which is
	* returned to the caller.
	*/
	volatile memory_node_t* second = ((void*) block) +
						  sizeof (memory_node_t) + size;

	if (initHdr(second, block->size - size - sizeof (memory_node_t)))
		return block;
	/* initialise the second block to the right size */

	second->previous = block; // fix the heap lists
	second->next = block->next;
	second->next->previous = second;

	block->next = second;
	block->size = size;
	block->previous->next = block;
	return block; // return the bottom block
}

static volatile memory_node_t*
splitMul(volatile memory_node_t* block, size_t size, boolean pageAlligned)
{
	// if the block should be pageAlligned
	if (pageAlligned)
	{
	// figure out whether or not the block is at the right place
		if (((addr_t) ((void*) block + sizeof (memory_node_t))) %
							      PAGEBOUNDARY == 0)
		{
		// if so we can manage with a simple split
	#ifdef MMTEST
			printf("Simple split\n");
	#endif
			/* If this block gets reached the block is at the offset
			 * in memory.*/
			return split(block, size);
		}
		else if ((addr_t) ((void*) block + sizeof (memory_node_t)) %
							      PAGEBOUNDARY != 0)
		{
		// if not we must do a bit more complex
	#ifdef MMTEST
			printf("Complex split\n");
	#endif
			/**
			 * If we get here, the base address of the block isn't
			 * alligned with the offset. Split the block and then
			 * use split on the higher block so the middle is page
			 * alligned.
			 * Below we figure out where the second block should
			 * start using some algorithms of which it isn't a
			 * shame if a beginner doesn't fully get it.
			 */
			addr_t secondAddr;
			addr_t base = (unsigned int) ((void*) block + 2 *
				     sizeof (memory_node_t)); /* base address */

			addr_t offset = PAGEBOUNDARY - (base % PAGEBOUNDARY);
			 /* the addrress is used to figure out the offset to the
			 *				        page boundary */
			secondAddr = (addr_t) ((void*) block + sizeof
							       (memory_node_t));
			/* put the base address into second */

			secondAddr += offset; /* add the offset to second */
			volatile memory_node_t* second = (void*) secondAddr;
			volatile memory_node_t* next = block->next;
						   /* Temporarilly store next */

			int secondSize = block->size - ((void*) second -
								 (void*) block);
			initHdr(second, secondSize);
			/* init the second block with the temp size */
			block->size = (void*) second - ((void*) block + sizeof
							       (memory_node_t));
			block->next = second;
			/* fix the heap lists to make a split or return
			 *					     possible */
			second->previous = block;
			second->next = next;
			if (second->size > size + sizeof (memory_node_t))
			{
				volatile memory_node_t *ret =
							    split(second, size);
				return ret; /* if the second block still is too
				large do a normal split because this will return
						   the right address anyways. */
			}
			else
			{
				return second;
				/* the size is right and at the right address,
					what more could we want. */
			}
		}
	}
	else
		/**
		* here we can just do a normal block split because there is no
		* address requirement.
		*/
		return split(block, size);
	return NULL;
}

static volatile memory_node_t*
merge_memnode(volatile memory_node_t* alpha, volatile memory_node_t* beta)
{
	// First we check for possible corruption
	if (alpha->hdrMagic != MM_NODE_MAGIC || beta->hdrMagic != MM_NODE_MAGIC)
	{
#ifdef MMTEST
		printf("HDR error\n");
		// debugging code
#endif
		return NULL; // return error
	}
	if ((((void*) alpha) + alpha->size + sizeof (memory_node_t) != beta) &&
		(((void*) beta) + beta->size + sizeof (memory_node_t) != alpha))
		// if the pointers don't match, we should not proceed.
		return NULL;
	// return error
	volatile memory_node_t* tmp;
	if (((void*) beta) + beta->size + sizeof (memory_node_t) == alpha)
	{
		// if the blocks are in reversed order, put them in the
		// right order
                tmp = alpha;
                alpha = beta;
                beta = tmp;
	}

	alpha->size += beta->size + sizeof (memory_node_t);
	alpha->next = beta->next;
	alpha->used = FALSE;

	return alpha;
}
