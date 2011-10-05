/*
 *   OpenLoader - PCI-BUS Controller
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

#include <mm/heap.h>

#include <sys/dev/pci.h>

struct ol_pci_node* pcidevs = NULL;
int iterate = 0;

static int
ol_pci_iterate(ol_pci_iterate_dev_t dev)
{
  ol_pci_id_t id;
  register ol_pci_addr_t addr;

  for (dev->bus = 0; dev->bus < OL_PCI_NUM_BUS; dev->bus++)
  {
    /* iterate through all busses */
    for (dev->device = 0; dev->device < OL_PCI_NUM_DEV; dev->device++)
    {
      /* Looping trough all devices */
      for (dev->func = 0; dev->func < OL_PCI_NUM_FUNC;
           dev->func++)
      {
        addr = ol_pci_calculate_address(dev,
                OL_PCI_REG_ID);
        id = __ol_pci_read_dword(addr);
        if ((id >> 16) == 0xffff)
          continue;

        if (dev->hook != NULL)
          if (dev->hook(dev)) return;

        if (dev->func == 0)
          if (!ol_pci_is_mf(dev)) break;
      }
    }
  }
}

static int
ol_pci_is_mf(ol_pci_iterate_dev_t dev)
{
  ol_pci_id_t header = __ol_pci_read_dword(ol_pci_calculate_address(
          dev, OL_PCI_REG_CACHELINE));
  if ((header >> 16) & OL_PCI_MF) return TRUE;
  else return FALSE;
}

static inline ol_pci_addr_t
ol_pci_calculate_address(ol_pci_iterate_dev_t dev, uint16_t reg)
{
  return ((1 << 31)
          | (dev->bus << 16) | (dev->device << 11)
          | (dev->func << 8) | reg);
}

void
ol_pci_init()
{
  iterate = 0;
  ol_pci_iterate_dev_t dev = kalloc(sizeof (*dev));
  if(dev == NULL)
    goto fail;
  dev->hook = &pci_add_list;
  ol_pci_iterate(dev);
  free(dev);
  return;
  
  fail:
  ol_dbg_heap();
  endProg();
}

static int
pci_add_list(ol_pci_iterate_dev_t itdev)
{
  iterate++;
  ol_pci_id_t class = __ol_pci_read_dword(ol_pci_calculate_address(itdev,
          OL_PCI_REG_CLASS)); /* get class and sub class */
  ol_pci_id_t id = __ol_pci_read_dword(ol_pci_calculate_address(itdev,
          OL_PCI_REG_ID)); /* id and vendor id */
  /* 
   * we don't have to check the values anymore since the iterator did that for
   * us..
   */
  if(pcidevs == NULL)
  {
    /* 
     * this is the first time that this function is called, so the list should
     * be initialized
     */
    pcidevs = kalloc(sizeof(*pcidevs));
    if(pcidevs == NULL) goto fail;
    pcidevs->next = NULL;
    pcidevs->previous = NULL;
    pcidevs->dev = NULL;
  }
  else
  {
    struct ol_pci_node * node = kalloc(sizeof(*node));
    if (node == NULL) goto fail;
  }
  
  struct ol_pci_dev * dev = kalloc(sizeof(*dev));
  if (dev == NULL) goto fail;
  dev->device = itdev->device;
  dev->func = itdev->func;
  dev->bus = itdev->bus;
  dev->id = (id>>16)&0xffff;
  dev->vendorID = id&0xffff;
  dev->class = (class>>24)&0xff;
  dev->subclass = (class>>16)&0xff;
  dev->flags = ol_pci_is_mf(itdev);
  dev->read = &ol_pci_read_dword;
  
//   init list here
  
  /*
   * create the actual device which will be added to the list
   */
  return FALSE; /* we want to list all devices */
  fail:
  printf("Iterate number: %i\n", iterate);
  ol_dbg_heap();
  endProg();
  return TRUE;

}

#ifdef __PCI_DEBUG
static int
show_pci_dev(ol_pci_iterate_dev_t dev)
{
  ol_pci_id_t class = ol_pci_read_dword(ol_pci_calculate_address(dev,
          OL_PCI_REG_CLASS));
  uint8_t subclass = (class >> 16) & 0xff;
  class >>= 24;

  switch (class)
  {
    case 0x1:
      if (!subclass)
        printf("PCI: Found SCSI bus controller\n");
      else if (subclass == 0x1)
        printf("PCI: Found IDE controller\n");
      else if (subclass == 0x2)
        printf("PCI: Found floppy disk controller\n");
      else if (subclass == 0x5)
        printf("PCI: Found SATA controller\n");
      break;

    case 0x2:
      if (!subclass)
        printf("PCI: Found ethernet controller\n");

    case 0x3:
      if (!subclass)
        printf("PCI: Found VGA controller\n");
      break;

    case 0x6: /* bridge */
      if (!subclass)
        printf("PCI: Found host bridge\n");
      else if (subclass == 0x1)
        printf("PCI: Found ISA bridge\n");
      else if (subclass == 0x4)
        printf("PCI: Found PCI-to-PCI bridge\n");
      break;

    default:
      break;
  }

  return 0; /* list all devices */
}
#endif

static uint32_t
__ol_pci_read_dword(ol_pci_addr_t addr)
{
  register uint32_t ret;
  outl(OL_PCI_CONFIG_ADDRESS, addr);
  iowait();
  ret = inl(OL_PCI_CONFIG_DATA);
  return ret;
}

static uint8_t
__ol_pci_read_byte(ol_pci_addr_t addr, uint16_t reg)
{
  register uint8_t ret;
  outl(OL_PCI_CONFIG_ADDRESS, addr & ~3);
  ret = inb(OL_PCI_CONFIG_DATA + (reg & 3));
  return ret;
}

inline uint32_t
ol_pci_read_dword(struct ol_pci_dev* dev, uint16_t reg)
{
  return __ol_pci_read_dword(ol_pci_calculate_address((ol_pci_iterate_dev_t)dev, 
                                                      reg));
}
