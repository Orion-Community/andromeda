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
#include <mm/map.h>
#include <networking/rtl8168.h>
#include <sys/dev/pci.h>
#include <drivers/pci/msi.h>

struct ol_pci_node* pcidevs;

/*
#ifdef __PCI_DEBUG
static int show_pci_dev(ol_pci_iterate_dev_t);
#endif
*/

static int ol_pci_is_mf(ol_pci_iterate_dev_t dev);
static inline ol_pci_addr_t ol_pci_calculate_address(
                                ol_pci_iterate_dev_t,
                                uint16_t);
static int pci_add_list(ol_pci_iterate_dev_t itdev);
static uint32_t __ol_pci_read_dword(ol_pci_addr_t addr);
static void debug_pci_list();

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
          if (dev->hook(dev)) return 0;

        if (dev->func == 0)
          if (!ol_pci_is_mf(dev)) break;
      }
    }
  }
  return -1;
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
  /* initialise the list */
  pcidevs = kalloc(sizeof(*pcidevs));
  if(pcidevs == NULL)
    goto fail;

  pcidevs->next = NULL;
  pcidevs->previous = NULL;
  pcidevs->dev = NULL;

  ol_pci_iterate_dev_t dev = kalloc(sizeof (*dev));
  if(dev == NULL)
    goto fail;
#if 0
  dev->hook = &show_pci_dev;
#else
  dev->hook = &pci_add_list;
#endif
  ol_pci_iterate(dev);
  free(dev);

#ifdef __PCI_DEBUG
  debug_pci_list();
#endif
  return;

  fail:
  ol_dbg_heap();
  endProg();
}

static int
pci_add_list(ol_pci_iterate_dev_t itdev)
{
  ol_pci_id_t class = __ol_pci_read_dword(ol_pci_calculate_address(itdev,
          OL_PCI_REG_CLASS)); /* get class and sub class */
  ol_pci_id_t id = __ol_pci_read_dword(ol_pci_calculate_address(itdev,
          OL_PCI_REG_ID)); /* id and vendor id */

  struct pci_dev *dev = kalloc(sizeof(*dev));
  if (dev == NULL)
    goto fail;

  dev->device = itdev->device;
  dev->func = itdev->func;
  dev->bus = itdev->bus;
  dev->id = (id>>16)&0xffff;
  dev->vendorID = id&0xffff;
  dev->class = (class>>24)&0xff;
  dev->subclass = (class>>16)&0xff;
  dev->flags = ol_pci_is_mf(itdev);
  dev->read = &ol_pci_read_dword;

  /* we're at the top of the list */
  if(pcidevs->dev == NULL)
  {
    /*
     * this is the first time that this function is called, so the list should
     * be initialized
     */
    pcidevs->dev = dev;
    pcidevs->next = NULL;
    pcidevs->previous = NULL;
    goto end;
  }
  else
  {
    struct ol_pci_node *node;
    for(node = pcidevs; node != NULL && node != node->next; node = node->next)
    {
      if(node->next == NULL)
      {

        node->next = kalloc(sizeof(struct ol_pci_node));
        if(node->next == NULL)
          goto fail;

        node->next->dev = dev;
        node->next->next = NULL;
        node->next->previous = node;
        goto end;
      }
    }
  }

  /*
   * create the actual device which will be added to the list
   */
  end:
  return FALSE; /* we want to list all devices */

  fail:
  printf("Out of memory in pci_add_list!\n");
  ol_dbg_heap();
  endProg();
  return TRUE;

}

static void
print_pci_dev(uint16_t class, uint16_t subclass)
{
  switch (class)
  {
    case 0x1:
      if (!subclass)
        printf("PCI: Found SCSI bus controller\n");
      else if (subclass == 0x1)
        printf("PCI: Found IDE controller\n");
      else if (subclass == 0x2)
        printf("PCI: Found floppy disk controller\n");
      else if (subclass == 0x6)
        printf("PCI: Found SATA controller\n");
      break;

    case 0x2:
      if (!subclass)
        printf("PCI: Found ethernet controller\n");
      break;

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
      printf("Unknown class. Class %i - subclass %i\n", class, subclass);
      break;
  }
}

static void
init_device_control(struct pci_dev *dev)
{
  switch(dev->class)
  {
    case 0x2:
//       if(!dev->subclass)
//         init_rtl_device(dev);
      break;

    default:
      break;
  }
}

static uint32_t
__ol_pci_read_dword(ol_pci_addr_t addr)
{
  register uint32_t ret;
  outl(OL_PCI_CONFIG_ADDRESS, addr);
  iowait();
  ret = inl(OL_PCI_CONFIG_DATA);
  return ret;
}

#pragma GCC diagnostic ignored "-Wunused-function"
static uint8_t
__ol_pci_read_byte(ol_pci_addr_t addr, uint16_t reg)
{
  register uint8_t ret;
  outl(OL_PCI_CONFIG_ADDRESS, addr & ~3);
  ret = inb(OL_PCI_CONFIG_DATA + (reg & ~3));
  return ret;
}

uint32_t
ol_pci_read_dword(struct pci_dev* dev, uint16_t reg)
{
  return __ol_pci_read_dword(ol_pci_calculate_address((ol_pci_iterate_dev_t)dev,
                                                      reg));
}

static void
__ol_pci_write_dword(ol_pci_addr_t addr, uint32_t data)
{
  outl(OL_PCI_CONFIG_ADDRESS, addr);
  outl(OL_PCI_CONFIG_DATA, data);
}

inline void
ol_pci_write_dword(struct pci_dev* dev, uint16_t reg,
                                 uint32_t data)
{
  ol_pci_addr_t addr = ol_pci_calculate_address((ol_pci_iterate_dev_t)dev, reg);
  __ol_pci_write_dword(addr, data);
}

#ifdef __PCI_DEBUG
static void
debug_pci_print_cp_list(struct pci_dev * dev)
{
  uint32_t cp = ol_pci_read_dword(dev, 0x34); /* value of the current pointer */
  uint32_t cp_list = ol_pci_read_dword(dev, (uint16_t)cp&0xff); /* cp_list data */
  cp = (cp_list>>8)&0xff;
  for(; cp != 0x0; cp = (cp_list>>8)&0xff, cp_list =
      ol_pci_read_dword(dev, (uint16_t)cp&0xff))
  {
    if((cp_list & 0xff) == 0x5)
    {
//       msi_create_msix_entry(dev, cp);
      init_device_control(dev);
    }
    else
      continue;
  }
}

static void
debug_pci_list()
{
  struct ol_pci_node *node;
  for(node = pcidevs; node != NULL && node != node->next; node = node->next)
  {
    print_pci_dev(node->dev->class, node->dev->subclass);
    if(node->dev->class == 0x2)
    {
      debug_pci_print_cp_list(node->dev);
    }
    if(node->next == NULL)
      break;
  }
}

#endif
