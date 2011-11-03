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
  msi->attrib.base = msi_calc_msix_base(dev, cp);
  
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
  
  page_map_kernel_entry(bar,bar);
  return (volatile void*)bar;
}

#ifdef MSIX_DEBUG
static void
debug_msix_entry(struct msi *msi, uint8_t cp)
{
  volatile void *base = msi->attrib.base;
  uint16_t msi_ctl = (ol_pci_read_dword(msi->attrib.dev, (uint16_t)cp) >> 16) & 0x3ff;
    /* write and read back */
  writel(base, 0xfee00000);
  printf("Found MSI-X entry; msg_addr: 0x%x; vector_ctrl: %i; cfg_space_size: %i\n",
      readl(base), readl(base+12), (msi_ctl+1)/4);
}
#endif
#endif
#endif
