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

void setIDT()
{
	gebl_idt_t idt;
	installExceptions();
	
	idt.limit = sizeof(gebl_idt_entry_t) * 256;
	idt.baseptr = 0x7800;
	
	installIDT(&idt);
}