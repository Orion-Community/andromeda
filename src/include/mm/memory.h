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

