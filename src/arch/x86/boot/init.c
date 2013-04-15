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

#include <mm/paging.h>
#include <mm/map.h>
#include <mm/vm.h>
#include <mm/memory.h>
#include <mm/cache.h>
#include <interrupts/int.h>
#include <arch/x86/idt.h>

#include <boot/mboot.h>

#include <sys/dev/ps2.h>
#include <sys/dev/pci.h>
#include <sys/sys.h>

#include <arch/x86/cpu.h>
//#include <arch/x86/apic/apic.h>
//#include <arch/x86/acpi/acpi.h>
#include <arch/x86/mp.h>
//#include <arch/x86/apic/ioapic.h>
#include <arch/x86/idt.h>
#include <arch/x86/pic.h>
#include <arch/x86/irq.h>
#include <arch/x86/pit.h>
#include <arch/x86/paging.h>

#include <interrupts/int.h>

#include <andromeda/cpu.h>
#include <andromeda/core.h>
#include <andromeda/clock.h>
#include <andromeda/cpu.h>
#include <andromeda/elf.h>
#include <andromeda/drivers.h>
#include <andromeda/error.h>
#include <mm/page_alloc.h>

#include <lib/byteorder.h>

// Define the place of the heap

multiboot_memory_map_t* mmap;
size_t mmap_size;

int page_dir_boot [0x400];
int page_table_boot [0x40000];

int vendor = 0;

void elfJmp(void*);
void coreAugment(void*);
void setIDT();

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
	coreAugment((void*)mod.addr);

	// Jump into the high memory image
	elfJmp((void*)mod.addr);

	return FALSE; //Doesn't get reached, ever, if all goes well
}

// The main function
int init(unsigned long magic, multiboot_info_t* hdr)
{
        setGDT();
        init_heap();
#ifdef SLAB
        slab_alloc_init();
#endif
        textInit();
        /**
         * \todo Make complement_heap so that it allocates memory from pte
         */
        complement_heap(&end, HEAPSIZE);
        addr_t tmp = (addr_t)hdr + THREE_GIB;
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
                panic("No memory flags!");
        if (!(hdr->flags & MULTIBOOT_INFO_MEM_MAP))
                panic("Invalid memory map");

        mmap = (multiboot_memory_map_t*) hdr->mmap_addr;

        /** Build the memory map and allow for allocation */
        x86_pte_init();
        page_alloc_init(mmap, (unsigned int)hdr->mmap_length);
        vm_init();
#ifdef PA_DBG
//         endProg();
#endif
        /** In the progress of phasing out */
        /** Set up paging administration */
        /*x86_page_init(memsize);
        mboot_page_setup(mmap, (uint32_t)hdr->mmap_length);
        mboot_map_modules((void*)hdr->mods_addr, hdr->mods_count);

        /** For now this is the temporary page table map *//*
        build_map(mmap, (unsigned int) hdr->mmap_length);
        page_init();
        */
        /** end of deprication */
        task_init();

        printf(WELCOME); // The only screen output that should be maintained
        pic_init();
        setIDT();
        setup_irq_data();

        if (dev_init() != -E_SUCCESS)
                panic("Couldn't initialise /dev");

        ol_pit_init(1024); // program pic to 1024 hertz

        debug("Size of the heap: 0x%x\tStarting at: %x\n", HEAPSIZE, heap);

        //acpi_init();
        ol_cpu_t cpu = kalloc(sizeof (*cpu));
        if (cpu == NULL)
                panic("OUT OF MEMORY!");
        ol_cpu_init(cpu);

        ol_ps2_init_keyboard();
        //ol_apic_init(cpu);
        //init_ioapic();
        ol_pci_init();
        debug("Little endian 0xf in net endian %x\n", htons(0xf));
#ifdef DBG
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
#ifdef PA_DBG
        addr_t p = (addr_t)page_alloc();
        page_free((void*)p);
        printf("Allocated: %X\n", p);
        page_dump();
#endif

        core_loop();
        return 0; // To keep the compiler happy.
}
