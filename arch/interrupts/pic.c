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
	outb(GEBL_ICW1_INIT+GEBL_ICW1_ICW4, GEBL_PIC1_COMMAND);
	outb(GEBL_ICW1_INIT+GEBL_ICW1_ICW4, GEBL_PIC2_COMMAND);
	iowait();

	// define the vectors
	outb(offset1, GEBL_PIC1_DATA);
	outb(offset2, GEBL_PIC2_DATA);
	iowait();

	// connect to the slave
	outb(GEBL_ICW3_MASTER, GEBL_PIC1_DATA);
	outb(GEBL_ICW3_SLAVE, GEBL_PIC2_DATA);
	iowait();
	
	outb(GEBL_ICW4_8086, GEBL_PIC1_DATA);
	outb(GEBL_ICW4_8086, GEBL_PIC2_DATA);
	iowait();
	
	outb(mask1, GEBL_PIC1_DATA);
	outb(mask2, GEBL_PIC2_DATA);
	iowait();
	
	outb(0xff, GEBL_PIC1_DATA);	// disable irq's, not yet implemented.
	outb(0xff, GEBL_PIC2_DATA);
	iowait();
}

void pic_eoi(uint8_t irq)
{
	if(irq >= 8)
	{
		outb(GEBL_PIC_EOI, GEBL_PIC2_COMMAND);
		return;
	}
	outb(GEBL_PIC_EOI, GEBL_PIC1_COMMAND);
}

void pic_init()
{
	pic_remap(GEBL_INTERRUPT_BASE, GEBL_INTERRUPT_BASE+8);
	panic("PIC not yet implemented");
}