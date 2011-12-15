/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

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

#include <stdlib.h>
#include <sys/dev/pci.h>
#include <arch/x86/apic/msi.h>

#ifndef IRQ_H
#define IRQ_H

#define MAX_IRQ_NUM 255
#define MAX_ISA_IRQ_NUM 0x10
#define IRQ_BASE 40

/*
 * An IRQ is the index number of the IRQ (eg irq 0 is the timer by default).
 * The irq vector is a the index number of the idt for that irq. So IRQ ==
 * vector might not be true.
 */
struct irq_data
{
  uint32_t irq;
  uint32_t irq_base;

  struct irq_cfg *irq_config;
};

extern struct irq_data irq_data[MAX_IRQ_NUM];
extern uint32_t irqs[IRQ_BASE];

/*
 * Interrupt headers
 */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
extern void irq30();
extern void irq40();

static inline struct irq_data*
get_irq_data(uint32_t irq)
{
  return &irq_data[irq];
}

static inline struct irq_cfg*
get_irq_cfg(uint32_t irq)
{
  return get_irq_data(irq)->irq_config;
}

static inline uint32_t
get_isa_irq_vector(uint32_t x)
{
  return irqs[x];
}

static inline uint32_t
get_irq_base(uint8_t i)
{
  return irqs[i];
}

static void dbg_irq_data(void);
static void __list_all_irqs();
struct irq_data *alloc_irq();
static int free_irq_entry(struct irq_data*);
static struct irq_cfg *setup_irq_cfg(int irq);

struct irq_cfg
{
  union
  {
    /* 
     * An interrupt is sent to the cpu using either a msi or a hardware pin, but not both.
     */
    uint8_t hw_pin; /* pin where the interrupt is sent to */
    struct msi_cfg *msi; /* msi message */
  };
  
  uint vector : 8;
  uint delivery_mode : 3;
  uint trigger: 1; /* 0 -> edge trigger | 1 -> level trigger */
};

#endif