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
#include <mm/paging.h>
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

#define STATIC_SEGMENTS 5
#define CODE_IDX  0
#define DATA_IDX  1
#define HEAP_IDX  2
#define PAGE_IDX  3
#define STACK_IDX 4
/**
 * \var vm_core_segments
 * \brief The statically allocated segments for the kernel
 *
 * The kernel is granted memory by the linker at compile time. This memory
 * resides int he 3GB range, and is to be referenced through the segment
 * descriptors.
 *
 * 5 Segments are chosen as they cover the following fields: Code, Data, Stack,
 * Heap and Static pagetables. More segments may be allocated for kernel modules later on, however
 * these can be allocated dynamically.
 *
 * The reason these segments are not allocated dynamically, is because at the
 * moment of referencing, the dynamic allocators have not been initialised yet.
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
#if 0
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

        int i = 0;
        for (; i < STATIC_SEGMENTS; i++)
        {

        }





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
#endif

int
vm_map_kernel_code(struct vm_segment* s)
{
        if (s == NULL)
                return -E_NULL_PTR;
        addr_t code_addr = (addr_t)&higherhalf;
        addr_t code_end = (addr_t)&rodata;

        s->virt_base = (void*)code_addr;
        s->size = code_end - code_addr;
        s->code = TRUE;

#ifdef PA_DBG
        printf( "Mapping kernel code\n\n"
                "BOOT:\t%X\nRODATA:\t%X\nsize\t%X\n",
                (int)code_addr,
                (int)code_end,
                s->size
        );
#endif

        return -E_SUCCESS;
}

int vm_map_kernel_data(s, start, end)
struct vm_segment *s;
void* start;
void* end;
{
        if (s == NULL || start == NULL || end == NULL)
                return -E_NULL_PTR;
        if (!PAGE_ALIGNED((addr_t)start) || !PAGE_ALIGNED((addr_t)end))
                return -E_INVALID_ARG;

        if ((addr_t)end < (addr_t)start)
                return -E_INVALID_ARG;

        s->virt_base = start;
        s->size = (addr_t)end - (addr_t)start;
        s->code = FALSE;

#ifdef PA_DBG
        printf(
                "segment:  %X\n"
                "start:    %X\n"
                "end:      %X\n"
                "size:     %X\n",
               (int)s,
               (int)start,
               (int)end,
               s->size
        );
#endif

        return -E_SUCCESS;
}

int
vm_map_kernel_stack(struct vm_segment* s)
{
        return -E_NOFUNCTION;
}

extern int page_dir_boot;
extern int initial_slab_space;

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
        }
        /* Map the segment into pte_core */
        vm_core.segments = vm_core_segments;
        /* Set the privilage level */
        vm_core.cpl = VM_CPL_CORE;

        int ret = 0;

        /* Map the relevant pages starting with code*/
        ret |= vm_map_kernel_code(&vm_core_segments[0]);

        /* Map the stack */
        /** \todo Write special function for mapping the stack */
        ret |= vm_map_kernel_stack(&vm_core_segments[4]);

        /* Map the page tables */
        ret |= vm_map_kernel_data(&vm_core_segments[1],
                        (void*)((int)(&page_dir_boot) + THREE_GIB),
                        &initial_slab_space);

        /* Map the static data */
        ret |= vm_map_kernel_data(&vm_core_segments[2], &rodata, &end);

        /* Map the heap */
        /** \todo Designate an area for the heap */
        ret |= vm_map_kernel_data(&vm_core_segments[3], &end,
                        ((addr_t)(&end)) + 0x1000000);

        /*
         * Kernel modules and init file systems will have to be mapped once the
         * other allocators have been initialised.
         */

        if (ret != -E_SUCCESS)
//                 panic("Memory could not correctly be mapped!");
                printf("Virtual memory system was not initialised correctly!\n");
#ifdef PA_DBG
        endProg();
#endif

        /**
         * \todo Map in the kernel modules loaded in by GRUB.
         */

        return -E_NOFUNCTION;
}

/**
 * @}
 * \file
 */
