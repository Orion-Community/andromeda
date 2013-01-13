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

/**
 * \defgroup paging
 * @{
 */

#ifndef _ARCH_X86_MM_PAGING_H
#define _ARCH_X86_MM_PAGING_H

#ifdef __cplusplus
extern "C" {
#endif


extern struct page_dir* pd;
extern struct page_dir *spd;
extern void* vpd[1024];
extern struct page_table page_table_boot;
extern struct page_dir page_dir_boot;

int x86_pte_set(void* phys, int cpl, struct page_table* pte);

#ifdef __cplusplus
}
#endif

#endif

/**
 *@} \file
 */
