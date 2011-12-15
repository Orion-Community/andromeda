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

#ifndef IOAPIC_H
#define	IOAPIC_H

#ifdef	__cplusplus
extern "C"
{
#endif

#define IOAPIC_DATA_ADDRESS(x) (volatile uint32_t*)(((volatile void*)x)+0x10)

typedef uint32_t ioapic_addr_t;
typedef struct ioapic
{
  uint8_t id;
  uint8_t apic_id:4;
  uint32_t int_base /* system interrupt base */, num_intr;
  volatile ioapic_addr_t* address;
  void (*write)(struct ioapic*, const uint8_t, const uint32_t);
  uint32_t (*read)(struct ioapic*, const uint8_t);
  struct ioapic *next;
  struct iopin *pin;
} *ioapic_t;

typedef struct iopin
{
  int vector : 8;
  int delmod : 3;
  int destmod : 1;
  int deliv_state : 1;
  int intpol : 1;
  int remote_irr : 1;
  int trig_mode : 1;
  int trig_mask : 1;
  uint64_t reserved : 39;
  int destfield : 8;
} *iopin_t;

static struct ioapic*
create_ioapic (ol_madt_ioapic_t madt_io);

int
init_ioapic();

static uint32_t ioapic_read_dword(ioapic_t io, const uint8_t offset);

static void add_ioapic();

static void
ioapic_write_dword(ioapic_t io, const uint8_t offset, const uint32_t value);

static int program_ioapic_pin(struct ioapic *io, int pin, int irq);
static int write_ioapic_pin(struct iopin *pin);

#ifdef __IOAPIC_DBG
void ioapic_debug();
#endif
#ifdef	__cplusplus
}
#endif

#endif	/* IOAPIC_H */
