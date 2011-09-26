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

#define MM_NODE_MAGIC 0xAF00BEA8
#define PAGEBOUNDARY 0x1000

#ifndef __HEAP_H
#define __HEAP_H

#include <types.h>
#include <thread.h>

struct memNode
{
  unsigned int size;
  boolean used;
  volatile struct memNode* next;
  volatile struct memNode* previous;
  unsigned int hdrMagic;
};
typedef struct memNode memory_node_t;

void heapAddBlocks(void*, int);

void* alloc(size_t, boolean);
void* nalloc(size_t);
int free(void*);

void
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

#define kalloc(size) alloc(size, FALSE)
#define malloc(size) alloc(size, FALSE)

// Alloc_max = 1 MB
#define ALLOC_MAX 0x100000
#define HEAPSIZE 0x100000

#ifdef MMTEST
void examineHeap();
void wait();
void testAlloc();
#endif
#ifdef DBG
void examineHeap();
#endif

extern long heapBase;
extern long heapSize;

extern volatile memory_node_t* heap;
extern volatile mutex_t prot;

#endif