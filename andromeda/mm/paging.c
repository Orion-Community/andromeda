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

#define PRESENTBIT    0x01
#define WRITEBIT      0x02
#define USERBIT       0x04
#define RESERVEDBIT   0x08
#define DATABIT       0x10

#define PRESENT  ((err && PRESENTBIT)  ? TRUE : FALSE)
#define WRITE    ((err && WRITEBIT)    ? TRUE : FALSE)
#define USER	 ((err && USERBIT)     ? TRUE : FALSE)
#define RESERVED ((err && RESERVEDBIT) ? TRUE : FALSE)
#define DATA     ((err && DATATBIT)    ? TRUE : FALSE)

#ifdef __INTEL

void cPageFault(isrVal_t regs)
{
  printf("PG\n");
  if (regs.cs != 0x8 && regs.cs != 0x18)
  {
    panic("Incorrect frame");
  }
  unsigned char err = (unsigned char) (regs.errCode & 0x7);
  
  printf("The pagefault was caused by: "); printhex((unsigned int)getCR2()); putc('\n');
  
  if (!USER)
  {
    panic("User mode not allowed yet!");
  }
  else if (!PRESENT && WRITE)
  {
    panic("Can not allocate pages yet!");
    // Allocate page here!
    unsigned long page = getCR2() << 0xC;
    unsigned long phys = (unsigned long)allocPage(COMPRESSED);
    if (phys == (unsigned long)NULL)
    {
      panic("No more free memory!");
    }
    if (!setPage((void*)phys, (void*)getCR2(), FALSE, TRUE))
    {
      freePage((void*)phys, COMPRESSED);
      panic("Setting the page failed dramatically!");
    }
  }
  else if (!PRESENT && !WRITE)
  {
    panic("Page non existent!");
    // Read the page from image
  }
  else if (PRESENT)
  {
    panic("Accessing illicit content!");
    // Kill process trying to access this page!
  }
  printf("Err code: "); printhex(err); putc('\n');
  panic("Paging isn't finished yet");
}

boolean setPage(void* virtAddr, void* physAddr, boolean ro, boolean usermode)
{
  #ifdef X86
  if (!CHECKALLIGN((unsigned long)virtAddr) || !CHECKALLIGN((unsigned long)physAddr))
  {
    return FALSE;
  }
  unsigned long pdIdx = ((unsigned long)virtAddr >> 22);
  unsigned long ptIdx = ((unsigned long)virtAddr >> 12) - (pdIdx << 10);
  unsigned long offset = (unsigned long)virtAddr%PAGESIZE;
  
  unsigned long CR3 = (unsigned long)getCR3();
  pageDir_t* pd = (pageDir_t*) (CR3 - (CR3 % PAGESIZE)); // Get the address of the page directory.
  unsigned long ptAddr = (unsigned long)pd[pdIdx].pageIdx*PAGESIZE; // Get the address of the page table.
  if (ptAddr == 0)
  {
    return FALSE;
  }
  pageTable_t* pt = (pageTable_t*) ptAddr;
  
  pt[ptIdx].pageIdx = (unsigned long)physAddr/PAGESIZE;
  pt[ptIdx].pcd = 0;
  pt[ptIdx].pwt = 0;
  pt[ptIdx].present = 1;
  pt[ptIdx].accessed = 0;
  pt[ptIdx].dirty = 0;
  pt[ptIdx].rw = (ro) ? 0 : 1; // Read only data?
  pt[ptIdx].userMode = (usermode) ? 0 : 1; // Usermode?
  pt[ptIdx].global = 0;
  pt[ptIdx].pat = 0;

  return TRUE;
  #endif
}

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
  
  #ifdef TESTPAGES
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
  int i, j, k;
  for(i = 0; i < PAGETABLES; i++)
  {
    k = 0;
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
	case MAPPEDIO:
	  pt[j].pageIdx = (i*PAGETABLES+j);
	  pt[j].pcd = 0;
	  pt[j].pwt = 0;
	  pt[j].present = 1;
	  pt[j].accessed = 0;
	  pt[j].dirty = 0;
	  pt[j].rw = 1;
	  pt[j].userMode = 0;
	  pt[j].global = 0;
	  pt[j].pat = 0;
	  k++;
	  // Map the page to physical memory
	  break;
	case NOTUSABLE:
	  // Keep it 0, as it is still not usable
	  break;
      }
      if (i*PAGETABLES+j == 0xB8000)
      {
	pt[j].pageIdx = (i*PAGETABLES+j);
	pt[j].pcd = 0;
	pt[j].pwt = 0;
	pt[j].present = 1;
	pt[j].accessed = 0;
	pt[j].dirty = 0;
	pt[j].rw = 1;
	pt[j].userMode = 0;
	pt[j].global = 0;
	pt[j].pat = 0;
      }
    }
    pageDir[i].pageIdx = (int)pt >> 0xC;
    pageDir[i].pcd = 0;
    pageDir[i].pwt = 0;
    pageDir[i].accessed = 0;
    pageDir[i].dirty = 0;
    pageDir[i].present = (k > 0) ? 1 : 0;
    pageDir[i].rw = 1;
    pageDir[i].pageSize = 0;
    pageDir[i].userMode = 0;
    pageDir[i].global = 0;
    // Add the page directory here.
  }
  return pageDir;
}

void initPaging ()
{
  pageDir_t* kernDir = setupPageDir();
  
  unsigned long cr3 = (unsigned long)kernDir;
  cr3 -= (cr3 % 0x1000);
  setCR3(cr3);
  
  toglePGbit();
}

#endif