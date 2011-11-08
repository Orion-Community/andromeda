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

/* MSI definitions */
#define MSI_LOWER_ADDR(x) ((x)+4)
#define MSI_UPPER_ADDR(x) ((x)+8)
#define MSI_MESSAGE_DATA(x,y) (y)?((x)+12):((x)+8)

/* MSIX definitions */
#define MSIX_BAR(index) ((4*(index))+0x10)
#define MSIX_ENTRY_SIZE 16

#define MSIX_LOW_ADDR 0
#define MSIX_UPPER_ADDR 4
#define MSIX_MESSAGE_DATA 8
#define MSIX_VECTOR_CTRL 12

struct msi_attribute
{
  int is_64 : 1; /* 0 -> 32 bit addr bus, 1 -> 64 bit */
  int is_msix : 1; /* 0 -> no msix, 1 -> msi-x available */
  uint8_t multiple : 3;
  int enabled : 1; /* 0 -> not enabled, 1 -> enabled (i.e. can send interrupts) */
  uint8_t cpos; /* position in the capabilities list */
  
  union { 
    volatile void *base; 
    uint8_t mask_position; 
  };
} __attribute__((packed));

struct msi
{
  uint32_t addr;
  uint32_t addr_hi;
  uint32_t message;
} __attribute__((packed));
typedef struct msi *msi_msg;

struct msi_cfg
{
  struct msi_attribute attrib;
  struct ol_pci_dev * dev;
  struct msi *msi;
  int masked : 1;
  uint32_t irq;
} __attribute__((packed));

/*
 * Setup an MSI driven irq.
 */
static int __msi_create_msix_entry(struct ol_pci_dev*, uint8_t);
static volatile void *msi_calc_msix_base(struct ol_pci_dev *, uint8_t);
static int __msi_write_message(struct msi_cfg *, struct msi *);
void msi_create_msix_entry(struct ol_pci_dev *dev, uint8_t cp);
static void msi_enable_msix_entry(struct msi_cfg *, int);

#ifdef MSIX_DEBUG
static void debug_msix_entry(struct msi_cfg *, struct msi *);
#endif

#endif
