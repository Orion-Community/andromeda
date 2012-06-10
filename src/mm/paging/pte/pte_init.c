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

// Initialise the pte system
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
        for (; idx < (addr_t)kernel_offset + kernel_size; idx += PAGESIZE)
        {
                pte_map((void*)idx, (void*)(idx - (addr_t)kernel_offset),
                                                                     pte_core);
        }

        return -E_SUCCESS;
}

// Add a shadow pte to an existing one
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

// Set a shadow pte entry
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
        pte->state ++;

        return -E_SUCCESS;
}

// Return the physical address of some virtual one
addr_t
pte_get_phys(addr_t virt)
{
        return -E_NOFUNCTION;
}

// Set some x86 dependent pte entry
int
x86_pte_set_entry(struct pte_shadow* pte, idx_t idx, void* phys)
{
        if (pte == NULL)
                return -E_NULL_PTR;
        if (idx > PTE_SIZE)
                return -E_INVALID_ARG;

        if (pte->pte == NULL)
                return -E_NULL_PTR;

        pte->pte->entry[idx].pageIdx = (addr_t)phys >> PTE_OFFSET;

        return -E_SUCCESS;
}

// Switch pte contexts
int
pte_switch()
{
        // Do something

        return -E_NOFUNCTION;
}

// Map physical region into a virtual one
int
pte_map(void* virt, void* phys, struct pte_shadow* pte)
{
        /**
         * \note Virt and Phys can be NULL
         * \note Virt ^ Phys % PAGESIZE must be 0
         */
        if (pte == NULL)
                return -E_NULL_PTR;

        if (((addr_t)virt ^ (addr_t)phys) % PAGESIZE != 0)
                return -E_INVALID_ARG;

        struct pte_shadow* cariage = pte;
        idx_t i = 0;
        addr_t v = 0;
        for (; i < PTE_DEEP; i++)
        {
                v = ((addr_t)virt) >> (PTE_OFFSET * i) & PTE_MASK;
                if (cariage->children[v] == NULL)
                        pte_set_entry(v, cariage, NULL);
                cariage = (struct pte_shadow*)cariage -> children[v];
        }
        cariage->children[v] = virt;

        return -E_NOFUNCTION;
}

// Recursively unmap a virtual address
int
pte_runmap(void* virt, struct pte_shadow* pte, idx_t deep)
{
        if (pte == NULL)
                return -E_NULL_PTR;

        addr_t v = (addr_t)virt >> (PTE_OFFSET * (PTE_DEEP - deep)) & PTE_MASK;
        if (deep != 0)
        {
                int ret = pte_runmap(virt, pte, deep--);
                if (ret != -E_SUCCESS)
                        return ret;
        }
        struct pte_shadow* tmp = pte->children[v];
        if (tmp->state == NULL && tmp->pte == NULL)
        {
                pte->state--;
                pte->children[v] = NULL;
        }
        return -E_SUCCESS;
}

// Unmap a virtual address
int
pte_unmap(void* virt, struct pte_shadow* pte)
{
        if (pte == NULL)
                return -E_NULL_PTR;
        if ((addr_t)virt % PAGESIZE != 0)
                return -E_INVALID_ARG;

        return pte_runmap(virt, pte, PTE_DEEP);
}

// Implement the page in the architectual layer
int pte_implement()
{
        return -E_NOFUNCTION;
}

// Purge the page from the architectual layer
int pte_purge()
{
        return -E_NOFUNCTION;
}

/**  @}\file */
