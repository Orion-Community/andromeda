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
#include <drivers/pci/msi.h>

#include "interrupts.h"

#ifndef IRQ_H
#define IRQ_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_IRQ_NUM 255
#define MAX_ISA_IRQ_NUM 0x10
#define IRQ_BASE 40

#define IRQ_LOW_PRI 1

#define TRIGGER_EDGE 0
#define TRIGGER_LEVEL 1
#define TRIGGER_LEVEL_ASSERT 1
#define TRIGGER_LEVEL_DEASSERT 0

#define enable_legacy_irq(x) pic_clear_irq_mask(x)

  /* dynamic code allocation */
#define DYNAMIC_IRQ_VALUE get_general_irqstub_size()-8
#define DYNAMIC_IRQ_HANDLER_VALUE get_general_irqstub_size()-4

  /**
   * \fp irq_handler_t(irq_stack)
   * \brief Function pointer definition for irq handlers
   */
  typedef void (*irq_base_handler_t)(struct general_irq_stack);
  typedef void (*gen_irq_stub_t)();
  typedef void (*irq_handler_t)(unsigned int, irq_stack_t);

/*
 * An IRQ is the index number of the IRQ (eg irq 0 is the timer by default).
 * The irq vector is a the index number of the idt for that irq. So IRQ ==
 * vector might not be true.
 */
struct irq_data
{
  uint32_t irq;
  uint32_t irq_base;
  void *irq_data;
  irq_base_handler_t base_handle;
  irq_handler_t handle;
  struct irq_cfg *irq_config;
};

extern struct irq_data irq_data[MAX_IRQ_NUM];
extern uint32_t irqs[IRQ_BASE];

extern void *__end_of_irq_stub;
extern void gen_irq_stub();
extern void exec_addr(void *addr);
extern void trigger_soft_irq30();
#ifdef IRQ_DBG

extern void trigger_soft_irq31();
#endif

void setup_irq_data(void);

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
  if (i < IRQ_BASE)
    return irqs[i];
  return -E_INVALID_ARG;
}

static inline uint32_t
get_general_irqstub_size()
{
  return ((addr_t)&__end_of_irq_stub) - ((addr_t)&gen_irq_stub);
}

static inline void*
get_general_irqstub_end()
{
  return (void*)&__end_of_irq_stub;
}

uint32_t debug_dynamic_irq(void);
int native_setup_irq_handler(unsigned int irq);
/*
static void __list_all_irqs();
static int free_irq_entry(struct irq_data*);
static struct irq_cfg *setup_irq_cfg(int irq);
static void setup_irq_handler(unsigned int irq);
*/

struct irq_data *alloc_irq();
unsigned int search_irq_num(irq_base_handler_t handle);

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

/**
 * \fn enable_irqs()
 * \brief Enable the IRQs
 *
 * This function sets the Interrupt Enable flag in the (E)FLAGS register.
 */
void enable_irqs();

/**
 * \fn disable_irqs()
 * \brief Disable the IRQs
 *
 * This function will clear the Interrupt Enable flag in de (E)FLAGS register.
 */
void disable_irqs();

#if 0
extern void irq16();
extern void irq17();
extern void irq18();
extern void irq19();
extern void irq20();
extern void irq21();
extern void irq22();
extern void irq23();
extern void irq24();
extern void irq25();
extern void irq26();
extern void irq27();
extern void irq28();
extern void irq29();
extern void irq30();
extern void irq31();
extern void irq32();
extern void irq33();
extern void irq34();
extern void irq35();
extern void irq36();
extern void irq37();
extern void irq38();
extern void irq39();
extern void irq40();
extern void irq41();
extern void irq42();
extern void irq43();
extern void irq44();
extern void irq45();
extern void irq46();
extern void irq47();
extern void irq48();
extern void irq49();
extern void irq50();
extern void irq51();
extern void irq52();
extern void irq53();
extern void irq54();
extern void irq55();
extern void irq56();
extern void irq57();
extern void irq58();
extern void irq59();
extern void irq60();
extern void irq61();
extern void irq62();
extern void irq63();
extern void irq64();
extern void irq65();
extern void irq66();
extern void irq67();
extern void irq68();
extern void irq69();
extern void irq70();
extern void irq71();
extern void irq72();
extern void irq73();
extern void irq74();
extern void irq75();
extern void irq76();
extern void irq77();
extern void irq78();
extern void irq79();
extern void irq80();
extern void irq81();
extern void irq82();
extern void irq83();
extern void irq84();
extern void irq85();
extern void irq86();
extern void irq87();
extern void irq88();
extern void irq89();
extern void irq90();
extern void irq91();
extern void irq92();
extern void irq93();
extern void irq94();
extern void irq95();
extern void irq96();
extern void irq97();
extern void irq98();
extern void irq99();
extern void irq100();
extern void irq101();
extern void irq102();
extern void irq103();
extern void irq104();
extern void irq105();
extern void irq106();
extern void irq107();
extern void irq108();
extern void irq109();
extern void irq110();
extern void irq111();
extern void irq112();
extern void irq113();
extern void irq114();
extern void irq115();
extern void irq116();
extern void irq117();
extern void irq118();
extern void irq119();
extern void irq120();
extern void irq121();
extern void irq122();
extern void irq123();
extern void irq124();
extern void irq125();
extern void irq126();
extern void irq127();
extern void irq128();
extern void irq129();
extern void irq130();
extern void irq131();
extern void irq132();
extern void irq133();
extern void irq134();
extern void irq135();
extern void irq136();
extern void irq137();
extern void irq138();
extern void irq139();
extern void irq140();
extern void irq141();
extern void irq142();
extern void irq143();
extern void irq144();
extern void irq145();
extern void irq146();
extern void irq147();
extern void irq148();
extern void irq149();
extern void irq150();
extern void irq151();
extern void irq152();
extern void irq153();
extern void irq154();
extern void irq155();
extern void irq156();
extern void irq157();
extern void irq158();
extern void irq159();
extern void irq160();
extern void irq161();
extern void irq162();
extern void irq163();
extern void irq164();
extern void irq165();
extern void irq166();
extern void irq167();
extern void irq168();
extern void irq169();
extern void irq170();
extern void irq171();
extern void irq172();
extern void irq173();
extern void irq174();
extern void irq175();
extern void irq176();
extern void irq177();
extern void irq178();
extern void irq179();
extern void irq180();
extern void irq181();
extern void irq182();
extern void irq183();
extern void irq184();
extern void irq185();
extern void irq186();
extern void irq187();
extern void irq188();
extern void irq189();
extern void irq190();
extern void irq191();
extern void irq192();
extern void irq193();
extern void irq194();
extern void irq195();
extern void irq196();
extern void irq197();
extern void irq198();
extern void irq199();
extern void irq200();
extern void irq201();
extern void irq202();
extern void irq203();
extern void irq204();
extern void irq205();
extern void irq206();
extern void irq207();
extern void irq208();
extern void irq209();
extern void irq210();
extern void irq211();
extern void irq212();
extern void irq213();
extern void irq214();
extern void irq215();
extern void irq216();
extern void irq217();
extern void irq218();
extern void irq219();
extern void irq220();
extern void irq221();
extern void irq222();
#endif

#ifdef __cplusplus
}
#endif

#endif
