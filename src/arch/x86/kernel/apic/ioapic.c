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
#include <arch/x86/cpu.h>

static volatile ioapic_t ioapic;

static int
create_ioapic (ol_madt_ioapic_t madt_io)
{
  ioapic = kalloc(sizeof (*ioapic));
  cpus->lock(&cpu_lock);
  if (ioapic == NULL) 
    goto nomem;
  else
  {
    ioapic->address = (ioapic_addr_t*) madt_io->address;
    ioapic->int_base = madt_io->global_system_interrupt_base;
    ioapic->id = madt_io->id;
    ioapic->read = &ioapic_read_dword;
    ioapic->write = &ioapic_write_dword;
  }
  cpus->unlock(&cpu_lock);
  return 0;
  
  nomem:
  ol_dbg_heap();
  panic("No free memory in heap in create_ioapic!");
}

int
init_ioapic ()
{
  ol_madt_ioapic_t madt_io = acpi_apics->ioapic->ioapic;
  if(madt_io == NULL)
    return -1;
  
  int i = 0;
  struct ol_madt_ioapic_node *node = acpi_apics->ioapic;
  for(node = acpi_apics->ioapic; node != NULL, node != node->next; 
      node = node->next)
  {
    i++;
#ifdef __APIC_DBG
    printf("APIC data len: %x\t%x\t%x\t%x\n", node->ioapic->address,node, node->next, 
           node->previous);
#endif
    if(node->next == NULL)
      break;
  }
  
      
  create_ioapic(madt_io);
  printf("Found %i I/O APICS. The base address of the I/O APIC is: 0x%x\n", i,
         (uint32_t) ioapic->address);
  return 0;
}

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
