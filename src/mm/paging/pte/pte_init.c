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

#include <mm/pte.h>
#include <andromeda/error.h>
#include <stdlib.h>
#include <defines.h>

/**
 * \AddToGroup PTE
 */

/**
 * \var pte_core
 * \brief The pte tracker for the kernel
 */
struct pte_shadow* pte_core = NULL;

int pte_init(void* kernel_offset, size_t kernel_size)
{
        if (kernel_offset == NULL)
                return -E_NULL_PTR;
        if (pte_core != NULL)
                return -E_ALREADY_INITIALISED;

        pte_core = kalloc(sizeof(*pte_core));
        if (pte_core == NULL)
                panic("Couldn't allocate memory for page adminsitration");
        memset(pte_core, 0, sizeof(*pte_core));

        addr_t idx = (addr_t)kernel_offset;
        idx_t pte0_size = kernel_size;
        idx_t pte1_size = kernel_size;
        addr_t page_cntr = 0;
#ifdef X86
        /**
         * \note Shift right 22 bits, gives pd (pte0) entry on x86
         * \note Shift right 12 bits, gives pt (pte1) entry on x86
         * \note 0x3FF is bitmask for the index
         */
#define PTE0_OFFSET 22
#define PTE1_OFFSET 12
#define PTE1_MASK 0x3FF
        idx >>= PTE0_OFFSET;
        pte0_size >>= PTE0_OFFSET;
        pte1_size = (pte1_size >> PTE1_OFFSET) & PTE1_MASK;
        for (; idx <= pte0_size; idx++)
        {
                pte_core->children[idx] = kalloc(sizeof(*pte_core));
                if (pte_core->children[idx] == NULL)
                        panic("Out of memory in pte_init");
                memset(pte_core->children[idx], 0, sizeof(*pte_core));

                int i = 0;
                if (pte0_size != idx)
                        for (; i < PTE_SIZE; i++)
                        {
                                pte_core->children[idx]->children[i] =
                                                           (struct pte_shadow*)
                                                                     page_cntr;
                                page_cntr += PAGESIZE;
                        }
                else
                        for (; i < pte1_size; i++)
                        {
                                pte_core->children[idx]->children[i] =
                                                           (struct pte_shadow*)
                                                                     page_cntr;
                                page_cntr += PAGESIZE;
                        }
        }
#else
        panic("pte_init not implemented!");
#endif
        return -E_SUCCESS;
}

int pte_switch()
{
        return -E_NOFUNCTION;
}

int pte_map(void* virt, void* phys, struct pte_shadow* pte)
{
        return -E_NOFUNCTION;
}

int pte_unmap(void* virt, struct pte_shadow* pte)
{
        return -E_NOFUNCTION;
}

/**  @}\file */
