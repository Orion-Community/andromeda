/* 
 * File:   memory.h
 * Author: michel
 *
 * Created on August 16, 2011, 3:15 PM
 */

#ifndef MEMORY_H
#define	MEMORY_H

#include <mm/heap.h>
#include <stdlib.h>


#ifdef	__cplusplus
extern "C"
{
#endif
        extern volatile ol_memnode_t heap;
        
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
         * This function will traverse trough the linked list of mem nodes,
         * starting at the given node. It will look for the first node which is
         * larger then the specified size.
         * 
         * @param The algorithm will search from this function onwards.
         * @param The minimum size the algorithm will search for.
         * @return Returns a pointer to the found memory node. If no nodes are
         * found, then it will return a null pointer.
         */
        //static volatile ol_memnode_t ol_search_node(ol_memnode_t, uint32_t);

#ifdef	__cplusplus
}
#endif

#endif	/* MEMORY_H */

