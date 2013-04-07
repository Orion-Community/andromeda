/*
    Andromeda
    Copyright (C) 2011, 2012  Bart Kuivenhoven

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

#include <mm/heap.h>

#ifndef __MEMORY_H
#define __MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#define SIZE_MEG 0x100000

void paging();
void memset(void*, int, size_t);
void memcpy(void*, void*, size_t);
int memcmp(void*, void*, size_t);
int init_heap();
int complement_heap(void*, size_t);

size_t strlen(char* string);

#ifdef __INTEL
void setGDT();
#endif

extern unsigned int mboot;
extern unsigned int end;

#ifdef X86
#define PAGESIZE 0x1000
#endif

#ifdef __cplusplus
}
#endif

#endif

/** \file */
