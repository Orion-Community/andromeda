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

extern uint32_t mboot;
extern uint32_t end;

void setupPageDir()
{
  struct pageDir* pd = alloc(sizeof(pd)*PAGEDIRS, TRUE);
  memset(pd, 0, sizeof(pd)*PAGEDIRS);
  
  uint32_t kSize = (uint32_t)&end - (uint32_t)&mboot;
  #ifdef PAGEDBG
  printf("Kern size in bytes: %X\n", kSize);
  #endif
  
  volatile addr_t baseAddr = (addr_t)&mboot % PAGESIZE;
  volatile addr_t end = ((addr_t)&end + (addr_t)HEAPSIZE);
  end += (PAGESIZE-(((addr_t)&end + (addr_t)HEAPSIZE)%PAGESIZE));
  printf("Absolute size in bytes: %X\n", (end - baseAddr));
}

void initPaging()
{
  setupPageDir();
}