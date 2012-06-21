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

/**
 * \fn mm_slab_move
 * \brief Move the requested entry from list to list.
 * \param from
 * \param to
 * \param entry
 * \return error code
 */
static int
mm_slab_move(from, to, entry)
slab_state from;
slab_state to;
struct mm_slab* entry;
{
        /*
         * first some argument checking (as always)
         */
        if (entry == NULL)
                return -E_NULL_PTR;

        if (from >= 3 || to >= 3 || to == from)
                return -E_INVALID_ARG;

        /*
         * Set the initial variables
         */
        struct mm_cache* cache = entry->cache;
        struct mm_slab* cariage = NULL;

        /*
         * Enter the atomic section
         */
        mutex_lock(&cache->lock);
        /*
         * Select the list to move from
         * If entry is first in the list, unlock it from the pointer then skip
         * the unlocking from the previous part.
         */
        switch (from)
        {
        case state_empty:
                cariage = cache->slabs_empty;
                if (cariage == entry)
                {
                        cache->slabs_empty = entry->next;
                        entry->next = NULL;
                        goto p1;
                }
                break;
        case state_partial:
                cariage = cache->slabs_partial;
                if (cariage == entry)
                {
                        cache->slabs_partial = entry->next;
                        entry->next = NULL;
                        goto p1;
                }
                break;
        case state_full:
                cariage = cache->slabs_full;
                if (cariage == entry)
                {
                        cache->slabs_empty = entry->next;
                        entry->next = NULL;
                        goto p1;
                }
                break;
        }
        /*
         * Try to unlock the entry from its previous pointer.
         * Assuming the entry is in the list, if not we're screwed.
         */
        while (cariage->next != entry && cariage->next != NULL)
                cariage = cariage->next;

        if (cariage->next == NULL)
        {
                /*
                 * Do the "We're screwed" part
                 */
                mutex_unlock(&cache->lock);
                return -E_CORRUPT;
        }

        cariage->next = entry->next;
        entry->next = NULL;

p1:
        /*
         * Set the entry to be the first of the requested list
         */
        switch (to)
        {
        case state_empty:
                entry->next = cache->slabs_empty;
                cache->slabs_empty = entry;
                break;
        case state_partial:
                entry->next = cache->slabs_partial;
                cache->slabs_partial = entry;
                break;
        case state_full:
                entry->next = cache->slabs_full;
                cache->slabs_full = entry;
                break;
        }
        /*
         * Leave the atomic section and return success
         */
        mutex_unlock(&cache->lock);
        return -E_SUCCESS;
}

/**
 * \fn mm_slab_alloc
 * \brief The actual allocation on slab level
 * \param slab
 * \return Pointer to allocated memory
 */
static void*
mm_slab_alloc(struct mm_slab* slab)
{
        /*
         * Some argument checking
         */
        if (slab == NULL)
                return NULL;

        /*
         * Entering the atomic part
         */
        mutex_lock(&slab->lock);

        /*
         * Set up the variables
         */
        int* map = slab->page_ptr;
        int idx = slab->first_free;
        /*
         * Set up the correct first free
         * From now on this memory can't be allocated any more
         */
        slab->first_free = map[idx];
        /*
         * Mark the entry as allocated
         */
        map[idx] = SLAB_ENTRY_ALLOCATED;

        /*
         * Do some counter maintainence
         */
        slab->objs_full ++;
        if (slab->objs_full == slab->objs_total)
                // Move this slab over from slabs_partial to slabs_full
                mm_slab_move(state_partial, state_full, slab);

        /*
         * Leaving the atomic part
         */
        mutex_unlock(&slab->lock);

        /*
         * Hand of the pointer to the just allocated memory
         */
        addr_t tmp = idx*slab->cache->obj_size;
        tmp += (addr_t)slab->obj_ptr;

        return (void*)tmp;
}

/**
 * \fn mm_slab_free
 * \brief The actual freeing code
 * \param slab
 * \param ptr
 * \return Error code
 */
static int
mm_slab_free(struct mm_slab* slab, void* ptr)
{
        if (slab == NULL || ptr == NULL)
                return -E_NULL_PTR;

        addr_t idx = (addr_t)ptr - (addr_t)slab->obj_ptr;
        if (idx % slab->cache->obj_size != 0)
                return -E_INVALID_ARG;

        idx /= slab->cache->obj_size;

        int* map = slab->page_ptr;

        mutex_lock(&slab->lock);

        if (map[idx] != SLAB_ENTRY_ALLOCATED)
        {
                mutex_unlock(&slab->lock);
                return -E_GENERIC;
        }
        map[idx] = slab->first_free;
        slab->first_free = idx;

        if (slab->objs_full == slab->objs_total)
                // Move slab from slabs_full to slabs_partial
                mm_slab_move(state_full, state_partial, slab);

        slab->objs_full --;
        if (slab->objs_full == 0)
                // Move slab from slabs_partial to slabs_empty
                mm_slab_move(state_partial, state_empty, slab);

        mutex_unlock(&slab->lock);

        return -E_NOFUNCTION;
}

/**
 * \todo Build mm_cache_alloc
 * \todo Build mm_cache_free
 * \todo Build kmem_alloc
 * \todo Build kmem_free
 */
void*
mm_cache_alloc(struct mm_cache* cache, uint16_t flags)
{
        if (cache == NULL || flags == 0)
                return NULL;

        mutex_lock(&cache->lock);
        if (cache->slabs_partial == NULL)
        {
                struct mm_slab* tmp = cache->slabs_empty;
                if (tmp == NULL)
                {
                        mutex_unlock(&cache->lock);
                        return NULL;
                }
                cache->slabs_empty = tmp->next;
                cache->slabs_partial = tmp;
                tmp->next = NULL;
        }
        mutex_unlock(&cache->lock);
        return mm_slab_alloc(cache->slabs_partial);
}

static struct mm_slab*
mm_cache_search_ptr(struct mm_slab* list, void* ptr)
{
        for (; list != NULL; list = list->next)
        {
                if (!(list->page_ptr < ptr))
                        continue;
                if (list->page_ptr + list->slab_size > ptr)
                        return list;
        }
        return NULL;
}

int
mm_cache_free(struct mm_cache* cache, void* ptr)
{
        if (cache == NULL || ptr == NULL)
                return -E_NULL_PTR;

        struct mm_slab* tmp = mm_cache_search_ptr(cache->slabs_full, ptr);
        if(tmp != NULL)
                tmp = mm_cache_search_ptr(cache->slabs_partial, ptr);

        return mm_slab_free(tmp, ptr);
}

void*
kmem_alloc(size_t size, uint16_t flags)
{
        if (size == 0 || flags == 0)
                return NULL;

        return NULL;
}

void
kmem_free()
{
}

#ifdef SLAB_DBG

int
mm_cache_test()
{
        return -E_NOFUNCTION;
}
#endif


/**
 * @}
 *\file
 */

#endif