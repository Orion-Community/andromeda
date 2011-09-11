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
                                        the pci device counts as one, and the 
                                        connector also as one, leaving 8 
                                        functions. */
        
        typedef uint32_t ol_pci_addr_t;
        typedef uint32_t ol_pci_id_t;
        
        typedef struct ol_pci_dev
        {
		uint32_t func; /* device function */
		uint32_t device; /* device type */
		uint32_t bus;  /* pci bus */
                int (*hook)(struct ol_pci_dev*);
        } *ol_pci_dev_t;
        

	static int
	ol_pci_iterate();

	static ol_pci_addr_t
	ol_pci_calculate_address(ol_pci_dev_t, uint16_t);

        static int
        ol_pci_is_mf(ol_pci_dev_t);

	static ol_pci_dev_t
	ol_pci_init_device(ol_pci_dev_t);

	static int
	ol_pci_dev_exist(ol_pci_dev_t);

	void
	ol_pci_init();
        
        static int 
        ol_pci_dbg_print_info(ol_pci_dev_t dev);
        
        /* PCI communication functions - They are inline (something like assembly
         macro's */
        static inline uint32_t
        ol_pci_read_dword(ol_pci_addr_t addr)
        {
                register uint32_t ret;
                outl(OL_PCI_CONFIG_ADDRESS, addr);
                ret = inl(OL_PCI_CONFIG_DATA);
                return ret;
        }
        
        static inline uint8_t
        ol_pci_read_byte(ol_pci_addr_t addr)
        {
                register uint8_t ret;
                outl(OL_PCI_CONFIG_ADDRESS, addr & ~3);
                ret = inb(OL_PCI_CONFIG_DATA);
                return ret;
        }

#ifdef	__cplusplus
}
#endif

#endif	/* PCI_H */

