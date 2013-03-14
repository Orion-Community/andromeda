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

/**
 * \var pd
 * \brief The physical page directory pointer
 * \var spd
 * \brief Use this to access the data of the actual page directory
 * \var vpd
 * \brief The virtual pointers of the page tables referenced by the page directory
 * \var page_table_boot
 * \brief The page tables described in the linker script
 * \var page_dir_boot
 * \brief The page directory described in the linker script
 * \var pte_cnt
 */
extern struct page_dir* pd;
extern struct page_dir *spd;
extern void* vpd[1024];
extern struct page_table page_table_boot;
extern struct page_dir page_dir_boot;
atomic_t pte_cnt[1024];

int x86_pte_set_range(struct pte_range* range);
int x86_pte_reset_range(struct pte_range* range);
int x86_pte_set_page(void* virt, void* phys, int cpl);

#ifdef __cplusplus
}
#endif

#endif

/**
 *@} \file
 */
