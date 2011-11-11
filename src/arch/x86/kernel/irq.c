/*
 *   The interrupts
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

#include <text.h>
#include <arch/x86/idt.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/irq.h>
#include <arch/x86/pic.h>
#include <sys/keyboard.h>
#include <stdlib.h>
#include <sys/dev/ps2.h>
#include <sys/dev/pci.h>


uint64_t pit_timer = 0;
uint64_t sleepTime = 0;
bool isSleeping = FALSE;
struct irq_data irq_data[MAX_IRQ_NUM];
uint32_t irqs[MAX_ISA_IRQ_NUM];

void cIRQ0(ol_irq_stack_t regs)
{
  if (isSleeping)
  {
    if (!(sleepTime == 0)) sleepTime--;
  }
  pit_timer += 1;
  pic_eoi(0);

  return;
}

void cIRQ1(ol_irq_stack_t regs)
{
  uint8_t c = ol_ps2_get_keyboard_scancode();
  kb_handle(c);
  pic_eoi(1);
  return;
}

void cIRQ2(ol_irq_stack_t regs)
{
  pic_eoi(2);
  return;
}

void cIRQ3(ol_irq_stack_t regs)
{
  pic_eoi(3);
  return;
}

void cIRQ4(ol_irq_stack_t regs)
{
  pic_eoi(4);
  return;
}

void cIRQ5(ol_irq_stack_t regs)
{
  pic_eoi(5);
  return;
}

void cIRQ6(ol_irq_stack_t regs)
{
  pic_eoi(6);
  return;
}

void cIRQ7(ol_irq_stack_t regs)
{
  return;
}

void cIRQ8(ol_irq_stack_t regs)
{
  pic_eoi(8);
  return;
}

void cIRQ9(ol_irq_stack_t regs)
{
  putc('a');
  pic_eoi(9);
  return;
}

void cIRQ10(ol_irq_stack_t regs)
{
  pic_eoi(10);
  return;
}

void cIRQ11(ol_irq_stack_t regs)
{
  pic_eoi(11);
  return;
}

void cIRQ12(ol_irq_stack_t regs)
{
  pic_eoi(12);
  return;
}

void cIRQ13(ol_irq_stack_t regs)
{
  pic_eoi(13);
  return;
}

void cIRQ14(ol_irq_stack_t regs)
{
  putc('a');
  pic_eoi(14);
  return;
}

void cIRQ15(ol_irq_stack_t regs)
{
  putc('b');
  pic_eoi(15);
  return;
}

static void
__list_all_irqs()
{
  irqs[0] = (uint32_t)&irq0;
  irqs[1] = (uint32_t)&irq1;
  irqs[2] = (uint32_t)&irq2;
  irqs[3] = (uint32_t)&irq3;
  irqs[4] = (uint32_t)&irq4;
  irqs[5] = (uint32_t)&irq5;
  irqs[6] = (uint32_t)&irq6;
  irqs[7] = (uint32_t)&irq7;
  irqs[8] = (uint32_t)&irq8;
  irqs[9] = (uint32_t)&irq9;
  irqs[10] = (uint32_t)&irq10;
  irqs[11] = (uint32_t)&irq11;
  irqs[12] = (uint32_t)&irq12;
  irqs[13] = (uint32_t)&irq13;
  irqs[14] = (uint32_t)&irq14;
  irqs[15] = (uint32_t)&irq15;
}

void
setup_irq_data(void)
{
  __list_all_irqs();
  int i = 0;
  uint16_t vector;
  for(; i < 16; i++)
  {
    vector = i + IDT_VECTOR_OFFSET;
    struct irq_data *data = get_irq_data(i);
    data->irq_base = get_isa_irq_vector(i);
    data->irq = i;
    data->irq_config = kalloc(sizeof(struct irq_cfg));
    data->irq_config->vector = (uint16_t)vector;
    install_irq_vector(data);
  }
  printf("Entry 2 vector: %x\n", get_irq_cfg(1)->vector);
}

void dbg_irq_data(void)
{
  int entry = alloc_idt_entry();
  struct irq_data *data = &irq_data[entry];
  if(entry != -1)
  {
    data->irq_base = (uint32_t)&irq30;
    data->irq = 17;
    data->irq_config = kalloc(sizeof(struct irq_cfg));
    data->irq_config->vector = (uint16_t)entry;
    install_irq_vector(data);
  }
  else
    return;
}
