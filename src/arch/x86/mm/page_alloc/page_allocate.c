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
#include <arch/x86/mm/page_alloc.h>
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
        if (first_free == PAGE_LIST_ALLOCATED)
                return NULL;

        mutex_lock(&page_alloc_lock);

        int allocated  = first_free;
        first_free = pagemap[first_free];
        pagemap[allocated] = PAGE_LIST_ALLOCATED;

        mutex_unlock(&page_alloc_lock);
        return (void*)(allocated*PAGE_ALLOC_FACTOR);
}

int page_free(void* page)
{
        mutex_lock(&page_alloc_lock);

        int p = (int)page/PAGE_ALLOC_FACTOR;
        if (p >= PAGE_LIST_SIZE)
                goto err;

        pagemap[p] = first_free;
        first_free = p;

        mutex_unlock(&page_alloc_lock);
        return -E_SUCCESS;

err:
        mutex_unlock(&page_alloc_lock);
        return -INVALID_ARG;
}
