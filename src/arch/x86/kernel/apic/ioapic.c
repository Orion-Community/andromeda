/*
 *   Andromeda Kernel - I/O APIC interface
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

#include <mm/map.h>

#include <arch/x86/acpi/acpi.h>
#include <arch/x86/cpu.h>
#include <arch/x86/irq.h>
#include <arch/x86/apic/ioapic.h>

static uint32_t ioapic_read_dword(ioapic_t io, const uint8_t offset);
static void ioapic_write_dword(ioapic_t, const uint8_t, const uint32_t);
static void add_ioapic();
static int program_ioapic_pin(struct ioapic *io, int pin, int irq);

static volatile ioapic_t ioapic;

static struct ioapic*
create_ioapic (ol_madt_ioapic_t madt_io)
{
  ioapic_t io = kmalloc(sizeof(*io));
  cpus->lock(&cpu_lock);
  if (io == NULL)
    goto nomem;
  else
  {
    page_map_kernel_entry(madt_io->address, madt_io->address);
    io->address = (ioapic_addr_t*) madt_io->address;
    io->int_base = madt_io->global_system_interrupt_base;
    io->id = madt_io->id;
    io->read = &ioapic_read_dword;
    io->write = &ioapic_write_dword;
  }
  program_ioapic_pin(io, 0, 0);
  cpus->unlock(&cpu_lock);
  return io;

  nomem:
  ol_dbg_heap();
  panic("No free memory in heap in create_ioapic!");
  return NULL; /* to keep the compiler happy */
}

int
init_ioapic ()
{
  ioapic = create_ioapic(acpi_apics->ioapic->ioapic);
  ioapic->next = NULL;
  add_ioapic();

  int i = 0;
  struct ol_madt_ioapic_node *node = acpi_apics->ioapic;
  for(node = acpi_apics->ioapic; node != NULL, node != node->next;
      node = node->next)
  {
    i++;
#ifdef __IOAPIC_DBG
    debug("I/O APIC address: %x\t%x\t%x\t%x\n", node->ioapic->address,node, node->next,
           node->previous);
#endif
    if(node->next == NULL)
      break;
  }

#ifndef __IOAPIC_DBG
  debug("Found %i I/O APIC(s). The base address of the I/O APIC is: 0x%x\n", i,
         (uint32_t) ioapic->address);
#endif
  return 0;
}

/**
 * Adds all the additional ioapics to the list.
 * This function is called from init_ioapic after the master io apic has been
 * created
 */
static void
add_ioapic()
{
  struct ol_madt_ioapic_node *node;
  struct ioapic *ionode;
  /*
   * Start searching for the second io apic, since the first one is already
   * created by init_ioapic.
   */
  for(node = acpi_apics->ioapic->next; node != NULL, node != node->next;
      node = node->next)
  {
    if(node == NULL)
      break;
    for(ionode = ioapic; ionode != NULL, ionode != ionode->next; ionode = ionode->next)
    {
      if(ionode->next == NULL)
      {
	struct ioapic *io = create_ioapic(node->ioapic);
        ionode->next = io;
        ionode->next->next = NULL;
        break;
      }
    }
    if(node == NULL || node->next == NULL)
      break;
  }
}

static int
program_ioapic_pin(struct ioapic *io, int pin, int irq)
{
  struct irq_cfg *cfg = get_irq_cfg(irq);
  struct iopin *iopin = &(io->pin[pin]);

  iopin->vector = cfg->vector;
  iopin->delmod = cfg->delivery_mode;
  iopin->destmod = PHYS_MODE;
  iopin->deliv_state = ((io->read(io, REDIR_TABLE_OFFSET+(pin*2))) >> 12) & 1;
  iopin->intpol = ACTIVE_HIGH;
  iopin->remote_irr = (cfg->trigger) ?
                        (((io->read(io, REDIR_TABLE_OFFSET+(pin*2))) >> 14) & 1)
                        : 0;
  iopin->trig_mode = cfg->trigger;
  iopin->int_mask = INT_NO_MASK;
  iopin->destfield = io->apic_id;

  return 0;
}

static void
add_io_pin(struct ioapic *io, struct iopin *pin, int num)
{}

static int
write_ioapic_pin(struct iopin *pin)
{
  return 0;
}

#ifdef __IOAPIC_DBG
void
ioapic_debug()
{
  debug("Testing I/O apic at address 0x%x\n", ioapic->address);
  debug("I/O apic version %x\n", ioapic_read_dword(ioapic, 0x1));
}
#endif

static uint32_t
ioapic_read_dword(ioapic_t io, const uint8_t offset)
{
  *(io->address) = offset;
  return *(ioapic_addr_t*)(((void*)io->address+0x10));
}

static void
ioapic_write_dword(ioapic_t io, const uint8_t offset, const uint32_t value)
{
  *(io->address) = offset;
  *(IOAPIC_DATA_ADDRESS(io->address)) = value;
}
