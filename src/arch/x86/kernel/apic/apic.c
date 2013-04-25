/*
 *   The OpenLoader project - Local APIC interface
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
#include <text.h>

#include <arch/x86/cpu.h>
#include <arch/x86/irq.h>
#include <arch/x86/acpi/acpi.h>
#include <arch/x86/apic/apic.h>

#include <sys/sys.h>

static void __apic_write_register(ol_apic_reg_t reg, uint16_t value);
static uint16_t __apic_read_register(ol_apic_reg_t reg);
static uint64_t correct_apic_address(uint64_t addr, ol_cpu_t cpu);

struct apic *apic;

uint8_t
cpu_get_num()
{
        struct ol_madt_apic_node *head, *carriage, *tmp;
        uint8_t ret = 0;
        for_each_ll_entry_safe_count(acpi_apics->apic, carriage, tmp, ret);
        return ret;
}

static void
route_pic_to_apic()
{
  outb(OL_IMCR_COMMAND, OL_APIC_IMCR_SELECT);
  outb(OL_IMCR_DATA, OL_APIC_IMCR_PASSTROUGH_APIC);
  iowait();
}

static void
route_apic_to_pic()
{
  outb(OL_IMCR_COMMAND, OL_APIC_IMCR_SELECT);
  outb(OL_IMCR_DATA, OL_APIC_IMCR_PIC_MODE);
  iowait();
}

static int
ol_detect_apic(ol_cpu_t cpu)
{
  cpu->lock(&cpu_lock);
  if((cpu->flags & 0x1))
  {
    ol_gen_registers_t regs = ol_cpuid(1);
    if(regs->edx & (1<<9))
    {
      /* apic is available */
      cpu->unlock(&cpu_lock);
      return 0;
    }
    cpu->unlock(&cpu_lock);
    return -1;
  }
  cpu->unlock(&cpu_lock);
  return -1;
}

int
ol_apic_init(ol_cpu_t cpu)
{
  if(ol_detect_apic(cpu))
    goto fail;

  addr_t apic_addr = correct_apic_address(cpu_read_msr(0x1b), cpu);
  page_map_kernel_entry(apic_addr, apic_addr);
  debug("APIC base address: 0x%x\n", apic_addr);

  /* create an apic object */
  apic = kmalloc(sizeof(*apic));
  apic->write = &__apic_write_register;
  apic->read = &__apic_read_register;
  apic->dest_mode = 0;
  apic->delivery_mode = 0;

  if(systables->mp != NULL)
  {
    debug("Value of the IMCRP bit: 0x%x\n", systables->mp->imcrp);
    if(systables->mp->imcrp)
      route_pic_to_apic();
  }

  uint16_t temp = apic->read(APIC_SPURIOUS_INTERRUPT_REGISTER);
  apic->write(APIC_SPURIOUS_INTERRUPT_REGISTER, temp | 0x100);

  struct ol_madt_apic_node *node;
  int i = 0;
  for(node = acpi_apics->apic; node != NULL, node != node->next; node = node->next)
  {
    i++;
#ifdef __APIC_DBG
  debug("APIC flags: %x\t%x\t%x\t%x\n", node->apic->flags, node->apic->apic_id,
           node->apic->proc_id, node);
  debug("Apic version: %x\tSIR: %x\n", __apic_read_register(APIC_VERSION_REGISTER),
         __apic_read_register(APIC_SPURIOUS_INTERRUPT_REGISTER));
#endif
    if(node->next == NULL)
      break;
  }
#ifdef __APIC_DBG
  debug("Found %i APIC(s)\n", i);
#endif

  return 0;
  fail:
    return -1;
}

static uint64_t
correct_apic_address(uint64_t addr, ol_cpu_t cpu)
{
  uint64_t corval = 0; /* correction value */
  int i = 0;
  for(; i<cpu->bus_width; i++)
  {
    corval |= 1;
    corval <<= 1;
  }
  corval <<= 11;
  return addr&corval;
}

static uint16_t
__apic_read_register(ol_apic_reg_t reg)
{
  return (uint16_t)(*((uint32_t*)reg));
}

static void
__apic_write_register(ol_apic_reg_t reg, uint16_t value)
{
  *((uint32_t*)reg) = value;
}
