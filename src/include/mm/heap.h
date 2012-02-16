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

#include <types.h>
#include <thread.h>
#include <mm/paging.h>

#ifndef __HEAP_H
#define __HEAP_H

#ifdef __cplusplus
extern "C" {
#endif

#define MM_NODE_MAGIC 0xAF00BEA8
#define PAGEBOUNDARY 0x1000

struct memNode
{
  unsigned int size;
  boolean used;
  volatile struct memNode* next;
  volatile struct memNode* previous;
  unsigned int hdrMagic;
};
typedef struct memNode memory_node_t;

// void heapAddBlocks(void*, int);

void* alloc(size_t, boolean);
void* nalloc(size_t);
int free(void*);

int
initHdr(volatile memory_node_t*, size_t);

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

void heapStub();

void heap_add_blocks(void* base, uint32_t size);

#define kalloc(size) alloc(size, FALSE)
#define kfree(ptr)   free(ptr)
#define knalloc(size) nalloc(size)
#define kzalloc(size) knalloc(size)

// Alloc_max = 1 MB
#define HEAPSIZE (0xf*0x100000)-((uint32_t)(&end) - offset)
#define ALLOC_MAX HEAPSIZE

#ifdef MMTEST
void examineHeap();
void wait();
void testAlloc();
#endif
#define examineHeap examine_heap
#define ol_dbg_heap examine_heap

#if 0
extern long heapBase;
extern long heapSize;
#endif

extern volatile memory_node_t* heap;
extern volatile mutex_t prot;

#ifdef __cplusplus
}
#endif

#endif