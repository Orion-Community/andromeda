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

#define PRESENT  ((err & PRESENTBIT)  ? TRUE : FALSE)
#define WRITE    ((err & WRITEBIT)    ? TRUE : FALSE)
#define USER	 ((err & USERBIT)     ? TRUE : FALSE)
#define RESERVED ((err & RESERVEDBIT) ? TRUE : FALSE)
#define DATA     ((err & DATABIT)     ? FALSE : TRUE)

#ifdef __INTEL

/**
 * This paging system is fun and all, and it works, but it is by no means
 * optimised. We need someone to take a look at this to optimise it for
 * both speed and memory savings.
 *
 * Another issue this system has, is that it isn't finished yet. This can
 * be a good thing as if a complete redesign is necessary, it can still
 * be done easilly.
 */

boolean state = COMPRESSED;
boolean pageDbg = FALSE;
void cPageFault(isrVal_t regs)
{
  if (pageDbg)
  {
    printf("PG\n");
  }
  if (pageDbg)
  {
    getCR2();
    printf("Pagefault: %X\n", getCR2());
//     panic("Stopping here!");
  }
  if (regs.cs != 0x8 && regs.cs != 0x18)
  {
    panic("Incorrect frame");
  }  
  unsigned long page = getCR2();
  #ifdef X86
  page &= 0xFFFFF000;
  #else
  
  if (pageDbg)
  {
    printf("Waiting for input\n");
    for (;;);
  }
  
  #endif
  unsigned char err = (unsigned char) (regs.errCode & 0x7);
  if (pageDbg)
  {
    printf("The pagefault was caused by: %X\n", page);
  }
  if (RESERVED)
  {
    panic("A reserved bit was set!");
  }
  
  if (USER)
  {
    panic("User mode not allowed yet!");
  }
  else if (!PRESENT && WRITE)
  {
    if (pageDbg)
      printf("Allocating!\n");
    // Allocate page here!
    pageState_t* phys = allocPage(state);
    if (pageDbg)
      printf("Virt: 0x%X\nPhys: 0x%X\nUsable? %X\n", page, phys->addr, phys->usable);
    
    if (phys->usable == FALSE)
    {
      panic("No more free memory!");
    }
    #ifdef DBG
    printf("Virt: 0x%X\n", page);
    #endif
    boolean test = setPage((void*)(page), (void*)phys->addr, FALSE, TRUE);
    if (!test)
    {
      #ifdef DBG
      printf("Virt: 0x%X\n", page);
      #endif
      freePage((void*)phys->addr, state);
      panic("Setting the page failed dramatically!");
    }
    if (pageDbg)
    {
      printf("Set: Phys: %X\n", getPhysAddr(page));
      printf("Success!\n");
    }
  }
  else if (!PRESENT && !WRITE)
  {
    if (pageDbg)
    {
      panic("Something went wrong!");
    }
    if (!DATA)
    {
      printf("Trying to execute an invalid location in memory\n");
    }
    printf("The attempted address: 0x%X\n", page);
    panic("Reading non existent page");
    // Read the page from image
  }
  else if (PRESENT)
  {
    panic("Accessing illicit content!");
    // Kill process trying to access this page!
  }
  #ifdef DBG
  printf("Err code: %X\n",err);
  #endif
  #ifdef WARN
  printf("WARNING:\tPaging isn't finished yet!\n");
  #endif
  if (pageDbg)
    printf("Return!\n");
}

boolean setPage(void* virtAddr, void* physAddr, boolean ro, boolean usermode)
{
  #ifdef X86
  if (!CHECKALLIGN((unsigned long)virtAddr) || !CHECKALLIGN((unsigned long)physAddr))
  {
    #ifdef DBG
    printf("Argument allignement\n");
    #endif
    #ifdef MODS
    printf("virt: 0X%x\nphys: 0x%X\n", virtAddr, physAddr);
    int idx = 0;
    for (; idx < 0xFFFFFFF; idx++);
    #endif
    return FALSE;
  }
  unsigned long pdIdx = ((unsigned long)virtAddr >> 22);
  unsigned long ptIdx = ((unsigned long)virtAddr >> 12) - (pdIdx << 10);
  
  unsigned long CR3 = (unsigned long)getCR3();
  pageDir_t* pd = (pageDir_t*) (CR3 - (CR3 % PAGESIZE)); // Get the address of the page directory.
  unsigned long ptAddr = (unsigned long)pd[pdIdx].pageIdx*PAGESIZE; // Get the address of the page table.
  if (ptAddr == 0)
  {
    #ifdef DBG
    printf("Illegal Page Table\n");
    #endif
    return FALSE;
  }
  pageTable_t* pt = (pageTable_t*) ptAddr;
  
  pt[ptIdx].pageIdx = (unsigned int)physAddr/PAGESIZE;
  pt[ptIdx].pcd = 0;
  pt[ptIdx].pwt = 0;
  pt[ptIdx].present = 1;
  pt[ptIdx].accessed = 0;
  pt[ptIdx].dirty = 0;
  pt[ptIdx].rw = (ro) ? 0 : 1; // Read only data?
  pt[ptIdx].userMode = (usermode) ? 0 : 1; // Usermode?
  pt[ptIdx].global = 0;
  pt[ptIdx].pat = 0;
  
  pd[pdIdx].present = 1;

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
  printf("CR3\t%u\n",(int)CR3);
  printf("PD\t%u\n",(int)pd);
  printf("PD idx\t%u\n",(int)pdIdx);
  printf("PT\t%u\n",(int)pt);
  printf("PT idx\t%u\n",(int)ptIdx);
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
  memset(pageDir, 0x0, 0x1000);
  int i, j, k;
  for(i = 0; i < PAGETABLES; i++)
  {
    k = 0;
    pageTable_t* pt = alloc(sizeof(pageTable_t)*PAGETABLES, TRUE);
    if (pt == NULL)
    {
      printf("Itteration %X\n", i);
      panic("Aiee, Null pointer!!! PageTable");
    }
    memset(pt, 0, 0x1000);
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
  if (state == COMPRESSED)
    toglePGbit();
}
#endif
