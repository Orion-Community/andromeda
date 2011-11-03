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

#define MSIX_BAR(index) ((4*index)+0x10)

struct msi_attribute
{
  int is_64 : 1; /* 0 -> 32 bit addr bus, 1 -> 64 bit */
  int is_msix : 1; /* 0 -> no msix, 1 -> msi-x available */
  uint8_t cpos; /* position in the capabilities list */
  
  union { volatile void *base; uint8_t base_mask; };
  struct ol_pci_dev *dev;
} __attribute__((packed));

struct msi_address
{
  int reserved : 2;
  int dest_mode : 1;
  int redir_hint : 1;
  int reserved2 : 8;
  int dest_id : 8;
  int reserved3 : 12;
  uint32_t addr_hi;
} __attribute__((packed));

struct msi
{
  int vector : 8;
  int delivery_mode : 3;
  int reserved : 3;
  int trigger_level : 1;
  int trigger: 1;
  uint64_t reserved2 : 48;
  struct msi_address addr;
  struct msi_attribute attrib;
} __attribute__((packed));
typedef struct msi *msi_msg;

/*
 * Setup an MSI driven irq.
 */
static int __msi_create_msix_entry(struct ol_pci_dev*, uint8_t);
static volatile void *msi_calc_msix_base(struct ol_pci_dev *, uint8_t);
void msi_create_msix_entry(struct ol_pci_dev *dev, uint8_t cp);

#ifdef MSIX_DEBUG
static void debug_msix_entry(struct msi*, uint8_t);
#endif

#endif
