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
#include <stdlib.h>

unsigned short bitmap[PAGES];

#ifdef __COMPRESSED

void buildMap(multiboot_memory_map_t* map, int size)
{
  long i, j;
  for(i = 0; i < PAGES; i++)
  {
    bitmap[i] = NOTUSABLE;
  }
  for(i = 0; i < size; i++)
  {
    for (j = map[i].addr; j < map[i].addr + map[i].len; j+=PAGESIZE)
    {
      bitmap[j/PAGESIZE] = (map[i].type == 1) ? FREE : NOTUSABLE;
    }
  }
}

void addModules(multiboot_module_t* mods, int count)
{
  long i, j;
  for (i = 0; i < count; i++)
  {
    for(j = mods[i].mod_start; j < mods[i].mod_end; j+=PAGESIZE)
    {
      bitmap[j/PAGESIZE] = MODULE;
    }
  }
}

void addCompressed()
{
  long i;
  
  for (i = ((long)(&mboot)%PAGESIZE+(long)(&mboot)); i < ((long)(&end)%PAGESIZE+(long)(&end)); i++)
  {
    bitmap[i] = COMPRESSED;
  }
}
#endif