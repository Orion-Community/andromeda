/*
 * Andromeda
 * Copyright (C) 2012  Bart Kuivenhoven
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
#include <mm/vm.h>
#include <types.h>
#include <andromeda/error.h>

/**
 * \AddToGroup paging
 * @{
 */

struct page_dir *pd = NULL;
extern struct page_dir page_dir_boot;

int x86_pte_init()
{
        pd = &page_dir_boot;
        return -E_NOFUNCTION;
}

void* pte_get_phys(void* virt, struct vm_segment* s)
{
        return NULL;
}

int x86_pte_set(struct vm_segment* s)
{
        return -E_NOFUNCTION;
}

int x86_pte_unset(struct vm_segment* s)
{
        return -E_NOFUNCTION;
}

/**
 * @} \file
 */
