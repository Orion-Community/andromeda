/*
 * Andromeda
 * Copyright (C) 2012  Bart Kuivenhoven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <andromeda/error.h>
#include <andromeda/system.h>
#include <mm/page_alloc.h>
#include <boot/mboot.h>
#include <mm/paging.h>

/**
 * \AddToGroup Page_alloc
 * @{
 */

/**
 * \todo move page alloc initialisation out to architecture x86
 */

int pagemap[PAGE_LIST_SIZE];
int first_free = PAGE_LIST_MARKED;
extern int boot;


int mboot_parse(multiboot_memory_map_t* map, int map_size)
{
        /* Pointer to the mboot list entry */
        multiboot_memory_map_t* mmap = map;

	int i = 0;
        /* While not outside of the mboot list */
        while((addr_t)mmap < (addr_t)map + map_size)
        {
		i++;
#ifdef PA_DBG
                printf( "Entry: %i\taddr: %X.%X\tsize: %X\ttype: %X\n",
			(int)i,
			(int)(mmap->addr >> 32),
                        (int)mmap->addr,
                        (int)mmap->len,
                        (int)mmap->type
                );
#endif
                if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE || (mmap->addr >> 32) != 0)
                        goto itteration_skip;

#ifdef PA_DBG
                printf("\tFree memory range\n");
#endif
                /* Parse each entry here */
                if (mmap->addr < SIZE_MEG && mmap->addr + mmap->size > SIZE_MEG)
                {
                        addr_t ptr = SIZE_MEG;
                        size_t size = mmap->len - (SIZE_MEG - mmap->addr);
                        for (; ptr < mmap->addr+size; ptr += PAGE_ALLOC_FACTOR)
                                page_unmark((void*)ptr);
                }
                if (mmap->addr >= SIZE_MEG)
                {
                        addr_t ptr = mmap->addr;
                        for (; ptr < mmap->addr + mmap->len && ptr < (addr_t)-1;
                                ptr += PAGE_ALLOC_FACTOR)
                        {
                                page_unmark((void*)ptr);
                        }
                }

        itteration_skip:
                mmap = (void*)((addr_t)mmap + mmap->size+sizeof(mmap->size));
        }
        return -E_SUCCESS;
}

int page_alloc_mark_kernel()
{
        /* Initialise the pointers */
        addr_t start_addr = (int)&boot;
        addr_t end_addr = (int)&end - THREE_GIB;

#ifdef PA_DBG
        /* Spam some debugging info */
        printf( "Kernel info:\tstart:\t%X\n"
                "\t\tend:\t%X\n",
                (int)start_addr,
                (int)end_addr
        );
#endif

        /* Mark each page, one by one by one ... */
        for (;start_addr < end_addr; start_addr += PAGE_ALLOC_FACTOR)
                page_mark((void*)start_addr);

        /* Yay, success. Lets end the function here */
        return -E_SUCCESS;
}

/**
 * \fn page_alloc_init
 * \brief Initialise the kernel memory map for physical page allocation
 */
int page_alloc_init(multiboot_memory_map_t* map, int map_size)
{
        /* Initialise the page map, to everything marked */
        int i = 0;
        while (i < PAGE_LIST_SIZE)
        {
                pagemap[i] = PAGE_LIST_MARKED;
                i++;
        }

        /*
         * Parse the grub memory map here to mark all the unusable pages as
         * allocated.
         */

        if (mboot_parse(map, map_size) != -E_SUCCESS)
                panic("Memory corruption in page_alloc_init");


        if (page_alloc_mark_kernel() != -E_SUCCESS)
                panic("Something went wrong in mapping kernel");

        /**
         * \todo mark memory in use by mods as allocated
         */

        return -E_NOFUNCTION;
}

int page_alloc_register()
{
        if (core.mm == NULL)
                return -E_ALREADY_INITIALISED;

        core.mm->page_alloc = page_alloc;
        core.mm->page_share = page_realloc;
        core.mm->page_free = page_free;

        return -E_SUCCESS;
}

/**
 * @}
 * \file
 */
