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
#include <arch/x86/mp.h>
#include <arch/x86/idt.h>
#include <arch/x86/pic.h>
#include <arch/x86/irq.h>
#include <arch/x86/paging.h>
#include <arch/x86/system.h>
#include <arch/x86/pte.h>
#include <arch/x86/bios.h>

#include <interrupts/int.h>

#include <andromeda/cpu.h>
#include <andromeda/core.h>
#include <andromeda/clock.h>
#include <andromeda/cpu.h>
#include <andromeda/elf.h>
#include <andromeda/drivers.h>
#include <andromeda/error.h>
#include <andromeda/system.h>
#include <andromeda/syscall.h>
#include <mm/page_alloc.h>

#include <lib/byteorder.h>

// Define the place of the heap

multiboot_memory_map_t* mmap;
size_t mmap_size;
size_t memsize = 0; // Size of memory in KiB

int page_dir_boot[0x400];
int page_table_boot[0x40000];

int vendor = 0;

void setIDT();

struct Bios_Data_Area bda;

int system_x86_mmu_init(struct sys_cpu* cpu)
{
        if (cpu == NULL || !hasmm() || !hasarch() || cpu->mmu != NULL) {
                panic("Setup conditions for mmu initialisation weren't met");
        }
        cpu->mmu = kmalloc(sizeof(*cpu->mmu));
        if (cpu->mmu == NULL)
                panic("Out of memory!");
        memset(cpu->mmu, 0, sizeof(*cpu->mmu));

        cpu->mmu->get_phys = x86_pte_get_phys;
        cpu->mmu->reset_page = x86_pte_unset_page;
        cpu->mmu->set_page = x86_pte_set_page;
        cpu->mmu->set_range = x86_pte_load_range;
        cpu->mmu->reset_range = x86_pte_unload_range;
        cpu->mmu->cleanup_range = x86_page_cleanup_range;

        return -E_SUCCESS;
}

int system_x86_cpu_init(int cpuid)
{
        if (getcpu(cpuid) != NULL)
                panic("Something went wrong in CPU initialisation!");
        /**
         * \todo  Detetct availability of CPU.
         */
        if (cpuid != 0) {
                /**
                 * \todo If CPU present, return CPUQ
                 */
                return -E_SUCCESS;
        }
        struct sys_cpu* cpu = kmalloc(sizeof(*cpu));
        if (cpu == NULL)
                panic("Out of memory!");
        memset(cpu, 0, sizeof(*cpu));
        setcpu(cpuid, cpu);

        cpu->halt = endProg;
        cpu->suspend = endProg;
        /** \todo Add cpu throttling and resuming functions in place */
        cpu->throttle = NULL;
        cpu->resume = NULL;

        cpu->disable_interrupt = disableInterrupts;
        cpu->enable_interrupt = enableInterrupts;

        system_x86_mmu_init(cpu);

        return -E_SUCCESS;
}

static void init_pic()
{
        int is_available = x86_eflags_test(X86_FLAGS_CPUID_TEST_BIT);
        if (is_available) {
                struct x86_gen_regs registers;
                x86_cpuid(1, &registers);
                if (registers.edx && (1 << 8)) {
                        /* An APIC is present */
                        /** \todo Initialise the Local APIC */
                        /** \todo Initialise the IO APIC */
                        debug("An APIC system was found!!!\n");
                        warning("No APIC code available. "
                                        "Still to be implemented\n");

                        warning("Enabling 8259 PIC instead :(\n");
                        pic_8259_init();
                        /** \todo Start other CPU cores here */
                        return;

                } else {
                        debug("No APIC was found ... :(\n");
                        debug("Going for the 8259 PIC\n");
                }
        } else {
                debug("CPUID not available\n");
                debug("Guess we'll have to go for the 8259 PIC\n");
        }

        pic_8259_init();

}

int system_x86_init()
{
        /** \todo Write the x86 function pointer library */
        warning("The code to set up x86 function pointers is "
                        "still to be written\n");

        setIDT();
        vm_init();

        init_pic();

        return -E_NOFUNCTION;
}

// The main function
int init(unsigned long magic, multiboot_info_t* hdr)
{
        setGDT();

        memset(&bda, 0, sizeof(bda));
        memcpy((void*)0x400, &bda, sizeof(bda));

        sys_setup_alloc();

        textInit();
        addr_t tmp = (addr_t) hdr + THREE_GIB;
        hdr = (multiboot_info_t*) tmp;

        if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
                printf("\nInvalid magic word: %X\n", magic);
                panic("");
        }
        if (hdr->flags & MULTIBOOT_INFO_MEMORY) {
                memsize = hdr->mem_upper;
                memsize += 1024;
        } else
                panic("No memory flags!");
        if (!(hdr->flags & MULTIBOOT_INFO_MEM_MAP))
                panic("Invalid memory map");

        mmap = (multiboot_memory_map_t*) hdr->mmap_addr;
        /** Build the memory map and allow for allocation */
        sys_setup_paging(mmap, (unsigned int) hdr->mmap_length);
        sys_setup_arch();

        printf(WELCOME); // The only screen output that should be maintained
#ifdef PA_DBG
//         endProg();
#endif

        if (hdr->flags & MULTIBOOT_INFO_ELF_SHDR)
                core_symbols_init(&hdr->u.elf_sec);

#ifdef SLOB
        debug("Size of the heap: 0x%x\tStarting at: %x\n", HEAPSIZE, heap);
#endif

        ol_cpu_t cpu = kmalloc(sizeof(*cpu));
        if (cpu == NULL)
                panic("OUT OF MEMORY!");
        x86_cpu_init(cpu);

        cpu_enable_interrupts(0);

        sys_setup_fs();
        sys_setup_modules();
        sys_setup_devices();
        sys_setup_net();

        sc_init();

        if (dev_init() != -E_SUCCESS) {
                panic("Couldn't initialise /dev");
        }

        ol_ps2_init_keyboard();

        debug("Little endian 0xf in net endian %x\n", htons(0xf));
#ifdef DBG
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

#ifdef TIMER_DBG
        timer_setup_test(0, X86_8259_INTERRUPT_BASE);
#endif
        core_loop();
        return 0; // To keep the compiler happy.
}
