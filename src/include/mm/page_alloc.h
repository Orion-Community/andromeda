/*
 *  Orion OS, The educational operatingsystem
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MM_PAGE_ALLOC_H
#define __MM_PAGE_ALLOC_H

#ifdef __cplusplus
extern "C"  {
#endif

#define PAGE_ALLOC_UNIT         0x10
#define PAGE_SIZE               0x1000
#define PAGE_ALLOC_FACTOR       (PAGE_ALLOC_UNIT*PAGE_SIZE)
#define PAGE_LIST_SIZE          0x10000
#define PAGE_LIST_ALLOCATED     0xFFFFFFFF
#define PAGE_LIST_MARKED        0xFFFFFFFE

void* page_alloc                ();
int   page_free                 (void* page);
int   page_mark                 (void* page);
int   page_unmark               (void* page);
int   page_alloc_init           ();

#ifdef __cplusplus
}
#endif

#endif