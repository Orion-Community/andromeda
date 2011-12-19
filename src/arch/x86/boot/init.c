/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven, Michel Megens

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

/**
 * This file holds the entry point to the C level kernel. What's done here is
 * the basic intialisation of for example:
 * - Paging,
 * - ACPI and
 * - Hardware abstraction.
 *
 * The init function also displays the welcome message.
 */

// Basic includes
#include <version.h>
#include <stdlib.h>
#include <unistd.h>
#include <kern/cpu.h>
#include <kern/elf.h>
#include <mm/paging.h>
#include <mm/map.h>
#include <interrupts/int.h>
#include <arch/x86/idt.h>
#include <boot/mboot.h>
#include <mm/map.h>
#include <tty/tty.h>
#include <arch/x86/pic.h>

#include <sys/dev/ps2.h>
#include <sys/dev/pci.h>
#include <sys/sys.h>

#include <arch/x86/cpu.h>
#include <arch/x86/apic/apic.h>
#include <arch/x86/acpi/acpi.h>

#include <kern/cpu.h>
#include <kern/core.h>

#include <arch/x86/apic/ioapic.h>

// Define the place of the heap

multiboot_memory_map_t* mmap;
size_t mmap_size;

char *welcome = "Andromeda " VERSION " - " NAME
"\nCopyright (C) 2010, 2011 - Michel Megens,"
"Bart Kuivenhoven\nThis program comes with ABSOLUTELY NO WARRANTY;\n"
"This is free software, and you are welcome to redistribute it.\n"
"For more info refer to the COPYING file in the source repository or look at\n"
"http://www.gnu.org/licenses/gpl-3.0.html\n";

int vendor = 0;

boolean setupCore(module_t mod)
{
	// Examine and augment the elf image here, return true if faulty
	switch (coreCheck((void*) mod.addr))
	{
	case 0:
		break;
	case -1:
		printf("Invalid elf image\n");
		return TRUE;
	case -2:
		printf("Entry point too low\n");
		return TRUE;
	case -3:
		printf("Kernel magic invalid\n");
		return TRUE;

	default:
		printf("Unknown return value");
		return TRUE;
	}
	coreAugment(mod.addr);

	// Jump into the high memory image
	elfJmp(mod.addr);

	return FALSE; //Doesn't get reached, ever, if all goes well
}

	// The main function

int init(unsigned long magic, multiboot_info_t* hdr)
{
	init_heap();
	complement_heap(&end, HEAPSIZE);
	textInit();
	addr_t tmp = (addr_t)hdr + offset;
	hdr = (multiboot_info_t*)tmp;

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf("\nInvalid magic word: %X\n", magic);
		panic("");
	}
	if (hdr->flags & MULTIBOOT_INFO_MEMORY)
	{
		memsize = hdr->mem_upper;
		memsize += 1024;
	}
	else
	{
		panic("No memory flags!");
	}
	if (hdr->flags & MULTIBOOT_INFO_MEM_MAP)
	{
		mmap = (multiboot_memory_map_t*) hdr->mmap_addr;
		build_map(mmap, (unsigned int) hdr->mmap_length);
	}
	else
	{
		panic("Invalid memory map");
	}

	page_init();
	printf("%s\n", welcome);
	setGDT();
	page_unmap_low_mem();
	pic_init();
	setIDT();

	printf("Size of the heap: 0x%x\tStarting at: %x\n", HEAPSIZE, &end);
	ol_cpu_t cpu = kalloc(sizeof (*cpu));
	ol_cpu_init(cpu);
	acpi_init();

        ol_ps2_init_keyboard();
        ol_apic_init(cpu);
        init_ioapic();
        setup_irq_data();
        ol_pci_init();
        alloc_irq();

#ifdef __IOAPIC_DBG
	ioapic_debug();
#endif

#ifdef __MEMTEST
        ol_detach_all_devices(); /* free's al the pci devices */
#endif
#ifdef __DBG_HEAP
	printf("Heap list:\n");
	ol_dbg_heap();
#endif
#ifndef DBG
	printf("\nSome (temp) debug info:\n");
	printf("CPU vendor: %s\n", cpus->vendor);

	if(systables->magic == SYS_TABLE_MAGIC)
	{
		printf("RSDP ASCII signature: 0x%x%x\n",
		*(((uint32_t*) systables->rsdp->signature) + 1),
		*(((uint32_t*) systables->rsdp->signature)));
		printf("MP specification signature: 0x%x\n", systables->mp->signature);
	}
#endif
	core_loop();
	return 0; // To keep the compiler happy.
}
