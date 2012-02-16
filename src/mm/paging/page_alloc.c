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
#include <types.h>

static struct mm_page_descriptor* pages = NULL;
boolean freeable_allocator = FALSE;
static mutex_t page_lock = mutex_unlocked;

/**
 * \fn mm_next_free_page
 * \return The first free page in the list of pages
 */
struct mm_page_descriptor*
mm_next_free_page(size_t size)
{
        return NULL;
}

/**
 * \fn mm_get_page
 * \return The page descriptor describing the requested address (if possible)
 */

static struct mm_page_descriptor*
mm_get_page(void* addr, bool phys)
{
        return NULL;
}

/**
 * \fn mm_page_split
 * \brief Split the page into two leaving the lowe page in the requested size
 * \param page
 * \brief The physical page to split
 * \param base_size
 * \brief The size of the lower page after the split
 * \return The pointer to the lower page
 */
static struct mm_page_descriptor*
mm_page_split(page, base_size)
struct mm_page_descriptor* page;
size_t base_size;
{
        return NULL;
}

/**
 * \fn mm_page_merge
 * \brief Put two pages together (if successive)
 * \param page1
 * \brief One of the pages to merge
 * \param page2
 * \brief The other page to merge
 * \return The resulting page descriptor
 */
static struct mm_page_descriptor*
mm_page_merge(page1, page2)
struct mm_page_descriptor* page1;
struct mm_page_descriptor* page2;
{
        return NULL;
}


/**
 * \fn mm_page_alloc
 * \brief Allocate a (number of) page(s)
 * \param size
 * \brief The number of pages we need to allocate
 * \return A pointer to the allocated page or NULL
 */
void*
mm_page_alloc(size_t size)
{
        return NULL;
}

/**
 * \fn mm_page_free
 * \brief Mark a page as unused
 * \param page
 * \brief The page to free
 * \return error code
 */
int
mm_page_free(void* page)
{
        return -E_NOFUNCTION;
}

/**
 * \fn x86_page_generate_pd
 * \brief Generate a page directory for virtual memory
 * \return The newly generated page directory (the physical address)
 */
struct page_dir*
x86_page_generate_pd(uint32_t proc_id)
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
x86_page_destroy_pd(struct page_dir* pd)
{
        return -E_NOFUNCTION;
}

int
x86_page_update_pd(uint32_t proc_id)
{
        return -E_NOFUNCTION;
}

void
mm_show_pages()
{
        struct mm_page_descriptor* carriage = pages;
        while (carriage != NULL)
        {
                debug(
                      "phys: %X\tvirt: %X\tsize: %X\tfree: %X\n",
                      (uint32_t)carriage->page_ptr,
                      (uint32_t)carriage->virt_ptr,
                      (uint32_t)carriage->size,
                      carriage->free
                );
                carriage = carriage->next;
        }
}

#define THREE_GIB 0xC0000000

int
x86_page_setup(multiboot_memory_map_t* map, int mboot_map_size)
{
        printf("Pages: %X\n", (uint32_t)pages);

        multiboot_memory_map_t* mmap = map;

        struct mm_page_descriptor* carriage = pages;
        printf("Setting up!\n");
        while ((addr_t)mmap < (addr_t)map + mboot_map_size)
        {
                debug("Size: %X\tbase: %X\tlength: %X\ttype: %X\n",
                        mmap->size,
                        (uint32_t)mmap->addr,
                        (uint32_t)mmap->len,
                        mmap->type
                );

                if ((uint32_t)mmap->addr == 0)
                {
                        pages->page_ptr = NULL;
                        pages->virt_ptr = (void*)THREE_GIB;

                        pages->size = mmap->len;
                        pages->free = (mmap->type == 1) ? TRUE : FALSE;
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
                        }
                        if (carriage -> next == NULL)
                                panic("Out of memory!");
                        carriage = carriage->next;
                        carriage->size = mmap->len;
                        carriage->free = (mmap->type == 1) ? TRUE : FALSE;
                        carriage->page_ptr = (void*)((uint32_t)mmap->addr);
                        carriage->virt_ptr = (void*)((uint32_t)(mmap->addr+THREE_GIB));
                }

                mmap = (void*)((addr_t)mmap + mmap->size+sizeof(mmap->size));
        }

        mm_show_pages();

        return -E_SUCCESS;
}

int
x86_page_init(size_t mem_size)
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
        pages->virt_ptr = (void*)THREE_GIB; /** Map all memoy to 3 GiB */

        pages->size = 0; /** Size will be set later */
        pages->last_referenced = 0; /** last_referenced doesn't matter yet */

        pages->swapable = FALSE;        /** pages isn't swappable */
        pages->free = TRUE;             /** The page is free */
        pages->dma = FALSE;             /** Not direct memory access */
        /** Can the page be freed to the allocator or not? */
        pages->freeable = (freeable_allocator) ? TRUE : FALSE;

        pages->lock = mutex_unlocked;
        return -E_SUCCESS;
}

/** \file */
