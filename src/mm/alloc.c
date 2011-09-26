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

volatile memory_node_t* heap; /* heap pointer */
volatile mutex_t prot;

void 
examineHeap()
{
  printf("Head\n0x%X\n", (int) heap);
  volatile memory_node_t* carige;
  for (carige = heap; carige != NULL; carige = carige->next)
  {
    printf("node: 0x%X\tsize: 0x%X\n", (int) carige, carige->size);
  }
}

// This code is called whenever a new block header needs to be created.
// It initialises the header to a good position and simplifies the code a bit.

void 
initHdr(volatile memory_node_t* block, size_t size)
{
  block->size = size;
  block->previous = NULL;
  block->next = NULL;
  block->used = FALSE;
  block->hdrMagic = MM_NODE_MAGIC;
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
  if (tmp != NULL);
  memset(tmp, 0, size);
  return tmp;
}

// Finds a block on the heap, which is free and which is large enough.
// In the case that pageAlligned is enabled the block also has to hold
// page alligned data (usefull for the page directory).

void* 
alloc(size_t size, boolean pageAlligned)
{
  if (size > ALLOC_MAX)
  {
    return NULL;
  }
  mutexEnter(prot);
  volatile memory_node_t* carige;
  for (carige = heap; carige != NULL; carige = carige->next)
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
        unsigned long offset = PAGEBOUNDARY - ((long) carige + sizeof (memory_node_t)) % PAGEBOUNDARY;
        offset %= PAGEBOUNDARY;
        unsigned long blockSize = offset + size;

        if (carige->size >= blockSize) // if the size is large enough to be split into
          // page alligned blocks, then do it.
        {
          volatile memory_node_t* ret = splitMul(carige, size, TRUE); // Split the block
          use_memnode_block(ret); // Mark the block as used

          //return the desired block
          mutexRelease(prot);
          return (void*) ret + sizeof (memory_node_t);
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
    else if (carige->size >= size && carige->size < size + sizeof (memory_node_t))
    {
      if (carige->used) // check the usage of the block
      {
        continue;
      }
      // If the block is the right size or too small to hold 2 separate blocks,
      // In which one of them is the size allocated, then allocate the entire block.
      printf("%x\n", use_memnode_block(carige));
      
      mutexRelease(prot);
      return (void*) carige + sizeof (memory_node_t);
    }
    else if (carige->size >= size + sizeof (memory_node_t)) // the block is too large
    {
      if (carige->used) // assert that the block isn't used
      {
        continue;
      }

      volatile memory_node_t* tmp = split(carige, size); // split the block
      if (use_memnode_block(tmp)) // mark the block used.
      {
        continue;
      }
      mutexRelease(prot);
      return (void*) tmp + sizeof (memory_node_t);
    }
    if (carige->next == NULL || carige->next == carige)
    {
      printf("Allocation at end of list!\nblocks: %X\tCarrige: %X\tsize: %X\n", (int) heap, (int) carige, (int) carige->size);
      if (carige->next == carige)
        printf("Loop in list!\n");
      break; // If we haven't found anything but we're at the end of the list
      // or heap corruption occured we break out of the loop and return
      // the default pointer (which is NULL).
    }
  }
  mutexRelease(prot);
  return NULL;
}

int 
free(void* ptr)
{
#ifdef MMTEST
  printf("Free!!!\n");
#endif
  if (ptr == NULL)
    return -1;
  mutexEnter(prot);
  volatile memory_node_t* block = (void*) ptr - sizeof (memory_node_t);
  volatile memory_node_t* carige;
  if (block->hdrMagic != MM_NODE_MAGIC)
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
  return_memnode_block(block); // actually mark the block unused.
  // more debugging code
#ifdef MMTEST
  printf("During:");
  examineHeap();
  printf("\n");
#endif
  // Now find a place for the block to fit into the heap list
  for (carige = heap; carige != NULL && carige->next != carige &&
       carige->next != heap; carige = carige->next) // Loop through the heap list
  {

    // if we found the right spot, merge the lot.
    if ((void*) block + block->size + sizeof (memory_node_t) == (void*) carige || (void*) carige + carige->size + sizeof (memory_node_t) == (void*) block)
    {
      volatile memory_node_t* test = merge_memnode(block, carige); // merging code
      if (test == NULL) // if the merge failed
      {
        printf("Merge failed\n");
#ifdef MMTEST
        printf("After\n");
        examineHeap();
        printf("\n");
        wait();
#endif
        continue;
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

static boolean
use_memnode_block(volatile memory_node_t* x)
{
  // mark the block as used and remove it from the heap list
  if (x->used == FALSE)
  {
    x->used = TRUE;
    if (x->previous != NULL) // if we're not at the top of the list
    {
      x->previous->next = x->next; // set the previous block to hold the next block
    }
    else // if we are at the top of the list, move the top of the list to the next block
    {
      heap = x->next;
    }
    if (x->next != NULL) // if we're not at the end of the list
    {
      x->next->previous = x->previous; // set the next block to hold the previous block
    }
    else
    {
      x->next->previous = NULL;
    }
    // Over here the block should be removed from the heap lists.
    return FALSE; // return that the block wasn't used.
  }
  else
  {
    return TRUE;
  }
}

static void 
return_memnode_block(volatile memory_node_t* block)
{
  // This code marks the block as unused and puts it back in the list.
  if (block->hdrMagic != MM_NODE_MAGIC) // Make sure we're not corrupting the heap
  {
#ifdef MMTEST
    printf("WARNING\n");
#endif
    return;
  }
  block->used = FALSE;
  volatile memory_node_t* carige;
  if ((void*) block < (void*) heap)
  {// if we're at the top of the heap list add the block there.
    heap -> previous = block;
    block -> next = heap;
    block -> previous = NULL;
    heap = block;
    return;
  }
  // We're apparently not at the top of the list
  for (carige = heap; carige != NULL; carige = carige->next) // Loop through the heap list.
  {
    if ((void*) carige + sizeof (memory_node_t) + carige->size <= (void*) block) // if the carige connects to the bottom of our block
    {
      block -> next = carige -> next;
      block -> previous = carige;
      carige -> next = block;

      return; // add the block to the list after the carige
    }
    else if ((void*) block + sizeof (memory_node_t) + block->size <= (void*) carige) // if the block connects to the bottom of the carige
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

static volatile memory_node_t*
split(volatile memory_node_t* block, size_t size)
{
  // This code splits the block into two parts, the lower of which is returned
  // to the caller.

  volatile memory_node_t* second = (volatile memory_node_t*)((void*) (block) + size + sizeof (memory_node_t)); // find out what the address of the upper block should be

  initHdr(second, block->size - size - sizeof (memory_node_t)); // initialise the second block to the right size

  second->previous = block; // fix the heap lists
  second->next = block->next;

  block->next = second;
  block->size = size;
  return block; // return the bottom block
}

static volatile memory_node_t* 
splitMul(volatile memory_node_t* block, size_t size, boolean pageAlligned)
{
  // if the block should be pageAlligned
  if (pageAlligned)
  {
    // figure out whether or not the block is at the right place
    if (((long) ((void*) block + sizeof (memory_node_t))) % PAGEBOUNDARY == 0)
    { // if so we can manage with a simple split
#ifdef MMTEST
      printf("Simple split\n");
#endif
      // If this block gets reached the block is at the offset in memory.
      return split(block, size);
    }
    else if ((long) ((void*) block + sizeof (memory_node_t)) % PAGEBOUNDARY != 0)
    { // if not we must do a bit more complex
#ifdef MMTEST
      printf("Complex split\n");
#endif
      // If we get here the base address of the block isn't alligned with the offset.
      // Split the block and then use split on the higher block so the middle is
      // pageAlligned.
      // Below we figure out where the second block should start using some algorithms
      // of which it isn't if a shame a beginner doesn't fully get it.
      unsigned long secondAddr;
      unsigned long base = (unsigned int) ((void*) block + 2 * sizeof (memory_node_t)); // the base address is put in an int with some header
      // sizes because the calculation requires them.
      unsigned long offset = PAGEBOUNDARY - (base % PAGEBOUNDARY); // the addrress is used to figure out the offset to the page boundary
      secondAddr = (unsigned long) ((void*) block + sizeof (memory_node_t)); // put the base address into second
      secondAddr += offset; // add the offset to second
      volatile memory_node_t* second = (void*) secondAddr; // put the actual address in second
      volatile memory_node_t* next = block->next; // Temporarilly store next

      int secondSize = block->size - ((void*) second - (void*) block); // second's temporary size gets calculated.
      initHdr(second, secondSize); // init the second block with the temporary size
      block->size = (void*) second - ((void*) block + sizeof (memory_node_t)); // fix the original block size as it isn't correct anymore.
      block->next = second; // fix the heap lists to make a split or return possible
      second->previous = block;
      second->next = next;
      if (second->size > size + sizeof (memory_node_t))
      {
#ifdef MMTEST
        printf("Split in three\n");
#endif
        volatile memory_node_t *ret = split(second, size);
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

static volatile memory_node_t* 
merge_memnode(volatile memory_node_t* alpha, volatile memory_node_t* beta)
{
  // First we check for possible corruption
  if (alpha->hdrMagic != MM_NODE_MAGIC || beta->hdrMagic != MM_NODE_MAGIC)
  {
#ifdef MMTEST
    printf("HDR error\n"); // debugging code
#endif
    return NULL; // return error
  }
  if ((((void*)alpha)+alpha->size+sizeof(memory_node_t) != beta) && 
    (((void*)beta)+beta->size+sizeof(memory_node_t) != alpha))
  { // if the pointers don't match, we should not proceed.
    return NULL; // return error
  }
  volatile memory_node_t* tmp;
  if (((void*)beta)+beta->size+sizeof(memory_node_t) == alpha)
  { // if the blocks are in reversed order, put them in the right order
    tmp = alpha;
    alpha = beta;
    beta = tmp;
  }

  alpha->size += beta->size + sizeof (memory_node_t);
  alpha->next = beta->next;
  alpha->used = FALSE;
  return alpha;
}
