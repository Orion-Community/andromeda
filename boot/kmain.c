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

// Heap of undetermined size
#define HEAPSIZE 0x0

unsigned char stack[0x10000];

int core()
{
  initHeap(HEAPSIZE, FALSE);
  textInit();
  printf("Success!\n");
  
  intInit();
  setGDT();
  
  
  
  // In the future this will do a little more
  for (;;) // Infinite loop, to make the kernel schedule when there is nothing to do
  {
    halt();
  }
  printf("You can now shutdown your PC\n");
}