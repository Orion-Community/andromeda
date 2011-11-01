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
  int vector : 8;
  int delivery_mode : 3;
  int reserved : 3;
  int trigger_level : 1;
  int trigger: 1;
  uint64_t reserved2 : 48;
  struct msi_address *address;
  struct msi_attribute *attributes;
} __attribute__((packed));
typedef struct msi *msi_msg;

struct msi_attribute
{
  int is_64 : 1; /* 0 -> 32 bit addr bus, 1 -> 64 bit */
  int is_msix : 1; /* 0 -> no msix, 1 -> msi-x available */
  uint8_t cpos; /* position in the capabilities list */
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
/*
 * Setup an MSI driven irq.
 */
static int msi_create_message(struct ol_pci_dev*, uint32_t);

#endif
