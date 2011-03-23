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

struct memNode
{
  unsigned int size;
  boolean used;
  struct memNode* next;
  struct memNode* previous;
  unsigned int hdrMagic;
};
typedef struct memNode memNode_t;

void initBlockMap();

void* alloc (size_t,boolean);
int free (void* ptr);

#define kalloc(a) alloc(a,FALSE)

#ifdef TESTALLOC
#define TESTA
#endif

#ifdef TESTALLIGNED
#define TESTA
#endif

#ifdef TESTA
void examineHeap();
#endif

extern int heapBase;
extern int heapSize;

#endif