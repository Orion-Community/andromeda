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
 * \defgroup PTE
 * @{
 */

#ifndef __MM_PTE_H
#define __MM_PTE_H

#include <mm/paging.h>
#include <defines.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PTE_CPL_USER 3
#define PTE_CPL_CORE 0
#define PTE_SIZE 0x400

/**
 * \struct pte
 * \brief Page table entry
 * \todo Move this structure out into arch/x86
 */
struct pte {
        /**
         * \var page_table
         * \brief The page table entries
         */
        struct page_table entry[PTE_SIZE];
};

/**
 * \struct pte_segment
 * \brief The description of a segment
 */
struct pte_segment {
        /**
         * \var next
         * \brief Pointer to the next segment in the list
         * \var virt_base
         * \brief A pointer indicating the start of the virtual memory described
         * \var pte
         * \brief The virtual reference to the page table
         * \var state
         * \brief An integer indicating the condition of the page table entry
         * \var swappable
         * \brief Indicator for page swapping to be allowed or not
         */
        struct pte_segment* next;

        void* virt_base;

        struct pte* pte;
        int mapped;
        int swappable;
        int code;
};

struct pte_descriptor {
        /**
         * \var segments
         * \brief List of available segments to the application
         * \var cpl
         * \brief code privilage level
         * \var pid
         * \brief process identifier
         */
        struct pte_segment* segments;
        unsigned int cpl;
        unsigned int pid;
};

#ifdef __cplusplus
};
#endif

int pte_init();

#endif

/** @} \file  */
