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
#include <mm/memory.h>
#include <mm/heap.h>
#include <error/panic.h>
#ifdef __MMTEST
#include <textio.h>
#endif

void *
kalloc(size_t size)
{
        return alloc(size, TRUE);
}

static void * 
alloc(size_t size, bool check)
{
        if(check && size > OL_MAX_ALLOC_SIZE) return NULL;
        
        ol_memnode_t x;
        uint32_t i = 0;
        
        /*
         * Search all heaps for a fitting node
         */
        for(x = heap; x != NULL; x = x->next)
        {
                if(!(x->magic ^ OL_HEAP_MAGIC)) panic("Heap corruption! - Invalid"
                        "block header magic detected!");
                
                else if(x == x->next) panic("Heap corruption detected!");
                
                else if(x->size < size+sizeof(ol_memnode_t) && x->size >= size)
                {
                        /*
                         * If we end up in this if statment, is the block that we
                         * just found large enough, but not large enough to split
                         * it in two pieces.
                         */
                        
                        if(x->inuse) continue; /* 
                                                  * if it is in use, try it all
                                                  * again from the beginning
                                                  */
                        return (void *) x->base;
                }
                
                else if(x->size > (size+sizeof(ol_memnode_t)))
                {
                        /*
                         * Well done, you found a block which is large enough to
                         * hold your data AND it is large enough to split it in
                         * two parts
                         */
                        if(x->inuse) continue;
                        ol_memnode_t tmp = ol_split_memnode(x, size);
                        return tmp->base;
                }
        }
        
        /*
         * Not in any of the heaps is a node fount which is large enough. When
         * alloc returns returns NULL-pointers you should allocate a new heap
         * or you have to free old data.
         */
        return NULL;
}

int
free(void * block)
{
        
}
