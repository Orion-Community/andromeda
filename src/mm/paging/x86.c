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
/**
 * \fn x86_page_dir_set
 * \brief Set an entry in the pagedirectory
 * \param pd
 * \brief The page directory pointer (the virtual one please)
 * \param pd_entry
 * \brief The entry to set
 * \param pt_ptr
 * \brief The physical pointer to the page directory
 * \param cpl
 * \brief code privilege level, 3 for usermode, 0 for kernelspace
 */
int x86_page_dir_set(pd, pd_entry, pt_ptr, cpl)
struct page_dir* pd;
uint16_t pd_entry;
void* pt_ptr;
uint8_t cpl;
{
        if (pd == NULL)
                return -E_INVALID_ARG;

        pd[pd_entry].pageIdx  = (uint32_t)pt_ptr >> 22;
        pd[pd_entry].present  = 1;
        pd[pd_entry].rw       = 1;
        pd[pd_entry].userMode = (cpl == USER_CPL) ? 1 : 0;
        pd[pd_entry].pwt      = 0;
        pd[pd_entry].pcd      = 0;
        pd[pd_entry].accessed = 0;
        pd[pd_entry].dirty    = 0;
        pd[pd_entry].pageSize = 0;
        pd[pd_entry].global   = 0;
        pd[pd_entry].ignored  = 0;
        return -E_SUCCESS;
}

/**
 * \fn x86_page_table_set
 * \brief Set an entry in the pagetable
 * \param pd
 * \brief The page table pointer (the virtual one please)
 * \param pd_entry
 * \brief The entry to set
 * \param pt_ptr
 * \brief The physical pointer to the page
 * \param cpl
 * \brief code privilege level, 3 for usermode, 0 for kernelspace
 */
int x86_page_table_set(pt, pt_entry, ptr, cpl)
struct page_table* pt;
uint16_t pt_entry;
void* ptr;
uint8_t cpl;
{
        if (pt == NULL)
                return -E_INVALID_ARG;
        pt[pt_entry].pageIdx  = (uint32_t)ptr >> 12;
        pt[pt_entry].present  = 1;
        pt[pt_entry].rw       = 1;
        pt[pt_entry].userMode = (cpl == USER_CPL) ? 1 : 0;
        pt[pt_entry].pwt      = 0;
        pt[pt_entry].pcd      = 0;
        pt[pt_entry].dirty    = 0;
        pt[pt_entry].accessed = 0;
        pt[pt_entry].pat      = 0;
        pt[pt_entry].global   = 0;
        pt[pt_entry].ignored  = 0;
        return -E_SUCCESS;
}

/**
 * \fn x86_get_pagedir
 * \return the physical address of the page directory
 */
addr_t x86_get_pagedir()
{
        return getCR3();
}

/**
 * \fn x86_page_generate_pd
 * \brief Generate a page directory for virtual memory
 * \return The newly generated page directory (the physical address)
 */
struct page_dir*
x86_page_generate_pd(struct task* task)
{
        return NULL;
}

/**
 * extern int mboot = start of kernel image
 * extern int end = end of kernel image + stdata
 * The memory map shows which regions of memory space are used for hardware
 * access.
 * The rest is either free or in use by the initrd or modules (which we don't
 * support yet).
 */

int
x86_page_destroy_pd(struct page_dir* pd)
{
        return -E_NOFUNCTION;
}

int
x86_page_set_list(struct task* list)
{
        return -E_NOFUNCTION;
}

/**
 * \fn x86_page_init
 * \brief Call this function ti initiate page administration on X86
 * \param mem_size
 * \brief The size of memory in KiB
 * \return Error code
 */
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

/**
 * \fn x86_page_map_higher_half
 * \brief Map the higher half part of the kernel
 * \return A satndard error code
 */
int
x86_page_map_higher_half()
{
        struct mm_page_descriptor* carriage = free_pages.head;

        addr_t phys;
        for (; carriage != NULL; carriage = carriage->next)
        {
                phys = (addr_t)carriage->page_ptr;
                if (phys < GIB)
                {
                        if (phys + carriage->size > GIB)
                                mm_page_split(&free_pages, carriage, GIB-phys);
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
                                mm_page_split(&allocated_pages, carriage,
                                              GIB - phys);
                        carriage->virt_ptr = (void*)(phys+THREE_GIB);
                }
        }
        return 0;
}

/**
 * \fn x86_map_kernel_element
 * \brief Map a specific kernel entry in the page lists
 * \param list
 * \brief The list to mark in
 * \param carriage
 * \brief The pointer to the block which has to be marked
 * \return A standardised error code
 */
int
x86_map_kernel_element(list, carriage)
struct mm_page_list* list;
struct mm_page_descriptor* carriage;
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

                if (mm_page_split(list, carriage, block_size) == NULL)
                        return -E_GENERIC;
        }
        // Mark the current page descriptor as allocated
        #ifdef PAGE_ALLOC_DBG
        debug("\nBefore\n");
        mm_show_pages();
        #endif
        mm_page_rm(&free_pages, carriage);
        #ifdef PAGE_ALLOC_DBG
        debug("\nAfter\n");
        mm_show_pages();
        #endif
        mm_page_append(&allocated_pages, carriage);
        carriage->free = FALSE;
        return -E_SUCCESS;
}

/**
 * \fn x86_map_kernel
 * \brief Map the kernel to physical memory
 * \return A standardised error code
 */
int
x86_map_kernel()
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
                        if (x86_map_kernel_element(&free_pages, carriage)
                                != -E_SUCCESS)
                                panic("Couldn't map kernel image!");
                }
        }
        return -E_SUCCESS;
}

/**
 * @}
 * \file
 */
