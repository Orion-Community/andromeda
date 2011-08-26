/*
 *   openLoader C entry point.
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
#include <stdlib.h>

#include <sys/io.h>
#include <sys/dev/ps2.h>
#include <sys/disk/ide.h>

#include <mm/mmap.h>
#include <mm/heap.h>

#include <interrupts/pic.h>
#include <interrupts/idt.h>

#include <sys/dev/pci.h>



void kmain(ol_mmap_register_t mmr)
{       
        ol_init_heap();
	textinit();
	clearscreen();


	println("The openLoader kernel is executing. \n");
        
	print("Current stack pointer: ");
	ol_registers_t regs = getregs();
	printnum(regs->esp, 16, FALSE, FALSE);
	putc(0xa);

	char status = inb(0x60);
	if((status & 2) == 2)
	{
		println("The A20 gate is open.");
		putc(0xa);
	}
	
	pic_init();
	setIDT();
        ol_ps2_init_keyboard();

// display mmap
	init_mmap(mmr);
	println("Multiboot memory map:\n");
	display_mmap(mmr);

#if 0
	uint8_t active = ide_init(bootdrive);
	ide_read(0x100, 1<<20, &bootdrive[active], 60);
	uint8_t eax = ata_identify();
	printnum(active, 16, FALSE, FALSE);
#endif
        
	putc(0xa);
        ol_pci_init();
	println("Waiting for service interrupts.. \n");

        ol_detach_all_devices();
        ol_dbg_heap();

	while(1) halt();
	println("End of program reached!");
	endprogram();
}
