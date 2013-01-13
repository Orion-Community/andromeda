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

/**
 * \AddToGroup paging
 * @{
 */

extern struct x86_page_dir page_dir_boot;
struct x86_page_dir *pagedir = &page_dir_boot;
static addr_t shadowdir[1024];

int x86_pte_start()
{
        memset(shadowdir, 0, 1024*sizeof(addr_t));
        return -E_NOFUNCTION;
}

/**
 * \fn x86_pte_set_pt
 * \brief Set a page table reference into the page dir.
 * \param pt
 * \brief The physical page pointer to the page table.
 * \param idx
 * \brief The index to put the entry.
 */
int x86_pte_set_pt(struct x86_page_table* pt, int idx)
{
        pagedir->entry[idx].pageIdx = (int)pt >> 12;
        pagedir->entry[idx].present = 1;
        pagedir->entry[idx].userMode = 1;
        return -E_SUCCESS;
}

int x86_pte_unset_pt(int idx)
{
        pagedir->entry[idx].present = 0;
        return -E_SUCCESS;
}

/**
 * @} \file
 */
