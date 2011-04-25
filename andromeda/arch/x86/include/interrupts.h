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

#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include <types.h>

extern int pic;
#define PIC 	0x1
#define APIC	0x2

#define INTBASE     0x20

extern int DetectAPIC();
extern unsigned int getCS();
extern unsigned int getDS();
extern unsigned int getSS();
extern unsigned int getESP();
void prepareIDT();

struct idtEntry
{
   unsigned short base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
   unsigned short sel;                 // Kernel segment selector.
   unsigned char  always0;             // This must always be zero.
   unsigned char  flags;               // More flags. See documentation.
   unsigned short base_hi;             // The upper 16 bits of the address to jump to.
} __attribute__((packed));
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