/*
 *   OpenLoader - RealTek network card driver
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
#include <networking/rtl8168.h>

void
print_mac(struct ol_pci_dev *dev)
{
  uint8_t mac[6];
  uint16_t base = (uint16_t)(ol_pci_read_dword(dev, 0x10)&PCI_IO_SPACE_MASK);
  printf("RealTek base: %x\n", base);
  int i = 0;
  for(; i < 6; i++)
  {
    mac[i] = inb(base+i);
  }
  printf("MAC address: ");
  
  for(i = 0; i<5; i++)
    printf("%x:", mac[i]);

  printf("%x\n", mac[5]);
}

void init_rtl_device(struct ol_pci_dev *dev)
{
  
}

static void 
sent_command_registers(struct rtlcommand *cmd, uint16_t port)
{
  /* first of all we have to sent the C+ command register */
  uint16_t ccommand = (cmd->ccommand.rxvlan << 5) | 
                          (cmd->ccommand.rxchecksum << 6);
  outw(port+CPLUS_COMMAND_PORT_OFFSET, ccommand);
  
  /*
   * then the normal command register has to be sent to the device. when that is
   * done, we can continue configuring other registers.
   */
  uint8_t command = (cmd->tx_enable << 2) | (cmd->rx_enable << 3) | 
                        (cmd->reset << 4);
  outb(port+COMMAND_PORT_OFFSET, command);
}
