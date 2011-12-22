/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PAGING_H
#define PAGING_H

#include <andromeda/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef X86
#define PAGES       0x100000
#define PAGESIZE    0x1000
#define PAGETABLES  0x400
#define PAGEDIRS    0x400
#define PAGEENTRIES 0x400

#define PAGE_BITS   0xFFF

#endif

#define CHECKALLIGN(a) ((a%PAGESIZE) ? FALSE : TRUE)

struct page_dir
{
  unsigned int present	: 1; // Must be 1 to be able to access
  unsigned int rw	: 1; // if 0, can not be written to
  unsigned int userMode : 1; // If 0, can not be accessed form usermode
  unsigned int pwt	: 1; // Page level write through, whatever that may be
  unsigned int pcd	: 1; // Page level cache disable bit
  unsigned int accessed : 1; // True if accessed
  unsigned int dirty	: 1; // Ignored when page size = 4 KB (true if written to)
  unsigned int pageSize : 1; // True for 1 MB, false for accessing pagetable
  unsigned int global	: 1; // For use in 4 MB pages only
  unsigned int ignored	: 3; // Ignored
  unsigned int pageIdx	: 20; // Pointer to either page in 4MB pages or page table in 4 KB pages
} __attribute__((packed));
typedef struct page_dir page_dir_t;

struct page_table
{
  unsigned int present	: 1; // Must be 1 to be able to access
  unsigned int rw	: 1; // if 0, can not be written
  unsigned int userMode : 1; // if 0, can not be accessed from usermode
  unsigned int pwt	: 1; // Page level write through, whatever that may be
  unsigned int pcd	: 1; // Page level cache disable bit
  unsigned int accessed : 1; // True if accessed
  unsigned int dirty	: 1; // True if written to
  unsigned int pat	: 1; // Don't know this one, keep it 0 according to intel docs
  unsigned int global	: 1; // Determines global translation
  unsigned int ignored	: 3; // Ignored
  unsigned int pageIdx	: 20; // Pointer to page
} __attribute__((packed));
typedef struct page_table page_table_t;

extern volatile addr_t offset;

void init_paging();
int page_unmap_low_mem();
addr_t page_phys_addr(addr_t, struct page_dir*);

#ifdef __cplusplus
}
#endif

#endif