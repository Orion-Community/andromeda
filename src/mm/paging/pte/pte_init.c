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

int
pte_init(void* kernel_offset, size_t kernel_size)
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
        idx >>= PTE0_OFFSET;
        pte0_size >>= PTE0_OFFSET;
        pte1_size = (pte1_size >> PTE1_OFFSET) & PTE_MASK;
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
                                ((struct pte_shadow*)(pte_core->children[idx]))
                                                                ->children[i] =
                                                              (void*)page_cntr;
                                page_cntr += PAGESIZE;
                        }
                else
                        for (; i < pte1_size; i++)
                        {
                                ((struct pte_shadow*)(pte_core->children[idx]))
                                                                ->children[i] =
                                                              (void*)page_cntr;
                                page_cntr += PAGESIZE;
                        }
        }
#else
        panic("pte_init not implemented!");
#endif
        return -E_SUCCESS;
}

struct pte_shadow*
pte_add_shadow(struct pte_shadow* parent, idx_t idx, struct pte* pte)
{
        struct pte_shadow* pte_s = kalloc(sizeof(*pte_s));
        if (pte_s == NULL)
                return NULL;
        memset(pte_s, 0, sizeof(*pte_s));

        pte_s->pte = pte;
        pte_s->parent = parent;
        parent->children[idx] = pte_s;

        return pte_s;
}

int
pte_set_entry(idx_t idx, struct pte_shadow* pte, struct pte_shadow* child)
{
        if (pte == NULL)
                return -E_NULL_PTR;
        if (idx > PTE_SIZE)
                return -E_INVALID_ARG;

        if (child == NULL)
        {
                child = kalloc(sizeof(*child));
                if (child == NULL)
                        panic("Could not allocate memory for page operation");
                memset(child, 0, sizeof(*child));
        }

        pte->children[idx] = child;

        return -E_SUCCESS;
}

int
x86_pte_set_entry(void* virt, void* phys, struct pte* pte)
{
        return -E_NOFUNCTION;
}

int
pte_switch()
{
        return -E_NOFUNCTION;
}

int
pte_map(void* virt, void* phys, struct pte_shadow* pte)
{
        if (virt == NULL || phys == NULL || pte == NULL)
                return -E_NULL_PTR;

        addr_t v = ((addr_t)virt) >> PTE_OFFSET;
        struct pte_shadow* cariage = pte;
        idx_t i = 0;
        for (; i < PTE_DEEP; i++)
        {
                if (cariage->children[v] == NULL)
                        pte_set_entry(v, cariage, NULL);
                cariage = (struct pte_shadow*)cariage -> children[v];
        }
        cariage->children[v] = virt;

        return -E_NOFUNCTION;
}

int
pte_unmap(void* virt, struct pte_shadow* pte)
{
        return -E_NOFUNCTION;
}

/**  @}\file */
