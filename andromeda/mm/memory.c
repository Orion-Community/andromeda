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

#include <stdlib.h>
#include <mm/paging.h>
#include <mm/map.h>

int initHeap(long size)
{
  long done = 0;
  int i = 0x100;
  for (; i < PAGES; i++)
  {
    if (bitmap[i] == FREE)
    {
      heapAddBlocks((void*)(i*PAGESIZE), PAGESIZE);
      done += PAGESIZE;
//       #ifdef DBG
//       printf("Added "); printhex(done); printf(" to heap\n");
//       #endif
      claimPage((long)i, COMPRESSED);
      if (size <= done)
      {
	break;
      }
    }
  }
  examineHeap();
  initPaging();

  return 0;
}

void memset(void* location, int value, int size)
{
  int i = 0;
  unsigned char* offset = (unsigned char*)location;
  for (; i <= size; i++)
  {
    offset[i] = (unsigned char)value;
  }
}
