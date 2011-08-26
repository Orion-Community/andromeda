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
        void
        ol_memcpy(void*, void*, size_t);

#ifdef	__cplusplus
}
#endif

#endif	/* MEMORY_H */

