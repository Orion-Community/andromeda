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
  void* base = NULL;
  int addSize = 0;
  long done = 0;
  int i = 0;
  for (; i < PAGES; i++)
  {
    if (bitmap[i] == FREE)
    {
      if (base == NULL)
      {
	base = i*PAGESIZE;
      }
      size += 1;
      done +=PAGESIZE;
      if (done >= size)
      {
	heapAddBlocks(base, addSize);
	break;
      }
    }
    else if(base != NULL && size != 0)
    {
      heapAddBlocks(base, addSize);
      base = NULL;
    }
  }
  
  initPaging();

  return 0;
}

void memset(void* location, int value, int size)
{
  int i = 0;
  int* offset = (int*)location;
  for (; i <= size; i++)
  {
    offset[i] = value;
  }
}
