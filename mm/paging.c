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

#include <mm/paging.h>
#include <mm/map.h>
#include <stdlib.h>

#ifdef __INTEL

extern mutex_t pageLock;
extern boolean pageDbg;
extern boolean state;

extern unsigned short bitmap[];

/// Put some page table structure here ...
volatile struct pageTable page_tables[0x400][0x400];
volatile struct pageDirectory page_direcotry[0x400];

void corePaging(short mmap[])
{
  state = CORE;
  pageDbg = TRUE;
  memcpy(bitmap, mmap, PAGES); // Get that memory map here
  printf("Memcpy done\n");
  
  // Re do the page table set up, which turns out to be a little bit tricky ...
  // Maybe we should do a rewrite with the GDT trick in it
  
  printf("Warning! Page tables haven't been implemented in high memory yet!\n");
}
#endif