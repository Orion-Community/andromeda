/*
    Andromeda
    Copyright (C) 2011 - 2013  Bart Kuivenhoven

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

#include <defines.h>
#include <andromeda/cpu.h>
#include <boot/mboot.h>
#include <thread.h>
#include <andromeda/sched.h>
#include <mm/memory.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup paging
 * This module handles paging
 * \addtogroup paging
 * @{
 */

#ifdef X86
#define PAGES           0x100000
#define PAGESIZE        0x1000
#define PAGETABLES      0x400
#define PAGEDIRS        0x400
#define PAGEENTRIES     0x400
#define MINIMUM_PAGES   0x1000
#define PAGE_BITS       0xFFF
#define BYTES_IN_PAGE   0x1000
#define GIB             0x40000000
#define THREE_GIB       0xC0000000
#define KERNEL_CPL      0
#define USER_CPL        3

#define PGIDX(a) ((a >> 12) & 0xFFFFF)

#endif /* X86 */

#define CHECKALLIGN(a) ((a%PAGESIZE) ? FALSE : TRUE)

extern volatile addr_t offset;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/** \file */
