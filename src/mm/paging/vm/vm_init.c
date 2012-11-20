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

#include <mm/vm.h>
#include <mm/page_alloc.h>
#include <andromeda/error.h>
#include <stdlib.h>
#include <stdio.h>
#include <defines.h>

/**
 * \AddToGroup VM
 * @{
 */

/**
 * \todo Use existing page tables to attach to these segments.
 * \todo Create userspace pte initialiser.
 * \todo Create kernelspace pte initialiser.
 * \todo Create vmem context switcher.
 * \todo Rebuild practically the entire memory subsystem.
 */

#define STATIC_SEGMENTS 64
/**
 * \var vm_core_segments
 * \brief The statically allocated segments for the kernel
 *
 * The kernel is granted some segment descriptors (16 pages) to cover its memory
 * needs without the need for a dynamic allocator, which isn't initialised at
 * the time the module is being set up.
 * This number is 64, because 64 times 1 page table equals the full 3 gigabyte
 * range dedicated for kernel use.
 *
 * \var vm_core
 * \brief The tracker for the kernel segments
 *
 * The kernel doesn't only need segment descriptors. it also needs something to
 * reference these descriptors with. This too is done statically because of the
 * reasons mentioned above.
 */
struct vm_segment vm_core_segments[STATIC_SEGMENTS];
struct vm_descriptor vm_core;

/**
 * \fn pte_init
 * \brief Initialise the pte subsystem
 * \return standard error code
 *
 * This system should keep track of physical pages coupled to virtual ones
 * through memory segmentation. Every segment is 16 pages in size, and can not
 * overlap in physical memory.
 * These segments are allocated from the page allocator, in the case of the
 * physical pages. The pte system figures out where to put the data in virtual
 * memory itself.
 *
 * \todo Add kernel pages to segments
 * \todo Create virt page allocation
 * \todo Start work on userspace pte system
 */

/* boot resides at address of first code segment */
extern int boot;
/* page_table_boot is at address of statically allocated pagetables */
extern int page_table_boot;

/**
 * \fn pte_map_kernel
 * \brief The function that maps the kernel into the segments
 */
void
vm_map_kernel()
{
        /* Get the initial pointers going */
        addr_t start_ptr = (addr_t)&boot + THREE_GIB;
        addr_t end_ptr = (addr_t)&end;
        /* The start pointer isn't necessarily 4 Meg aligned */
        start_ptr -= (start_ptr % VM_MEM_SIZE);

        /*
         * Map the segments here
         * Because we assume 4 MB alignment, the 1 MG region at the start of
         * memor is also mapped. This region is marked as unallocatable in the
         * page allocator, and there is no desire to get as much memory out of
         * it as possible. Just don't use it.
         * It is mapped, and if we have to interface it, use this.
         */
        int j = 0;
        for (; start_ptr < end_ptr; start_ptr += VM_MEM_SIZE, j++)
        {
                /* Point the segment to the right pagetable */
                struct vm_segment* s = &vm_core_segments[j];
                s->pte = (void*)((addr_t)&page_table_boot + start_ptr);
                s->mapped = true;

#ifdef PA_DBG
                printf("Mapping: %X\tidx: %X\n", (int)start_ptr, j);
#endif
        }
#ifdef PA_DBG
        panic("Testing!");
#endif
}

/**
 * \fn vm_init
 * \brief The function that initialises the pte core and segments
 */
int
vm_init()
{
        /* Nullify the pte core, so it can be set up */
        memset(&vm_core, 0, sizeof(vm_core));

        /* Initialise all the segments! */
        int i = 0;
        while (i < STATIC_SEGMENTS)
        {
                /* nullify the segment and point to next element */
                struct vm_segment* s = &vm_core_segments[i];
                memset(s, 0, sizeof(*s));
                s->next = &vm_core_segments[i++];

                /* Point the segments to the correct places */
                s->virt_base = (void*)(THREE_GIB + i * VM_MEM_SIZE);
        }
        /* Map the segment into pte_core */
        vm_core.segments = vm_core_segments;
        /* Set the privilage level */
        vm_core.cpl = VM_CPL_CORE;
        /* Map the relevant pages */
        vm_map_kernel();

        /**
         * \todo Map in the kernel modules loaded in by GRUB.
         */

        return -E_NOFUNCTION;
}

/**
 * @}
 * \file
 */
