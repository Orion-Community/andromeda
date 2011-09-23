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

volatile mutex_t page_lock = 0;
module_t modules[MAX_MODS];

uint32_t* bitmap = NULL;
size_t memsize;

void build_map(multiboot_memory_map_t* map, int mboot_map_size)
{
  addr_t memory_map_end;
  bitmap = kalloc(memsize*0x20);
  
  
}

int page_get_bit(addr_t addr)
{
  if (bitmap == NULL) return -E_BMP_NOMAP;
  return bitmap[map_idx(addr)] & (1 << map_off(addr));
}

int page_set_bit(addr_t addr)
{
  if (bitmap == NULL) return -E_BMP_NOMAP;
  bitmap[map_idx(addr)] |= (1 << map_off(addr));
}

int page_reset_bit(addr_t addr)
{
  if (bitmap == NULL) return -E_BMP_NOMAP;
  /**
   * bitmap[index] and is not 0 xor (1 shift left map_offset) or in human
   * english: set this bit to 0;
   */
  bitmap[map_idx(addr)] &= ~0^(1 << map_off(addr));
}