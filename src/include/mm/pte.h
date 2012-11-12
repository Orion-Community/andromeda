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

struct pte_shadow;
extern struct pte_shadow* pte_core;

/**
 * \def PTE_OFFSET
 * \brief Twelve bits have to be shifted out of the address, for the next idx
 */
#define PTE_OFFSET 12

#ifdef X86
#define KERN_ADDR THREE_GIB
#define PTE_SIZE 0x400
/**
 * \note Shift right 22 bits, gives pd (pte0) entry on x86
 * \note Shift right 12 bits, gives pt (pte1) entry on x86
 * \note 0x3FF is bitmask for the index
 */
#define PTE0_OFFSET 22
#define PTE1_OFFSET 12
#define PTE_MASK 0x3FF

#define PTE_DEEP 2

#endif /* x86 */


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
 * \struct pte_shadow
 * \brief The shadow discriptor of the page table
 */
struct pte_shadow {
        /**
         * \var pte
         * \brief The virtual reference to the page table
         * \var children
         * \brief A set of pointers to the leaves / branches of the pte table
         * \var parent
         * \brief A reference to the parent for quick lookups
         * \var state
         * \brief An integer indicating the condition of the page table entry
         */
        struct pte* pte;
        void* children[PTE_SIZE];
        struct pte_shadow* parent;
        int state;
};
/**
 * \struct pte_segment
 * \brief Virtual memory segment discriptor
 */
struct pte_segment {
        /**
         * \var pagetables
         * \brief pagetable discriptors
         * \var pte_shadow
         * \brief shadow table discriptors
         */
        struct pte* pagetables[0x10];
        struct pte_shadow* shadowtables[0x10];
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
