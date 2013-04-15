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

#ifndef __MSI_H
#define __MSI_H

#include <arch/x86/irq.h>
#include <stdlib.h>
#include <types.h>
#include <sys/dev/pci.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MSI definitions */
#define MSI_LOWER_ADDR(x) ((x)+4)
#define MSI_UPPER_ADDR(x) ((x)+8)
#define MSI_MESSAGE_DATA(x,y) (y)?((x)+12):((x)+8)
#define MSI_MESSAGE_CONTROL(x) (x)

/* Message shift defines */
#define MSI_VECTOR_SHIFT 0
#define MSI_VECTOR_DATA(x) (((x) & MSI_VECTOR_MASK) << MSI_VECTOR_SHIFT)
#define MSI_VECTOR_MASK 0xff

#define MSI_DELIVERY_MODE_SHIFT 8
#define MSI_DELIVERY_MODE_DATA(x) (((x) & MSI_DELIVERY_MODE_MASK) << \
                                            MSI_DELIVERY_MODE_SHIFT)
#define MSI_DELIVERY_MODE_MASK 7

#define MSI_TRIGGER_LEVEL_SHIFT 14
#define MSI_TRIGGER_LEVEL_DATA(x) (((x) & MSI_TRIGGER_LEVEL_MASK) << \
                                            MSI_TRIGGER_LEVEL_SHIFT)
#define MSI_TRIGGER_LEVEL_MASK 1

#define MSI_TRIGGER_SHIFT 15
#define MSI_TRIGGER_DATA(x) (((x) & MSI_TRIGGER_MASK) << MSI_TRIGGER_SHIFT)
#define MSI_TRIGGER_MASK 1

#define MSI_LOWER_BASE_ADDRESS 0xfee00000
#define MSI_HIGH_BASE_ADDRESS 0x0

#define MSI_ADDR_DEST_MODE_PHYSICAL_SHIFT 2
#define MSI_ADDR_DEST_MODE_PHYSICAL_MASK 1
#define MSI_ADDR_DEST_MODE_PHYSICAL (((0) & MSI_ADDR_DEST_MODE_PHYSICAL_MASK) << \
                                          MSI_ADDR_DEST_MODE_PHYSICAL_SHIFT)

#define MSI_ADDR_DEST_MODE_LOGICAL_SHIFT 2
#define MSI_ADDR_DEST_MODE_LOGICAL_MASK 1
#define MSI_ADDR_DEST_MODE_LOGICAL (((1) & MSI_ADDR_DEST_MODE_LOGICAL_MASK) << \
                                          MSI_ADDR_DEST_MODE_LOGICAL_SHIFT)

#define MSI_ADDR_REDIR_HINT_SHIFT 3
#define MSI_ADDR_REDIR_HINT_MASK 1
#define MSI_ADDR_REDIR_CPU (((0) & MSI_ADDR_REDIR_HINT_MASK) << \
                                          MSI_ADDR_REDIR_HINT_SHIFT)
#define MSI_ADDR_REDIR_LOWPRI (((1) & MSI_ADDR_REDIR_HINT_MASK) << \
                                          MSI_ADDR_REDIR_HINT_SHIFT)

#define MSI_ADDR_DEST_ID_SHIFT 12
#define MSI_ADDR_DEST_ID_MASK 0xff
#define MSI_ADDR_DEST_ID(x) (((x) & MSI_ADDR_DEST_ID_MASK) << \
                                MSI_ADDR_DEST_ID_SHIFT)

/* MSIX definitions */
#define MSIX_BAR(index) ((4*(index))+0x10)
#define MSIX_ENTRY_SIZE 16

#define MSIX_LOW_ADDR 0
#define MSIX_UPPER_ADDR 4
#define MSIX_MESSAGE_DATA 8
#define MSIX_VECTOR_CTRL 12

struct msi_attribute
{
  uint is_64 : 1; /* 0 -> 32 bit addr bus, 1 -> 64 bit */
  uint is_msix : 1; /* 0 -> no msix, 1 -> msi-x available */
  uint multiple : 3;
  uint enabled : 1; /* 0 -> not enabled, 1 -> enabled (i.e. can send interrupts) */
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
  struct msi_msg
  {
    uint vector : 8;
    uint dm : 3;
    uint trig_lvl : 1;
    uint trigger : 1;
  } msg;
} __attribute__((packed));

struct msi_cfg
{
  struct msi_attribute attrib;
  struct pci_dev * dev;
  struct msi *msi;

  union
  {
    void (*msix_write) (volatile void*, uint32_t);
    void (*msi_write) (struct pci_dev*, uint16_t, uint32_t);
  };

  union
  {
    uint32_t (*msix_read) (volatile void*);
    uint32_t (*msi_read) (struct pci_dev*, uint16_t);
  };

  int masked : 1;
  uint32_t irq;
} __attribute__((packed));

/*
 * Setup an MSI driven irq.
 */
/*
static int __msi_create_msix_entry(struct pci_dev *, uint8_t, struct irq_data *);
static volatile void *msi_calc_msix_base(struct pci_dev *, uint8_t);
static uint32_t msi_convert_message(struct msi_msg *msg);
static int msi_build_message(struct msi *msi, uint32_t msg);
static int __msi_write_message(struct msi_cfg *, struct msi *);
static int __msi_read_message(struct msi_cfg *cfg, struct msi *msg);
*/
void msi_create_msix_entry(struct pci_dev *dev, uint8_t cp);
//static void msi_enable_msix_entry(struct msi_cfg *, int);
void setup_msi_entry(struct pci_dev *, uint8_t);

#if 0
#ifdef MSIX_DEBUG
static void debug_msix_entry(struct msi_cfg *);
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif
