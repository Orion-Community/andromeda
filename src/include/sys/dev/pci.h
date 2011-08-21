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
        
        typedef uint32_t ol_pci_addr_t;
        
        typedef struct ol_pci_dev
        {
		uint32_t func; /* device function */
		uint32_t device; /* device type */
		uint32_t bus;  /* pci bus */
		ol_pci_addr_t address;
        } *ol_pci_dev_t;

	static int
	ol_pci_iterate(int (*hook)(uint8_t, uint8_t, uint8_t));

	static void
	ol_pci_calculate_address(ol_pci_dev_t, uint16_t);

	static int
	ol_pci_get_function_number(ol_pci_dev_t);

	static ol_pci_dev_t
	ol_pci_init_device(ol_pci_dev_t);

	static int
	ol_pci_dev_exist(ol_pci_dev_t);

	void
	ol_pci_init();


#ifdef	__cplusplus
}
#endif

#endif	/* PCI_H */

