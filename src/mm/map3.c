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

#include <mm/map.h>
#include <thread.h>
#include <boot/mboot.h>
#include <stdlib.h>
#include <error/error.h>

#define map_idx(a) (a / (sizeof(bitmap[0])*8))
#define map_off(a) (a % (sizeof(bitmap[0])*8))
#define map_size   (memsize/0x4)

volatile mutex_t page_lock = 0;
module_t modules[MAX_MODS];

struct page *page_map = NULL;
size_t memsize;

void build_map(multiboot_memory_map_t* map, int mboot_map_size)
{
  addr_t memory_map_end;
  page_map = kalloc(map_size*sizeof(page_map));
  
  memset(page_map, 0, sizeof(page_map)*map_size);
  #ifdef PAGEDBG
  printf("Map size: %XB\tMap size: %X entries\tMemsize: %XB\n", map_size*sizeof(page_map), map_size, memsize*1024);
  #endif
}