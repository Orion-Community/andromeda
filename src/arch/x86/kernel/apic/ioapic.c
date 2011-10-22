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

#include <arch/x86/acpi/acpi.h>
#include <arch/x86/apic/ioapic.h>
#include <arch/x86/apic/msi.h>
#include <arch/x86/cpu.h>

static volatile ioapic_t ioapic;

static struct ioapic*
create_ioapic (ol_madt_ioapic_t madt_io)
{
  ioapic_t io = kalloc(sizeof(*io));
  cpus->lock(&cpu_lock);
  if (io == NULL) 
    goto nomem;
  else
  {
    io->address = (ioapic_addr_t*) madt_io->address;
    io->int_base = madt_io->global_system_interrupt_base;
    io->id = madt_io->id;
    io->read = &ioapic_read_dword;
    io->write = &ioapic_write_dword;
  }
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
    printf("I/O APIC address: %x\t%x\t%x\t%x\n", node->ioapic->address,node, node->next, 
           node->previous);
#endif
    if(node->next == NULL)
      break;
  }

#ifndef __IOAPIC_DBG
  printf("Found %i I/O APIC(s). The base address of the I/O APIC is: 0x%x\n", i,
         (uint32_t) ioapic->address);
#endif
  return 0;
}

/*
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
    struct ioapic *io = create_ioapic(node->ioapic);
    for(ionode = ioapic; ionode != NULL, ionode != ionode->next; ionode = ionode->next)
    {
      if(ionode->next == NULL)
      {
        ionode->next = io;
        ionode->next->next = NULL;
        break;
      }
    }
    if(node == NULL || node->next == NULL)
      break;
  }
}

#ifdef __IOAPIC_DBG
void
ioapic_debug()
{
  printf("Testing I/O apic at address 0x%x\n", ioapic->address);
  printf("I/O apic version %x\n", ioapic_read_dword(ioapic, 0x1));
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
