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
__msi_write_message(struct msi *msi)
{
  if(msi->attrib.is_msix)
  {
    writel(msi->attrib.base + MSIX_LOW_ADDR, msi->addr.addr_low);
    writel(msi->attrib.base + MSIX_UPPER_ADDR, msi->addr.addr_hi);
    writel(msi->attrib.base + MSIX_MESSAGE_DATA, msi_get_msg_data(msi));
  }
  else
    return -1;
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
#ifdef MSIX
void
msi_create_msix_entry(struct ol_pci_dev *dev, uint8_t cp)
{
  __msi_create_msix_entry(dev, cp);
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
  writel(msi->attrib.base + MSIX_VECTOR_CTRL, 0);
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
  uint16_t msi_ctl = (ol_pci_read_dword(msi->attrib.dev, (uint16_t)cp) >> 16) & 0x3ff;
    /* write and read back */

  printf("Found MSI-X entry; msg_addr: 0x%x; vector_ctrl: %i; cfg_space_size: %i\n",
      readl(base), readl(base+12), (msi_ctl+1)/4);
}
#endif
#endif
#endif
