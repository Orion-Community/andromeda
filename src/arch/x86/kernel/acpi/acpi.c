/*
 *   The OpenLoader project - ACPI interface
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

#include <sys/sys.h>

#include <mm/map.h>

#include <arch/x86/acpi/acpi.h>

struct acpi_apic_lists *acpi_apics = NULL;
static void ol_acpi_enumerate(uint8_t type, acpi_enum_hook_t hook);
static void acpi_apic_add_list(void *apic);
static void acpi_ioapic_add_list(void *io);

int
acpi_init()
{
  ol_get_system_tables();
  acpi_apics = kmalloc(sizeof(*acpi_apics));
  if (acpi_apics == NULL)
          panic("Out of memory!");

  /* reserve memory for at least one apic and i/o apic */
  acpi_apics->apic = kmalloc(sizeof(struct ol_madt_apic_node));
  if (acpi_apics->apic == NULL)
          panic("Out of memory!");
  acpi_apics->apic->next = NULL;
  acpi_apics->apic->previous = NULL;
  acpi_apics->apic->apic = NULL;

  acpi_apics->ioapic = kmalloc(sizeof(*(acpi_apics->ioapic)));
  if (acpi_apics->ioapic == NULL)
          panic("Out of memory!");
  acpi_apics->ioapic->next = NULL;
  acpi_apics->ioapic->previous = NULL;
  acpi_apics->ioapic->ioapic = NULL;

  ol_acpi_enumerate(0, &acpi_apic_add_list);
  ol_acpi_enumerate(1, &acpi_ioapic_add_list);
  return 0;
}

static ol_acpi_madt_t
ol_acpi_get_madt()
{
  if(systables->magic != SYS_TABLE_MAGIC)
  {
    if(ol_get_system_tables())
      return NULL;
    else if(systables->magic != SYS_TABLE_MAGIC)
      return NULL;
  }

  ol_acpi_rsdt_t rsdt = (void*) systables->rsdp->rsdt;

  void * table;

  page_map_kernel_entry((addr_t)rsdt, (addr_t)rsdt);
  uint32_t len = (rsdt->length - sizeof (*rsdt)) / 4, i = 0; /* default length */
  for (table = (void*) rsdt + sizeof (*rsdt); i < len; i++, table += 4)
  {
    page_map_kernel_entry((addr_t)(*(void**)table), (addr_t)(*(void**)table));
                                                           /*
                                                            * these addresses
                                                            * should be mapped
                                                            * 1:1.
                                                            */
    if (!memcmp((void*) *((uint32_t*) table), "APIC", 4))
    {
      ol_acpi_madt_t madt = (ol_acpi_madt_t) *((uint32_t*) table);
      int i = 0;
      uint8_t checksum = 0;
      for(; i < madt->length; i++)
      {
        checksum += *(((uint8_t*)madt)+i); /* check the checksum */
      }
      if(!checksum)
        return (ol_acpi_madt_t) *((uint32_t*) table);
      else
        return NULL;
    }
  }
  return(NULL);
}

static void
ol_acpi_enumerate(uint8_t type, acpi_enum_hook_t hook)
{
  ol_acpi_madt_t madt = ol_acpi_get_madt();
  ol_madt_field_header_t header;

  for (header = ((void*) madt) + sizeof (*madt); (void*) header < ((void*) madt) +
       madt->length; header = (ol_madt_field_header_t) (((void*) header) +
                                                        header->length))
  {
    if (header->type == type) /* processor apics have type number 0 */
    {
      hook((void*)header);
    }
  }
  return;
}

static void
acpi_apic_add_list(void *apic)
{
    /* we're at the top of the list, initialise it */
  if(acpi_apics->apic->apic == NULL)
  {
    acpi_apics->apic->apic = (struct ol_madt_apic*)apic;
    goto end;
  }
  else
  {
    struct ol_madt_apic_node *carriage;
    for(carriage = acpi_apics->apic; carriage != NULL && carriage != carriage->next;
      carriage = carriage->next)
    {
      if(carriage->next == NULL)
      {
        carriage->next = kmalloc(sizeof(struct ol_madt_apic_node));
        carriage->next->previous = carriage;
        carriage->next->next = NULL;
        carriage->next->apic = (struct ol_madt_apic*)apic;
        goto end;
      }
    }
  }
  end:
  return;
}

static void
acpi_ioapic_add_list(void *io)
{
  /* we're at the top of the list, initialise it */
  if(acpi_apics->ioapic->ioapic == NULL)
  {
    acpi_apics->ioapic->ioapic = (struct ol_madt_ioapic*)io;
    acpi_apics->ioapic->next = NULL;
    acpi_apics->ioapic->previous = NULL;
    goto end;
  }
  else
  {
    struct ol_madt_ioapic_node *carriage;
    for(carriage = acpi_apics->ioapic; carriage != NULL && carriage != carriage->next;
      carriage = carriage->next)
    {
      if(carriage->next == NULL)
      {
        carriage->next = kmalloc(sizeof(struct ol_madt_ioapic_node));
        carriage->next->previous = carriage;
        carriage->next->next = NULL;
        carriage->next->ioapic = (struct ol_madt_ioapic*)io;
        goto end;
      }
    }
  }
  end:
  return;
}

void*
ol_acpi_get_ioapic()
{return NULL;}
