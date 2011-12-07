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

#include <arch/x86/apic/msi.h>
#include <sys/dev/pci.h>
#include <mm/map.h>

#ifdef MSI
static int
__msi_write_message(struct msi_cfg *cfg, struct msi *msi)
{
  uint32_t msg = msi_convert_message(&msi->msg);
  printf("MSI message data: %x - IRQ vector: %x\n", msg, msi->msg.vector);
  if(cfg->attrib.is_msix)
  {
    writel(cfg->attrib.base + MSIX_LOW_ADDR, msi->addr);
    if(cfg->attrib.is_64)
      writel(cfg->attrib.base + MSIX_UPPER_ADDR, msi->addr_hi);
    writel(cfg->attrib.base + MSIX_MESSAGE_DATA, msg);
  }
  else
  {
    struct ol_pci_dev *dev = cfg->dev;
    ol_pci_write_dword(dev, MSI_LOWER_ADDR(cfg->attrib.cpos), msi->addr);
    if(cfg->attrib.is_64)
    {
      ol_pci_write_dword(dev, MSI_UPPER_ADDR(cfg->attrib.cpos), msi->addr_hi);
      ol_pci_write_dword(dev, MSI_MESSAGE_DATA(cfg->attrib.cpos,1),msg);
    }
    else
      ol_pci_write_dword(dev, MSI_MESSAGE_DATA(cfg->attrib.cpos,0),msg);
  }
  return 0;
}

static int
__msi_read_message(struct msi_cfg *cfg, struct msi *msg)
{
  uint32_t msi_data;
  if(cfg->attrib.is_msix)
  {
    msg->addr = readl(cfg->attrib.base + MSIX_LOW_ADDR);
    printf("%x\n", readl(cfg->attrib.base + MSIX_LOW_ADDR));
    if(cfg->attrib.is_64)
      msg->addr = readl(cfg->attrib.base + MSIX_UPPER_ADDR);
    msi_data = readl(cfg->attrib.base + MSIX_MESSAGE_DATA);
  }
  else
  {
    struct ol_pci_dev *dev = cfg->dev;
    msg->addr = ol_pci_read_dword(dev, MSI_LOWER_ADDR(cfg->attrib.cpos));
    if(cfg->attrib.is_64)
    {
      msg->addr_hi = ol_pci_read_dword(dev, MSI_UPPER_ADDR(cfg->attrib.cpos));
      msi_data = ol_pci_read_dword(dev, MSI_MESSAGE_DATA(cfg->attrib.cpos,1));
    }
    else
      ol_pci_read_dword(dev, MSI_MESSAGE_DATA(cfg->attrib.cpos,0));
  }
  msi_build_message(msg, msi_data);

  return 0;
}

void
setup_msi_entry(struct ol_pci_dev *dev, uint8_t cp)
{
#ifdef MSIX
  uint8_t msix = (uint8_t)(ol_pci_read_dword(dev, cp) & 0xff);
  //printf("%x\n", msix);
  __msi_create_msix_entry(dev, cp);
#endif
}

#ifdef MSIX
void
msi_create_msix_entry(struct ol_pci_dev *dev, uint8_t cp)
{
  __msi_create_msix_entry(dev, cp);
}

/**
 * Enable an msix entry. The entry count is encoded as entry number N-1.
 */
static void
msi_enable_msix_entry(struct msi_cfg *cfg, int entry)
{
  int index = entry*MSIX_ENTRY_SIZE;
  writel(cfg->attrib.base+index+MSIX_VECTOR_CTRL, 0);
}

static int
__msi_create_msix_entry(struct ol_pci_dev *dev, uint8_t cp)
{
  struct msi_cfg *cfg = kalloc(sizeof(*cfg));
  struct msi msi;
  uint16_t ctrl = ol_pci_read_dword(dev, cp) >> 16;
  cfg->attrib.cpos = cp;
  cfg->dev = dev;
  cfg->attrib.is_msix = 1;
  if(ctrl & (1 << 7))
    cfg->attrib.is_64 = 1;
  else
    cfg->attrib.is_64 = 0;
  cfg->attrib.base = msi_calc_msix_base(dev, cp);
  
  msi.addr = 0xfee00000; /* upper bits of the msi address are always 0xfee */
  msi.addr_hi = 0;
  msi.msg.vector = alloc_idt_entry();
  msi.msg.dm = 1;
  msi.msg.trig_lvl = 1;
  msi.msg.trigger = 0;
  cfg->msi = &msi;
  
  __msi_write_message(cfg, &msi);
  msi_enable_msix_entry(cfg, 0); /* enable first entry */
  
  debug_msix_entry(cfg);
  return 0;
}

static volatile void*
msi_calc_msix_base(struct ol_pci_dev *dev, uint8_t cp)
{
  uint32_t bar_nr = ol_pci_read_dword(dev, ((uint16_t)cp)+0x4) & 7;
#ifdef X64
  uint64_t bar_low = ol_pci_read_dword(dev, MSIX_BAR(bar_nr));
  uint64_t bar;
  if((bar_low & 0x6) == 2)
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

#ifdef MSIX_DEBUG
static void
debug_msix_entry(struct msi_cfg *cfg)
{
  struct msi *msi = kalloc(sizeof(*msi));
  __msi_read_message(cfg, msi);
  struct ol_pci_dev *dev = cfg->dev;
  uint8_t irq = ol_pci_read_dword(dev, OL_PCI_INTERRUPT_LINE) & 0xff;
  volatile void *base = cfg->attrib.base;
  //printf("test: 0x%x\n", irq);
  uint16_t msi_ctl = (ol_pci_read_dword(cfg->dev, (uint16_t)cfg->attrib.cpos) >> 16) & 0x3ff;

  printf("msi-x: 64: %x; base: 0x%x; msg_data: %x; cfg_space_size: %i\n",
      cfg->attrib.is_64, msi->addr, msi_convert_message(&msi->msg), (msi_ctl+1)/4);
  free(msi);
}
#endif
#endif
#endif
