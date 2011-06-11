/*
 *   GoldenEagle Bootloader C entry point.
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

#include <textio.h>
#include <sys/stdlib.h>
#include <sys/io.h>
#include <mm/mmap.h>

void kmain(void)
{
	textinit();
	clearscreen();
	
	println("GoldenEagle kernel is executing. \n");

	char status = inb(0x60);
	
	println("Multiboot memory map:\n");
	gebl_display_mmap();

	putc(0xa);
	
	struct gebl_register_pack * regs = getregs();
	printnum(regs->esp, 16, FALSE, FALSE);
	putc(0xa);

	if((status & 2) == 2)
	{
		println("The A20 gate is open.");
	}
	
	while(1) halt();
}