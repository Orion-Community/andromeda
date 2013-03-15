/*
 *   Andromeda Project - Message Signaled Interrupts
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
#include <drivers/pci/msi.h>
//#include <arch/x86/apic/apic.h>
#include <arch/x86/irq.h>
#include <arch/x86/idt.h>
#include <sys/dev/pci.h>
#include <mm/map.h>
#include <io.h>

#ifdef MSI

#ifdef MSIX
static uint32_t msi_convert_message(struct msi_msg *msg);
static int msi_build_message(struct msi *msi, uint32_t msg);
static int __msi_create_msix_entry(struct pci_dev*, uint8_t, struct irq_data*);
static volatile void* msi_calc_msix_base(struct pci_dev *dev, uint8_t cp);
#endif

static uint32_t msi_convert_message(struct msi_msg *msg);
static int msi_build_message(struct msi *msi, uint32_t msg);

static int
__msi_write_message(struct msi_cfg *cfg, struct msi *msi)
{
  uint32_t msg = msi_convert_message(&msi->msg);
  if(cfg->attrib.is_msix)
  {
    cfg->msix_write(cfg->attrib.base+MSIX_LOW_ADDR, msi->addr);
    if(cfg->attrib.is_64)
      cfg->msix_write(cfg->attrib.base+MSIX_UPPER_ADDR, msi->addr_hi);
    else
      cfg->msix_write(cfg->attrib.base+MSIX_UPPER_ADDR, MSI_HIGH_BASE_ADDRESS);
    cfg->msix_write(cfg->attrib.base+MSIX_MESSAGE_DATA, msg);
  }
  else
  {
    struct pci_dev *dev = cfg->dev;
    cfg->msi_write(dev, MSI_LOWER_ADDR(cfg->attrib.cpos), msi->addr);
    if(cfg->attrib.is_64)
    {
      cfg->msi_write(dev, MSI_UPPER_ADDR(cfg->attrib.cpos), msi->addr_hi);
      cfg->msi_write(dev, MSI_MESSAGE_DATA(cfg->attrib.cpos,1),msg);
    }
    else
      cfg->msi_write(dev, MSI_MESSAGE_DATA(cfg->attrib.cpos,0),msg);
  }
  return 0;
}

static int
__msi_read_message(struct msi_cfg *cfg, struct msi *msg)
{
  /**
   * \note I had to add initialiser to msi_data, verify value please.
   */
  uint32_t msi_data = 0;
  if(cfg->attrib.is_msix)
  {
    msg->addr = cfg->msix_read(cfg->attrib.base + MSIX_LOW_ADDR);
    if(cfg->attrib.is_64)
      msg->addr_hi = cfg->msix_read(cfg->attrib.base + MSIX_UPPER_ADDR);
    else
      msg->addr_hi = MSI_HIGH_BASE_ADDRESS;
    msi_data = cfg->msix_read(cfg->attrib.base + MSIX_MESSAGE_DATA);
  }
  else
  {
    struct pci_dev *dev = cfg->dev;
    msg->addr = cfg->msi_read(dev, MSI_LOWER_ADDR(cfg->attrib.cpos));
    if(cfg->attrib.is_64)
    {
      msg->addr_hi = cfg->msi_read(dev, MSI_UPPER_ADDR(cfg->attrib.cpos));
      msi_data = cfg->msi_read(dev, MSI_MESSAGE_DATA(cfg->attrib.cpos,1));
    }
    else
      cfg->msi_read(dev, MSI_MESSAGE_DATA(cfg->attrib.cpos,0));
  }
  msi_build_message(msg, msi_data);

  return 0;
}

#ifdef MSIX_DEBUG
static void
debug_msix_entry(struct msi_cfg *cfg)
{
  struct msi *msi = kalloc(sizeof(*msi));
  __msi_read_message(cfg, msi);
  /**
   * \note Again are these things really necessary?
  struct pci_dev *dev = cfg->dev;
  uint8_t irq = ol_pci_read_dword(dev, OL_PCI_INTERRUPT_LINE) & 0xff;
  volatile void *base = cfg->attrib.base;
  */

  uint16_t msi_ctl = (ol_pci_read_dword(cfg->dev, (uint16_t)cfg->attrib.cpos) >> 16) & 0x3ff;

  printf("msi-x: enabled: %x; base: 0x%x; msg_data: %x; cfg_space_size: %i\n",
      ol_pci_read_dword(cfg->dev, cfg->attrib.cpos)>>31, msi->addr, msi_convert_message(&msi->msg), (msi_ctl+1)/4);
  free(msi);
}
#endif

void
setup_msi_entry(struct pci_dev *dev, uint8_t cp)
{
#ifdef MSIX
  msi_create_msix_entry(dev, cp);
#endif
}

#ifdef MSIX
void
msi_create_msix_entry(struct pci_dev *dev, uint8_t cp)
{
  struct irq_data *irq = alloc_irq();
  irq->irq_base = (uint32_t)get_irq_base(40);
  install_irq_vector(irq);
  __msi_create_msix_entry(dev, cp, irq);
}

/**
 * Enable an msix entry. The entry count is encoded as entry number N-1.
 */
static void
msi_enable_msix_entry(struct msi_cfg *cfg, int entry)
{
  int index = entry*MSIX_ENTRY_SIZE;
  /* enable the vector */
  cfg->msix_write(cfg->attrib.base+index+MSIX_VECTOR_CTRL, 0);
  /* enable the msix message control */
  uint32_t msg_ctrl = ol_pci_read_dword(cfg->dev,
                                        MSI_MESSAGE_CONTROL(cfg->attrib.cpos));
  msg_ctrl |= 1<<31;
  ol_pci_write_dword(cfg->dev, MSI_MESSAGE_CONTROL(cfg->attrib.cpos), msg_ctrl);
}

static int
__msi_create_msix_entry(struct pci_dev *dev, uint8_t cp, struct irq_data *irq)
{
  struct msi_cfg *cfg = kalloc(sizeof(*cfg));
  struct msi *msi = kalloc(sizeof(*msi));
  /**
   * \note Is this variable here really necessary?
   uint16_t ctrl = ol_pci_read_dword(dev, cp) >> 16;
   */
  cfg->attrib.cpos = cp;
  cfg->dev = dev;
  cfg->attrib.is_msix = 1;
  cfg->attrib.is_64 = 1;
  cfg->attrib.base = msi_calc_msix_base(dev, cp);
  cfg->irq = irq->irq;

  msi->addr = MSI_LOWER_BASE_ADDRESS | MSI_ADDR_DEST_ID(0) |
              ((apic->delivery_mode == IRQ_LOW_PRI) ? MSI_ADDR_REDIR_LOWPRI :
                                        MSI_ADDR_REDIR_CPU) |
              ((apic->dest_mode == 0) ? MSI_ADDR_DEST_MODE_PHYSICAL :
                                        MSI_ADDR_DEST_MODE_LOGICAL);
  printf("address: %x + dest_mode %x\n", msi->addr, apic->dest_mode);
  msi->addr_hi = MSI_HIGH_BASE_ADDRESS;
  msi->msg.vector = irq->irq_config->vector;
  msi->msg.dm = irq->irq_config->delivery_mode;
  msi->msg.trig_lvl = TRIGGER_LEVEL_ASSERT;
  msi->msg.trigger = irq->irq_config->trigger;
  cfg->msi = msi;

  cfg->msix_read = &readl;
  cfg->msix_write = &writel;

  irq->irq_config->msi = cfg;
  __msi_write_message(cfg, msi);
  msi_enable_msix_entry(cfg, 0); /* enable first entry */

  debug_msix_entry(cfg);
  return 0;
}

static volatile void*
msi_calc_msix_base(struct pci_dev *dev, uint8_t cp)
{
  uint32_t bar_nr = ol_pci_read_dword(dev, ((uint16_t)cp)+0x4)&PCI_MEM_SPACE_MASK;
#ifdef X64
  uint64_t bar_low = ol_pci_read_dword(dev, MSIX_BAR(bar_nr));
  uint64_t bar;
  if(((bar_low & 0x6) >> 1) == 2)
  {
    uint64_t bar_hi = ol_pci_read_dword(dev, (MSIX_BAR(bar_nr)+4));
    bar = bar_low | (bar_hi << 32);
  }
  else
    bar = bar_low;
#else
  uint32_t bar = ol_pci_read_dword(dev, MSIX_BAR(bar_nr));
#endif
  if(bar & 1)
    bar &= ~3;
  else
    bar &= ~0xf;

  page_map_kernel_entry(bar,bar); /* map the address 1:1 */
  return (volatile void*)bar;
}

static uint32_t
msi_convert_message(struct msi_msg *msg)
{
  return (uint32_t)(MSI_VECTOR_DATA(msg->vector) | MSI_DELIVERY_MODE_DATA(msg->dm) |
          MSI_TRIGGER_LEVEL_DATA(msg->trig_lvl) | MSI_TRIGGER_DATA(msg->trigger));
}

static int
msi_build_message(struct msi *msi, uint32_t msg)
{
  msi->msg.vector = msg&0xff;
  msi->msg.dm = (msg >> 8) & 0x7;
  msi->msg.trig_lvl = (msg >> 14) & 1;
  msi->msg.trigger = (msg >> 15) & 1;
  return 0;
}

#endif
#endif
