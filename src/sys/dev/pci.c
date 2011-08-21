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
#include <sys/dev/pci.h>

#ifndef __PCI_H

static int
ol_pci_iterate(int (*hook)(uint8_t, uint8_t, uint8_t));
	
static int
ol_pci_get_function_number(ol_pci_dev_t);
	
static ol_pci_dev_t
ol_pci_init_device(ol_pci_dev_t);
	
static int
ol_pci_dev_exist(ol_pci_dev_t);

static void
ol_pci_calculate_address(ol_pci_dev_t dev, uint16_t reg)
{
	dev->address = (1 << 31) | (dev->bus << 16) | (dev->device << 11)
	(dev->function << 8) | ((offset & 0x3f) << 2) ((reg << 2) & 0x3f)
	return;
}


void
ol_pci_init();

#endif