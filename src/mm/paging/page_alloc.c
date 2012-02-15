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

#include <stdio.h>
#include <stdlib.h>
#include <mm/paging.h>
#include <boot/mboot.h>
#include <mm/heap.h>

static struct mm_page_descriptor* pages = NULL;
boolean freeable_allocator = FALSE;

static struct mm_page_descriptor*
mm_get_page(void* addr, bool phys)
{
        return NULL;
}

static struct mm_page_descriptor*
mm_page_split(page, base_size)
struct mm_page_descriptor* page;
size_t base_size;
{
        return NULL;
}

static struct mm_page_descriptor*
mm_page_merge(page1, page2)
struct mm_page_descriptor* page1;
struct mm_page_descriptor* page2;
{
        return NULL;
}

void*
mm_page_alloc(size_t size)
{
        return NULL;
}

int
mm_page_free(void* page)
{
        return -E_NOFUNCTION;
}

struct page_dir*
mm_page_generate_pd()
{
        return NULL;
}

/**
 * extern int mboot = start of kernel image
 * extern int end = end of kernel image + stdata
 * The memory map shows which regions of memory space are used for hardware
 *      access.
 * The rest is either free or in use by the initrd or modules (which we don't
 *      support yet).
 */

int
mm_page_setup(multiboot_memory_map_t* map, int mboot_map_size)
{
        printf("Pages: %X\n", (uint32_t)pages);

        multiboot_memory_map_t* mmap = map;

        struct mm_page_descriptor* carriage = pages;
        printf("Setting up!\n");
        while ((addr_t)mmap < (addr_t)map + mboot_map_size)
        {
                debug("Size: %X\tbase: %X%X\tlength: %X%X\ttype: %X\n",
                        mmap->size,
                        (uint32_t)(mmap->addr>>32), (uint32_t)mmap->addr,
                        (uint32_t)(mmap->len>>32), (uint32_t)mmap->len,
                        mmap->type
                );

                if (mmap->addr & 0xFFFFFFFF == 0)
                {
                        pages->page_ptr = NULL;
                        pages->virt_ptr = (void*)0xC0000000;

                        pages->size = mmap->len;
                        pages->free = (map->type == 1) ? TRUE : FALSE;
                }
                else
                {
                        if (freeable_allocator)
                        {
                                carriage->next = kalloc(sizeof(*pages));
                                carriage->next->freeable = TRUE;
                        }
                        else
                        {
                                carriage->next = boot_alloc(sizeof(*pages));
                                carriage->next->freeable = FALSE;
                                debug("Using boot alloc\n");
                        }
                        if (carriage -> next == NULL)
                                panic("Out of memory!");
                        carriage = carriage->next;
                        carriage->size = mmap->len;
                        carriage->free = (mmap->type == 1) ? TRUE : FALSE;
                }

                mmap = (void*)((addr_t)mmap + mmap->size+sizeof(mmap->size));
        }

        return -E_NOFUNCTION;
}

int
mm_page_init(size_t mem_size)
{
        debug("Machine mem size: %X, required: %X\n",
              mem_size*0x400,
              MINIMUM_PAGES*BYTES_IN_PAGE
        );

        if (mem_size*0x400 < MINIMUM_PAGES*BYTES_IN_PAGE)
                panic("Machine has not enough memory!");

        if (freeable_allocator)
                pages = kalloc(sizeof(*pages));
        else
                pages = boot_alloc(sizeof(*pages));

        if (pages == NULL)
                panic("Out of memory!");

        pages->next = NULL;
        pages->page_ptr = NULL; /** page ptr = 0 */
        pages->virt_ptr = (void*)0xC0000000; /** Map all memoy to 3 GiB */

        pages->size = 0; /** Will be set later */
        pages->last_referenced = 0; /** Doesn't matter yet */

        pages->swapable = FALSE;        /** Not swappable */
        pages->free = TRUE;             /** Free */
        pages->dma = FALSE;             /** No direct memory access */
        /** Can the page be freed to the allocator or not? */
        pages->freeable = (freeable_allocator) ? TRUE : FALSE;

        pages->lock = mutex_unlocked;
        return -E_SUCCESS;
}

/** \file */
