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
static struct mm_cache* caches = NULL;
static mutex_t init_lock = mutex_unlocked;

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
