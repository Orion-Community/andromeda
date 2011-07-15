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

#include <mm/memory.h>
#include <thread.h>
#include <stdlib.h>

#define STUBSIZE 0x4000

char stub[STUBSIZE];

void heapStub()
{
  printf ("Reached\n");
  int i = 0;
  for (; i < 0x1fffffff; i++);
  mutexEnter(prot);
  memNode_t* node = (memNode_t*)stub;
  initHdr(node, STUBSIZE-sizeof(memNode_t));
  blocks=node;
  mutexRelease(prot);
}