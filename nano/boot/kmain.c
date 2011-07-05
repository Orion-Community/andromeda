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

/*
 * This is the main function for the nano kernel.
 * What this thing does is interpret the arguments handed to us by the bootloader.
 * Next it will load in the module received from the bootloader.
 * This module is actually the main kernel with all the drivers and
 * the less basic tasks, along with a copy of this kernel, all situated
 * at 3 GiB. This is always possible due to the virtual memory possibilities
 * opened up to us by paged memory.
 *
 * This kernel will produce:
 * - A memory map
 * - A paging system to enable virtual memory
 * - A piece of the VGA driver to enable writing text to screen.
 */

// Basic includes
#include <stdlib.h>
#include <unistd.h>
#include <kern/cpu.h>
#include <kern/elf.h>
#include <mm/paging.h>
#include <mm/map.h>
#include <interrupts/int.h>
#include <boot/mboot.h>
#include <mm/map.h>

#include <kern/cpu.h>

unsigned char stack[0x8000];

// Define the place of the heap

void testMMap(multiboot_info_t* hdr);

multiboot_memory_map_t* mmap;
size_t mmap_size;

#define HEAPSIZE 0x1000000


/* Uncompressed

#define HEAP 0xE0000000
#define HEAPSIZE 10000000

*/

int vendor = 0;

// Print a welcome message
void announce()
{
//   textInit();
  println("Compressed kernel loaded");
  println("Decompressing the kernel");
}

// The main function
int kmain(unsigned long magic, multiboot_info_t* hdr)
{
  textInit();
  //#ifdef __COMPRESSED
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    printf("\nInvalid magic word: %X\n", magic);
    panic("");
  }
  if (hdr->flags && MULTIBOOT_INFO_MEM_MAP)
  {
    mmap = (multiboot_memory_map_t*)hdr->mmap_addr;
    buildMap(mmap, (int)hdr->mmap_length);
  }
  else
  {
    panic("Invalid memory map");
  }
  if (hdr->flags && MULTIBOOT_INFO_MODS && hdr->mods_count > 0)
  {
    addModules((multiboot_module_t*)hdr->mods_addr, (int)hdr->mods_count);
    addCompressed();
  }
  else
  {
    panic("Invalid modules");
  }
  //#endif
  
  #ifdef DBG
  printf("Addr of stackbase: "); printhex((int)&stack); putc('\n');
  #endif
  
  //#ifdef __COMPRESSED
  #ifdef DBG
  testMMap(hdr);/*
  int i;
  for (i = 0; i<0x1FFFFFFF; i++);*/
  #endif
  //#endif
  
  // Initialise the heap
  initHeap(HEAPSIZE);
  prepareIDT();
  //intInit(); 	     // Interrupts are allowed again.
		     // Up untill this point they have
		     // been disabled.
  // If in the compressed image
  //#ifdef __COMPRESSED
  announce(); // print welcome message
  //#endif
  #ifdef __INTEL
  // Intel specific function
  setGDT();  // Also in decompressed kernel as the compressed image could be overwritten

  #ifdef VENDORTELL
  switch(getVendor())
  {
    case VENDOR_INTEL:
      printf("You're using a Genuine Intel\n");
      break;
    case VENDOR_AMD:
      printf("You're using an authentic AMD\n");
      break;
    default:
      printf("You're using a system not officially supported\n");
  }
  #endif
  #endif
  
  #ifdef MMTEST
  wait();
  #endif
  
  #ifdef DBG
  int *a = kalloc(sizeof(int));
  printf("Phys addr of: %x = %x\n", (int)a, (int)getPhysAddr(a));
  free(a);
  a = (int*)0xC0000000;
  *a = 0xDEADBEEF;
  #endif
  #ifdef MODS
  if(elfExec((void*)modules[0].addr) == 1)
  {
    printf("ELF success!\n");
  }
  else
  {
    printf("ELF fail!!!!\n");
  }
  #endif
  
  #ifdef MMTEST
  testAlloc();
  printf("End test\n");
  #endif
  for (;;) // Infinite loop, to make the kernel schedule when there is nothing to do
  {
    printf("\nTest int(2874):%i\nTest doube (3.25):%d\n\nYou can now shutdown your PC\n",2874,3.25);
    halt();
  }
  return 0; // To keep the compiler happy.
}
