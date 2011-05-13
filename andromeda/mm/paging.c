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
void* getPhysAddr(void* addr)
{
  void* ret = NULL;
  #ifdef X86
  unsigned long pdIdx = ((unsigned long)addr >> 22);
  unsigned long ptIdx = ((unsigned long)addr >> 12) - (pdIdx << 10);
  unsigned long offset = (unsigned long)addr%PAGESIZE;
  
  unsigned long CR3 = (unsigned long)getCR3();
  pageDir_t* pd = (pageDir_t*) (CR3 - (CR3 % PAGESIZE)); // Get the address of the page directory.
  unsigned long ptAddr = (unsigned long)pd[pdIdx].pageIdx*PAGESIZE; // Get the address of the page table.
  pageTable_t* pt = (pageTable_t*) ptAddr;
  
  unsigned long tmpRet = (unsigned long)pt[ptIdx].pageIdx*PAGESIZE; // Get the page address
  tmpRet += offset; // Add the offset
  
  ret = (void *)tmpRet; // return the variable
  
  #ifdef DBG
  printf("CR3\t"); printhex((int)CR3); putc('\n');
  printf("PD\t"); printhex((int)pd); putc('\n');
  printf("PD idx\t"); printhex((int)pdIdx); putc('\n');
  printf("PT\t"); printhex((int)pt); putc('\n');
  printf("PT idx\t"); printhex((int)ptIdx); putc('\n');
  #endif
  
  #endif
  
  return ret;
}

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
	  // Do nothing, the 0 page is fine
	  break;
	case COMPRESSED:
	case MODULE:
	  pt[j].pageIdx = (i*PAGETABLES+j);
	  // Map the page to physical memory
	  break;
	case NOTUSABLE:
	  // Keep it 0, as it is still not usable
	  break;
      }
    }
    pageDir[i].pageIdx = (int)pt >> 0xC;
    // Add the page directory here.
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