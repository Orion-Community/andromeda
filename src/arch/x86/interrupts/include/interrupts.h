/*
 *   The interrupts header.
 *   Copyright (C) 2011  Michel Megens
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/stdlib.h>

#ifndef __INTERRUPT_H
#define __INTERRUPT_H

struct idtentry
{
	uint16_t base_low;
	uint16_t sel;
	uint8_t zero;
	uint8_t flags;
	uint16_t base_high;
} __attribute__((packed));
typedef struct idtentry OL_idt_entry_t;

struct idt
{
	uint16_t limit;
	uint32_t baseptr;
} __attribute__((packed));
typedef struct idt OL_idt_t;

struct isrstack
{
	uint16_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t interruptHandler, errorCode;
	uint32_t eip, cs, eflags, proc_esp;
	uint16_t ss;
} __attribute__((packed));
typedef struct isrstack ol_isr_stack;

bool inKernelRing();
extern void setEntry(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
extern void installIDT(OL_idt_t * idt);

/*
 * The exception headers which are found in the idt
 */
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

/*
 * Exception implementation headers
 */
extern void cDivByZero();
extern void cNmi();
extern void cBreakp();
extern void cOverflow();
extern void cBound();
extern void cInvalOp();
extern void cNoMath();
extern void cDoubleFault();
extern void cDepricated();
extern void cInvalidTSS();
extern void cSnp();
extern void cStackFault();
extern void cGenProt();
extern void cPageFault();
extern void cFpu();
extern void cAlligned();
extern void cMachine();
extern void cSimd();

/*
 * Interrupt headers
 */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
extern void irq30();

/*
 * Iterrupt implementation functions
 */
void cIRQ0();
void cIRQ1();
void cIRQ2();
void cIRQ3();
void cIRQ4();
void cIRQ5();
void cIRQ6();
void cIRQ7();
void cIRQ8();
void cIRQ9();
void cIRQ10();
void cIRQ11();
void cIRQ12();
void cIRQ13();
void cIRQ14();
void cIRQ15();
extern void cIRQ30();
#endif
