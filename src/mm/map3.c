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

#define map_size   (memsize/0x4)

module_t modules[MAX_MODS];

struct page *page_map = NULL;
size_t memsize;

volatile mutex_t map_lock = 0;

int build_map(multiboot_memory_map_t* map, int mboot_map_size)
{
  addr_t memory_map_end;
  page_map = kalloc(map_size*sizeof(page_map));
  
  memset(page_map, 0, sizeof(page_map)*map_size);
  #ifdef PAGEDBG
  printf("Map size: %X B\tMemsize: %X B\n", map_size*sizeof(page_map), memsize*1024);
  #endif
  return -E_BMP_NOMAP;
}

uint32_t map_find_endoflist(uint32_t idx)
{
  for (; page_map[idx].next_idx != 0; idx = page_map[idx].next_idx)
  {
    if (page_map[idx].next_idx == FREE && page_map[idx].prev_idx)
      return -E_BMP_CORRUPT;
  }
  return idx;
}

int map_add_page(uint32_t list_start, uint32_t page_index)
{
  if (list_start >= map_size)
    return -E_BMP_NOIDX;
  else if (page_index >= map_size)
    return -E_BMP_NOIDX;
  else if (page_map == NULL)
    return -E_BMP_NOMAP;
  
  uint32_t list_end = map_find_endoflist(list_start);
  
  if (list_end == (uint32_t)-E_BMP_CORRUPT)
    return -E_BMP_CORRUPT;
  
  page_map[list_end].next_idx = page_index;
  
  page_map[page_index].prev_idx = list_end;
  page_map[page_index].next_idx = 0;
  
  return -E_SUCCESS;
}

int map_rm_page(uint32_t page_index)
{
  if (page_index >= map_size)
    return -E_BMP_NOIDX;
  else if (page_map == NULL)
    return -E_BMP_NOMAP;
  
  return -E_NOFUNCTION;
}

uint32_t map_alloc_page(uint32_t list_idx)
{
  uint32_t idx = 0;
  for (; idx < map_size; idx++)
  {
    if (page_map[idx].next_idx == FREE && page_map[idx].prev_idx == FREE)
    {
      map_add_page(list_idx, idx);
      return idx;
    }
  }
  return (uint32_t)-E_BMP_NOMEM;
}