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

boolean pageDbg = false;

struct pageDir pd[0x400];
struct pageTable pt[0x400][0x400];

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
  #ifdef PAGDBG
  printf("Type of error: 0x%X\n", registers.errCode);
  #endif

  panic("Page faults currently under construction");
}

void setupPageDir()
{
  panic("Paging currently under construction");
}

void initPaging()
{
  setupPageDir();
}