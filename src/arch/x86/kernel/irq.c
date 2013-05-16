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

#include "clock.h"
#include <text.h>
#include <stdlib.h>

#include <arch/x86/idt.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/irq.h>
#include <arch/x86/pic.h>

#include <interrupts/int.h>

#include <sys/keyboard.h>
#include <sys/dev/ps2.h>
#include <sys/dev/pci.h>

#include <andromeda/system.h>

#ifndef __PIE__
#error "The x86 kernel module should be compiled position independently!"
#endif

uint64_t pit_timer = 0;
uint64_t sleepTime = 0;
bool isSleeping = FALSE;
struct irq_data irq_data[MAX_IRQ_NUM];
uint32_t irqs[IRQ_BASE];

void cIRQ0(irq_stack_t regs)
{
  if (isSleeping)
  {
    if (!(sleepTime == 0)) sleepTime--;
  }
  pit_timer += 1;

  pic_eoi(0);

  return;
}

void cIRQ1(irq_stack_t regs)
{
  uint8_t c = ol_ps2_get_keyboard_scancode();
  kb_handle(c);
  pic_eoi(1);
  return;
}

void cIRQ2(irq_stack_t regs)
{
  pic_eoi(2);
  return;
}

void cIRQ3(irq_stack_t regs)
{
  pic_eoi(3);
  return;
}

void cIRQ4(irq_stack_t regs)
{
  pic_eoi(4);
  return;
}

void cIRQ5(irq_stack_t regs)
{
  pic_eoi(5);
  return;
}

void cIRQ6(irq_stack_t regs)
{
  pic_eoi(6);
  return;
}

void cIRQ7(irq_stack_t regs)
{
  pic_eoi(7);
  return;
}

void cIRQ8(irq_stack_t regs)
{
  printf("test\n");
  outb(CMOS_SELECT, CMOS_RTC_IRQ);
  inb(CMOS_DATA);
  pic_eoi(8);
  return;
}

void cIRQ9(irq_stack_t regs)
{
  putc('a');
  pic_eoi(9);
  return;
}

void cIRQ10(irq_stack_t regs)
{
  pic_eoi(10);
  return;
}

void cIRQ11(irq_stack_t regs)
{
  pic_eoi(11);
  return;
}

void cIRQ12(irq_stack_t regs)
{
  pic_eoi(12);
  return;
}

void cIRQ13(irq_stack_t regs)
{
  pic_eoi(13);
  return;
}

void cIRQ14(irq_stack_t regs)
{
  putc('a');
  pic_eoi(14);
  return;
}

void cIRQ15(irq_stack_t regs)
{
  putc('b');
  pic_eoi(15);
  return;
}

void
do_irq(struct general_irq_stack stack)
{
#ifdef IRQ_DBG
  debug("Called irq: %x\n", stack.irq);
#endif
  struct irq_data *data = get_irq_data(stack.irq);
  data->handle(stack.irq, stack.regs);
  return;
}

static void
__list_all_irqs()
{
  memset(irq_data, 0, MAX_IRQ_NUM*sizeof(*irq_data));
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
  int i = 0;
  for(; i < MAX_IRQ_NUM; i++)
  {
    irq_data[i].irq = i;
  }
}

void
setup_irq_data(void)
{
  __list_all_irqs();
  int i = 1;
  uint16_t vector;
  for(; i < 16; i++)
  {
    vector = i + IDT_VECTOR_OFFSET;
    struct irq_data *data = get_irq_data(i);
    data->irq_base = get_isa_irq_vector(i);
    data->irq = i;
    data->irq_config = kmalloc(sizeof(struct irq_cfg));
    data->irq_config->vector = (uint16_t)vector;
    install_irq_vector(data);
  }
}

/**
 * Setup a default irq config. The default irq configration is a low priority,
 * edge triggered interrupt. The only field which is not set is the hardware pin/
 * msi field.
 */
static struct irq_cfg *
setup_irq_cfg(int irq)
{
  struct irq_data *data = get_irq_data(irq);
  if(data->irq_config == NULL)
    data->irq_config = kmalloc(sizeof(*(data->irq_config)));

  struct irq_cfg *cfg = data->irq_config;
  cfg->trigger = 0;
  cfg->delivery_mode = 1;
  cfg->vector = alloc_idt_entry();

  return data->irq_config;
}

struct irq_data *
alloc_irq()
{
  int i = 16;
  for(; i < MAX_IRQ_NUM; i++)
  {
    if(irq_data[i].irq_base == 0 && irq_data[i].irq_config == NULL)
    {
      setup_irq_cfg(irq_data[i].irq);

      return &irq_data[i];
    }
  }
  return NULL;
}

static int
free_irq_entry(struct irq_data* irq)
{
  void *base = (void*)irq_data;
  void *top = (void*)&irq_data[MAX_IRQ_NUM];
  if((void*)irq < base || (void*)irq > top) /*
                                             * the irq address should be in the irq address
                                             * range.
                                             */
    return -1;
  else
  {
    kfree(irq->irq_config);
    uint32_t irqnum = irq->irq;
    memset((void*)irq, 0, sizeof(*irq));
    irq->irq = irqnum;
    return -E_SUCCESS;
  }
  return -1;
}

static void
setup_irq_handler(unsigned int irq)
{
  struct irq_data *idata = get_irq_data(irq);
  unsigned int stub_size = get_general_irqstub_size();
  void *stub = kmalloc(stub_size);
  memcpy(stub, gen_irq_stub, stub_size);

  /*
   * Now we will change the content of the IRQ handler data:
   *
   * The irq_handler first, then the irq number
   */
  writel(stub+DYNAMIC_IRQ_HANDLER_VALUE, (unsigned int)&do_irq);
  writel(stub+DYNAMIC_IRQ_VALUE, idata->irq);
  idata->irq_base = (unsigned int)stub;
}

int
native_setup_irq_handler(unsigned int irq)
{
  struct irq_data *data = get_irq_data(irq);
  if(data->handle == NULL)
  {
    return -E_NULL_PTR;
  }
  else
  {
    setup_irq_handler(irq);
  }

  /* now we check if we might have to reset some values.. */
  if(data->base_handle == NULL || data->base_handle == &do_irq)
  {
    data->base_handle = &do_irq;
    return -E_SUCCESS;
  }
  else
  {
    /*
     * A different base handler has to be used.
     */
    writel((void*)data->irq_base+DYNAMIC_IRQ_HANDLER_VALUE, (uint32_t)data->
                                                                   base_handle);
  }
  return 0;
}

void
disable_irqs()
{
        __asm__ __volatile__("cli\n");
}

void
enable_irqs()
{
        __asm__ __volatile__("sti\n");
}

#ifdef IRQ_DBG
uint32_t
debug_dynamic_irq()
{
  struct irq_data *data = alloc_irq();
  setup_irq_handler(data->irq);
  debug("irq: %x - vector: %x\n", data->irq, data->irq_config->vector);
  install_irq_vector(data);
  return data->irq_config->vector;
}
#endif
