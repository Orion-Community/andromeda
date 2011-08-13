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
#include <mm/map.h>
#include <thread.h>
#include <boot/mboot.h>
#include <stdlib.h>

mutex_t pageLock = 0;

module_t modules[MAX_MODS];
unsigned short bitmap[PAGES];

boolean claimPage(unsigned long page, unsigned short owner)
{
  if (bitmap[page] != FREE || page > PAGES)
  {
    return FALSE;
  }
  bitmap[page] = owner;
  
  return TRUE;
}

pageState_t* allocPage(unsigned short owner)
{
  if (pageLock != 0)
    printf("Lock state: %X\n", (int)pageLock);
  mutexEnter(pageLock);
  unsigned long i;
  pageState_t* addr = kalloc(sizeof(pageState_t));
  if (addr == NULL)
  {
    panic("AIEE, OUT OF MEMORY!\n");
  }
  for (i = 0; i < PAGES; i++)
  {
    if (bitmap[i] == FREE)
    {
      if (claimPage(i, owner))
      {
	addr->addr = i*PAGESIZE;
	addr->usable = TRUE;
	mutexRelease(pageLock);
	return addr;
      }
    }
  }
  addr->addr = 0;
  addr->usable = FALSE;
  mutexRelease(pageLock);
  return addr;
}

void freePage(void* page, unsigned short owner)
{
  if (bitmap[((unsigned long)page>>0xC)] != owner)
  {
    return;
  }
  bitmap[(unsigned long)page] = FREE;
}