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
 * \defgroup x86_paging
 * @{
 */

#ifndef _ARCH_X86_MM_PAGING_H
#define _ARCH_X86_MM_PAGING_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SLAB

#include <mm/cache.h>

extern struct mm_cache* x86_pte_pt_cache;
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
extern struct page_dir *vpd;
extern void* vpt[0x400];
extern struct page_table page_table_boot;
extern struct page_dir page_dir_boot;
atomic_t pte_cnt[0x400];

struct page_dir
{
          unsigned int present  : 1; // Must be 1 to be able to access
          unsigned int rw       : 1; // if 0, can not be written to
          unsigned int userMode : 1; // If 0, can not be accessed form usermode
          unsigned int pwt      : 1; // Page level write through, whatever that may be
          unsigned int pcd      : 1; // Page level cache disable bit
          unsigned int accessed : 1; // True if accessed
          unsigned int dirty    : 1; // Ignored when page size = 4 KB (true if written to)
          unsigned int pageSize : 1; // True for 1 MB, false for accessing pagetable
          unsigned int global   : 1; // For use in 4 MB pages only
          unsigned int ignored  : 3; // Ignored
          unsigned int pageIdx  : 20; // Pointer to either page in 4MB pages or page table in 4 KB pages
} __attribute__((packed));
typedef struct page_dir page_dir_t;

struct page_table
{
          unsigned int present  : 1; // Must be 1 to be able to access
          unsigned int rw       : 1; // if 0, can not be written
          unsigned int userMode : 1; // if 0, can not be accessed from usermode
          unsigned int pwt      : 1; // Page level write through, whatever that may be
          unsigned int pcd      : 1; // Page level cache disable bit
          unsigned int accessed : 1; // True if accessed
          unsigned int dirty    : 1; // True if written to
          unsigned int pat      : 1; // Don't know this one, keep it 0 according to intel docs
          unsigned int global   : 1; // Determines global translation
          unsigned int unloaded : 1;
          unsigned int ignored  : 2; // Ignored
          unsigned int pageIdx  : 20; // Pointer to page
} __attribute__((packed));
typedef struct page_table page_table_t;

struct x86_pte_meta {
        struct page_dir pd[1024];
        void* vpt[1024];
};

#ifdef __cplusplus
}
#endif

#endif

/**
 *@} \file
 */
