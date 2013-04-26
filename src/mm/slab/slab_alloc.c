/*
 *  Andromeda
 *  Copyright (C) 2012  Bart Kuivenhoven
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
#include <andromeda/core.h>

#ifdef SLAB_DBG
struct mm_cache* last_cache = NULL;
#endif

/**
 * \AddToGroup slab
 * @{
 */

extern struct mm_cache* caches;

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
                        cache->slabs_full = entry->next;
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
mm_slab_alloc(struct mm_slab* slab, int flags)
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
         * If a race conflict has accidentally occured
         * Simply give it another shot!
         *
         * Race conflicts can occur because the call to the mm_slab_alloc
         * function isn't done in an atomic acton.
         */
        if (slab->objs_full == slab->objs_total)
        {
                mutex_unlock(&slab->lock);
                return mm_cache_alloc(slab->cache, flags);
        }

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
        addr_t tmp = idx*slab->cache->alignment;
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
        /*
         * Some standard argument checking
         * along with setting up the variables
         */
        if (slab == NULL || ptr == NULL)
                return -E_NULL_PTR;

        addr_t idx = (addr_t)ptr - (addr_t)slab->obj_ptr;
        if (idx % slab->cache->alignment != 0)
                return -E_INVALID_ARG;

        idx /= slab->cache->alignment;

        int* map = slab->page_ptr;

        /*
         * Verify that the entry actually is allocated
         */
        if (map[idx] != SLAB_ENTRY_ALLOCATED)
                return -E_GENERIC;


        /*
         * And now the atomic parts
         */
        mutex_lock(&slab->lock);

        /*
         * Mark the entry as free
         */
        map[idx] = slab->first_free;
        slab->first_free = idx;

        /*
         * Move the slab if no longer full
         */
        if (slab->objs_full == slab->objs_total)
                // Move slab from slabs_full to slabs_partial
                mm_slab_move(state_full, state_partial, slab);

        /*
         * Update the allocated object counter
         */
        slab->objs_full --;
        /*
         * Move the slab if it is empty
         */
        if (slab->objs_full == 0)
                // Move slab from slabs_partial to slabs_empty
                mm_slab_move(state_partial, state_empty, slab);

        /*
         * Exit the atomic parts now
         */
        mutex_unlock(&slab->lock);

        /*
         * Return the success error code
         */
        return -E_SUCCESS;
}

/**
 * \fn mm_cache_alloc
 * \brief Allocate memory from a particular cache
 * \param cache
 * \param flags
 * \brief How should the allocator behave
 * \note flags are yet to be implemented
 * \return The allocated memory or NULL pointer
 */
void*
mm_cache_alloc(struct mm_cache* cache, uint16_t flags)
{
        /*
         * Some standard argument checking
         */
        if (cache == NULL)
                return NULL;

        /*
         * Enter the atomic section
         * Move the slabs around if necessary
         */
        mutex_lock(&cache->lock);

        /*
         * If the slabs_partial list is empty, try to put a value in there
         */
        if (cache->slabs_partial == NULL)
        {
                /*
                 * If the cache is really empty, return NULL
                 */
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
        /*
         * Leave the atomic section
         */
        mutex_unlock(&cache->lock);

        /*
         * Allocate the memory and get the related pointer
         */
        void* ret = mm_slab_alloc(cache->slabs_partial, flags);
        /*
         * If a constructor exists, run it
         */
        if (cache->ctor != NULL)
                cache->ctor(ret, cache, flags);

        /*
         * Can we now finally return the pointer?
         */
        return ret;
}

/**
 * \fn mm_cache_search_ptr
 * \brief Figure out in which slab a pointer resides
 * \param list
 * \brief The list of slabs in which it should be
 * \param ptr
 * \brief The pointer to look for
 * \return The pointer to the slab or NULL
 */
static struct mm_slab*
mm_cache_search_ptr(struct mm_slab* list, void* ptr)
{
        /*
         * Iterate through the list untill the slab has been found ...
         */
        for (; list != NULL; list = list->next)
        {
                /*
                 * If both conditions are met, return the list pointer
                 */
                if (!(list->page_ptr < ptr))
                        continue;
                if (list->page_ptr + list->slab_size > ptr)
                        return list;
        }
        /*
         * We didn't find anything, so return NULL
         */
        return NULL;
}

/**
 * \fn mm_cache_free
 * \brief Free a pointer from the cache
 * \param cache
 * \param ptr
 * \return Error code or success
 */
int
mm_cache_free(struct mm_cache* cache, void* ptr)
{
        /*
         * Standard argument checking
         */
        if (cache == NULL || ptr == NULL)
                return -E_NULL_PTR;

        /*
         * Enter the atomic section, we don't want to have the slab to move
         * while we're looking for it.
         */
        mutex_lock(&cache->lock);

        struct mm_slab* tmp = mm_cache_search_ptr(cache->slabs_full, ptr);
        if(tmp == NULL)
                tmp = mm_cache_search_ptr(cache->slabs_partial, ptr);

        /*
         * We have found the pointer (we hope) and nothing is going to change
         * that, so we can now leave the atomic section
         */
        mutex_unlock(&cache->lock);

        /*
         * If the argument is correct, return the freeing error code else
         * return an ivalid argument code
         */
        if (tmp == NULL)
                return -E_INVALID_ARG;

        /*
         * Call the destructor if relevant
         */
        if (cache->dtor != NULL)
                cache->dtor(ptr, cache, 0);

        /*
         * And do the actual freeing bit (can you beleive this, only one line!!!
         */
        return mm_slab_free(tmp, ptr);
}

/**
 * \todo Build kmem_alloc
 * \todo Build kmem_free
 */

static struct mm_cache*
kmem_find_size(struct mm_cache* cache, size_t size)
{
        /*
         * Find the smallest allocation candidate
         */
        struct mm_cache* stage = NULL;
        size_t stage_s = ~0;
        for (; cache != NULL; cache = cache->next)
        {
                if (cache->obj_size >= size && cache->obj_size < stage_s)
                {
                        stage = cache;
                        stage_s = cache->obj_size;
                }
        }
        return stage;
}

static struct mm_cache*
kmem_find_next_candidate(struct mm_cache* cache, size_t size)
{
        for(; cache != NULL; cache = cache->next)
                if (cache->obj_size == size)
                        return cache;
        return NULL;
}

void*
kmem_alloc(size_t size, uint16_t flags)
{
        if (size == 0)
                return NULL;
        /*
         * Remember to check the flags once implemented
         */

        struct mm_cache* candidate = kmem_find_size(caches, size);
        if (candidate == NULL)
                return NULL;

        void* ret = mm_cache_alloc(candidate, flags);
        if (ret != NULL)
                goto found;

        size = candidate->obj_size;
        candidate = kmem_find_next_candidate(caches, size);
        while (1)
        {
                if (candidate == NULL)
                        return NULL;
                ret = mm_cache_alloc(candidate, flags);
                if (ret != NULL)
                        goto found;
                candidate = kmem_find_next_candidate(candidate, size);
        }
        return NULL;
found:
#ifdef SLAB_DBG
        last_cache = candidate;
#endif
        return ret;
}

void
kmem_free(void* ptr, size_t size)
{
        if (size == 0 || ptr == NULL)
                panic("Invalid object in kmem_free!");
        struct mm_cache* candidate = kmem_find_size(caches, size);

        if (candidate == NULL)
                return;
        size = candidate->obj_size;

        if (mm_cache_free(candidate, ptr) == -E_SUCCESS)
                goto found;

        candidate = kmem_find_next_candidate(caches, size);
        while (1)
        {
                if (candidate == NULL)
                        return;
                if (mm_cache_free(candidate, ptr) == -E_SUCCESS)
                        goto found;
                candidate = kmem_find_next_candidate(candidate, size);
        }

found:
#ifdef SLAB_DBG
        last_cache = candidate;
#endif
        return;
}

#ifdef SLAB_DBG
void
mm_dump_slab(struct mm_slab* slab)
{
        int* array = slab->page_ptr;
        int i = 0;
        int next = slab->first_free;
        for (; next >= 0; i++)
        {
                if (i % 8 == 0)
                        debug("\n");
                debug("%X ->\t", array[next]);
                next = array[next];
        }
        debug("\n");
        demand_key();
}

void
mm_dump_slabs(struct mm_slab* slab)
{
        for (; slab != NULL; slab = slab->next)
        {
                debug(
                        "Dumping slab:     %X\n"
                        "slab->next:       %X\n"
                        "slab->first_free: %X\n"
                        "slab->objs_full:  %X\n"
                        "slab->objs_total: %X\n"
                        "slab->slab_size:  %X\n",
                      (uint32_t)slab,
                      (uint32_t)slab->next,
                      slab->first_free,
                      slab->objs_full,
                      slab->objs_total,
                      slab->slab_size
                );
                mm_dump_slab(slab);
        }
}

void
mm_dump_cache(struct mm_cache* cache)
{
        debug(
                "Dumping:     %X\n"
                "Cache name:  %s\n"
                "Obj size:    %i\n"
                "Empty ptr:   %X\n"
                "Partial ptr: %X\n"
                "Full ptr:    %X\n",
              (uint32_t)cache,
              cache->name,
              cache->obj_size,
              (uint32_t)cache->slabs_empty,
              (uint32_t)cache->slabs_partial,
              (uint32_t)cache->slabs_full
        );
        demand_key();
        mm_dump_slabs(cache->slabs_empty);
        mm_dump_slabs(cache->slabs_partial);
        mm_dump_slabs(cache->slabs_full);
}

int
mm_test_bulk(struct mm_cache* tst, int bastard_mode)
{
        debug("\nTesting bulk\n");
        int objs_total = tst->slabs_empty->objs_total;
        void* array[objs_total];
        int i = 0;
        for (; i < objs_total; i++)
        {
                array[i] = mm_cache_alloc(tst, 0);
                if (array[i] == NULL)
                {
                        debug("Test failed, could not allocate\n\n");
                        mm_dump_cache(tst);
                        return -E_GENERIC;
                }
        }
        debug("Bulk seems successful\n");
        mm_dump_cache(tst);
        for (i = 0; i < objs_total; i++)
                debug("%X\t", array[i]);
        demand_key();

        if (bastard_mode != 0)
        {
                void* bastard = mm_cache_alloc(tst, 0);
                if (bastard != NULL)
                {
                        debug("Test failed as mm_cache_alloc returned %X\n",
                                (uint32_t)bastard
                        );
                        mm_dump_cache(tst);
                        return -E_GENERIC;
                }
                debug("\nBastard test completed\n");
        }
        debug("\nTesting bulk free\n");
        for (i = 0; i < objs_total; i++)
        {
                if (mm_cache_free(tst, array[i]) != -E_SUCCESS)
                {
                        debug("Test failed, could not free\n\n");
                        mm_dump_cache(tst);
                        return -E_GENERIC;
                }
        }
        debug("Bulk free seems successful\n");
        mm_dump_cache(tst);
        return -E_SUCCESS;
}

int
mm_cache_test()
{
        if (caches == NULL)
                return -E_NOT_YET_INITIALISED;

        debug("\n\nTesting slab allocation\n");

        struct mm_cache* tst = &caches[4];
        mm_dump_cache(tst);

        void* tmp = mm_cache_alloc(tst, 0);
        if (tmp == NULL)
        {
                debug("Test failed, could not allocate\n\n");
                mm_dump_cache(tst);
                return -E_GENERIC;
        }
        debug("Test allocation: %X\n", (uint32_t) tmp);
        debug("\nTesting freeing\n");
        (tst);
        int tmp1 = mm_cache_free(tst, tmp);
        if (tmp1 != -E_SUCCESS)
        {
                debug("Test failed, could not free\n\n");
                mm_dump_cache(tst);
                return -E_GENERIC;
        }

        debug("Free successful\n");
        mm_dump_cache(tst);

        if (mm_test_bulk(tst, 0) == -E_SUCCESS)
        {
                debug("\n\nGoing for second bulk\n\n");
                if (mm_test_bulk(tst, 1) != -E_SUCCESS)
                        return -E_GENERIC;
        }
        else
                return -E_GENERIC;

        debug("Testing kmem_alloc\n");
        tmp = kmem_alloc(0x20, 0);
        mm_dump_cache(last_cache);

        debug("Test allocation: %X\n", tmp);
        debug("last_cache:      %X\n", last_cache);
        demand_key();

        struct mm_cache* stage = last_cache;
        kmem_free(tmp, 0x20);
        if (stage != last_cache)
        {
                debug("Freeing failed\n");
                mm_dump_cache(stage);
                mm_dump_cache(last_cache);
                return -E_GENERIC;
        }
        debug("Freeing succeeded\n");
        mm_dump_cache(last_cache);

        debug("\nBulk allocating 0x20 in size\n");
        int size =last_cache->slabs_empty->objs_total;
        void* bulk[size];
        int idx = 0;
        for (; idx < size; idx++)
        {
                bulk[idx] = kmem_alloc(0x20, 0);
                if (bulk[idx] == NULL)
                {
                        debug(
                            "Bulk allocating through kmem_alloc failed at: %i\n"
                            , idx
                        );
                        mm_dump_cache(last_cache);
                        return -E_GENERIC;
                }
        }
        debug("Allocated all of them!\n");
        mm_dump_cache(last_cache);
        for (idx = 0; idx < size; idx++)
        {
                kmem_free(bulk[idx], 0x20);
        }
        debug("Freed all of them!\n");
        mm_dump_cache(last_cache);

        debug("\nTest successful\n");

        return -E_NOFUNCTION;
}
#endif

/**
 * @}
 *\file
 */
