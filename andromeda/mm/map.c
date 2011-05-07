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

#ifdef X86
#define PAGES     0x100000
#define PAGESIZE  0x1000
#endif

#define FREE	  0x0000
#define COMPRESSED0x0001
#define MODULE	  0x0002
#define NOTUSABLE 0xFFFF

unsigned short mmap[PAGES];

void buildMap(multiboot_memory_map_t* map, int size)
{
  memset(mmap, NOTUSABLE, PAGES*sizeof(short));
  long i, j;
  for(i = 0; i < size; i++)
  {
    for (j = map[i].addr; j < map[i].addr + map[i].len; j+=PAGESIZE)
    {
      mmap[j/PAGESIZE] = (map[i].type == 1) ? FREE : NOTUSABLE;
    }
  }
}

void addModules(multiboot_module_t* mods, int count)
{
  long i, j;
  for (i = 0; i < count; i++)
  {
    for(j = mods[i].mod_start; j < mod_end; j+=PAGESIZE)
    {
      mmap[j/PAGESIZE] = MODULE;
    }
  }
}
extern int mboot;
extern int end;
void addCompressed()
{
  long i;
  
  for (i = (long)(&mboot)%PAGESIZE; i < (long)(&end))%PAGESIZE+(long)(&end); i++)
  {
    mmap[i] = COMPRESSED;
  }
}