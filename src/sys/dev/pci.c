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

#include <mm/heap.h>

static int
ol_pci_iterate(ol_pci_dev_t dev)
{
        ol_pci_id_t id;
        register ol_pci_addr_t addr;
        uint32_t header;
        
        for(dev->bus = 0; dev->bus < OL_PCI_NUM_BUS; dev->bus++)
        {
                /* iterate through all busses */
                for(dev->device = 0; dev->device < OL_PCI_NUM_DEV; dev->device++)
                {
                        /* Looping trough all devices */
                        for(dev->func = 0; dev->func < OL_PCI_NUM_FUNC; 
                                dev->func++) 
                        {
                                addr = ol_pci_calculate_address(dev, 
                                                                OL_PCI_REG_ID);
                                id = ol_pci_read_dword(addr);
                                if((id >> 16) == 0xffff)
                                { /* functions must implement function 0 */
                                        
                                        if(dev->func == 0) break;
                                        else continue;

                                }
                                if(dev->hook(dev)) return;
                                
                                /* if the device isn't multi function -> bailout */
                                addr = ol_pci_calculate_address(dev, 
                                                        OL_PCI_REG_CACHELINE);
                                header = ol_pci_read_dword(addr);
                                if(!ol_pci_is_mf(dev)) break;
                        }
                }
        }
}
	
static int
ol_pci_is_mf(ol_pci_dev_t dev)
{
        ol_pci_id_t header = ol_pci_read_dword(ol_pci_calculate_address(
                dev, OL_PCI_REG_CACHELINE));
        if((header >> 16) & OL_PCI_MF) return TRUE;
        else return FALSE;
}

static ol_pci_addr_t
ol_pci_calculate_address(ol_pci_dev_t dev, uint16_t reg)
{
        register ol_pci_addr_t x = ((1 << 31) | (dev->bus << 16) | 
                (dev->device << 11) | (dev->func << 8) | (reg & 0xfc));
        
        return x;
}

void
ol_pci_init()
{
        ol_pci_dev_t dev = kalloc(sizeof(struct ol_pci_dev));
#ifdef __DEBUG
        dev->hook = &ol_pci_dbg_print_info;
#endif
        ol_pci_iterate(dev);
        free(dev);
}

#ifdef __DEBUG
static int 
ol_pci_dbg_print_info(ol_pci_dev_t dev)
{
        ol_pci_id_t id = ol_pci_read_dword(ol_pci_calculate_address(
                          dev, OL_PCI_REG_CLASS));
        id >>= 16;
        printnum((uint32_t)id, 16, FALSE, FALSE);
        print("  -  ");

        id = ol_pci_read_dword(ol_pci_calculate_address(
                        dev, OL_PCI_REG_ID));
        id &= 0xffff;
        printnum((uint32_t)id, 16, FALSE, FALSE);
        putc(0xa);

        return FALSE; /* we want to list all devices */

}
#endif
