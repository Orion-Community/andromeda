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

void setGates(idtEntry_t* IDT, int cs)
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
  table = kalloc(sizeof(idtEntry_t)*SIZE);
  idt_t* idt = kalloc(sizeof(idt_t));
  setGates(table, cs);
  setIDT(idt, table, SIZE);
  #ifdef IDTTEST
  printEntry(&table[4]);
  #endif
  loadIdt(idt);
  
  panic("Not enough IDT code written yet!");
}