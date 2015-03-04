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
#include <andromeda/system.h>
#include <stdlib.h>
#include <stdio.h>
#include <defines.h>

/**
 * \addtogroup VM
 * @{
 */

/**
 * \todo Create a segment(s) for kernel modules
 * \todo Use existing page tables to attach to these segments.
 * \todo Create userspace pte initialiser.
 * \todo Create kernelspace pte initialiser.
 * \todo Create vmem context switcher.
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
 * Heap and Static pagetables. More segments may be allocated for kernel modules
 * later on, however these can be allocated dynamically.
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
struct tree_root* vm_loaded[CPU_LIMIT];

static int vm_map_kernel_code(struct vm_segment* s)
{
        if (s == NULL)
                return -E_NULL_PTR;

        addr_t code_addr = (addr_t) &higherhalf;
        addr_t code_end = (addr_t) &rodata;

        s->virt_base = (void*) code_addr;
        s->size = code_end - code_addr;
        s->code = TRUE;
        memcpy(s->name, ".code", sizeof(".code"));

        vm_segment_mark_loaded_global(s);

#ifdef PA_DBG
        debug( "Mapping kernel code\n\n"
                        "BOOT:\t%X\nRODATA:\t%X\nsize\t%X\n",
                        (int)code_addr,
                        (int)code_end,
                        s->size
        );
#endif

        return -E_SUCCESS;
}

static int vm_map_kernel_data(struct vm_segment* s, void* start, void* end,
                char* name)
{
        if (s == NULL || start == NULL || end == NULL)
                return -E_NULL_PTR;
        if (!PAGE_ALIGNED((addr_t )start) || !PAGE_ALIGNED((addr_t )end))
                return -E_INVALID_ARG;

        if ((addr_t) end < (addr_t) start)
                return -E_INVALID_ARG;

        s->virt_base = start;
        s->size = (addr_t) end - (addr_t) start;
        s->code = FALSE;

        size_t name_len = strlen(name);
        if (name_len >= SEGMENT_NAME_LENGTH)
                name_len = SEGMENT_NAME_LENGTH - 2;
        memcpy(s->name, name, strlen(name) + 1);
        s->name[SEGMENT_NAME_LENGTH - 1] = (char) 0;

        vm_segment_mark_loaded_global(s);
#ifdef PA_DBG
        debug(
                        "name:     %s\n"
                        "segment:  %X\n"
                        "start:    %X\n"
                        "end:      %X\n"
                        "size:     %X\n",
                        s->name,
                        (int)s,
                        (int)start,
                        (int)end,
                        s->size
        );
#endif

        return -E_SUCCESS;
}

/**
 * \fn vm_map_kernel_stack
 * \brief Create a new region for the stack and move it over there
 * The stack has been created at boot time, in a very strange, but easy to
 * implement, location. Because the stack is at such an odd location it has to
 * be moved. This move is to be made by this function.
 * \param s
 * \return A standard error code
 */
static int vm_map_kernel_stack(struct vm_segment* s)
{
        if (s == NULL)
                return -E_NULL_PTR;

        memcpy(s->name, ".stack", sizeof(".stack"));
        s->virt_base = NULL;
        s->size = 0;

        s->free = NULL;
        s->allocated = NULL;

        vm_segment_mark_loaded_global(s);

        return -E_NOFUNCTION;
}

static int vm_kernel_add_range(struct vm_segment* s)
{
        if (s == NULL)
                return -E_NULL_PTR;

        s->free = vm_range_alloc();
        if (s->free == NULL)
                return -E_NOMEM;

        memset(s->free, 0, sizeof(*(s->free)));
        s->free->base = s->virt_base;
        s->free->size = s->size;
        s->free->parent = s;

        return -E_SUCCESS;
}

extern int page_dir_boot;
extern int initial_slab_space;

/**
 * \fn vm_init
 * \brief The function that initialises the pte core and segments
 */
int vm_init()
{
        /* Nullify the pte core, so it can be set up */
        memset(&vm_core, 0, sizeof(vm_core));
        idx_t i = 0;
        for (; i < CPU_LIMIT; i++) {
                vm_loaded[i] = tree_new_avl_early();
                if (vm_loaded[i] == NULL) {
                        panic("Out of memory!");
                }
        }

        /* Initialise all the segments! */
        i = 0;
        struct vm_segment* s = NULL;
        while (i < STATIC_SEGMENTS) {
                /* nullify the segment and point to next element */
                s = &vm_core_segments[i];
                memset(s, 0, sizeof(*s));
                s->next = &vm_core_segments[++i];
                s->parent = &vm_core;
        }
        s->next = NULL;
        /* Map the segment into pte_core */
        vm_core.segments = vm_core_segments;
        /* Set the privilege level */
        vm_core.cpl = VM_CPL_CORE;

        int ret = 0;

        /* Set up the range allocator, we'll need this one for the heap */
        vm_range_alloc_init();

        /* For the physical page allocator, let's keep everything aligned */
        addr_t data_end = (addr_t) &end;
        if (data_end % PAGE_ALLOC_FACTOR != 0) {
                data_end += PAGE_ALLOC_FACTOR - data_end % PAGE_ALLOC_FACTOR;
        }

        /* Map the relevant pages starting with code*/
        int code = vm_map_kernel_code(&vm_core_segments[0]);
        if (code != -E_SUCCESS) {
                warning("Code mapping failed!\n");
        }

        /* Map the stack */
        /** \todo Write special function for mapping the stack */
        int stack = vm_map_kernel_stack(&vm_core_segments[4]);
        if (stack != -E_SUCCESS) {
                warning("Stack mapping failed!\n");
        }

        /* Map the page tables */
        int pd = vm_map_kernel_data(&vm_core_segments[1],
                        (void*) ((int) (&page_dir_boot) + THREE_GIB),
                        &initial_slab_space, ".PD");
        if (pd != -E_SUCCESS) {
                warning("page dir mapping failed!\n");
        }

        /* Map the static data */
        int data = vm_map_kernel_data(&vm_core_segments[2], &rodata,
                        (void*) data_end, ".data");
        if (data != -E_SUCCESS) {
                warning("static data mapping failed!\n");
        }

        /* Map the heap */
        /** \todo Designate an area for the heap */
        int heap = vm_map_kernel_data(&vm_core_segments[3], (void*) data_end,
                        (void*) (data_end + 0x1000000), ".heap");
        if (heap != -E_SUCCESS) {
                warning("heap data mapping failed!\n");
        }

        heap |= vm_kernel_add_range(&vm_core_segments[3]);
        if (heap != -E_SUCCESS) {
                warning("heap data range mapping failed!\n");
        }

        ret = code | stack | pd | data | heap;

        /*
         * Kernel modules and init file systems will have to be mapped once the
         * other allocators have been initialised.
         */

        if (ret != -E_SUCCESS) {
//                 panic("Memory could not correctly be mapped!");
                warning("Virtual memory system not initialised correctly!\n");
        }

        /*
         * Unmap all the memory below the 1GiB mark.
         * It's no longer useful to us.
         */
        int idx = 0;
        for (; idx < 0x40000000; idx += PAGESIZE)
        {
                page_unmap(0, (void*) idx);
        }

        /**
         * \todo Map in the kernel modules loaded in by GRUB.
         */

        mm_vm_range_buffer_start = 1;

#ifdef VM_TEST
        if (vm_test() != -E_SUCCESS) {
                panic("Failure in vm_test code!");
        }
#endif
        return ret;
}

/**
 * @}
 * \file
 */
