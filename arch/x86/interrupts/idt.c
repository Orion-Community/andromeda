/*
 *   The idt tables are set here.
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
#include "include/interrupts.h"

void installExceptions()
{
	setEntry( 0, (uint32_t)divByZero , 0x08, 0x8E);
	setEntry( 1, (uint32_t)depricated, 0x08, 0x8E);
	setEntry( 2, (uint32_t)nmi , 0x08, 0x8E);
	setEntry( 3, (uint32_t)breakp , 0x08, 0x8E);
	setEntry( 4, (uint32_t)overflow , 0x08, 0x8E);
	setEntry( 5, (uint32_t)bound , 0x08, 0x8E);
	setEntry( 6, (uint32_t)invalOp , 0x08, 0x8E);
	setEntry( 7, (uint32_t)noMath , 0x08, 0x8E);
	setEntry( 8, (uint32_t)doubleFault , 0x08, 0x8E);
	setEntry( 9, (uint32_t)depricated , 0x08, 0x8E);
	setEntry(10, (uint32_t)invalidTSS, 0x08, 0x8E);
	setEntry(11, (uint32_t)snp, 0x08, 0x8E);
	setEntry(12, (uint32_t)stackFault, 0x08, 0x8E);
	setEntry(13, (uint32_t)genProt, 0x08, 0x8E);
	setEntry(14, (uint32_t)pageFault, 0x08, 0x8E);
	setEntry(15, (uint32_t)depricated, 0x08, 0x8E);
	setEntry(16, (uint32_t)fpu, 0x08, 0x8E);
	setEntry(17, (uint32_t)alligned, 0x08, 0x8E);
	setEntry(18, (uint32_t)machine, 0x08, 0x8E);
	setEntry(19, (uint32_t)simd, 0x08, 0x8E);
}

void installInterrupts(int offset1, int offset2)
{
	setEntry(offset1+0, (uint32_t)irq0, 0x08, 0x8e);
	setEntry(offset1+1, (uint32_t)irq1, 0x08, 0x8e);
	setEntry(offset1+2, (uint32_t)irq2, 0x08, 0x8e);
	setEntry(offset1+3, (uint32_t)irq3, 0x08, 0x8e);
	setEntry(offset1+4, (uint32_t)irq4, 0x08, 0x8e);
	setEntry(offset1+5, (uint32_t)irq5, 0x08, 0x8e);
	setEntry(offset1+6, (uint32_t)irq6, 0x08, 0x8e);
	setEntry(offset1+7, (uint32_t)irq7, 0x08, 0x8e);
	setEntry(offset2+0, (uint32_t)irq8, 0x08, 0x8e);
	setEntry(offset2+1, (uint32_t)irq9, 0x08, 0x8e);
	setEntry(offset2+2, (uint32_t)irq10, 0x08, 0x8e);
	setEntry(offset2+3, (uint32_t)irq11, 0x08, 0x8e);
	setEntry(offset2+4, (uint32_t)irq12, 0x08, 0x8e);
	setEntry(offset2+5, (uint32_t)irq13, 0x08, 0x8e);
	setEntry(offset2+6, (uint32_t)irq14, 0x08, 0x8e);
	setEntry(offset2+7, (uint32_t)irq15, 0x08, 0x8e);

	// software api interrupts
	setEntry(0x30, (uint32_t)irq30, 0x08, 0x8e);
}

void setIDT()
{
	gebl_idt_t idt;
	installExceptions();
	installInterrupts(0x20, 0x28);
	
	idt.limit = sizeof(gebl_idt_entry_t) * 256;
	idt.baseptr = 0x7800;
	
	installIDT(&idt);
	setInterrupts();
}