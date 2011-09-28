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

#include <stdlib.h>
#include <thread.h>
#include <mm/heap.h>
#define SIZE ((size <= ALLOC_MAX) ? size : ALLOC_MAX)

void
ol_dbg_heap()
{
  printf("%s\t%s\t%s\t%s\n", "Base","Prev","Next","Size");
  volatile memory_node_t* x = heap;
  for (; x != NULL; x = x->next)
  {
    printf("%x\t%x\t%x\t%x\n", x, x->previous, x->next, x->size);
  }
}

//Makes use of the memory bitmap to select the pages that are usable.
//Since the heap has only limited allocation space, there also needs
//to be a regeon that's used for memory mapping.

void heapAddBlocks(void* base, int size) // Requests size in bytes
{
  mutexEnter(prot);
  while (size > 0)
  {
    initHdr(base, SIZE - sizeof (memory_node_t));
    size -= SIZE;
    if (heap == NULL)
    {
      heap = base;
#ifdef DBG
      printf("Creating head of heap\n");
#endif
    }
    else
    {
      mutexRelease(prot); // To prevent the mutex from conflicting with itself basically
      free((void*) base + sizeof (memory_node_t));
      mutexEnter(prot);
    }
    base += SIZE;
  }
  mutexRelease(prot);
}

void heapCoreBlocks(void* base, int size)
{
  mutexEnter(prot);
  memory_node_t* node = (memory_node_t*) base;
  initHdr(node, size - sizeof (memory_node_t));
  heap = node;
  mutexRelease(prot);
}