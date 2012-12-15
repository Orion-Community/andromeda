/*
 *  Andromeda
 *  Copyright (C) 2012  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \defgroup VM
 * @{
 */

#ifndef __MM_PTE_H
#define __MM_PTE_H

#include <mm/paging.h>
#include <defines.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VM_CPL_USER 3
#define VM_CPL_CORE 0
#define PTE_SIZE 0x400
#define VM_MEM_SIZE (PTE_SIZE*PAGESIZE)

#ifdef X86
#define PAGE_ALIGNED(a) ((a & 0xFFF) == 0)
#else
#define PAGE_ALIGNED(a) ((a % PAGESIZE) == 0)
#endif

/**
 * \struct VM
 * \brief The virtual memory subsystem
 * \todo Move this structure out into arch/x86
 */
struct pte {
        /**
         * \var page_table
         * \brief The page table entries
         */
        struct page_table entry[PTE_SIZE];
};

struct vm_segment;

/**
 * \struct pte_subs
 * \brief A list of sub segments, so the pte_segment doesn't grow to big
 */
struct vm_subs {
        struct vm_segment* segment[PTE_SIZE];
};

/**
 * \struct pte_segment
 * \brief The description of a segment
 *
 * pte segments can be used to describe every level in the intel page table
 * system, for ia32 and AMD64 architectures. With modifications to pte they
 * should even be usable in other architectures as well.
 *
 * On the lowest level the pte segment describes 4 megs on intel. This however
 * can vary per architecture. Segments can also cover a number of sub segments
 * rather than pages. This is to allow for the tree like structure the Intel
 * uses in its designs for 64 bits architectures.
 */
struct vm_segment {
        /**
         * \var next
         * \brief Pointer to the next segment in the list
         * \var sub
         * \brief A pointer to a list of sub segments (for 64 bits machines)
         * \var virt_base
         * \brief A pointer indicating the start of the virtual memory described
         * \var pte
         * \brief The virtual reference to the page table
         * \var state
         * \brief An integer indicating the condition of the page table entry
         * \var swappable
         * \brief Indicator for page swapping to be allowed or not
         */
        struct vm_segment* next;

        void* virt_base;
        size_t size;

        struct mm_page_list* pages;
        bool mapped;
        bool swappable;
        bool code;
};

struct vm_descriptor {
        /**
         * \var segments
         * \brief List of available segments to the application
         * \var cpl
         * \brief code privilage level
         * \var pid
         * \brief process identifier
         */
        struct vm_segment* segments;
        unsigned int cpl;
        unsigned int pid;
};

int vm_segment_map(struct vm_segment* s, struct mm_page_descriptor* p);
int vm_free(struct vm_descriptor* p);

void* pte_get_phys(void* virt, struct vm_segment* s);

#ifdef __cplusplus
};
#endif

int vm_init();

#endif

/** @} \file  */
