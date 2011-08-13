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
#include <fs/fs.h>
#include <kern/sched.h>
#ifdef GRAPHICS
#include "../drivers/graphics/Include/VGA.h"
//#include "../drivers/graphics/Include/graphics.h"
#endif
#ifdef BEEP
#include "../drivers/system beep/Include/beep.h"
#endif

// Heap of 256 MiB
#define HEAPSIZE 0x10000000

unsigned char stack[0x10000];

int init(unsigned short memorymap[], module_t mods[])
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
  // Let's create our own page tables and directory
  corePaging();
  
  // Set the CPU up so that it no longer requires the nano image
  setGDT();
  // Set up the filesystem
  #ifndef NOFS
    fsInit(NULL);
  #endif

  #ifdef BEEP
    printf("Beep...");
    beep();
    printf("Done\n");
  #endif
  
  #ifdef GRAPHICS
    if (!vgaInit())
      panic("Initizing VGA driver failt!");
    /**
     * Test vga driver...
     */
    imageBuffer img = newImageBuffer(32,32);         // Create a new image buffer (25 x 20 pixels)
    int i = 0;
    for (;i<32;i++)
    {
      memset((void*)((int)img.buffer+i*32),(i+1),i); // Make a figure.
    }
    drawBuffer(img,64,64);                           // Draw the buffer at [64,64]!
    drawBufferPart(img,46,64,16,32,0,0);             // Draw buffer from [0,0] to [16,32] at [46,64] !
  #endif
  
  // In the future this will do a little more
  printf("You can now shutdown your PC\n");
  for (;;) // Infinite loop, to make the kernel schedule when there is nothing to do
  {
    halt();
  }
}
