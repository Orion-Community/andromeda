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
#include <interrupts.h>
#include <irq.h>
#include <stdlib.h>

#define SIZE 256

int pic = 0;
extern memNode_t* blocks;

idtEntry_t* table;
void setIdtGate(unsigned char num, unsigned int base, unsigned short sel, unsigned char flags)
{
    table[num].base_lo = base & 0xFFFF;
    table[num].base_hi = (base >> 16) & 0xFFFF;
    table[num].sel     = sel;
    table[num].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    table[num].flags   = flags /* | 0x60 */;
}

void setExceptions()
{
  setIdtGate( 0, (unsigned int)divByZero , 0x08, 0x8E);
  setIdtGate( 1, (unsigned int)depricated, 0x08, 0x8E);
  setIdtGate( 2, (unsigned int)nmi , 0x08, 0x8E);
  setIdtGate( 3, (unsigned int)breakp , 0x08, 0x8E);
  setIdtGate( 4, (unsigned int)overflow , 0x08, 0x8E);
  setIdtGate( 5, (unsigned int)bound , 0x08, 0x8E);
  setIdtGate( 6, (unsigned int)invalOp , 0x08, 0x8E);
  setIdtGate( 7, (unsigned int)noMath , 0x08, 0x8E);
  setIdtGate( 8, (unsigned int)doubleFault , 0x08, 0x8E);
  setIdtGate( 9, (unsigned int)depricated , 0x08, 0x8E);
  setIdtGate(10, (unsigned int)invalidTSS, 0x08, 0x8E);
  setIdtGate(11, (unsigned int)snp, 0x08, 0x8E);
  setIdtGate(12, (unsigned int)stackFault, 0x08, 0x8E);
  setIdtGate(13, (unsigned int)genProt, 0x08, 0x8E);
  setIdtGate(14, (unsigned int)pageFault, 0x08, 0x8E);
  setIdtGate(15, (unsigned int)depricated, 0x08, 0x8E);
  setIdtGate(16, (unsigned int)fpu, 0x08, 0x8E);
  setIdtGate(17, (unsigned int)alligned, 0x08, 0x8E);
  setIdtGate(18, (unsigned int)machine, 0x08, 0x8E);
  setIdtGate(19, (unsigned int)simd, 0x08, 0x8E);
}
#ifndef __COMPRESSED
void setIRQ(int offset1, int offset2)
{
  setIdtGate(offset1+0, (unsigned int)irq0 , 0x08, 0x8E);
  setIdtGate(offset1+1, (unsigned int)irq1 , 0x08, 0x8E);
  setIdtGate(offset1+2, (unsigned int)irq2 , 0x08, 0x8E);
  setIdtGate(offset1+3, (unsigned int)irq3 , 0x08, 0x8E);
  setIdtGate(offset1+4, (unsigned int)irq4 , 0x08, 0x8E);
  setIdtGate(offset1+5, (unsigned int)irq5 , 0x08, 0x8E);
  setIdtGate(offset1+6, (unsigned int)irq6 , 0x08, 0x8E);
  setIdtGate(offset1+7, (unsigned int)irq7 , 0x08, 0x8E);
  setIdtGate(offset1+0, (unsigned int)irq8 , 0x08, 0x8E);
  setIdtGate(offset2+1, (unsigned int)irq9 , 0x08, 0x8E);
  setIdtGate(offset2+2, (unsigned int)irq10, 0x08, 0x8E);
  setIdtGate(offset2+3, (unsigned int)irq11, 0x08, 0x8E);
  setIdtGate(offset2+4, (unsigned int)irq12, 0x08, 0x8E);
  setIdtGate(offset2+5, (unsigned int)irq13, 0x08, 0x8E);
  setIdtGate(offset2+6, (unsigned int)irq14, 0x08, 0x8E);
  setIdtGate(offset2+7, (unsigned int)irq15, 0x08, 0x8E);
}
#endif

void setIDT(idt_t *idt, idtEntry_t* table, unsigned int elements)
{
  idt->limit = sizeof(idtEntry_t)*SIZE;
  idt->base  = (int)table;
}

char* testStr = "Checkpoint\n";

void prepareIDT()
{
  int cs = getCS();
  #ifdef IDTTEST
  printhex(cs); putc('\n');
  printhex(getDS()); putc('\n');
  #endif
  table = kalloc(sizeof(idtEntry_t)*SIZE);
  idt_t* idt = kalloc(sizeof(idt_t));
  if (table == NULL || idt == NULL)
  {
     panic ("Aiee, NULL pointer!!!");
  }
  setExceptions();
  #ifndef __COMPRESSED
  setIRQ(INTBASE, INTBASE+8);
  #endif
  setIDT(idt, table, SIZE);
  loadIdt(idt);
  #ifdef WARN
  printf("WARNING: Exceptions need a better implementation!\n");
  #endif
}