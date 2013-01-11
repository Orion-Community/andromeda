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

#ifndef __MM_CACHE_H
#define __MM_CACHE_H

#include <defines.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SLAB

/**
 * \defgroup slab
 * The slab allocator
 * @{
 */

#define CACHE_NAME_SIZE 255
#define SLAB_MAX_OBJS 256
#define SLAB_ENTRY_ALLOCATED ((~0)-1)
#define SLAB_ENTRY_FALSE ((~0))

typedef enum {state_empty, state_partial, state_full} slab_state;

/**
 * \struct slab
 * \brief The slab descriptor
 */

struct mm_slab {
        /**
         * \var next
         * \var cache
         * \var obj_ptr
         * \brief pointer to first object in slab
         * \var page_ptr
         * \brief Which page are we talking about?
         * \var slab_size
         * \var objs_full
         * \var objs_total
         */
        struct mm_slab* next;
        struct mm_cache* cache;

        void* obj_ptr;
        void* page_ptr;

        size_t slab_size;

        int first_free;
        int objs_full;
        int objs_total;
        mutex_t lock;
};

typedef void (*cinit)(void*, struct mm_cache*, uint32_t flags);

/**
 * \struct mm_cache
 * \brief The slab allocation caches
 */

struct mm_cache {
        char name[CACHE_NAME_SIZE];
        struct mm_slab* slabs_full;
        struct mm_slab* slabs_partial;
        struct mm_slab* slabs_empty;

        size_t obj_size;
        size_t alignment;

        cinit ctor;
        cinit dtor;

        struct mm_cache* next;
        struct mm_cache* prev;

        mutex_t lock;
};


int slab_alloc_init();
int test_calculation_functions();
struct mm_cache* mm_cache_init(char*, size_t, size_t, cinit, cinit);
void* mm_cache_alloc(struct mm_cache* cache, uint16_t flags);
int mm_cache_free(struct mm_cache* cache, void* ptr);

#ifdef SLAB_DBG
int mm_cache_test();
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 *\file
 */
