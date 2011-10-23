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

#ifndef __PCI_H
#define __PCI_H

#ifdef	__cplusplus
extern "C"
{
#endif

#define OL_PCI_CONFIG_ADDRESS 0xcf8
#define OL_PCI_CONFIG_DATA 0xcfc

  /* PCI device structure register fields */
#define  OL_PCI_REG_ID           0x00
#define  OL_PCI_REG_VENDOR       0x00
#define  OL_PCI_REG_DEVICE       0x02
#define  OL_PCI_REG_COMMAND      0x04
#define  OL_PCI_REG_STATUS       0x06
#define  OL_PCI_REG_REVISION     0x08
#define  OL_PCI_REG_CLASS        0x08
#define  OL_PCI_REG_CACHELINE    0x0c
#define  OL_PCI_REG_LAT_TIMER    0x0d
#define  OL_PCI_REG_HEADER_TYPE  0x0e
#define  OL_PCI_REG_BIST         0x0f
#define  OL_PCI_REG_ADDRESSES    0x10

  /* PCI properties */
#define OL_PCI_MF 0x80
#define OL_PCI_NUM_BUS 256
#define OL_PCI_NUM_DEV 32
#define OL_PCI_NUM_FUNC 10-1-1 /* PCI has a max of 10 loads minus one for the
                                        the pci device, and the 
                                        connector also as one, leaving 8 
                                        functions. */

#define PCI_MECH_1 0x1
#define PCI_MECH_2 0x2

  typedef uint32_t ol_pci_addr_t;
  typedef uint32_t ol_pci_id_t;

  typedef struct ol_pci_iterate_dev
  {
    uint32_t func; /* device function */
    uint32_t device; /* device type */
    uint32_t bus; /* pci bus */
    int (*hook)(struct ol_pci_iterate_dev*);
  } *ol_pci_iterate_dev_t;

  typedef struct ol_pci_dev
  {
    uint32_t func; /* device function */
    uint32_t device; /* device type */
    uint32_t bus; /* pci bus */
    uint16_t id;
    uint16_t vendorID;
    uint16_t class;
    uint16_t subclass;
    uint32_t flags; /*
                     * bit 0: if set -> dev is mf
                     */
    uint32_t (*read)(struct ol_pci_dev*, uint16_t);
  } *ol_pci_dev_t;
  
  typedef struct ol_pci_node
  {
    struct ol_pci_node* next;
    struct ol_pci_node* previous;
    struct ol_pci_dev* dev;
  } *ol_pci_node_t;
  
  static int
  ol_pci_iterate();

  static ol_pci_addr_t
  ol_pci_calculate_address(ol_pci_iterate_dev_t, uint16_t);

  static int
  ol_pci_is_mf(ol_pci_iterate_dev_t);

  static ol_pci_iterate_dev_t
  ol_pci_init_device(ol_pci_iterate_dev_t);

  static int
  ol_pci_dev_exist(ol_pci_iterate_dev_t);

  void
  ol_pci_init();

  static int
  pci_add_list(ol_pci_iterate_dev_t);
  
#ifdef __PCI_DEBUG
  static int
  show_pci_dev(ol_pci_iterate_dev_t);
#endif

  static void
  print_pci_dev(uint16_t class, uint16_t subclass);
  
  /* PCI communication functions */
  static uint32_t
  __ol_pci_read_dword(ol_pci_addr_t);

  static uint8_t
  __ol_pci_read_byte(ol_pci_addr_t, uint16_t);
  
  inline uint32_t
  ol_pci_read_dword(struct ol_pci_dev* dev, uint16_t reg);
  
  static void
  debug_pci_list();
#ifdef	__cplusplus
}
#endif

#endif	/* PCI_H */

