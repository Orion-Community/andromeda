#include <interrupts.h>
#include <error/panic.h>
#include <mm/memory.h>
#include <types.h>
#include <text.h>

idtEntry_t* IDT;

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

void setGates(int cs)
{
  
  panic("Two interrupts missing");
  
  setIdtGate(&IDT[0],divByZero, cs, TRUE, 0);
  setIdtGate(&IDT[2],nmi, cs, TRUE, 0);
  setIdtGate(&IDT[3],breakp, cs, TRUE, 0);
  setIdtGate(&IDT[4],overflow, cs, TRUE, 0);
  setIdtGate(&IDT[5],bound, cs, TRUE, 0);
  setIdtGate(&IDT[6],invalOp, cs, TRUE, 0);
  setIdtGate(&IDT[7],noMath, cs, TRUE, 0);
  setIdtGate(&IDT[8],doubleFault, cs, TRUE, 0);
  setIdtGate(&IDT[9],depricated, cs, TRUE, 0);
  setIdtGate(&IDT[10],invalidTSS, cs, TRUE, 0);
  // Segment Not present should go here
  // Stack Segment Fault should go here
  setIdtGate(&IDT[13],genProt, cs, TRUE, 0);
  setIdtGate(&IDT[14],pageFault, cs, TRUE, 0);
  setIdtGate(&IDT[16],fpu, cs, TRUE, 0);
  setIdtGate(&IDT[17],alligned, cs, TRUE, 0);
  setIdtGate(&IDT[18],machine, cs, TRUE, 0);
  setIdtGate(&IDT[19],simd, cs, TRUE, 0);
}

void prepareIDT()
{
  int cs = getCS();
  printhex(cs); putc('\n');
  IDT = kalloc(sizeof(idtEntry_t)*255);
  setGates(cs);
  panic("Not enough IDT code written yet!");
}