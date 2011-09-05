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
#include <tty/tty.h>
#include <fs/fs.h>

#include <kern/cpu.h>

unsigned char stack[0x8000];

// Define the place of the heap

void testMMap(multiboot_info_t* hdr);

multiboot_memory_map_t* mmap;
size_t mmap_size;

#define HEAPSIZE 0x1000000

int vendor = 0;

// Print a welcome message
void announce()
{
//   textInit();
  println("Compressed kernel loaded");
  println("Decompressing the kernel");
}

boolean setupCore(module_t mod)
{
  // Examine and augment the elf image here, return true if faulty
  switch(coreCheck((void*)mod.addr))
  {
    case 0:
      break;
    case -1:
      printf("Invalid elf image\n");
      return TRUE;
    case -2:
      printf("Entry point too low\n");
      return TRUE;
    case -3:
      printf("Kernel magic invalid\n");
      return TRUE;;
    default:
      printf("Unknown return value");
      return TRUE;
  }
  coreAugment(mod.addr);
  
  // Jump into the high memory image
  elfJmp(mod.addr);
  
  return FALSE; //Doesn't get reached, ever, if all goes well
}

// The main function
int init(unsigned long magic, multiboot_info_t* hdr)
{
  textInit();
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
  
//   if (hdr->flags && MULTIBOOT_INFO_MODS && hdr->mods_count > 0)
//   {
//     addModules((multiboot_module_t*)hdr->mods_addr, (int)hdr->mods_count);
//     addCompressed();
//   }
//   else
//   {
//     panic("Invalid modules");
//   }


  setGDT();
  
  // Initialise the heap
  initHeap(HEAPSIZE);
  
  intInit(); 	     // Interrupts are allowed again.
		     // Up untill this point they have
		     // been disabled.
  
  // If in the compressed image
  announce(); // print welcome message
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
  
  #ifdef MMTEST
  testAlloc();
  printf("End test\n");
  #endif

  fsInit(NULL);
  list(_fs_root);
  
//   #ifndef TESTING
//   if (setupCore(modules[0]))
//   {
//     panic("Core image couldn't be loaded!");
//   }
//   #endif

  printf("You can now shutdown your PC\n");
  for (;;) // Infinite loop, to make the kernel wait when there is nothing to do
  {
     halt();
  }
  return 0; // To keep the compiler happy.
}
