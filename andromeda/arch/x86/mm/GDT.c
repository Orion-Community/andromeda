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
 * Ok, I'll be fair with you, I couldn't find any proper documentation on 
 * segmentation, so half of the code you see here is actually nicked
 * from Brans kernel development tutorial.
 * http://www.osdever.net/tutorials/view/brans-kernel-development-tutorial
 */

#ifdef __INTEL
#include <mm/memory.h>
#include <mm/heap.h>
#include <GDT.h>
#include <types.h>

#ifdef GDTTEST
#include <text.h>
#endif
void setEntry(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran);
gdtEntry_t *GDT = NULL;

#define ENTRIES 5

// All this does is set the general descriptor table to a flat memory model.
// For all I know this is only necessary on intel machines as they support
// segmentation. We'll use paging for our memory protection.

void setGDT()
{
  #ifdef GDTTEST
  printf("GDT ptr size\tGDT Entry size\n");
  printhex(sizeof(gdt_t)); printf("\t\t");
  printhex(sizeof(gdtEntry_t)); putc('\n');
  #endif
  
  /*
   * Set up a GDT. (1)
   */
  GDT = alloc(sizeof(gdtEntry_t)*ENTRIES, TRUE);
  setEntry(0, 0, 0, 0, 0);                // Null segment
  setEntry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
  setEntry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
  setEntry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
  setEntry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
  
  /*
   * Load the GDT into the CPU.(1)
   */
  
  gdt_t *gdt = kalloc (sizeof(gdt_t));
  gdt->limit = sizeof(gdtEntry_t)*ENTRIES;
  gdt->baseAddr = (unsigned int)((void*)GDT);
  #ifdef GDTTEST
  printhex(gdt->limit); putc('\t');
  printhex(gdt->baseAddr); putc('\n');
  printf("checkpoint 1\n");
  #endif
  lgdt(gdt);
  #ifdef GDTTEST
  printf("checkpoint 2\n");
  #endif
}

#ifdef X86
void setEntry(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran)
{
   GDT[num].base_low    = (base & 0xFFFFFF);
   GDT[num].base_high   = (base >> 24) & 0xFF;

   GDT[num].limit_low   = (limit & 0xFFFF);
   GDT[num].granularity = (limit >> 16) & 0x0F;

   GDT[num].granularity |= gran & 0xF0;
   GDT[num].access      = access;
}
#endif

#endif
/*
(1) A GDT or general descriptor table is a table that holds the segment data.
    To get to know more about the data, see GDT.h.
    Whenever an application does a memory reference it is passed by the GDT to
    figure out to which address the address given should be mapped.
    The addresses on the intel machine are built up out of segment registers
    and the pointer itself.
    
    The segment registers hold an index into the GDT.
    
    For more infon on segmentation, read volume 1 of the basic architevture
    documentation by intel.
*/