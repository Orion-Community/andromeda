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
struct idtentry
{
	uint16_t base_low;
	uint16_t sel;
	uint8_t zero;
	uint8_t flags;
	uint16_t base_high;
} __attribute__((packed));
typedef struct idtentry gebl_idt_entry_t;

struct idt
{
	uint16_t limit;
	uint32_t baseptr;
} __attribute__((packed));
typedef struct idt gebl_idt_t;

struct isrstack
{
	uint16_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t interruptHandler, errorCode;
	uint32_t eip, cs, eflags, proc_esp;
	uint16_t ss;
} __attribute__((packed));
typedef struct isrstack gebl_isr_stack;

extern void setEntry(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
extern void installIDT(gebl_idt_t * idt);

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
#endif