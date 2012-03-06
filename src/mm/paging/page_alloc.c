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
#include <mm/memory.h>

static struct mm_page_list free_pages;
static struct mm_page_list allocated_pages;

boolean freeable_allocator = FALSE;
static mutex_t page_lock = mutex_unlocked;

/**
 * \fn mm_page_append
 * \brief Append a page descriptor to the end of the list
 * \param list
 * \brief The list to append to
 * \param node
 * \brief The node to append
 * \return NULL for error, node for success
 * \TODO Rewrite mm_page_append function
 */
struct mm_page_descriptor*
mm_page_append(struct mm_page_list* list, struct mm_page_descriptor* node)
{
        if (list == NULL || node == NULL)
                return NULL;
        if (list->tail == NULL && list->head != NULL)
                return NULL;
        if (list->tail == NULL)
        {
                list->head = node;
                list->tail = node;
                node->next = NULL;
                node->prev = NULL;
                debug("Alternate append\n");
                return node;
        }
        debug("list tail: %X\n", (int)list->tail);
        list->tail->next = node;
        node->prev = list->tail;
        node->next = NULL;
        list->tail = node;
        debug("Prefered append\n");
        return node;
}

/**
 * \fn mm_page_rm
 * \brief Remove an item from a page list
 * \param node
 * \brief The node to be removed from the list
 * \return The released node
 * \TODO Rewrite the mm_page_rm function
 */
struct mm_page_descriptor*
mm_page_rm(struct mm_page_list* list, struct mm_page_descriptor* node)
{
        if (node == NULL || list == NULL)
                return NULL;

        if (node->prev == NULL || list->head == node)
        {
                if (list->head != node)
                        return NULL;
                list->head = node->next;
        }
        if (node->next == NULL || list->tail == node)
        {
                if (list->tail != node)
                        return NULL;
                list->tail = node->prev;
        }
        node->prev->next = node->next;
        node->next->prev = node->prev;
        node->next = NULL;
        node->prev = NULL;

        debug("rm succeeded\n");
        debug("list head: %X\tlist tail: %X\n", (int)list->head, (int)list->tail);
        return node;
}

/**
 * \fn mm_get_page
 * \return The page descriptor describing the requested address (if possible)
 */

static struct mm_page_descriptor*
mm_get_page(addr_t addr, bool physical, bool free)
{
        struct mm_page_descriptor* carriage;
        if (free)
                carriage = free_pages.head;
        else
                carriage = allocated_pages.head;

        if (carriage == NULL || addr % PAGESIZE != 0)
                return NULL;

        for (; carriage != NULL; carriage = carriage->next)
        {
                if (physical)
                {
                        addr_t phys = (addr_t)carriage->page_ptr;
                        if (phys <= addr && phys + carriage->size > addr)
                        {
                                return carriage;
                        }
                }
                else
                {
                        addr_t virt = (addr_t)carriage->virt_ptr;
                        if (virt <= addr && virt+carriage->size > addr)
                        {
                                return carriage;
                        }
                }
        }

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
 * \warning Use the returned pointer to reset the carriage if itterating
 */
static struct mm_page_descriptor*
mm_page_split(page, base_size)
struct mm_page_descriptor* page;
size_t base_size;
{
        if (page == NULL || base_size < PAGESIZE ||base_size % PAGESIZE != 0)
                return NULL;

        if (page->size == base_size)
                return NULL;

        mutex_lock(&page->lock);

        struct mm_page_descriptor* tmp;
        if (freeable_allocator)
                tmp = kalloc(sizeof(*tmp));
        else
                tmp = boot_alloc(sizeof(*tmp));

        if (tmp == NULL)
        {
                mutex_unlock(&page->lock);
                debug("OUT OF MEMORY!\n");
                return NULL;
        }

        memcpy(tmp, page, sizeof(*tmp));

        tmp->size = page->size-base_size;
        tmp->virt_ptr = (void*)((addr_t)page->virt_ptr + base_size);
        tmp->page_ptr = (void*)((addr_t)page->page_ptr + base_size);
        tmp->prev = page;
        page->next = tmp;
        page->size = base_size;
        tmp->freeable = freeable_allocator;
        mutex_unlock(&page->lock);
        mutex_unlock(&tmp->lock);
        return page;
}

/**
 * \fn mm_page_merge
 * \brief Put two pages together (if successive)
 * \param page1
 * \brief One of the pages to merge
 * \param page2
 * \brief The other page to merge
 * \return The resulting page descriptor
 * \warning Use returned value to continue if in a loop
 */
static struct mm_page_descriptor*
mm_page_merge(page1, page2)
struct mm_page_descriptor* page1;
struct mm_page_descriptor* page2;
{
        if (page1 == NULL || page2 == NULL)
                return NULL;
        addr_t phys1 = (addr_t)page1->page_ptr;
        addr_t phys2 = (addr_t)page2->page_ptr;

        if (!(phys1+page1->size != phys2 || phys2+page2->size != phys1))
                return NULL;

        if (page1->free != page2->free || page1->dma != page2->dma)
                return NULL;

        if (phys2+page2->size == phys1)
        {
                struct mm_page_descriptor* tmp_desc = page2;
                page2 = page1;
                page1 = tmp_desc;

                addr_t tmp_addr = phys2;
                phys2 = phys1;
                phys1 = tmp_addr;
        }

        page1->size += page2->size;
        page2->prev->next = page2->next;
        if (page2->freeable)
                kfree(page2);

        return page1;
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
        if (size % PAGESIZE)
                return NULL;
        struct mm_page_descriptor* carriage = free_pages.head;
        if (carriage == NULL)
                return NULL;
        mutex_lock(&page_lock);

        for (; carriage != NULL; carriage = carriage->next)
        {
                if (carriage->free && carriage->size >= size)
                {
                        if (carriage->size > size)
                                carriage = mm_page_split(carriage, size);
                        if (carriage == NULL)
                                goto err;

                        carriage->free = FALSE;
                        mm_page_rm(&free_pages, carriage);
                        mm_page_append(&allocated_pages, carriage);
                        mutex_unlock(&page_lock);
                        return carriage->page_ptr;
                }
        }

err:
        mutex_unlock(&page_lock);
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
        mutex_lock(&page_lock);
        struct mm_page_descriptor* to_free = mm_get_page((addr_t)page, TRUE, FALSE);
        debug("Page to free: %X\n", (int)page);
        debug("Page descriptor to free: %X\n", (int)to_free);

        if (to_free == NULL)
        {
                mutex_unlock(&page_lock);
                return -E_GENERIC;
        }

        if (to_free->page_ptr != page)
        {
                mutex_unlock(&page_lock);
                return -E_INVALID_ARG;
        }

        to_free->free = TRUE;
        if(mm_page_rm(&allocated_pages, to_free) == NULL)
                debug("page_rm failed\n");
        if(mm_page_append(&free_pages, to_free) == NULL)
                debug("append failed\n");
/*
        struct mm_page_descriptor* carriage = free_pages.head;
        for (; carriage->next != NULL && carriage != NULL;
                                                      carriage = carriage->next)
        {
                addr_t next = (addr_t)carriage->next;
                addr_t phys = (addr_t)carriage->page_ptr;
                if (phys+carriage->size == next && carriage->free == TRUE &&
                                                   carriage->next->free == TRUE)
                {
                        carriage = mm_page_merge(carriage, carriage->next);
                        if (carriage == NULL)
                        {
                                warning("Page stack corruption!\n");
                                mutex_unlock(&page_lock);
                                return -E_NULL_PTR;
                        }
                }
        }
*/

        mutex_unlock(&page_lock);

        return -E_SUCCESS;
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
        struct mm_page_descriptor* carriage = free_pages.head;
        debug("Free pages\n");
        while (carriage != NULL)
        {
                debug(
                      "phys: %X\tvirt: %X\tsize: %X\tfree: %X\tcarriage: %X\n",
                      (uint32_t)carriage->page_ptr,
                      (uint32_t)carriage->virt_ptr,
                      (uint32_t)carriage->size,
                      carriage->free,
                      (uint32_t)carriage
                );
                carriage = carriage->next;
        }
        debug("Allocated pages\n");
        carriage = allocated_pages.head;
        while (carriage != NULL)
        {
                debug(
                      "phys: %X\tvirt: %X\tsize: %X\tfree: %X\tcarriage: %X\n",
                      (uint32_t)carriage->page_ptr,
                      (uint32_t)carriage->virt_ptr,
                      (uint32_t)carriage->size,
                      carriage->free,
                      (uint32_t)carriage
                );
                carriage = carriage->next;
        }
}

int
mm_map_kernel_element(struct mm_page_descriptor* carriage)
{
        addr_t phys = (addr_t)carriage->page_ptr;
        addr_t end_ptr = (addr_t)&end - THREE_GIB;
        if (phys + carriage->size > end_ptr)
        {
                // Some splitting needs to be done!
                if (carriage->free == FALSE)
                        return -E_SUCCESS;

                size_t block_size = end_ptr-phys;
                if (block_size % PAGESIZE)
                        block_size += PAGESIZE-(block_size%PAGESIZE);

                if (mm_page_split(carriage, block_size) == NULL)
                        return -E_GENERIC;
        }
        // Mark the current page descriptor as allocated
        debug("\nBefore\n");
        mm_show_pages();
        mm_page_rm(&free_pages, carriage);
        debug("\nAfter\n");
        mm_show_pages();
        mm_page_append(&allocated_pages, carriage);
        carriage->free = FALSE;
        return -E_SUCCESS;
}

int
mm_map_kernel()
{
        addr_t end_ptr = (addr_t)&end - THREE_GIB;
        struct mm_page_descriptor* carriage = free_pages.head;
        struct mm_page_descriptor* tmp;
        for (; carriage != NULL; carriage = tmp)
        {
                tmp = carriage->next;
                addr_t phys = (addr_t)carriage->page_ptr;
                if (phys < end_ptr)
                {
                        if (mm_map_kernel_element(carriage) != -E_SUCCESS)
                                panic("Couldn't map kernel image!");
                }
        }
        return -E_SUCCESS;
}

int
mm_page_map_higher_half()
{
        struct mm_page_descriptor* carriage = free_pages.head;

        addr_t phys;
        for (; carriage != NULL; carriage = carriage->next)
        {
                phys = (addr_t)carriage->page_ptr;
                if (phys < GIB)
                {
                        if (phys + carriage->size > GIB)
                                mm_page_split(carriage, GIB - phys);
                        carriage->virt_ptr = (void*)(phys+THREE_GIB);
                }
        }

        carriage = allocated_pages.head;
        for (; carriage != NULL; carriage = carriage->next)
        {
                phys = (addr_t)carriage->page_ptr;
                if (phys < GIB)
                {
                        if (phys + carriage->size > GIB)
                                mm_page_split(carriage, GIB - phys);
                        carriage->virt_ptr = (void*)(phys+THREE_GIB);
                }
        }
        return 0;
}

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
        printf("Pages: %X\n", (uint32_t)free_pages.head);

        multiboot_memory_map_t* mmap = map;

        printf("Setting up!\n");

        while ((addr_t)mmap < (addr_t)map + mboot_map_size)
        {
                debug("Size: %X\tbase: %X\tlength: %X\ttype: %X\n",
                        mmap->size,
                        (uint32_t)mmap->addr,
                        (uint32_t)mmap->len,
                        mmap->type
                );

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
        debug("\nFirst run\n");
        mm_show_pages();

        mm_page_map_higher_half();
        debug("\nSecond run\n");
        mm_show_pages();

        mm_map_kernel();
        debug("\nThird run (maps the kernel)\n");
        mm_show_pages();

        for (;;);

        /** mm_page_alloc doesn't move the page to the allocated list */
        void* addr = mm_page_alloc(0x1000);
        debug("\nFourth run\n");
        mm_show_pages();

        mm_page_free(addr);
        debug("\nFifth run\n");
        mm_show_pages();

        for (;;);

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

        memset(&free_pages, 0, sizeof(free_pages));
        memset(&allocated_pages, 0, sizeof(allocated_pages));

        struct mm_page_descriptor* meg;
        if (freeable_allocator)
                meg = kalloc(sizeof(*meg));
        else
                meg = boot_alloc(sizeof(*meg));

        if (meg == NULL)
                panic("OUT OF MEMORY!");

        memset(meg, 0, sizeof(*meg));
        meg->freeable = freeable_allocator;
        meg->page_ptr = NULL;
        meg->size = SIZE_MEG; /** meg->size = one megabyte */

        allocated_pages.head = meg;
        allocated_pages.tail = meg;


        return -E_SUCCESS;
}

/** \file */
