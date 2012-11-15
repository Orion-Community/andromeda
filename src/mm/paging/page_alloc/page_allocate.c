/*
 *  Orion OS, The educational operatingsystem
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <andromeda/error.h>
#include <mm/page_alloc.h>
#include <thread.h>

extern int pagemap[];
extern int first_free;

spinlock_t page_alloc_lock = mutex_unlocked;

/**
 * \fn page_alloc
 * \brief Allocate a predefined number of pages
 */
void* page_alloc()
{
        /* Is there still memory left? */
        if (first_free == PAGE_LIST_ALLOCATED || first_free == PAGE_LIST_MARKED)
                return NULL;

        /* Enter critical */
        mutex_lock(&page_alloc_lock);

        /* Fetch the pages to allocate */
        int allocated  = first_free;
        /* Move the free pointer to the next free */
        first_free = pagemap[first_free];
        /* Mark the allocated pages as allocated */
        pagemap[allocated] = PAGE_LIST_ALLOCATED;

        /* Leave critical */
        mutex_unlock(&page_alloc_lock);
        /* Convert to address and return pointer */
        return (void*)(allocated*PAGE_ALLOC_FACTOR);
}

addr_t pagemap_find_reference(addr_t p)
{
        if (p % PAGE_ALLOC_FACTOR != 0)
                return -E_INVALID_ARG;

        addr_t i = first_free;
        for (; i != PAGE_LIST_ALLOCATED || PAGE_LIST_MARKED; i = pagemap[i])
        {
                if (pagemap[i] == p)
                        return i;
        }
        return -E_INVALID_ARG;
}

/**
 * \fn page_mark
 * \brief Mark an allocatable page as unallocatable
 */
int page_mark(void* page)
{
        if ((addr_t)page % PAGE_ALLOC_FACTOR != 0)
                return -E_INVALID_ARG;

        addr_t p = (addr_t)page / PAGE_ALLOC_FACTOR;

        mutex_lock(&page_alloc_lock);

        if (pagemap[p] == PAGE_LIST_ALLOCATED)
                panic("An unallocatable page was allocated!");

        if (first_free == p)
                first_free = pagemap[first_free];

        else
        {
                addr_t i = pagemap_find_reference(p);
                if (i != -E_INVALID_ARG)
                        pagemap[i] = pagemap[p];
        }

        pagemap[p] = PAGE_LIST_MARKED;

        mutex_unlock(&page_alloc_lock);

        return -E_SUCCESS;
}

/**
 * \fn page_mark
 * \brief Mark an unallocatable page as allocatable
 */
int page_unmark(void* page)
{
        if ((addr_t)page % PAGE_ALLOC_FACTOR != 0)
                return -E_INVALID_ARG;

        addr_t p = (addr_t)page / PAGE_ALLOC_FACTOR;
        mutex_lock(&page_alloc_lock);

        if (pagemap[p] != PAGE_LIST_MARKED)
                goto err;

        pagemap[p] = first_free;
        first_free = p;

err:
        mutex_unlock(&page_alloc_lock);
        return -E_NOFUNCTION;
}

/**
 * \fn page_free
 */
int page_free(void* page)
{
        /* Determine validity of the pointer */
        if ((addr_t)page % PAGE_ALLOC_FACTOR != 0)
                return -E_INVALID_ARG;
        addr_t p = (addr_t)page / PAGE_ALLOC_FACTOR;

        /* Enter critical */
        mutex_lock(&page_alloc_lock);

        /* Mark pages as free */
        pagemap[p] = first_free;
        /* Make pages first free (caching reasons) */
        first_free = p;

        /* Leave critical */
        mutex_unlock(&page_alloc_lock);
        return -E_SUCCESS;
}
