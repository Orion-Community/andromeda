/*
 * Andromeda
 * Copyright (C) 2011  Bart Kuivenhoven
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
/**
 * \AddToGroup paging
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <mm/paging.h>
#include <boot/mboot.h>

int
mboot_map_special_entry(addr_t ptr,addr_t virt,size_t size,bool free,bool dma)
{
        struct mm_page_descriptor* tmp;
        if (freeable_allocator)
                tmp = kalloc(sizeof(*tmp));
        else
                tmp = boot_alloc(sizeof(*tmp));

        if (tmp == NULL)
                panic("OUT OF MEMORY!");

        memset(tmp, 0, sizeof(*tmp));
        tmp->page_ptr = (void*)ptr;
        tmp->virt_ptr = (void*)virt;
        tmp->size = size;
        tmp->free = free;
        tmp->dma = dma;
        if (free)
                mm_page_append(&free_pages, tmp);
        else
                mm_page_append(&allocated_pages, tmp);

        return -E_SUCCESS;
}

int
mboot_page_setup(multiboot_memory_map_t* map, int mboot_map_size)
{
        multiboot_memory_map_t* mmap = map;

        while ((addr_t)mmap < (addr_t)map + mboot_map_size)
        {
                #ifdef PAGE_ALLOC_DBG
                debug("Size: %X\tbase: %X\tlength: %X\ttype: %X\n",
                      mmap->size,
                      (uint32_t)mmap->addr,
                      (uint32_t)mmap->len,
                      mmap->type
                );
                #endif

                struct mm_page_descriptor* tmp;
                if (mmap->addr < SIZE_MEG)
                {
                        if (mmap->addr+mmap->size > SIZE_MEG)
                        {
                                mboot_map_special_entry(SIZE_MEG, SIZE_MEG,
                                                 mmap->addr+mmap->size-SIZE_MEG,
                                       (mmap->type == 1) ? TRUE : FALSE, FALSE);
                        }
                        goto itteration_skip;
                }
                if (freeable_allocator)
                        tmp = kalloc(sizeof(*tmp));
                else
                        tmp = boot_alloc(sizeof(*tmp));
                if (tmp == NULL)
                        panic("Out of memory!");
                memset(tmp, 0, sizeof(*tmp));
                tmp->freeable = freeable_allocator;

                tmp->page_ptr = (void*)((addr_t)mmap->addr);
                tmp->virt_ptr = tmp->page_ptr;
                tmp->size = (size_t)mmap->len;

                if (mmap->type != 1)
                {
                        if (mm_page_append(&allocated_pages, tmp) == NULL)
                                panic("Couldn't add page!");
                        tmp->free = FALSE;
                }
                else
                {
                        if (mm_page_append(&free_pages, tmp) == NULL)
                                panic("Couldn't add page!");
                        tmp->free = TRUE;
                }
                itteration_skip:
                mmap = (void*)((addr_t)mmap + mmap->size+sizeof(mmap->size));
        }
#ifdef PAGE_ALLOC_DBG
        debug("\nFirst run\n");
        mm_show_pages();
#endif

        mm_page_map_higher_half();
#ifdef PAGE_ALLOC_DBG
        debug("\nSecond run\n");
        mm_show_pages();
#endif

        mm_map_kernel();
#ifdef PAGE_ALLOC_DBG
        debug("\nThird run (maps the kernel)\n");
        mm_show_pages();

        /** mm_page_alloc doesn't move the page to the allocated list */
        void* addr = mm_page_alloc(0x1000);
        debug("\nFourth run\n");
        mm_show_pages();

        mm_page_free(addr);
        debug("\nFifth run\n");
        mm_show_pages();
#endif

        return -E_SUCCESS;
}

/**
 * @}
 * \file
 */
