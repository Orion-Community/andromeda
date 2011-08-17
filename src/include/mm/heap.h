/*
 *   openLoader project - Heap header file.
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

#ifndef __HEAP_H
#define	__HEAP_H

#ifdef	__cplusplus
/*
 * Just in the case I ever implement some C++ in my kernel
 */

extern "C" {
#endif

        /*
         * This structure defines the heap.
         */
        struct ol_memory_node
        {
                uint8_t magic;
                uint8_t flags;
                uint32_t size;
                void *base;
                struct ol_memory_node *previous;
                struct ol_memory_node *next;
        } __attribute__((packed));
        typedef struct ol_memory_node *ol_memnode_t;
        
        void ol_dbg_heap();

        /**
         * This alloc does zero safety checks. It just searches for available ram
         * and then it returns it to you. This you should use this allocator when
         * you would like to allocate a very big block.
         * 
         * @param Amount of bytes you want.
         * @return Pointer to the memory if it exists, NULL pointer otherwise.
         */
        void * salloc(size_t); /* This alloc is only used to allocate
                                               * memory for the guest system and
                                               * its modules.
                                               */
        
        /**
         * For all the usual, daily memory allocations. It does certain safety
         * checks before it returns the request memory (if available).
         * 
         * @param Amount of ram you are asking for.
         * @return Pointer to the allocated RAM.
         */
        void * kalloc(size_t);
        
        /**
         * Allocate the given amount of memory (if it is available).
         * @param Amount of ram to allocate.
         * @param Should the allocator do certain checks (such as checking if
         * the requested size is bigger than the maximum allow). True if it should
         * do safety checks, false other wise.
         * @return Pointer to the allocated memory if it is available, if it is
         * NOT available then it will return with a NULL pointer.
         */
        static void * alloc(size_t, bool);
        
        
        /**
         * Initializes the heap.
         */
        void ol_init_heap();
        
        /**
         * Add a heap header to a memory block.
         * @param Block pointer
         * @param Block size
         */
        static ol_memnode_t ol_add_heaphdr(void *, size_t);
        
        /**
         * Add a new block to the heap.
         * @param base Pointer to the bottom of the block.
         * @param len Length of the block.
         * @return The block.
         */
        static volatile ol_memnode_t ol_add_block(void*, size_t);
        
        /**
         * Merges two blocks again.
         * 
         * @param block one
         * @param block two
         * @return Pointer to the new, merged block. If the merge failed it will
         * return a NULL pointer.
         */
        static volatile ol_memnode_t 
        ol_merge_memnodes(ol_memnode_t, ol_memnode_t);
        
        /**
         * Splits a size_t part of the given block.
         * 
         * @param The block to split.
         * @param The amount of bytes which should be split away from the
         * block.
         * @return If the split was successful, the bottom block. A NULL pointer
         * otherwise.
         */
        volatile ol_memnode_t ol_split_memnode(ol_memnode_t, size_t);
        
        /**
         * Check if a block is in use or not, and mark it as used if it was not
         * in use yet.
         * 
         * @param The block to test.
         * @return TRUE if the block was not in use yet and is marked as in use
         * successfully. If it was already in use, FALSE.
         */
        static bool useblock(ol_memnode_t);
        
        static void ol_use_heap_block(ol_memnode_t);
        
        static void ol_return_heap_block(ol_memnode_t);
        
#define OL_BLOCK_UNUSED 0x0
#define OL_BLOCK_INUSE 0x1
#define OL_HEAP_MAGIC 0xfafa
#define OL_NODE_SIZE(start, end) ((start-end)*-1)-sizeof(struct ol_memory_node)
#define OL_BLOCK_BASE(nodeaddr) (sizeof(struct ol_memory_node)+nodeaddr)
#define OL_MAX_ALLOC_SIZE 0x10000 /* allocate max 64KiB of date in normal allocs */
        
#ifdef	__cplusplus
}
#endif

#endif	/* HEAP_H */

