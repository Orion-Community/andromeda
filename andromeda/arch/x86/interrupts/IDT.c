#include <interrupts.h>
#include <error/panic.h>
#include <mm/memory.h>
#include <types.h>
#include <text.h>

#define SIZE 256

void setIdtGate (idtEntry_t* gate, void* handler, unsigned int selector, boolean trap, unsigned char privilege)
{
  gate->baseLow = (int)handler & 0xFFFF;
  gate->baseHigh = (int)handler >> 0x10 & 0xFFFF;
  gate->IT = trap & 0x1;
  gate->selector = selector & 0xFFFF;
  
  gate->reserved = 0;
  gate->zero = 0;
  gate->three = 3;
  gate->null = 0;
  gate->present = 1;
}

void setGates(idtEntry_t* IDT, int cs)
{
  boolean trap = TRUE;
  setIdtGate(&IDT[0],divByZero, cs, trap, 0);
  setIdtGate(&IDT[2],nmi, cs, trap, 0);
  setIdtGate(&IDT[3],breakp, cs, trap, 0);
  setIdtGate(&IDT[4],overflow, cs, trap, 0);
  setIdtGate(&IDT[5],bound, cs, trap, 0);
  setIdtGate(&IDT[6],invalOp, cs, trap, 0);
  setIdtGate(&IDT[7],noMath, cs, trap, 0);
  setIdtGate(&IDT[8],doubleFault, cs, trap, 0);
  setIdtGate(&IDT[9],depricated, cs, trap, 0);
  setIdtGate(&IDT[10],invalidTSS, cs, trap, 0);
  setIdtGate(&IDT[11],snp, cs, trap, 0);
  setIdtGate(&IDT[12],stackFault, cs, trap, 0);
  setIdtGate(&IDT[13],genProt, cs, trap, 0);
  setIdtGate(&IDT[14],pageFault, cs, trap, 0);
  setIdtGate(&IDT[16],fpu, cs, trap, 0);
  setIdtGate(&IDT[17],alligned, cs, trap, 0);
  setIdtGate(&IDT[18],machine, cs, trap, 0);
  setIdtGate(&IDT[19],simd, cs, trap, 0);
}

setIDT(idt_t *idt, idtEntry_t* table, unsigned int elements)
{
  idt->limit = sizeof(idt_t)*SIZE - 1;
  idt->base  = (int)table;
}

void prepareIDT()
{
  int cs = getCS();
  #ifdef IDTTEST
  printhex(cs); putc('\n');
  printhex((int)cbp); putc('\n');
  #endif
  idtEntry_t* table = kalloc(sizeof(idtEntry_t)*SIZE);
  idt_t* idt = kalloc(sizeof(idt_t));
  setGates(table, cs);
  setIDT(idt, table, SIZE);
  loadIdt(idt);
  panic("Not enough IDT code written yet!");
}