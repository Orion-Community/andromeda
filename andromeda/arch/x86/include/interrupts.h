#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include <types.h>

extern int DetectAPIC();
extern unsigned int getCS();
void prepareIDT();

struct idtEntry
{
  unsigned int baseLow : 16;	// Lower address of exception/interrupt handler
  unsigned int selector : 16;	// Segment selector
  unsigned int reserved : 5; 	// Just put this to 0
  unsigned int zero : 3;	// Defined as 0
  unsigned int IT : 1;		// 0 for interrupt 1 for trap
  unsigned int three : 2;	// fill with max size (which is three)
  unsigned int size : 1;	// 1 for 32-bits, else 16
  unsigned int null : 1;	// put to 0
  unsigned int dpl : 2;		// Indicates the privilege ring
  unsigned int present : 1;	// Page is present (always 1)
  unsigned int baseHigh : 16;	// Higher address of exception/interrupt handler
}__attribute__((packed));
typedef struct idtEntry idtEntry_t;

struct idt
{
  unsigned int limit : 16;
  unsigned int base  : 32;
}__attribute__((packed));
typedef struct idt idt_t;
extern void loadIdt(idt_t*);

extern void divByZero();
void cDivByZero(isrVal_t);

extern void nmi();
void cNmi(isrVal_t);

extern void breakp();
void cbp(isrVal_t);

extern void overflow();
void coverflow(isrVal_t);

extern void bound();
void cBound(isrVal_t);

extern void invalOp();
void cInvalOp(isrVal_t);

extern void noMath();
void cNoMath(isrVal_t);

extern void doubleFault();
void cDoubleFault(isrVal_t);

extern void depricated();
void ignore(isrVal_t);

extern void invalidTSS();
void cInvalidTSS(isrVal_t);

extern void snp();
void cSnp(isrVal_t);

extern void stackFault();
void cStackFault(isrVal_t);

extern void genProt();
void cGenProt(isrVal_t);

extern void pageFault();
void cPageFault(isrVal_t);

extern void fpu();
void cFpu(isrVal_t);

extern void alligned();
void cAlligned(isrVal_t);

extern void machine();

extern void simd();
void cSimd(isrVal_t);
#endif