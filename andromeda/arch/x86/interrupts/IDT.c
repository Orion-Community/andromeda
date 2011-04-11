#include <interrupts.h>
#include <error/panic.h>
#include <mm/memory.h>
#include <types.h>
#include <text.h>

#define SIZE 256

#ifdef IDTTEST
void printEntry(idtEntry_t* gate)
{
  int *a = ((void*)gate);
  int *b = ((void*)gate)+4;
  putc('\n');
  printhex(*a); putc('\n');
  printhex(*b); putc('\n');
} 
#endif

void setIdtGate (idtEntry_t* gate, int handler, unsigned int selector, boolean trap, unsigned char privilege)
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
  setIdtGate(&IDT[0],(int)divByZero, cs, trap, 0);
  setIdtGate(&IDT[2],(int)nmi, cs, trap, 0);
  setIdtGate(&IDT[3],(int)breakp, cs, trap, 0);
  setIdtGate(&IDT[4],(int)overflow, cs, trap, 0);
  setIdtGate(&IDT[5],(int)bound, cs, trap, 0);
  setIdtGate(&IDT[6],(int)invalOp, cs, trap, 0);
  setIdtGate(&IDT[7],(int)noMath, cs, trap, 0);
  setIdtGate(&IDT[8],(int)doubleFault, cs, trap, 0);
  setIdtGate(&IDT[9],(int)depricated, cs, trap, 0);
  setIdtGate(&IDT[10],(int)invalidTSS, cs, trap, 0);
  setIdtGate(&IDT[11],(int)snp, cs, trap, 0);
  setIdtGate(&IDT[12],(int)stackFault, cs, trap, 0);
  setIdtGate(&IDT[13],(int)genProt, cs, trap, 0);
  setIdtGate(&IDT[14],(int)pageFault, cs, trap, 0);
  setIdtGate(&IDT[16],(int)fpu, cs, trap, 0);
  setIdtGate(&IDT[17],(int)alligned, cs, trap, 0);
  setIdtGate(&IDT[18],(int)machine, cs, trap, 0);
  setIdtGate(&IDT[19],(int)simd, cs, trap, 0);
}

setIDT(idt_t *idt, idtEntry_t* table, unsigned int elements)
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
  idtEntry_t* table = kalloc(sizeof(idtEntry_t)*SIZE);
  idt_t* idt = kalloc(sizeof(idt_t));
  setGates(table, cs);
  setIDT(idt, table, SIZE);
  #ifdef IDTTEST
  printEntry(((void*)&table)+3*sizeof(idtEntry_t));
  #endif
  loadIdt(idt);
  
  cs /= 0;
  
  panic("Not enough IDT code written yet!");
}