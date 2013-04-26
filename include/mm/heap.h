/*
    Andromeda
    Copyright (C) 2011, 2012  Bart Kuivenhoven, Michel Megens

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

#ifdef SLOB
void* alloc(size_t, uint16_t);
void* nalloc(size_t);
void free(void*, size_t);

int initHdr(volatile memory_node_t*, size_t);
void heapStub();
void heap_add_blocks(void* base, uint32_t size);
#define examineHeap examine_heap
#define ol_dbg_heap examine_heap
void examine_heap();
#endif

#if 0
#define knalloc(size) nalloc(size)
#define kzalloc(size) knalloc(size)
#endif

// Alloc_max = 1 MB
#define THREE_GIB 0xC0000000
#define HEAPSIZE (0xf*0x100000)-((uint32_t)(&end) - THREE_GIB)
#define ALLOC_MAX HEAPSIZE

#ifdef MMTEST
void wait();
void testAlloc();
#endif

#if 0
extern long heapBase;
extern long heapSize;
#endif

extern volatile memory_node_t* heap;
extern volatile mutex_t prot;
extern boolean freeable_allocator;

#ifdef __cplusplus
}
#endif
#endif

/** \file */
