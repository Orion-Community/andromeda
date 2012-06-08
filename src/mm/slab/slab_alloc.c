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
 * \todo Build mm_cache_alloc
 * \todo Build mm_cache_free
 * \todo Build kmem_alloc
 * \todo Build kmem_free
 */
void* mm_cache_alloc(struct mm_cache* cache, uint16_t flags)
{
        if (cache == NULL || flags == 0)
                return NULL;
        return NULL;
}

void mm_cache_free()
{
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