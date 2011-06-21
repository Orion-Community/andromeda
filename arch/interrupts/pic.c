/*
 *   PIC functions
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
#include <error/panic.h>
#include "include/pic.h"

void pic_remap(uint32_t offset1, uint32_t offset2)
{
	uint8_t mask1, mask2;

	mask1 = inb(GEBL_PIC1_DATA);
	mask2 = inb(GEBL_PIC2_DATA);

	// sent the initialise commands to the master and slave pic
	outb(GEBL_PIC1_COMMAND, GEBL_ICW1_INIT+GEBL_ICW1_ICW4);
	outb(GEBL_PIC2_COMMAND, GEBL_ICW1_INIT+GEBL_ICW1_ICW4);
	iowait();

	// define the vectors
	outb(GEBL_PIC1_DATA, offset1);
	outb(GEBL_PIC2_DATA, offset2);
	iowait();

	// connect to the slave
	outb(GEBL_PIC1_DATA, GEBL_ICW3_MASTER);
	outb(GEBL_PIC2_DATA, GEBL_ICW3_SLAVE);
	iowait();
	
	outb(GEBL_PIC1_DATA, GEBL_ICW4_8086);
	outb(GEBL_PIC2_DATA, GEBL_ICW4_8086);
	iowait();
	
	outb(GEBL_PIC1_DATA, mask1);
	outb(GEBL_PIC2_DATA, mask2);
	iowait();
	
	outb(GEBL_PIC1_DATA, 0xff);	// disable irq's, not yet implemented.
	outb(GEBL_PIC2_DATA, 0xff);
	iowait();
}

void pic_eoi(uint8_t irq)
{
	if(irq >= 8)
	{
		outb(GEBL_PIC2_COMMAND, GEBL_PIC_EOI);
		return;
	}
	outb(GEBL_PIC1_COMMAND, GEBL_PIC_EOI);
}

void pic_init()
{
	pic_remap(GEBL_INTERRUPT_BASE, GEBL_INTERRUPT_BASE+8);
	panic("PIC not yet implemented");
}