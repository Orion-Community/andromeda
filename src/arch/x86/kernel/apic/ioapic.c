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

static volatile ioapic_t ioapic;

static int
create_ioapic(ol_madt_ioapic_t madt_io)
{
  ioapic = kalloc(sizeof(*ioapic));

  if(ioapic != NULL)
  {
    ioapic->address = madt_io[0].address;
    ioapic->int_base = madt_io[0].global_system_interrupt_base;
    ioapic->id = madt_io[0].id;
  }
  else
    return -1;
}

int
init_ioapic()
{
  ol_madt_ioapic_t madt_io = ol_acpi_get_ioapic();
  create_ioapic(madt_io);
  printf("The address of the I/O APIC is: 0x%x\n", ioapic->address);
}
