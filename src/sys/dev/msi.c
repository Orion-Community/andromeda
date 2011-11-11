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
static struct msi *msgs = NULL;
boolean msi_enabled = FALSE;

int
msi_enable_msi()
{
  if(msgs == NULL)
    msgs = kalloc(sizeof(*msgs));
  if(msgs != NULL)
    msi_enabled = true;
  else return -1;
  
  return 0;
}

static int
__msi_write_message(struct msi *msi)
{
  if(msi->attrib.is_msix)
  {
    writel(msi->attrib.base + MSIX_LOW_ADDR, msi->addr.addr_low);
    writel(msi->attrib.base + MSIX_UPPER_ADDR, msi->addr.addr_hi);
    writel(msi->attrib.base + MSIX_MESSAGE_DATA, msi_get_msg_data(msi));
  }
  else
  {
    struct ol_pci_dev *dev = msi->attrib.dev;
    ol_pci_write_dword(dev, MSI_LOWER_ADDR(msi->attrib.cpos), msi->addr.addr_low);
    if(msi->attrib.is_64)
    {
      ol_pci_write_dword(dev, MSI_UPPER_ADDR(msi->attrib.cpos), msi->addr.addr_hi);
      ol_pci_write_dword(dev, MSI_MESSAGE_DATA(msi->attrib.cpos,1), msi_get_msg_data(msi));
    }
    else
      ol_pci_write_dword(dev, MSI_MESSAGE_DATA(msi->attrib.cpos,0),msi_get_msg_data(msi));
  }
  return 0;
}

static uint32_t
msi_get_msg_data(struct msi *msi)
{
  uint32_t ret = 0;
  ret |= msi->vector;
  ret |= (msi->delivery_mode)<<8;
  ret |= (msi->trigger_level)<<14;
  ret |= (msi->trigger)<<15;
  return ret;
}

static void
msi_add_config_data(struct msi *msi, uint32_t n)
{
  if(msgs == NULL)
  {
    msgs = kalloc(sizeof(*msi));
    msgs[n] = *msi;
  }
  else
  {
    struct msi *temp = msi_resize_msi_data(msi);
    msgs = temp;
  }
}

static struct msi*
msi_resize_msi_data(struct msi *msi)
{
  return NULL;
}

#ifdef MSIX
void
msi_create_msix_entry(struct ol_pci_dev *dev, uint8_t cp)
{
  if(!msi_enabled)
    msi_enable_msi();
    
  __msi_create_msix_entry(dev, cp);
}

/**
 * Enable an msix entry. The entry count is encoded as entry number N-1.
 */
static void
msi_enable_msix_entry(struct msi *msi, int entry)
{
  int index = entry*MSIX_ENTRY_SIZE;
  writel(msi->attrib.base+index+MSIX_VECTOR_CTRL, 0);
}

static int
__msi_create_msix_entry(struct ol_pci_dev *dev, uint8_t cp)
{
  struct msi *msi = kalloc(sizeof(*msi));
  msi->attrib.cpos = cp;
  msi->attrib.dev = dev;
  msi->attrib.is_msix = 1;
  msi->attrib.is_64 = 0;
  msi->attrib.base = msi_calc_msix_base(dev, cp);
  
  msi->addr.addr_low = 0xfee00000; /* upper bits of the msi address are always 0xfee */
  msi->addr.addr_hi = 0;
  
  __msi_write_message(msi);
  msi_enable_msix_entry(msi, 0); /* enable first entry */
  msi_add_config_data(msi, 0);
  debug_msix_entry(msi, cp);
  return 0;
}

static volatile void*
msi_calc_msix_base(struct ol_pci_dev *dev, uint8_t cp)
{
  uint32_t bar_nr = ol_pci_read_dword(dev, ((uint16_t)cp)+0x4) & 7;
  uint32_t bar = ol_pci_read_dword(dev, MSIX_BAR(bar_nr));
  if(bar & 1)
    bar &= 0xfffffffc;
  else
    bar &= 0xfffffff0;
  
  page_map_kernel_entry(bar,bar); /* map the address 1:1 */
  return (volatile void*)bar;
}

#ifdef MSIX_DEBUG
static void
debug_msix_entry(struct msi *msi, uint8_t cp)
{
  volatile void *base = msi->attrib.base;
  printf("test: 0x%x\n", msgs[0].attrib.base);
  uint16_t msi_ctl = (ol_pci_read_dword(msi->attrib.dev, (uint16_t)cp) >> 16) & 0x3ff;
    /* write and read back */

  printf("Found MSI-X entry; msg_addr: 0x%x; vector_ctrl: %i; cfg_space_size: %i\n",
      readl(base), readl(base+12), (msi_ctl+1)/4);
}
#endif
#endif
#endif
