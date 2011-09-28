/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven, Michel Megens

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

/**
 * This file holds the entry point to the C level kernel. What's done here is
 * the basic intialisation of for example:
 * - Paging,
 * - ACPI and
 * - Hardware abstraction.
 *
 * The init function also displays the welcome message.
 */

// Basic includes
#include <stdlib.h>
#include <unistd.h>
#include <kern/cpu.h>
#include <kern/elf.h>
#include <mm/paging.h>
#include <mm/map.h>
#include <interrupts/int.h>
#include <arch/x86/idt.h>
#include <boot/mboot.h>
#include <mm/map.h>
#include <tty/tty.h>
#include <fs/fs.h>
#include <arch/x86/pic.h>

#include <sys/dev/ps2.h>
#include <sys/dev/pci.h>

#include <arch/x86/cpu.h>
#include <arch/x86/apic/apic.h>
#include <arch/x86/acpi/acpi.h>

#include <kern/cpu.h>

#include <arch/x86/apic/ioapic.h>

unsigned char stack[0x8000];

// Define the place of the heap

void testMMap(multiboot_info_t* hdr);

multiboot_memory_map_t* mmap;
size_t mmap_size;

char *welcome = "Andromeda 0.0.4 - Copyright (C) 2010, 2011 - Michel Megens, \
Bart Kuivenhoven\nThis program comes with ABSOLUTELY NO WARRANTY;\n\
This is free software, and you are welcome to redistribute it.\n\
For more info refer to the COPYING file in the source repository or look at\n\
http://www.gnu.org/licenses/gpl-3.0.html\n";

int vendor = 0;

boolean setupCore(module_t mod)
{
  // Examine and augment the elf image here, return true if faulty
  switch (coreCheck((void*) mod.addr))
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
      return TRUE;
      ;
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
  printf("%s\n", welcome);
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    printf("\nInvalid magic word: %X\n", magic);
    panic("");
  }
  if (hdr->flags & MULTIBOOT_INFO_MEMORY)
  {
    memsize = hdr->mem_upper;
    memsize += 1024;
  }
  else
  {
    panic("No memory flags!");
  }
  if (hdr->flags & MULTIBOOT_INFO_MEM_MAP)
  {
    mmap = (multiboot_memory_map_t*) hdr->mmap_addr;
    build_map(mmap, (unsigned int) hdr->mmap_length);
  }
  else
  {
    panic("Invalid memory map");
  }
  

  setGDT();

  // Initialise the heap
  initHeap(HEAPSIZE);
  printf("Size of the heap: 0x%x\tStarting at: %x\n", HEAPSIZE,&end);
  ol_cpu_t cpu = kalloc(sizeof (*cpu));
  ol_cpu_init(cpu);
  ol_get_system_tables();

  pic_init(); 
  setIDT();
  ol_ps2_init_keyboard();
  init_ioapic();

  ol_pci_init();
#ifndef NOFS
  fsInit(NULL);
#ifdef FSTEST
  list(_fs_root);
#endif
#endif

#ifdef __MEMTEST
	ol_detach_all_devices(); /* free's al the pci devices */
	free(cpu);
#endif
#ifdef __DBG_HEAP
  ol_dbg_heap();
#endif
  
  printf("\nSome (temp) debug info:\n");
  printf("%s\n", cpus[0].vendor);
  printf("RSDP ASCII signature: 0x%x%x\n",*(((uint32_t*) rsdp->signature) + 1),
          *(((uint32_t*) rsdp->signature)));

  printf("You can now shutdown your PC\n");
  for (;;) // Infinite loop, to make the kernel wait when there is nothing to do
  {
    halt();
  }
  return 0; // To keep the compiler happy.
}
