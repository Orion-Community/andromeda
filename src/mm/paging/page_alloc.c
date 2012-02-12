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


static struct mm_page_descriptor* pages = NULL;

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

int
mm_page_setup(multiboot_memory_map_t* map, int mboot_map_size)
{
        if (pages != NULL)
                return -E_ALREADY_INITIALISED;

        multiboot_memory_map_t* mmap = map;

        idx_t idx = 0;
        while ((addr_t)mmap < (addr_t)map + mboot_map_size)
        {


                mmap = (void*)((addr_t)mmap + mmap->size+sizeof(mmap->size));
        }

        return -E_NOFUNCTION;
}

int
mm_page_init(size_t mem_size)
{
        debug("Machine mem size: %X, required: %X\n",
                                                                 mem_size*0x400,
                                                   MINIMUM_PAGES*BYTES_IN_PAGE);
        if (mem_size*0x400 < MINIMUM_PAGES*BYTES_IN_PAGE)
                panic("Machine has not enough memory!");

        pages->next = NULL;
        pages->page_ptr = NULL;
        pages->virt_ptr = (void*)0xC0000000;

        pages->swapable = FALSE;
        pages->free = TRUE;
        pages->dma = FALSE;
        pages->allocator = TRUE;

        pages->lock = mutex_unlocked;
}

/** \file */
