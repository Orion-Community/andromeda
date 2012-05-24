/*
 *  Andromeda
 *  Copyright (C) 2011  Bart Kuivenhoven
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

struct pte_shadow;
extern struct pte_shadow* pte_core;

#define PTE_SIZE 0x400
#ifdef X86
#define KERN_ADDR 0xC0000000
#endif

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
        struct page_table table[PTE_SIZE];
};

/**
 * \struct pte_shadow
 * \brief The shadow discriptor of the page table
 */
struct pte_shadow {
        /**
         * \var pte
         * \brief The virtual reference to the page table
         * \var children
         * \brief The virtual descriptor of the children
         * \var state
         * \brief An integer indicating the condition of the page table entry
         */
        struct pte* pte;
        struct pte_shadow* children[PTE_SIZE];
        struct pte_shadow* parent;
        int state;
};

/**
 * \todo write pte_init
 * \todo write pte_switch
 * \todo write pte_map
 * \todo write pte_unmap
 *
 * \fn pte_init
 * \brief Setup the first pte administration
 * \param kern_boundary
 * \brief Where does the kernel start?
 *
 * \fn pte_switch
 * \brief Make the pte system take over control of paging
 *
 * \fn pte_map
 * \brief Map a virtual page to a physical one
 * \param phys
 * \param virt
 * \param pte
 *
 * \fn pte_unmap
 * \brief Unmap a particular page
 * \param phys
 * \param pte
 */
int pte_init(void* kernel_offset, size_t kernel_size);
int pte_switch();
int pte_map(void* virt, void* phys, struct pte_shadow* pte);
int pte_unmap(void* virt, struct pte_shadow* pte);
#ifdef __cplusplus
};
#endif

#endif

/** @} \file  */
