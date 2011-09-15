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
typedef struct memNode memNode_t;

void heapAddBlocks(void*, int);

void* alloc (size_t,boolean);
void* nalloc (size_t);
int free (void* ptr);
void initHdr(volatile memNode_t* block, size_t size);

void heapStub();

#define kalloc(a) alloc(a,FALSE)

// Alloc_max = 1 MB
#define ALLOC_MAX 0x100000

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

extern volatile memNode_t* blocks;
extern volatile mutex_t prot;

#endif