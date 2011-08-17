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
        heap->flags = OL_BLOCK_UNUSED;
        heap->next = NULL;
        heap->previous = NULL;
        heap->size = OL_NODE_SIZE(0x8000, 0xa0000);
        heap->base = (void *)OL_BLOCK_BASE((uint32_t)heap);
}

void *
kalloc(size_t size)
{
        return alloc(size, TRUE);
}

int
free(void * block)
{
        ol_memnode_t x = (void *)block-sizeof(struct ol_memory_node);
        ol_memnode_t y = x->next;
        
        x = ol_merge_memnodes(y, x);
        printnum((uint32_t)x->size, 16, FALSE, FALSE);
        putc(0xa);

}

static volatile ol_memnode_t
ol_add_block(void *base, size_t len)
{
        ol_add_heaphdr(base, len);
        
        return base;
}

static ol_memnode_t
ol_add_heaphdr(void *b, size_t size)
{
        ((ol_memnode_t)b)->flags = OL_BLOCK_UNUSED;
        ((ol_memnode_t)b)->magic = (uint8_t)OL_HEAP_MAGIC;
        ((ol_memnode_t)b)->size = size;
        ((ol_memnode_t)b)->next = NULL;
        ((ol_memnode_t)b)->previous = NULL;
        ((ol_memnode_t)b)->base = (void*)OL_BLOCK_BASE((uint32_t)b);
        
        return (ol_memnode_t)b;
}

volatile ol_memnode_t 
ol_split_memnode(ol_memnode_t blk, size_t size)
{

        ol_memnode_t x = ol_add_heaphdr(((void *)blk)+size, blk->size-size-sizeof(
               struct ol_memory_node));
        x->previous = blk;
        x->next = blk->next;
        blk->size = size;
        blk->next = x;
        
        
#ifdef __MMTEST
        printnum((uint32_t)x->size, 16, FALSE, FALSE);
        putc(0xa);
        println("SPLIT");
#endif
        return blk;
}

static void * 
alloc(size_t size, bool check)
{
        if(check && size > OL_MAX_ALLOC_SIZE) return NULL;
        
        ol_memnode_t x;
        
        /*
         * Search all heaps for a fitting node
         */
        for(x = heap; x != NULL; x = x->next)
        {
                if(!(x->magic ^ OL_HEAP_MAGIC)) panic("Heap corruption! - Invalid"
                        "block header magic detected!");
                
                else if(x == x->next) panic("Heap corruption detected!");
                
                else if(x->size < size+sizeof(struct ol_memory_node) 
                        && x->size >= size)
                {
                        /*
                         * If we end up in this if statement, is the block that we
                         * just found large enough, but not large enough to split
                         * it in two pieces.
                         */
                        
                        if((x->flags & OL_BLOCK_INUSE) != 0) continue; /* 
                                                  * if it is in use, try it all
                                                  * again from the beginning
                                                  */
                        return (void *) x->base;
                }
                
                else if(x->size > (size+sizeof(struct ol_memory_node)))
                {
                        /*
                         * Well done, you found a block which is large enough to
                         * hold your data AND it is large enough to split it in
                         * two parts
                         */
                        if((x->flags & OL_BLOCK_INUSE) != 0) continue;
                        
                        ol_memnode_t tmp = ol_split_memnode(x, size);
                        ol_use_heap_block(tmp);
#ifdef __MMTEST
                        printnum((uint32_t)tmp->next->previous, 16, FALSE, FALSE);
                        putc(0xa);
#endif
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

static volatile ol_memnode_t 
ol_merge_memnodes(ol_memnode_t a, ol_memnode_t b)
{
        //if((a->magic != OL_HEAP_MAGIC ) || (b->magic != OL_HEAP_MAGIC)) panic("Heap "
         //       "corruption! - Invalid block header magic detected!");
        
        if((a->next != b) && (b->next != a)) return NULL;
        if(b->next == a)
        {
                /*
                 * They are reversed so a should be b and visa versa.
                 */
                putc(0x42);
                ol_memnode_t tmp = a;
                a = b;
                b = tmp;
        }
        /*
         * Now we will do the actual merge..
         */
        a->size += b->size+sizeof(struct ol_memory_node);
        a->next = b->next;
        a->flags = OL_BLOCK_UNUSED;
        return a;
}

static void
ol_use_heap_block(ol_memnode_t x)
{
        if(x->previous != NULL)
        {
                /*
                 * we are not at the beginning of the list
                 */
                x->previous->next = x->next;
        }
        else
        {
                /*
                 * if we are at the beginning of the list
                 */
                x->next->previous = NULL;
        }
        if(x->next != NULL)
        {
                /*
                 * we are not at the end of the list
                 */
                x->next->previous = x->previous;
        }
        else
        {
                /*
                 * we are at the end of the list
                 */
                x->previous->next = NULL;
        }
}
