/*
 *   openLoader project - Memory Manager
 *   Copyright (C) 2011  Michel Megens
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <mm/heap.h>

/*
 * It is possible to maintain more then one heap. E.g. one heap can be defined
 * to range from 0x8000 to 0xa0000 (that is the base heap actually) and there 
 * can be a second heap which could be - for example - range from 0x100000 
 * to 0x200000.
 * 
 * The allocator will traverse through all nodes in all heaps (until it
 * found a node which is large enough or when it has searched the last heap in 
 * vain (and thus ran into a NULL heap)).
 * 
 * Another reason you might want more then one heap, is that I can allocate an
 * entire heap for the operating system I'm going to load and its modules.
 */

#include <stdlib.h>
#include <mm/heap.h>
#ifdef __MMTEST
#include <textio.h>
#endif

volatile ol_memnode_t heap;

void 
ol_init_heap()
{
        heap = (ol_memnode_t)0x8000;
        
        heap->magic = (uint8_t)OL_HEAP_MAGIC;
        heap->inuse = FALSE;
        heap->next = NULL;
        heap->previous = NULL;
        heap->size = OL_NODE_SIZE(0x8000, 0xa0000);
        heap->base = (void *)OL_BLOCK_BASE((uint32_t)heap);
}

volatile ol_memnode_t
ol_add_block(void *base, size_t len)
{
        ol_add_heaphdr(base, len);
        
        return base;
}

static void
ol_add_heaphdr(void *b, size_t size)
{
        ((ol_memnode_t)b)->inuse = TRUE; /* dangerous to use at the moment */
        ((ol_memnode_t)b)->magic = (uint8_t)OL_HEAP_MAGIC;
        ((ol_memnode_t)b)->size = size;
        ((ol_memnode_t)b)->next = NULL;
        ((ol_memnode_t)b)->previous = NULL;
        
        free(b);
}

volatile ol_memnode_t 
ol_split_memnode(ol_memnode_t blk, size_t size)
{
#ifdef __MMTEST
        println("SPLIT");
#endif
        return blk;
}
