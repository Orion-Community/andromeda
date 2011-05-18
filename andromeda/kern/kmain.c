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
 * This is the main function for both the compressed and decompressed image.
 * The compressed image is nothing more than a basic kernel with image loading
 * code, to load the decompressed image. An approach still has to be chosen on
 * how to load the decompressed image.
 *
 * One angle is to load the decompressed image in from disk.
 * Another is to get a module from GRUB which we can put into place our selves.
 * The latter is the most gracefull, however, the former will suffice and is
 * probably a whole lot easier.
 */

// Basic includes
#include <stdlib.h>
#include <kern/cpu.h>
#include <mm/paging.h>
#include <interrupts/int.h>

unsigned char stack[0x2000];

// Define the place of the heap
#ifdef __COMPRESSED

#include <boot/mboot.h>
#include <mm/map.h>

multiboot_memory_map_t* mmap;
size_t mmap_size;

#define HEAPSIZE 0x1000000

#else

#define HEAP 0xE0000000
#define HEAPSIZE 10000000

#endif

#include <kern/cpu.h>

int vendor = 0;

// Print a welcome message
void announce()
{
//   textInit();
  println("Compressed kernel loaded");
  println("Decompressing the kernel");
}

// The main function
#ifdef __COMPRESSED
int kmain(unsigned long magic, multiboot_info_t* hdr)
#else
int kmain()
#endif
{
  textInit();
  #ifdef __COMPRESSED
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    printf("\nInvalid magic word: ");
    printhex(magic);
    putc('\n');
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
  if (hdr->flags && MULTIBOOT_INFO_MODS)
  {
    addModules((multiboot_module_t*)hdr->mods_addr, (int)hdr->mods_count);
    addCompressed();
  }
  else
  {
    panic("Invalid modules");
  }
  #endif
  
  #ifdef DBG
  printf("Addr of stackbase: "); printhex((int)&stack); putc('\n');
  #endif
  
  // Initialise the heap
  initHeap(HEAPSIZE);
  intInit(); 	     // Interrupts are allowed again.
		     // Up untill this point they have
		     // been disabled.
		     
  // If in the compressed image
  #ifdef __COMPRESSED
  announce(); // print welcome message
  #endif
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

  #ifdef __COMPRESSED
  #ifdef DBG1
  if (hdr->flags && MULTIBOOT_INFO_MEM_MAP)
  {
    printf ("mmap_addr = "); printhex((unsigned) hdr->mmap_addr);
    printf(", mmap_length = ");printhex((unsigned) hdr->mmap_length); putc('\n');
    for (mmap = (multiboot_memory_map_t *) hdr->mmap_addr;
             (unsigned long) mmap < hdr->mmap_addr + hdr->mmap_length;
             mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
                                      + mmap->size + sizeof (mmap->size)))
    {
      printf (" size = ");
      printhex((unsigned) mmap->size);
      printf(", base_addr = ");
      printhex(mmap->addr >> 32);
      printhex(mmap->addr & 0xffffffff);
      printf (" length = ");
      printhex(mmap->len >> 32);
      printhex(mmap->len & 0xffffffff);
      printf(", type = ");
      printhex((unsigned) mmap->type);
      putc('\n');
    }
    printf("End pointer = "); printhex((int)&end); putc('\n');
  }
       
  if (hdr->flags && MULTIBOOT_INFO_MODS)
  {
    int mods = hdr->mods_count;
    multiboot_module_t* modules = (multiboot_module_t*)hdr->mods_addr;
    printf("No. modules: "); printhex(mods); putc('\n');
    int i = 0;
    for (; i < mods; i++)
    {
      printf("Base addr = "); printhex(modules[i].mod_start); putc('\n');
      printf("End  addr = "); printhex(modules[i].mod_end);   putc('\n');
      printf("CMD  line = "); printf((char*)modules[i].cmdline); putc('\n');
    }
  }
  else
  {
    printf("No modules found!\n");
  }
  #endif
  #endif
  
  int *a = kalloc(sizeof(int));
  printf("Phys addr of: %x = %x\n", (int)a, (int)getPhysAddr(a));
  free(a);
  
  #ifdef MMTEST
  testAlloc();
  printf("End test\n");
  #endif
  #ifdef __COMPRESSED
  //exec(coreImg);
  #endif
  for (;;) // Infinite loop, to make the kernel schedule when there is nothing to do
  {
    // If this loop gets reached more than once:
    // asm volatile ("int3");
    printf("You can now shutdown your PC\n");
    halt();
  }
  return 0; // To keep the compiler happy.
}
