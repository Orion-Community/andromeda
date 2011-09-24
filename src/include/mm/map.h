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

#ifndef MAP_H
#define MAP_H

#include <mm/paging.h>
#include <types.h>
#include <boot/mboot.h>

#define FREE	   0x0000
#define MODULE	   0x0001
#define COMPRESSED 0x0002
#define MAPPEDIO   0x0003
#define CORE       0x0004
#define NOTUSABLE  0xFFFF

#define MAX_MODS   0x20


typedef struct
{
  unsigned long addr;
  boolean usable;
} pageState_t;

struct module_s
{
  unsigned long addr;
  unsigned long end;
};

typedef struct module_s module_t;

/**
 * This bitmap entry will look a bit like a FAT structure, with the main
 * difference that we're talking pages here, instead of files.
 *
 * If the next index = 0, it's the last page in the list, if the previous index
 * = 0, it's the first page in the list. Somewhere is a pointer that refers to
 * the first page and tells us who/what owns this region in memory
 */

struct page
{
  addr_t next_idx;
  addr_t prev_idx;
};


extern module_t modules[];
extern size_t memsize;

pageState_t* set_page(addr_t page);
void reset_page(addr_t page);

void build_map(multiboot_memory_map_t*, int);
#endif