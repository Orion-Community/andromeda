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

#include <stdlib.h>
#include <arch/x86/idt.h>
#include <arch/x86/pic.h>
#include <arch/x86/irq.h>
#include <arch/x86/interrupts.h>
#include <andromeda/system.h>
#include <andromeda/syscall.h>
#include <mm/memory.h>

static void
x86_idt_install_entry(uint16_t, uint32_t, uint16_t, uint8_t, struct idt*);

static void install_exceptions(struct idt* idt)
{
        x86_idt_install_entry(0, (uint32_t) divByZero, 0x08, 0x8E, idt);
        x86_idt_install_entry(1, (uint32_t) depricated, 0x08, 0x8E, idt);
        x86_idt_install_entry(2, (uint32_t) nmi, 0x08, 0x8E, idt);
        x86_idt_install_entry(3, (uint32_t) breakp, 0x08, 0x8E, idt);
        x86_idt_install_entry(4, (uint32_t) overflow, 0x08, 0x8E, idt);
        x86_idt_install_entry(5, (uint32_t) bound, 0x08, 0x8E, idt);
        x86_idt_install_entry(6, (uint32_t) invalOp, 0x08, 0x8E, idt);
        x86_idt_install_entry(7, (uint32_t) noMath, 0x08, 0x8E, idt);
        x86_idt_install_entry(8, (uint32_t) doubleFault, 0x08, 0x8E, idt);
        x86_idt_install_entry(9, (uint32_t) depricated, 0x08, 0x8E, idt);
        x86_idt_install_entry(10, (uint32_t) invalidTSS, 0x08, 0x8E, idt);
        x86_idt_install_entry(11, (uint32_t) snp, 0x08, 0x8E, idt);
        x86_idt_install_entry(12, (uint32_t) stackFault, 0x08, 0x8E, idt);
        x86_idt_install_entry(13, (uint32_t) genProt, 0x08, 0x8E, idt);
        x86_idt_install_entry(14, (uint32_t) pageFault, 0x08, 0x8E, idt);
        x86_idt_install_entry(15, (uint32_t) depricated, 0x08, 0x8E, idt);
        x86_idt_install_entry(16, (uint32_t) fpu, 0x08, 0x8E, idt);
        x86_idt_install_entry(17, (uint32_t) alligned, 0x08, 0x8E, idt);
        x86_idt_install_entry(18, (uint32_t) machine, 0x08, 0x8E, idt);
        x86_idt_install_entry(19, (uint32_t) simd, 0x08, 0x8E, idt);
        x86_idt_install_entry(INT_SYSCALL, (uint32_t) asm_syscall, 0x08, 0x8E,
                        idt);
}

static void install_8259_interrupts(uint16_t offset1, uint16_t offset2,
                struct idt* idt)
{
        x86_idt_install_entry(offset1 + 0, (uint32_t) irq0, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset1 + 1, (uint32_t) irq1, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset1 + 2, (uint32_t) irq2, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset1 + 3, (uint32_t) irq3, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset1 + 4, (uint32_t) irq4, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset1 + 5, (uint32_t) irq5, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset1 + 6, (uint32_t) irq6, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset1 + 7, (uint32_t) irq7, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset2 + 0, (uint32_t) irq8, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset2 + 1, (uint32_t) irq9, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset2 + 2, (uint32_t) irq10, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset2 + 3, (uint32_t) irq11, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset2 + 4, (uint32_t) irq12, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset2 + 5, (uint32_t) irq13, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset2 + 6, (uint32_t) irq14, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        x86_idt_install_entry(offset2 + 7, (uint32_t) irq15, 0x08,
        IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);

        // software api interrupts
//      ol_idt_install_entry(0x30, (uint32_t)irq30, 0x08, IDT_PRESENT_BIT |
//      IDT_INTERRUPT_GATE, idt);
}

void setIDT()
{
        struct idt* idt = kmalloc(sizeof(*idt));
        if (idt == NULL)
                panic("OUT OF MEMORY in idt alloc!");
        memset(idt, 0, sizeof(*idt));

        idt->limit = sizeof(struct idt_entry) * 256;

        idt->baseptr = kmalloc(idt->limit);
        if (idt->baseptr == NULL)
                panic("OUT OF MEMORY allocating descriptor table!");
        memset(idt->baseptr, 0, idt->limit);

        install_exceptions(idt);
        install_8259_interrupts(X86_8259_INTERRUPT_BASE,
                        X86_8259_INTERRUPT_BASE + 8, idt);
        installIDT(idt);
}

#if 0
void dump_idt()
{
        struct idt* idt = (struct idt*) get_idt();
        printf("Idt desc addr: %X\n", (int) idt);
        if (idt == NULL)
        panic("NULL IDT!");
        printf("Idt addr: %X\n", (int) idt->baseptr);

        int i = 0;
        unsigned int j = 0;
        for (; i < 0xFF; i++) {
                printf("%i: %X\t", i,
                                idt->baseptr[i].base_high << 16
                                | idt->baseptr[i].base_low);
                if (i % 0x10 == 0) {
                        for (j = 0; j < 0x8FFFFFFF; j++) {
                                printf("");
                        }
                }
        }
        for (j = 0; j < 0x8FFFFFFF; j++) {
                printf("");
                /*
                 * Force the system to wait, to allow us to read the messages.
                 * I know this is a hacky-hacky way, but in this moment in time
                 * (at the time of writing and in this phase in boot) there is
                 * no good way of persisting the messages for later reading.
                 */
        }
}
#endif

static void x86_idt_install_entry(uint16_t num, uint32_t base, uint16_t sel,
                uint8_t flags, struct idt* idt)
{
        idt->baseptr[num].base_high = (base >> 16) & 0xffff;
        idt->baseptr[num].base_low = base & 0xffff;
        idt->baseptr[num].flags = flags;
        idt->baseptr[num].sel = sel;
        idt->baseptr[num].zero = 0;
}
