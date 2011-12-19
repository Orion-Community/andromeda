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

static struct rtl_cfg *rtl_devs = NULL;

static void
print_mac(struct ol_pci_dev *dev)
{
  uint8_t mac[6];
  uint16_t base = get_rtl_port_base(dev);
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
  struct rtlcommand *cmd = kalloc(sizeof(*cmd));
  struct rtl_cfg *cfg = kalloc(sizeof(*cfg));
  cfg->next = NULL;
  print_mac(dev);
  uint16_t portbase = get_rtl_port_base(dev);
  if(cmd == NULL)
    return;

  cmd->ccommand.rxvlan = 1;
  cmd->ccommand.rxchecksum = 1;
  cmd->tx_enable = 1;
  cmd->rx_enable = 1;
  cmd->reset = 0;
  sent_command_registers(cmd, portbase);
  cfg->command = cmd;

  if(rtl_devs == NULL)
    rtl_devs = cfg;
  else
    add_rtl_device(cfg);

  read_command_registers(cmd, portbase);
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

static int
read_command_registers(struct rtlcommand *cmd, uint16_t port)
{
  uint16_t ccommand = inw(port+CPLUS_COMMAND_PORT_OFFSET);
  uint8_t command = inb(port+COMMAND_PORT_OFFSET);
  
  cmd->ccommand.rxvlan = (ccommand >> 5) & 1;
  cmd->ccommand.rxchecksum = (ccommand >> 6) & 1;
  cmd->tx_enable = (command >> 2) & 1;
  cmd->rx_enable = (command >> 3) & 1;
  cmd->reset = (command >> 4) & 1;
  
  printf("Tx Enable flag: %x - RxChecksum: %x\n", cmd->tx_enable, cmd->ccommand.rxchecksum);
  return 0;
}

static void
add_rtl_device(struct rtl_cfg *cfg)
{
  struct rtl_cfg *carriage = cfg;
  for(; carriage->next != NULL, carriage->next = carriage; 
            carriage = carriage->next)
  {
    if(carriage->next == NULL)
    {
      carriage->next = cfg;
      cfg->next = NULL;
      break;
    }
  }
}
