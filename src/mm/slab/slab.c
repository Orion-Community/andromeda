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

#ifdef SLAB

#include <stdio.h>
#include <stdlib.h>
#include <mm/cache.h>
/**
 * \AddToGroup slab
 * @{
 */

/**
 * \def NO_STD_CACHES
 * \brief This define is related to the initial_slab_space in the linker script.
 * \warning If changing this number, check that the linker script still is ok.
 */
#define NO_STD_CACHES 11
#define SLAB_MIN_OBJS 16

extern int initial_slab_space;

static struct mm_cache* caches = NULL;
static struct mm_cache initial_caches[NO_STD_CACHES];
static struct slab initial_slabs[NO_STD_CACHES];
static void* init_slab_ptr = NULL;
static mutex_t init_lock = mutex_unlocked;

/**
 * \fn calc_no_pages
 * \brief Calculate the number of pages (in bytes) required for an initial slab
 * \note Also takes allocation region into account
 * \param element_size
 * \param no_elements
 * \param alignment
 * \return result of calculation in bytes
 */
size_t
calc_no_pages(size_t element_size, idx_t no_elements, size_t alignment)
{
        /* Check the arguments */
        if (element_size == 0)
                return 0;

        /* Get the size of the elements right */
        if (element_size % alignment != 0)
                element_size += alignment - element_size % alignment;

        /* How much space is required for the allocation_frame */
        size_t allocation_frame = SLAB_MAX_OBJS * sizeof(int);
        if (allocation_frame % alignment != 0)
                allocation_frame += alignment - allocation_frame % alignment;

        /* How much space is required for the actual elements + frame */
        size_t req = allocation_frame + element_size * no_elements;

        /* Pad the requirement to page size */
        if (req % PAGESIZE != 0)
                req += PAGESIZE - req % PAGESIZE;

        return req;
}

/**
 * \fn calc_data_offset
 * \brief Calculate the offset of the data to the start of the slab
 * \param alignment
 * \return The result of the calculation in pages
 */
size_t
calc_data_offset(size_t alignment)
{
        size_t offset = SLAB_MAX_OBJS * sizeof(int);
        if (offset % alignment != 0)
                offset += alignment - offset % alignment;
        return offset;
}

/**
 * \fn calc_max_no_objects
 * \brief Calculate the max number of objects per unit of memory
 * \param alignment
 * \brief To what are the addresses aligned
 * \param obj_space
 * \brief What is the space in bytes we can use to allocate
 * \param obj_size
 * \brief How big are the objects we're going to allocate
 * \return The number of objects usable within the unit of memory
 */
size_t
calc_max_no_objects(size_t alignment, size_t obj_space, size_t obj_size)
{
        if (obj_size == 0 || obj_space == 0)
                return 0;

        if (obj_size % alignment != 0)
                obj_size += alignment - obj_size % alignment;

        size_t alloc_bytes = SLAB_MAX_OBJS * sizeof(int);
        if (alloc_bytes % alignment != 0)
                alloc_bytes += alignment - alloc_bytes % alignment;

        obj_space -= alloc_bytes;
        return obj_space / obj_size;
}

int test_calc_unit(int size, int alignment, int no_objects)
{
        char* txt = kalloc(256);
        if (txt == NULL)
                return -E_NOMEM;
        memset(txt, 0, 256);

        int pages = calc_no_pages(size, no_objects, alignment);
        int no_elements = calc_max_no_objects(alignment, pages, size);
        int offset = calc_data_offset(alignment);

        sprintf(txt, "Pages: %8X\telements: %8X\toffset: %8X\n", pages,
                                                           no_elements, offset);
        debug(txt);

        free(txt);
        return -E_SUCCESS;
}

int
test_calculation_functions()
{
        if (test_calc_unit(16, 16, 16) != -E_SUCCESS)
                return -E_GENERIC;
        if (test_calc_unit(32, 16, 16) != -E_SUCCESS)
                return -E_GENERIC;
        if (test_calc_unit(16, 32, 16) != -E_SUCCESS)
                return -E_GENERIC;
        if (test_calc_unit(0x1000, 0x1000, 16) != -E_SUCCESS)
                return -E_GENERIC;
        if (test_calc_unit(0x400, 0x1000, 16) != -E_GENERIC)
                return -E_GENERIC;

        return -E_SUCCESS;
}

int
cache_find_slab_space(struct mm_cache* cache, idx_t slab_idx)
{
        register size_t no_pages = calc_no_pages(cache->obj_size,
                                                                  SLAB_MIN_OBJS,
                                                              cache->alignment);
        register size_t no_elements = calc_max_no_objects(cache->alignment,
                                                                       no_pages,
                                                               cache->obj_size);
        register size_t data_offset = calc_data_offset(cache->alignment);

        cache->slabs_empty = &initial_slabs[slab_idx];
        memset(cache->slabs_empty, 0, sizeof(*cache->slabs_empty));
        cache->slabs_empty->page_ptr = init_slab_ptr;
        cache->slabs_empty->obj_ptr = init_slab_ptr + data_offset;
        cache->slabs_empty->slab_size = no_pages;
        cache->slabs_empty->cache = cache;
        cache->slabs_empty->objs_total = no_elements;

        init_slab_ptr+= no_pages;
        return -E_SUCCESS;
}

/**
 * \fn slab_alloc_init
 * \brief Initialise the first caches, so the first allocations can be made
 * \return Standard error code
 */
int slab_alloc_init()
{
        textInit();
        debug("Initial slab ptr: %X\n", &initial_slab_space);
        caches = initial_caches;
        int idx = 0;
        init_slab_ptr = &initial_slab_space;
        /** Configure the first caches, one by one */
        for (; idx < NO_STD_CACHES; idx++)
        {
                int alignment = pow(2, idx+4);
                /** Memset first, then set some pointers */
                memset(&caches[idx], 0, sizeof(*caches));
                caches[idx].obj_size = alignment;
                caches[idx].alignment = alignment;

                if (idx != 0)
                        caches[idx].prev = &caches[idx-1];
                else
                        caches[idx].prev = NULL;
                if (idx != NO_STD_CACHES-1)
                        caches[idx].next = &caches[idx+1];
                else
                        caches[idx].prev = NULL;
                debug("Object size of cache[%X] = %X\n", idx,
                                                          caches[idx].obj_size);

                cache_find_slab_space(&caches[idx], idx);
        }
        debug("Final slab ptr: %X\n", init_slab_ptr);
        debug("Address of higherhalf: %X\n", &higherhalf);
//         for (;;);
        return -E_NOFUNCTION;
}

int
cache_init(char* name, size_t obj_size, size_t cache_size)
{
        return -E_NOFUNCTION;
}

int
init_slab()
{
        mutex_lock(&init_lock);
        if (caches != NULL)
        {
                mutex_unlock(&init_lock);
                return -E_ALREADY_INITIALISED;
        }

        caches = (void*)&initial_slab_space;
        struct mm_cache* carriage = caches;
        register int i = 0;
        for (; i < 0x10; i++)
        {
                memset(carriage, 0, sizeof(*carriage));
                carriage++;
        }

        mutex_unlock(&init_lock);

        return -E_NOFUNCTION;
}

/**
 * @}
 *\file
 */

#endif
