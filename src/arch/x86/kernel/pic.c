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

#include <stdlib.h>
#include <andromeda/panic.h>
#include <arch/x86/pic.h>
#include <arch/x86/pit.h>
#include <arch/x86/irq.h>
#include <text.h>

void pic_remap(uint32_t offset1, uint32_t offset2)
{
	uint8_t mask1, mask2;

	mask1 = inb(OL_PIC1_DATA);
	mask2 = inb(OL_PIC2_DATA);

	// sent the initialise commands to the master and slave pic
	outb(OL_PIC1_COMMAND, OL_ICW1_INIT|OL_ICW1_ICW4);
	outb(OL_PIC2_COMMAND, OL_ICW1_INIT|OL_ICW1_ICW4);
	iowait();

	// define the vectors
	outb(OL_PIC1_DATA, offset1);
	outb(OL_PIC2_DATA, offset2);
	iowait();

	// connect to the slave
	outb(OL_PIC1_DATA, OL_ICW3_MASTER);
	outb(OL_PIC2_DATA, OL_ICW3_SLAVE);
	iowait();

	outb(OL_PIC1_DATA, OL_ICW4_8086);
	outb(OL_PIC2_DATA, OL_ICW4_8086);
	iowait();

	outb(OL_PIC1_DATA, mask1);
	outb(OL_PIC2_DATA, mask2);
	iowait();

	outb(0x21, 0x3c);
	outb(0xa1, 0x3f);

#if 0
	outb(OL_PIC2_DATA, 0xff);	// disable irq's
	outb(OL_PIC2_COMMAND, 0xff);
	iowait();
#endif
}

void pic_eoi(uint8_t irq)
{
	if(irq >= 8)
	{
		outb(OL_PIC2_COMMAND, OL_PIC_EOI);
	}
	outb(OL_PIC1_COMMAND, OL_PIC_EOI);
}

void pic_init()
{
  pic_remap(OL_INTERRUPT_BASE, OL_INTERRUPT_BASE+8);
  ol_pit_init(100); // program pic to 100 hertz
}