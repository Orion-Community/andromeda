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

void addPageTable(pageDir_t* pd, pageTable_t* pt, int idx)
{
  pd[idx].pageIdx = (unsigned int)&pt >> 0xC;
}

pageDir_t* setupPageDir()
{
  pageDir_t* pageDir = alloc(sizeof(pageDir_t)*PAGEDIRS, TRUE);
  if(pageDir == NULL)
  {
    panic("Aieee, Null pointer!!! Paging");
  }
  memset(pageDir, 0, sizeof(pageDir_t)*PAGEDIRS);
  int i, j;
  for(i = 0; i < PAGETABLES; i++)
  {
    pageTable_t* pt = alloc(sizeof(pageTable_t)*PAGETABLES, TRUE);
    if (pt == NULL)
    {
      printf("Itteration "); printhex(i); putc('\n');
      panic("Aiee, Null pointer!!! PageTable");
    }
    memset(pt, 0, sizeof(pageTable_t)*PAGETABLES);
    for (j = 0; j < PAGES/PAGETABLES; j++)
    {
      switch(bitmap[i*PAGETABLES+j])
      {
	case FREE:
	  break;
	case COMPRESSED:
	  break;
	case MODULE:
	  break;
	case NOTUSABLE:
	  break;
      }
    }
  }
  return pageDir;
}

void initPaging ()
{
  #ifdef WARN
  printf("Warning! The paging code hasn't been written yet\n");
  #else
  panic("Paging wasn't initialised!");
  #endif
  
  pageDir_t* kernDir = setupPageDir();
  setCR3((unsigned long)kernDir);
  if (!pgbit)
  {
//      toglePGbit();
  }
}

#endif