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
#include <tty/tty.h>
#include <mm/map.h>

// Heap of 256 MiB
#define HEAPSIZE 0x10000000

unsigned char stack[0x10000];

int core(unsigned short memorymap[], module_t mods[])
{
  // Install all the necessary data for complex memory management
  memcpy(bitmap, memorymap, PAGES);
  memcpy(modules, mods, MAX_MODS);
  
  // Set up the new screen
  textInit();
  // Install a new heap at the right location.
  heapStub();
  extendHeap(&end, HEAPSIZE);
  
  // Set up the new interrupts
  intInit();
  
  corePaging();
  
  // Set the CPU up so that it no longer requires the nano image
  setGDT();
  
  // Sinus test
  double d = sin(0.5);
  printf("Sin(0.5) = %d\n",d);
  printf("Sin(0.5) = %d\n",sin(0.5)); // Somehow this doesn't work. Gives a varity of weird error, witch can change every compilation (even if you don't edit this rule, try comment the 2 rule before...) 
  
  // In the future this will do a little more
  printf("You can now shutdown your PC\n");

  for (;;) // Infinite loop, to make the kernel schedule when there is nothing to do
  {
    halt();
  }
}
