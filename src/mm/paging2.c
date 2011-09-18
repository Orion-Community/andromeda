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
#include <mm/heap.h>
#include <error/error.h>

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

extern volatile mutex_t pageLock;
boolean pageDbg = false;

void cPageFault(isrVal_t registers)
{
  unsigned long page = getCR2();
  #ifdef PAGEDBG
  printf("PG\n");
  #endif
  
  if (registers.cs != 0x8 || registers.cs != 0x18)
  {
    panic("Incorrect frame!");
  }
  #ifdef PAGEDBG
  printf("Type of error: 0x%X\n", registers.errCode);
  #endif

  panic("Page faults currently under construction");
}

uint16_t page_map[PAGETABLES];

int mapPage(addr_t virtual, addr_t physical, struct pageDir *pd, 
                                                               boolean userMode)
{
  while(mutexTest(pageLock))
  {
    #ifdef PAGEDBG
    printf("Paging is locked!\n");
    #endif
  }
  struct pageTable* pt;
  addr_t pd_entry = virtual >> 22;
  addr_t pt_entry = (virtual >> 12) & 0x3FF;
  
  if (pd[pd_entry].present == FALSE)
  {
    pt = alloc(sizeof(pt)*PAGETABLES, TRUE);
    if (pt == NULL)
    {
      mutexRelease(pageLock);
      return -E_NOMEM;
    }
    memset(pt, 0, sizeof(pt)*PAGETABLES);
    pd[pd_entry].pageIdx  = ((addr_t)pt)/PAGESIZE;
    pd[pd_entry].present  = TRUE;
    pd[pd_entry].rw       = TRUE;
    pd[pd_entry].userMode = userMode;
  }
  else
  {
    if (pd[pd_entry].userMode == FALSE)
      pd[pd_entry].userMode = userMode;
    
    pt = (struct pageTable*)(pd[pd_entry].pageIdx*PAGESIZE);
    if (pt == NULL)
    {
      mutexRelease(pageLock);
      return -E_PAGE_MAPPING;
    }
  }
  if (pt[pt_entry].present != FALSE)
  {
    mutexRelease(pageLock);
    return -E_PAGE_MAPPING;
  }
  
  pt[pt_entry].pageIdx  = physical >> 0x12;
  pt[pt_entry].present  = TRUE;
  pt[pt_entry].rw       = TRUE;
  pt[pt_entry].userMode = userMode;
  
  page_map[pd_entry]++;
  mutexRelease(pageLock);
  return -E_SUCCESS;
}

int releasePage(addr_t virtual, struct pageDir *pd)
{
  while (mutexTest(pageLock))
  {
    #ifdef PAGEDBG
    printf("Paging is locked!\n");
    #endif
  }
  addr_t pd_entry = virtual >> 22;
  addr_t pt_entry = virtual >> 12;
  
  if (pd[pd_entry].present == FALSE)
  {
    mutexRelease(pageLock);
    return -E_PAGE_NOPAGE;
  }
  
  struct pageTable *pt = (struct pageTable*)(pd[pd_entry].pageIdx*PAGESIZE);
  
  if (pt[pt_entry].present == FALSE)
  {
    mutexRelease(pageLock);
    return -E_PAGE_NOPAGE;
  }
  
  pt[pt_entry].present = FALSE;
  pt[pt_entry].pageIdx = 0;
  
  page_map[pd_entry]--;
  if (page_map[pd_entry] == 0)
  {
    pd[pd_entry].present = FALSE;
    free((void*)(pd[pd_entry].pageIdx*PAGESIZE));
    pd[pd_entry].pageIdx = 0;
  }
  mutexRelease(pageLock);
}

int setupPageDir()
{
  struct pageDir *pd = alloc(sizeof(pd)*PAGEDIRS, TRUE);
  if (pd == NULL)
    return -E_NOMEM;
  memset(pd, 0, sizeof(pd)*PAGEDIRS);
  
  uint32_t kSize = (uint32_t)&end - (uint32_t)&mboot;
  #ifdef PAGEDBG
  printf("Kern size in bytes: %X\n", kSize);
  #endif
  
  volatile addr_t baseAddr = (addr_t)&mboot % PAGESIZE;
  volatile addr_t end = ((addr_t)&end + (addr_t)HEAPSIZE);
  end += (PAGESIZE-(((addr_t)&end + (addr_t)HEAPSIZE)%PAGESIZE));
  #ifdef PAGEDBG
  printf("Absolute size in bytes: %X\n", (end - baseAddr));
  #endif
  return pd;
}

void initPaging()
{
  memset(page_map, 0, PAGETABLES*2); /* While the map is of 16 bits entries the
                                                      2 must remain in place!!*/
  setCR3(setupPageDir());
/*
  setPGBit();
*/
}