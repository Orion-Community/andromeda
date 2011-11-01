/*
 *   Andromeda Kernel - MSI
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

/*
 * MSI's are Message Signaled Interrupts; interrupts sent to the apic using
 * memory i/o.
 */

#include <stdlib.h>
#include <types.h>
#include <sys/dev/pci.h>

#ifndef __MSI_H
#define __MSI_H

struct msi
{
  uint32_t addr_low, addr_hi;
  int vector : 8;
  int delivery_mode : 3;
  int reserved : 3;
  int trigger_level : 1;
  int trigger: 1;
  uint64_t reserved2 : 48;
} __attribute__((packed));
typedef struct msi *msi_msg;

typedef struct msi_attribute
{
  int is_64 : 1; /* 0 -> 32 bit addr bus, 1 -> 64 bit */
  int is_msix : 1; /* 0 -> no msix, 1 -> msi-x available */

} *msi_attribute_t;

struct msi_address
{
  uint32_t reserved : 2;
  uint32_t dest_mode : 1;
  uint32_t redir_hint : 1;
  uint32_t reserved2 : 8;
  uint32_t dest_id : 8;
  uint32_t reserved3 : 12;
}
/*
 * Setup an MSI driven irq.
 */
static int setup_msi_irq(struct ol_pci_dev*);

#endif
