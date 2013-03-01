/*
 * Andromeda
 * Copyright (C) 2012  Bart Kuivenhoven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <andromeda/error.h>
#include <mm/page_alloc.h>
#include <thread.h>
/**
 * \AddToGroup Page_alloc
 * @{
 */
extern int pagemap[];
extern int first_free;

spinlock_t page_alloc_lock = mutex_unlocked;

/**
 * \fn page_alloc
 * \brief Allocate a predefined number of physical pages
 */
void* page_alloc()
{
        /* Is there still memory left? */
        if (first_free <= 0)
                return NULL;

        /* Enter critical */
        mutex_lock(&page_alloc_lock);

        /* Fetch the pages to allocate */
        int allocated = first_free;
        if (pagemap[first_free] < 0)
        {
                mutex_unlock(&page_alloc_lock);
                return NULL;
        }
        /* Move the free pointer to the next free */
        first_free = pagemap[first_free];
        /* Mark the allocated pages as allocated by one source */
        pagemap[allocated] = -1;

        /* Leave critical */
        mutex_unlock(&page_alloc_lock);
        /* Convert to address and return pointer */
        return (void*)(allocated*PAGE_ALLOC_FACTOR);
}

/**
 * \fn page_realloc
 * \brief Update the reference count to this piece of memory
 * \param ptr
 * \return The pointer to the reallocated page
 */
void* page_realloc(void* ptr)
{
        if (ptr == NULL)
                return NULL;

        void* ret = NULL;

        addr_t key = (addr_t)ptr;
        if (key % PAGE_ALLOC_FACTOR != 0)
                return NULL;
        int idx = key/PAGE_ALLOC_FACTOR;

        mutex_lock(&page_alloc_lock);

        if (pagemap[idx] >= 0 || pagemap[idx] == PAGE_LIST_MARKED)
                goto err;

        pagemap[idx]--;

        ret = ptr;
err:
        mutex_unlock(&page_alloc_lock);
        return ret;
}

/**
 * \fn pagemap_find_reference
 * \brief internal allocator function
 * \warning Assumes to be ran while page_alloc_lock is locked
 * \param p
 * \brief index of page to find
 * \return index to found page
 */
addr_t pagemap_find_reference(addr_t p)
{
        /* Verify the index to the best of our abilities */
        if (p >= PAGE_LIST_SIZE)
                return -E_INVALID_ARG;
        /* While not at end of list look at next entry */
        addr_t i = 0;
        for (; i < PAGE_LIST_SIZE; i++)
        {
                /* If reference found, return it */
                if (pagemap[i] == p)
                        return i;
        }
        /* Otherwise the input was wrong */
        return -E_INVALID_ARG;
}

void* page_claim(void* page)
{
        if (page == NULL)
                return NULL;

        addr_t idx = (addr_t)page;
        if (idx % PAGE_ALLOC_FACTOR != 0)
                return NULL;

        idx /= PAGE_ALLOC_FACTOR;

        mutex_lock(&page_alloc_lock);
        int ref = pagemap_find_reference(idx);
        if (ref != -E_INVALID_ARG)
                pagemap[ref] = pagemap[idx];

        if (first_free == idx)
                first_free = pagemap[idx];

        pagemap[idx] = -1;

        mutex_unlock(&page_alloc_lock);
        return page;
}

/**
 * \fn page_mark
 * \brief Mark an allocatable physical page as unallocatable
 */
int page_mark(void* page)
{
        /* Verify the input */
        if ((addr_t)page % PAGE_ALLOC_FACTOR != 0)
                return -E_INVALID_ARG;

        /* Put the index into p */
        addr_t p = (addr_t)page / PAGE_ALLOC_FACTOR;

        /* Enter critical, don't forget to leave */
        mutex_lock(&page_alloc_lock);

        /* Do you know any better time to panic than memory corruption? */
        if (pagemap[p] < 0 && pagemap[p] != PAGE_LIST_MARKED)
        {
                printf("page ptr: %X\n"
                       "page key: %X\n"
                       "page idx: %X\n",
                       (int)page,
                       p,
                       pagemap[p]);
                panic("An unallocatable page was allocated!");
        }

        /* If first free equals p, move it up */
        if (first_free == p)
                first_free = pagemap[first_free];
        /* Else do stuff in the list */
        else
        {
                addr_t i = pagemap_find_reference(p);
                if (i != -E_INVALID_ARG)
                        pagemap[i] = pagemap[p];
        }

        /* Mark the page as being unavailable */
        pagemap[p] = PAGE_LIST_MARKED;

        /* Phew, we can leave critical again */
        mutex_unlock(&page_alloc_lock);

        /* Boy am I glad it is over */
        return -E_SUCCESS;
}

/**
 * \fn page_mark
 * \brief Mark an unallocatable physical page as allocatable
 */
int page_unmark(void* page)
{
        /* Verify the pointer */
        if ((addr_t)page % PAGE_ALLOC_FACTOR != 0)
                return -E_INVALID_ARG;

        /* Get the index */
        addr_t p = (addr_t)page / PAGE_ALLOC_FACTOR;
        /* Enter critical */
        mutex_lock(&page_alloc_lock);

        /* Oh noes!!! Wait a sec, there's nothing to be done here! */
        if (pagemap[p] != PAGE_LIST_MARKED)
                goto err;

        /* Mark the page as usable */
        if (first_free < 0)
                pagemap[p] = PAGE_LIST_END;
        else
                pagemap[p] = first_free;

        /* Nah, allocate this page as soon as possible! */
        first_free = p;

err:
        /* Finally, leave critical */
        mutex_unlock(&page_alloc_lock);
        /* That went well, didn't it? */
        return -E_SUCCESS;
}

/**
 * \fn page_free
 * \brief Mark a physical page as free
 */
int page_free(void* page)
{
        /* Determine validity of the pointer */
        if ((addr_t)page % PAGE_ALLOC_FACTOR != 0)
                return -E_INVALID_ARG;
        addr_t p = (addr_t)page / PAGE_ALLOC_FACTOR;

        /* Enter critical */
        mutex_lock(&page_alloc_lock);

        if (pagemap[p] >= 0)
                goto err;
        if (++pagemap[p] == 0)
        {
                /* Mark pages as free */
                pagemap[p] = first_free;
                /* Make pages first free (caching reasons) */
                first_free = p;
        }

err:
        /* Leave critical */
        mutex_unlock(&page_alloc_lock);
        return -E_SUCCESS;
}

#ifdef PA_DBG
void page_dump()
{
        int i = first_free;
        int j = 0;
        for (; i > 0 && i != PAGE_LIST_MARKED; i = pagemap[i], j++)
        {
                if (!(j % 8))
                        demand_key();
                int val = pagemap[i];
                if (val < 0)
                        printf("This: %X -> -%X\n", i, -val);
                else
                        printf("This: %X -> %X\n", i, val);
        }
}
void page_dump2()
{
        int i = 0;
        printf("PAGE_LIST_SIZE: %X\n", PAGE_LIST_SIZE);
        for (; i < PAGE_LIST_SIZE; i++)
        {
                if (!(i % 8))
                        demand_key();
                int val = pagemap[i];
                if (val < 0)
                        printf("This2: %X -> -%X\n", i, -val);
                else
                        printf("This2: %X -> %X\n", i, val);
        }
}
#endif

/**
 * @}
 * \file
 */
