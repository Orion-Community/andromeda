/*
 * Andromeda
 * Copyright (C) 2013  Bart Kuivenhoven
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

#include <mm/page_alloc.h>
#include <mm/paging.h>
#include <mm/vm.h>
#include <types.h>
#include <andromeda/error.h>
#include "paging.h"

/**
 * \AddToGroup paging
 * @{
 */

/**
 * \fn x86_pte_set_pt
 * \brief Set a page table reference into the page dir.
 * \param pt
 * \brief The physical page pointer to the page table.
 * \param idx
 * \brief The index to put the entry.
 */
int x86_pte_set_pt(struct page_table** pt, int idx)
{
        spd[idx].pageIdx = (int)pt >> 12;
        spd[idx].present = 1;
        spd[idx].userMode = 1;
        return -E_SUCCESS;
}

/**
 * \fn x86_pte_unset_pt
 * \brief Disable a page table
 * \param idx
 * \brief The page table to disable
 */
int x86_pte_unset_pt(int idx)
{
        spd[idx].present = 0;
        return -E_SUCCESS;
}

int x86_pte_set_page(void* virt, void* phys, int cpl)
{
        if (virt == NULL || phys == NULL ||(((int)virt|(int)phys) & 0xFFF) != 0)
                return -E_INVALID_ARG;

        addr_t v = (addr_t)virt >> 12;

        int pte = v & 0x3FF;
        int pde = (v >> 10) & 0x3FF;

        struct page_table** pt;
        if ((pt = vpd[pde]) == NULL)
        {
                /**
                 * \todo Allocate and install new pagetable here
                 * pt = new pt;
                 */
                panic("Page table allocaton not yet written");
                x86_pte_set_pt(pt, pde);
        }
        x86_pte_set(phys, cpl, pt[pte]);

        return -E_NOFUNCTION;
}

int x86_pte_unset_page(void* virt)
{
        return -E_NOFUNCTION;
}

/**
 * @} \file
 */
