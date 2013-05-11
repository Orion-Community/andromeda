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

#ifdef __INTEL
#include <arch/x86/GDT.h>
#include <stdlib.h>
#include <mm/paging.h>

gdtEntry_t GDT[5];

/**
 * The code below holds ifdefs called FAST. This is because the code that's
 * between those statements uses full registers at a time, in stead of the
 * individual bits. The limitations opposed by both pieces of code are the same
 * though.
 *
 * For performance reasons, compile with the FAST flag enabled. We've defaulted
 * not to, because the other piece of code is better for educational use.
 */

#ifdef FAST
void setEntry(int num, unsigned int base, unsigned int limit,
                                      unsigned char access, unsigned char gran);
#else
void setEntry(int num, unsigned int base, unsigned int limit,
                                           unsigned int type, unsigned int dpl);
#endif

#define ENTRIES 5

/// All this does is set the general descriptor table to a flat memory model.
/// For all I know this is only necessary on intel machines as they support
/// segmentation. We'll use paging for our memory protection.

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
  #ifdef FAST
  setEntry(0, 0, 0, 0, 0);                // Null segment
  setEntry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
  setEntry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
  setEntry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
  setEntry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
  #else
  //void setEntry (int num, unsigned int base,
  //unsigned int limit, unsigned int type, unsigned int dpl)
  setEntry (0, 0, 0, 0, 0);
  setEntry (1, 0, 0xFFFFFFFF, 0xA, 0x0);
  setEntry (2, 0, 0xFFFFFFFF, 0x2, 0x0);
  setEntry (3, 0, 0xFFFFFFFF, 0xA, 0x3);
  setEntry (4, 0, 0xFFFFFFFF, 0x2, 0x3);
  #endif

  /*
   * Load the GDT into the CPU.(1)
   */

  struct gdtPtr gdt;
  gdt.limit = sizeof(gdtEntry_t)*ENTRIES;
  gdt.baseAddr = (unsigned int)((void*)GDT);
  #ifdef GDTTEST
  printhex(gdt->limit); putc('\t');
  printhex(gdt->baseAddr); putc('\n');
  printf("checkpoint 1\n");

  int i;
  for (i = 0; i < ENTRIES*2; i++)
  {
    int *out = ((void*)GDT)+i*4;
    printhex(*out); putc('\n');
  }

  #endif
  lgdt(&gdt);
  #ifdef GDTTEST
  printf("checkpoint 2\n");
  #endif
}

#ifdef X86
#ifdef FAST
void setEntry(int num, unsigned int base, unsigned int limit,
                                       unsigned char access, unsigned char gran)
{
   GDT[num].base_low    = (base & 0xFFFFFF);
   GDT[num].base_high   = (base >> 24) & 0xFF;

   GDT[num].limit_low   = (limit & 0xFFFF);
   GDT[num].granularity = (limit >> 16) & 0x0F;

   GDT[num].granularity |= gran & 0xF0;
   GDT[num].access      = access;
}
#else
void setEntry (int num, unsigned int base, unsigned int limit,
                                            unsigned int type, unsigned int dpl)
{
  if (num == 0)
  {
    GDT[0].limit = 0;
    GDT[0].baseLow = 0;
    GDT[0].type = 0;
    GDT[0].s = 0;
    GDT[0].dpl = 0;
    GDT[0].one = 0;
    GDT[0].limitHigh = 0;
    GDT[0].avl = 0;
    GDT[0].zero = 0;
    GDT[0].mode = 0;
    GDT[0].granularity = 0;
    GDT[0].baseHigh = 0;
  }
  else
  {
    limit = limit >> 8;
    GDT[num].baseLow	= (base & 0xFFFFFF);
    GDT[num].baseHigh	= (base >> 24) & 0xFF;
    GDT[num].limit	= (limit & 0xFFFF);
    GDT[num].limitHigh	= (limit >> 16) & 0xF;
    GDT[num].dpl 	= dpl & 0x3;
    GDT[num].type	= type & 0xF;
    GDT[num].zero	= 0;
    GDT[num].one	= 1;
    GDT[num].granularity= 1;
    GDT[num].s		= 1;
    GDT[num].mode	= 1;
  }
}
#endif
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

    For more info on segmentation, read volume 1 of the basic architecture
    documentation by intel.
*/

