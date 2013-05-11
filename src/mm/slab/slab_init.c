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

#include <stdio.h>
#include <stdlib.h>
#include <mm/cache.h>
#include <mm/page_alloc.h>
#include <mm/vm.h>
#include <andromeda/system.h>


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

struct mm_cache* caches = NULL;
static struct mm_cache initial_caches[NO_STD_CACHES];
static struct mm_slab initial_slabs[NO_STD_CACHES];
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
        if (req % PAGE_ALLOC_FACTOR != 0)
                req += PAGE_ALLOC_FACTOR - req % PAGE_ALLOC_FACTOR;

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

#ifdef SLAB_DBG
int test_calc_unit(int size, int alignment, int no_objects)
{
        char txt[256];
        memset(txt, 0, 256);

        int pages = calc_no_pages(size, no_objects, alignment);
        int no_elements = calc_max_no_objects(alignment, pages, size);
        int offset = calc_data_offset(alignment);

        sprintf(txt, "Pages: %8X\telements: %8X\toffset: %8X\n", pages,
                                                           no_elements, offset);
        debug(txt);
        return -E_SUCCESS;
}

int
test_calculation_functions()
{
        int ret = 1;
        if (test_calc_unit(16, 16, 16) != -E_SUCCESS)
                goto err;
        ret ++;
        if (test_calc_unit(32, 16, 16) != -E_SUCCESS)
                goto err;
        ret++;
        if (test_calc_unit(16, 32, 16) != -E_SUCCESS)
                goto err;
        ret ++;
        if (test_calc_unit(0x1000, 0x1000, 16) != -E_SUCCESS)
                goto err;
        ret ++;
        if (test_calc_unit(0x400, 0x1000, 16) != -E_SUCCESS)
                goto err;
        ret ++;

        if (mm_cache_init("Blaat", sizeof(struct mm_cache), 255, NULL, NULL)
                                                                        == NULL)
                goto err;
        ret ++;

        return -E_SUCCESS;
err:
        debug("The test failed on item: %X\n", ret);
        return ret;
}
#endif

/**
 * \fn slab_setup
 * \brief The code that actually sets up the slab itself
 * \param slab
 * \brief The slab to be initialised
 * \param cache
 * \brief The cache which is its parent
 * \param pages
 * \brief The pointer to the pages we can use
 * \param no_pages
 * \brief The number of pages that are allocated to us, WRITTEN IN BYTES
 * \param no_elements
 * \brief The number of elements that we've got in our slab
 * \return An error code or success
 */
int
slab_setup (slab, cache, pages, no_pages, no_elements)
struct mm_slab* slab;
struct mm_cache* cache;
void* pages;
register size_t no_pages;
register size_t no_elements;
{
        if (slab == NULL || cache == NULL || pages == NULL)
                return -E_NULL_PTR;
        if (no_pages == 0 || no_elements == 0)
                return -E_INVALID_ARG;
        register size_t data_offset = calc_data_offset(cache->alignment);

        memset(slab, 0, sizeof(*slab));
        slab->page_ptr = pages;
        slab->obj_ptr = pages + data_offset;
        slab->slab_size = no_pages;
        slab->cache = cache;
        slab->objs_total = no_elements;

        memset(pages, 0, no_pages);

        int i = 0;
        int* alloc_space = pages;

        for (; i < no_elements; i++)
                alloc_space[i] = i+1;

        int j = i;
        for (;j < SLAB_MAX_OBJS; j++)
                alloc_space[j] = SLAB_ENTRY_FALSE;

#ifdef SLAB_DBG
#ifdef SLAB_SHOW_OBJS
        debug(
                "%i    : %X\n"
                "%i + 1: %X\n"
                "%i - 1: %i\n"
                "0     : %i\n",
               i, alloc_space[i],
               i, alloc_space[i+1],
               i, alloc_space[i-1],
               alloc_space[0]
        );
#endif
#endif

        return -E_SUCCESS;
}

int
cache_find_slab_space(struct mm_cache* cache, idx_t slab_idx)
{
        size_t no_pages = calc_no_pages(cache->obj_size, SLAB_MIN_OBJS,
                                                              cache->alignment);
        size_t no_elements = calc_max_no_objects(cache->alignment, no_pages,
                                                               cache->obj_size);
        cache->slabs_empty = &initial_slabs[slab_idx];

        if (slab_setup(cache->slabs_empty, cache, init_slab_ptr, no_pages,
                                                     no_elements) != -E_SUCCESS)
                panic("Something somewhere went terribly wrong (SLAB)!");

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
#ifdef SLAB_DBG
        debug("Initial slab ptr: %X\n", &initial_slab_space);
#endif
        caches = initial_caches;
        int idx = 0;
        init_slab_ptr = &initial_slab_space;
        int alignment = 4;
        /** Configure the first caches, one by one */
        for (; idx < NO_STD_CACHES; idx++)
        {
                alignment += alignment;
                /** Memset first, then set some pointers */
                memset(&caches[idx], 0, sizeof(*caches));
                caches[idx].obj_size = alignment;
                caches[idx].alignment = alignment;
                sprintf(caches[idx].name, "size-%i", alignment);

                if (idx != 0)
                        caches[idx].prev = &caches[idx-1];
                else
                        caches[idx].prev = NULL;
                if (idx != NO_STD_CACHES-1)
                        caches[idx].next = &caches[idx+1];
                else
                        caches[idx].prev = NULL;
#ifdef SLAB_DBG
                debug("Object size of cache[%X] = %X\n", idx,
                                                          caches[idx].obj_size);
#endif
                cache_find_slab_space(&caches[idx], idx);
        }
#ifdef SLAB_DBG
        debug("Final slab ptr: %X\n", init_slab_ptr);
        debug("Address of higherhalf: %X\n", &higherhalf);
//         for (;;);
#endif
        return -E_SUCCESS;
}

int slab_sys_register()
{
        if (core.mm == NULL)
                return -E_NOT_YET_INITIALISED;

        core.mm->alloc = kmem_alloc;
        core.mm->free = kmem_free;

        return -E_SUCCESS;
}

static mutex_t cache_lock = mutex_unlocked;

/**
 * \fn mm_cache_init
 * \brief Initialise and return a new cache
 * \param name
 * \brief A description of the cache
 * \param obj_size
 * \param alignment
 * \param ctor
 * \brief A constructor for objects to be allocated
 * \param dtor
 * \brief A deconstuctor for the objects to be freed
 * \return The new cache
 */
struct mm_cache*
mm_cache_init(name, obj_size, alignment, ctor, dtor)
char* name;
size_t obj_size;
size_t alignment;
cinit ctor;
cinit dtor;
{
        struct mm_cache* cariage = caches;
        if (caches == NULL || name == NULL)
                return NULL;
        if (obj_size == 0)
                return NULL;

        mutex_lock(&cache_lock);

        for (; cariage->next != NULL; cariage = cariage->next);

        cariage->next = kmalloc(sizeof(*cariage->next));
        if (cariage->next == NULL)
                goto err_nomem;
        cariage = cariage->next;
        memset(cariage, 0, sizeof(*cariage));

        memcpy(cariage->name, name, strlen(name));
        cariage->obj_size = obj_size;
        cariage->alignment = alignment;
        cariage->ctor = ctor;
        cariage->dtor = dtor;

        mutex_unlock(&cache_lock);

        return cariage;

err_nomem:
        mutex_unlock(&cache_lock);
        return NULL;
}

struct mm_slab*
mm_slab_init (cache, obj_size, no_objects, alignment)
struct mm_cache* cache;
size_t obj_size;
size_t no_objects;
size_t alignment;
{
        if (cache == NULL || obj_size == 0 || no_objects == 0 || alignment == 0)
                goto err;
        struct mm_slab* slab = kmalloc(sizeof(*slab));
        if (slab == NULL)
                goto err;

        size_t no_pages = calc_no_pages(cache->obj_size, SLAB_MIN_OBJS,
                                                              cache->alignment);
        size_t no_elements = calc_max_no_objects(cache->alignment, no_pages,
                                                               cache->obj_size);
        /**
         * \todo Actually allocate a page in mm_slab_init
         */
        void* page = vm_get_kernel_heap_pages(no_pages);
        if (page == NULL)
                goto err_alloced;

        if (slab_setup(cache->slabs_empty, cache, page, no_pages, no_elements)
                                                                  != -E_SUCCESS)
                goto err_page;

        return slab;
err_page:
        /**
         * \todo Actually free the page in mm_slab_init
         */
        // Should free pages here!
err_alloced:
        kfree(slab);
err:
        return NULL;
}

/**
 * @}
 *\file
 */
