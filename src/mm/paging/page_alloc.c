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
/**
 * \AddToGroup paging
 * @{
 * \todo Figure out a more optimized way to keep track of physical pages
 */

#include <stdio.h>
#include <stdlib.h>
#include <mm/paging.h>
#include <boot/mboot.h>
#include <mm/heap.h>
#include <types.h>
#include <mm/memory.h>

struct mm_page_list free_pages;
struct mm_page_list allocated_pages;

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
                return node;
        }
        list->tail->next = node;
        node->prev = list->tail;
        node->next = NULL;
        list->tail = node;
        return node;
}

/**
 * \fn mm_page_rm
 * \brief Remove an item from a page list
 * \param node
 * \brief The node to be removed from the list
 * \return The released node
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
 * \warning The resulting page descriptor can no longer be used when itterating
 */
struct mm_page_descriptor*
mm_page_split(list, page, base_size)
struct mm_page_list* list;
struct mm_page_descriptor* page;
size_t base_size;
{
        if (page == NULL || base_size < PAGESIZE ||base_size % PAGESIZE != 0)
                return NULL;

        if (page->size == base_size)
                return NULL;

        mutex_lock(&page->lock);

        struct mm_page_descriptor* tmp = 0;
        if (freeable_allocator)
                tmp = kalloc(sizeof(*tmp));

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
        if (tmp->next == NULL)
                list->tail = tmp;
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
 * \warning The resulting page descriptors can no longer be used when itterating
 */
struct mm_page_descriptor*
mm_page_merge(lst, page1, page2)
struct mm_page_list* lst;
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

        if (lst->head == page2)
                lst->head = page1;
        if (lst->tail == page2)
                lst->tail = page1;

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
                                carriage = mm_page_split(&free_pages, carriage,
                                                                          size);
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

        struct mm_page_descriptor* carriage = free_pages.head;
        for (; carriage->next != NULL && carriage != NULL;
                                                      carriage = carriage->next)
        {
                addr_t phys = (addr_t)to_free->page_ptr;
                addr_t it_phys = (addr_t)carriage->page_ptr;
                if (carriage->free == FALSE)
                        panic("Page administration failure!");
                if (it_phys + carriage->size == phys)
                {
                        // Merge the page descriptors
                        to_free = mm_page_merge(&free_pages, to_free, carriage);
                        carriage = free_pages.head;
                }
                else if (phys + to_free->size == it_phys)
                {
                        // Also merge the page descriptors
                        to_free = mm_page_merge(&free_pages, to_free, carriage);
                        carriage = free_pages.head;
                }
        }

        mutex_unlock(&page_lock);

        return -E_SUCCESS;
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

/**
 * @}
 * \file
 */
