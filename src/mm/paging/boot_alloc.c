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

#define BOOT_HEAP_SIZE 0x10000

static char boot_heap_mem[BOOT_HEAP_SIZE];
static void* boot_heap = &boot_heap_mem;
static size_t boot_heap_free = BOOT_HEAP_SIZE;

void*
boot_alloc(size_t size)
{
        if (boot_heap_free < size)
                return NULL;

        void* ret = boot_heap;
        boot_heap = (void*)((addr_t)boot_heap + size);
        return ret;
}

/** \file */
