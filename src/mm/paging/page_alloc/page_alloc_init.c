/*
 *  Orion OS, The educational operatingsystem
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
#include <andromeda/error.h>
#include <mm/page_alloc.h>
#include <boot/mboot.h>

int pagemap[PAGE_LIST_SIZE];
int first_free = PAGE_LIST_MARKED;

int mboot_parse(multiboot_memory_map_t* map, int map_size)
{
        /* Pointer to the mboot list entry */
        multiboot_memory_map_t* mmap = map;

        /* While not outside of the mboot list */
        while((addr_t)mmap < (addr_t)map + map_size)
        {
                if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE)
                        goto itteration_skip;
                /* Parse each entry here */
                if (mmap->addr < SIZE_MEG && mmap->addr + mmap->size > SIZE_MEG)
                {
                        addr_t ptr = SIZE_MEG;
                        size_t size = mmap->size - (SIZE_MEG - mmap->addr);
                        for (; ptr < mmap->addr+size; ptr += PAGE_ALLOC_FACTOR)
                                page_unmark(ptr);
                }
                if (mmap->addr >= SIZE_MEG)
                {
                        addr_t ptr = mmap->addr;
                        size_t size = mmap->size;
                        for (; ptr < mmap->addr + mmap->size;
                                ptr += PAGE_ALLOC_FACTOR)
                        {
                                page_unmark(ptr);
                        }
                }

        itteration_skip:
                mmap = (void*)((addr_t)mmap + mmap->size+sizeof(mmap->size));
        }
}

int page_alloc_init()
{
        int i = 0;
        while (i < PAGE_LIST_SIZE)
        {
                pagemap[i] = PAGE_LIST_MARKED;
        }

        pagemap[PAGE_LIST_SIZE-1] = PAGE_LIST_MARKED;
        pagemap[0] = PAGE_LIST_MARKED;

        /**
         * Parse the grub memory map here to mark all the unusable pages as
         * Allocated
         */



        return -E_NOFUNCTION;
}
