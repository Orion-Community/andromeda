/* Andromeda
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
 * \fn x86_pte_set_page
 * \brief Set the page table entry to the correct value
 * \param virt
 * \param phys
 * \param cpl
 * \param pte
 */
int x86_pte_set(void* phys, int cpl, struct page_table* pte)
{
        if (pte == NULL)
                panic("Incorrect conditions for setting pte");

        pte->userMode = (cpl == 0) ? 0 : 1;
        pte->pageIdx = (int)phys >> 12;
        pte->present = 1;

        return -E_SUCCESS;
}

/**
 * \fn x86_pte_unset_page
 * \brief Set the page table entry as being cleared
 */
int x86_pte_unset(struct page_table* pte)
{
        if (pte == 0)
                panic("Incorrect conditions for unsetting pte");

        pte->present = 0;
        return -E_SUCCESS;
}

/**
 * @} \file
 */
