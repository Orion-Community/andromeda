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

#include <stdio.h>
#include <stdlib.h>
#include <mm/cache.h>
/**
 * \AddToGroup slab
 * @{
 */

/** \def NO_STD_CACHES */
#define NO_STD_CACHES 13
static struct mm_cache* caches = NULL;
static struct mm_cache initial_caches[NO_STD_CACHES];
static mutex_t init_lock = mutex_unlocked;

/**
 * \fn slab_alloc_init
 * \brief Initialise the first caches, so the first allocations can be made
 * \return Standard error code
 */
int slab_alloc_init()
{
        textInit();
        caches = initial_caches;
        int idx = 0;
        /** Configure the first caches, one by one */
        for (; idx < NO_STD_CACHES; idx++)
        {
                /** Memset first, then set some pointers */
                memset(&caches[idx], 0, sizeof(*caches));
                caches[idx].obj_size = pow(2, idx+4);
                if (idx != 0)
                        caches[idx].prev = &caches[idx-1];
                if (idx != NO_STD_CACHES-1)
                        caches[idx].next = &caches[idx+1];
                printf("Object size of cache[%X] = %X\n", idx, caches[idx].obj_size);
                /** Slab setup goes here ... */
        }
        for (;;);
        return -E_NOFUNCTION;
}

int
cache_init(char* name, size_t obj_size, size_t cache_size)
{
        return -E_NOFUNCTION;
}

extern int initial_slab_space;

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
