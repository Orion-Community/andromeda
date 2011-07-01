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
#include <kern/cpu.h>

extern int pic;
#define PIC 	0x1
#define APIC	0x2

#define INTBASE     0x20

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
extern void nmi();
extern void breakp();
extern void overflow();
extern void bound();
extern void invalOp();
extern void noMath();
extern void doubleFault();
extern void depricated();
extern void invalidTSS();
extern void snp();
extern void stackFault();
extern void genProt();
extern void pageFault();
extern void fpu();
extern void alligned();
extern void machine();
extern void simd();
#endif