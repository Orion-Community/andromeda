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
#include <stdlib.h>

mutex_t pageLock = 0;

module_t modules[32];
unsigned short bitmap[PAGES];

boolean claimPage(unsigned long page, unsigned short owner)
{
  if (bitmap[page] != FREE || page > PAGES)
  {
    return FALSE;
  }
  bitmap[page] = owner;
  
  return TRUE;
}

pageState_t* allocPage(unsigned short owner)
{
  mutexEnter(pageLock);
  unsigned long i;
  pageState_t* addr = kalloc(sizeof(pageState_t));
  if (addr == NULL)
  {
    panic("AIEE, OUT OF MEMORY!\n");
  }
  for (i = 0; i < PAGES; i++)
  {
    if (bitmap[i] == FREE)
    {
      if (claimPage(i, owner))
      {
	addr->addr = i*PAGESIZE;
	addr->usable = TRUE;
	mutexRelease(pageLock);
	return addr;
      }
    }
  }
  addr->addr = 0;
  addr->usable = FALSE;
  mutexRelease(pageLock);
  return addr;
}

void freePage(void* page, unsigned short owner)
{
  if (bitmap[((unsigned long)page>>0xC)] != owner)
  {
    return;
  }
  bitmap[(unsigned long)page] = FREE;
}

#ifdef __COMPRESSED

void buildMap(multiboot_memory_map_t* map, int size)
{
  long i, j, end;
  for(i = 0; i < PAGES; i++)
  {
    bitmap[i] = NOTUSABLE;
  }
  end = (long)map + (long)size;
  while ((long)map < end)
  {
    for (j = map->addr; j < map->addr + map->len; j+=PAGESIZE)
    {
      bitmap[j/PAGESIZE] = (map->type == 1) ? FREE : NOTUSABLE;
    }
    map = (multiboot_memory_map_t*)((long)map+(long)map->size+sizeof(map->size));
  }
  bitmap[0xB8] = MAPPEDIO;
  bitmap[0xB0] = MAPPEDIO;
}

void addModules(multiboot_module_t* mods, int count)
{
  long i, j;
  for (i = 0; i < count; i++)
  {
    modules[i].addr = mods[i].mod_start;
    modules[i].end = mods[i].mod_end;
    for(j = mods[i].mod_start; j < mods[i].mod_end; j+=PAGESIZE)
    {
      bitmap[j/PAGESIZE] = MODULE;
    }
  }
}

void addCompressed()
{
  long i;
  unsigned long final = (unsigned long)&end;
  if ((int)&end % PAGESIZE)
  {
    final += PAGESIZE;
  }
  for (i = ((long)(&mboot)/PAGESIZE); i < ((long)final/PAGESIZE); i++)
  {
    bitmap[i] = COMPRESSED;
  }
}
#endif