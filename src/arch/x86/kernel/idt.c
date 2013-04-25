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
#include <arch/x86/irq.h>
#include <arch/x86/interrupts.h>
#include <andromeda/system.h>
#include <mm/memory.h>

static void
ol_idt_install_entry(uint16_t, uint32_t, uint16_t, uint8_t, ol_idt_t);
static int get_empty_idt_entry_number();

static void
installExceptions(ol_idt_t idt)
{
        ol_idt_install_entry( 0, (uint32_t)divByZero , 0x08, 0x8E, idt);
        ol_idt_install_entry( 1, (uint32_t)depricated, 0x08, 0x8E, idt);
        ol_idt_install_entry( 2, (uint32_t)nmi , 0x08, 0x8E, idt);
        ol_idt_install_entry( 3, (uint32_t)breakp , 0x08, 0x8E, idt);
        ol_idt_install_entry( 4, (uint32_t)overflow , 0x08, 0x8E, idt);
        ol_idt_install_entry( 5, (uint32_t)bound , 0x08, 0x8E, idt);
        ol_idt_install_entry( 6, (uint32_t)invalOp , 0x08, 0x8E, idt);
        ol_idt_install_entry( 7, (uint32_t)noMath , 0x08, 0x8E, idt);
        ol_idt_install_entry( 8, (uint32_t)doubleFault , 0x08, 0x8E, idt);
        ol_idt_install_entry( 9, (uint32_t)depricated , 0x08, 0x8E, idt);
        ol_idt_install_entry(10, (uint32_t)invalidTSS, 0x08, 0x8E, idt);
        ol_idt_install_entry(11, (uint32_t)snp, 0x08, 0x8E, idt);
        ol_idt_install_entry(12, (uint32_t)stackFault, 0x08, 0x8E, idt);
        ol_idt_install_entry(13, (uint32_t)genProt, 0x08, 0x8E, idt);
        ol_idt_install_entry(14, (uint32_t)pageFault, 0x08, 0x8E, idt);
        ol_idt_install_entry(15, (uint32_t)depricated, 0x08, 0x8E, idt);
        ol_idt_install_entry(16, (uint32_t)fpu, 0x08, 0x8E, idt);
        ol_idt_install_entry(17, (uint32_t)alligned, 0x08, 0x8E, idt);
        ol_idt_install_entry(18, (uint32_t)machine, 0x08, 0x8E, idt);
        ol_idt_install_entry(19, (uint32_t)simd, 0x08, 0x8E, idt);
}

static void
installInterrupts(uint16_t offset1, uint16_t offset2, ol_idt_t idt)
{
        ol_idt_install_entry(offset1+0, (uint32_t)irq0, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset1+1, (uint32_t)irq1, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset1+2, (uint32_t)irq2, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset1+3, (uint32_t)irq3, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset1+4, (uint32_t)irq4, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset1+5, (uint32_t)irq5, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset1+6, (uint32_t)irq6, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset1+7, (uint32_t)irq7, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset2+0, (uint32_t)irq8, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset2+1, (uint32_t)irq9, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset2+2, (uint32_t)irq10, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset2+3, (uint32_t)irq11, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset2+4, (uint32_t)irq12, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset2+5, (uint32_t)irq13, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset2+6, (uint32_t)irq14, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);
        ol_idt_install_entry(offset2+7, (uint32_t)irq15, 0x08, IDT_PRESENT_BIT |
        IDT_INTERRUPT_GATE, idt);

        // software api interrupts
//      ol_idt_install_entry(0x30, (uint32_t)irq30, 0x08, IDT_PRESENT_BIT |
//      IDT_INTERRUPT_GATE, idt);
}

void setIDT()
{
  ol_idt_t idt = kmalloc(sizeof(struct idt));
  idt->limit = sizeof(ol_idt_entry_t) * 256;
  idt->baseptr = kmalloc(idt->limit);
  memset(idt->baseptr, 0, idt->limit);

  installExceptions(idt);
  //installInterrupts(0x20, 0x28, idt);
  installIDT(idt);

  debug("First empty idt entry: %x\n", get_empty_idt_entry_number());
}

static void
ol_idt_install_entry(uint16_t num, uint32_t base,
        uint16_t sel, uint8_t flags, ol_idt_t idt)
{
  idt->baseptr[num].base_high = (base >> 16) & 0xffff;
  idt->baseptr[num].base_low = base & 0xffff;
  idt->baseptr[num].flags = flags;
  idt->baseptr[num].sel = sel;
  idt->baseptr[num].zero = 0;
}

/**
 * Get an empty idt entry.
 */
static int
get_empty_idt_entry_number()
{
  ol_idt_t idt = (ol_idt_t)get_idt();

  /*
   * An entry is defined as 'empty' when the 32-bits base and flags are 0.
   */
  int i = IDT_VECTOR_OFFSET; /* start after exceptions */
  for(; i < idt->limit/sizeof(ol_idt_entry_t); i++)
  {
    uint32_t base = idt->baseptr[i].base_low | ((idt->baseptr[i].base_high) >> 16);
    if(!(idt->baseptr[i].flags) && !base)
    {
      return i;
    }
  }
  return -1;
}

int
alloc_idt_entry()
{
  return get_empty_idt_entry_number();
}

int
free_idt_entry(uint16_t vector)
{
  ol_idt_t idt = (ol_idt_t)get_idt();
  memset((void*)&(idt->baseptr[vector]), 0, sizeof(idt->baseptr[vector]));
}

int
install_irq_vector(struct irq_data *data)
{
  uint8_t entry = data->irq_config->vector;
  if(0 != entry)
  {
    ol_idt_t idt = (ol_idt_t)get_idt();

    ol_idt_install_entry(entry, data->irq_base, 0x8, IDT_PRESENT_BIT | IDT_INTERRUPT_GATE,
                         idt);
    installIDT(idt);
#ifdef __IRQ_DBG
    printf("IRQ num: %i | IRQ base: %x | Next available entry: %x | Vector: %x\n",
           data->irq, data->irq_base, get_empty_idt_entry_number(),
           entry);
#endif
    return 0;
  }
  else
    return -1;
}
