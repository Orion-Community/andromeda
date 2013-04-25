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

#include <stdlib.h>
#include <thread.h>
#include <mm/heap.h>
#include <andromeda/system.h>
#define SIZE ((size <= ALLOC_MAX) ? size : ALLOC_MAX)

//Makes use of the memory bitmap to select the pages that are usable.
//Since the heap has only limited allocation space, there also needs
//to be a regeon that's used for memory mapping.

// void heapAddBlocks(void* base, int size) // Requests size in bytes
// {
//   mutexEnter(prot);
//   while (size > 0)
//   {
//     initHdr(base, SIZE - sizeof (memory_node_t));
//     size -= SIZE;
//     if (heap == NULL)
//     {
//       heap = base;
// #ifdef DBG
//       printf("Creating head of heap\n");
// #endif
//     }
//     else
//     {
//       mutexRelease(prot);
//       // To prevent the mutex from conflicting with itself basically
//       free((void*) base + sizeof (memory_node_t));
//       mutexEnter(prot);
//     }
//     base += SIZE;
//   }
//   mutexRelease(prot);
// }

int
heap_inset_block(volatile memory_node_t* heap_l, volatile memory_node_t *block)
{
	if (heap_l == NULL || block == NULL)
		return -E_HEAP_GENERIC;

	addr_t block_address = (addr_t) block;

	volatile memory_node_t *carriage = heap_l;
	volatile memory_node_t *last = NULL;

        if ((addr_t)heap_l >= block_address)
        {
//                 heap = block;
//                 block->next = heap_l;
//                 heap_l->previous = block;
//                 return -E_SUCCESS;
                printf("Heap: %X\tblock: %X\n", (int) heap_l, (int)block);
                panic("Un expected condition");
        }

        while (carriage != NULL && (addr_t)heap_l < block_address)
        {
                last = carriage;
                carriage = carriage->next;
        }
	if (last == NULL)
		return -E_HEAP_GENERIC;

	block->next = last->next;
	block->previous = last;
	last->next = block;
	if (block->next != NULL)
		block->next->previous = block;
	return -E_SUCCESS;
}

/**
 * Heap_add_blocks adds a regeon of address space to the heap
 */

void
heap_add_blocks(void* base, uint32_t size)
{
	volatile memory_node_t* node = (memory_node_t*) base;
	initHdr(node, size - sizeof (memory_node_t));
	if (heap == NULL)
	{
		mutex_lock(&prot);
		heap = node;
		mutex_unlock(&prot);
	}
	else
	{
		mutex_lock(&prot);
		if (heap_inset_block(heap, node) != -E_SUCCESS)
			panic("Could not add blocks to map");
		mutex_unlock(&prot);
	}
}

int slob_sys_register()
{
        if (core.mm == NULL)
                return -E_NOT_YET_INITIALISED;
        core.mm->alloc = alloc;
        core.mm->free = free;

        return -E_SUCCESS;
}
