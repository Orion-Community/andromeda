/*
 *  Andromeda
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
#include <mm/cache.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef SLAB

/**
 * \AddToGroup slab
 * @{
 */

void* mm_slab_alloc(struct mm_slab* slab)
{
        if (slab == NULL)
                return NULL;

        return NULL;
}

int mm_slab_free(struct mm_slab* slab, void* ptr)
{
        return -E_NOFUNCTION;
}

/**
 * \todo Build mm_cache_alloc
 * \todo Build mm_cache_free
 * \todo Build kmem_alloc
 * \todo Build kmem_free
 */
void* mm_cache_alloc(struct mm_cache* cache, uint16_t flags)
{
        if (cache == NULL || flags == 0)
                return NULL;

        if (cache->slabs_partial == NULL)
        {
                struct mm_slab* tmp = cache->slabs_empty;
                if (tmp == NULL)
                        return NULL;
                cache->slabs_empty = tmp->next;
                cache->slabs_partial = tmp;
                tmp->next = NULL;
        }
        return mm_slab_alloc(cache->slabs_partial);
}

static struct mm_slab*
mm_cache_search_ptr(struct mm_slab* list, void* ptr)
{
        for (; list != NULL; list = list->next)
        {
                if (list->page_ptr < ptr && list->page_ptr + list->slab_size > ptr)
                        return list;
        }
        return NULL;
}

int mm_cache_free(struct mm_cache* cache, void* ptr)
{
        if (cache == NULL || ptr == NULL)
                return -E_NULL_PTR;

        struct mm_slab* tmp = mm_cache_search_ptr(cache->slabs_full, ptr);
        if(tmp != NULL)
                tmp = mm_cache_search_ptr(cache->slabs_partial, ptr);

        return mm_slab_free(tmp, ptr);
}

void* kmem_alloc(size_t size, uint16_t flags)
{
        if (size == 0 || flags == 0)
                return NULL;

        return NULL;
}

void kmem_free()
{
}


/**
 * @}
 *\file
 */

#endif