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
#include <networking/net.h>
#include <andromeda/drivers.h>

static struct rtl_cfg *rtl_devs = NULL;

static void
get_mac(struct pci_dev *dev, struct netdev *netdev)
{
  uint8_t mac[MAC_ADDR_SIZE];
  uint16_t base = get_rtl_port_base(dev, 0);
  int i = 0;
  for(; i < 6; i++)
  {
    mac[i] = inb(base+i);
  }
  memcpy(netdev->hwaddr, mac, MAC_ADDR_SIZE);
  printf("MAC address: ");

  for(i = 0; i<5; i++)
    printf("%x:", netdev->hwaddr[i]);

  printf("%x\n", netdev->hwaddr[5]);
}

void init_rtl_device(struct pci_dev *dev)
{
  struct rtlcommand *cmd = kalloc(sizeof(*cmd));
  struct rtl_cfg *cfg = kalloc(sizeof(*cfg));
  cfg->next = NULL;

  int i = 0;
  uint16_t portbase;
  do
  {
    portbase = get_rtl_port_base(dev, i*4);
    i++;
  } while(portbase == 0 && i <= 5);
  debug("RealTek base: %x\n", portbase);
  cfg->portbase = portbase;
  
  cfg->raw_rx_buff = kalloc(RX_BUFFER_SIZE);
  cfg->rx_buff_length = RX_BUFFER_SIZE;
  cfg->raw_tx_buff = kalloc(TX_BUFFER_SIZE);
  cfg->tx_buff_length = TX_BUFFER_SIZE;

  if(cmd == NULL)
    return;

  if(rtl_devs == NULL)
    rtl_devs = cfg;
  else
    add_rtl_device(cfg);

  reset_rtl_device(cfg);
  cmd->ccommand.rxvlan = 1;
  cmd->ccommand.rxchecksum = 1;
  cmd->tx_enable = 1;
  cmd->rx_enable = 1;
  cmd->reset = 0;
  cfg->command = cmd;

  sent_command_registers(cmd, portbase);
  read_command_registers(cmd, portbase);

  init_core_driver(dev);
  debug("Tx Enable flag: %x - RxChecksum: %x\n", cmd->tx_enable,
                                                      cmd->ccommand.rxchecksum);
}

static int
init_core_driver(pci_dev_t pci)
{
  struct rtl_cfg *carriage;
  for_each_ll_entry_safe(get_rtl_dev_list(), carriage)
  {
    struct device *dev = kalloc(sizeof(*dev));
    dev->dev_id = device_id_alloc(dev);
    carriage->device_id = dev->dev_id;
    
    struct netdev *netdev = kalloc(sizeof(*netdev));
    netdev->dev = pci;
    netdev->dev_id = dev->dev_id;
    
    get_mac(pci, netdev);
    register_net_dev(dev, netdev);
    
    if(carriage->next == NULL)
      break;
    else
      continue;
  }
}

static void
sent_command_registers(struct rtlcommand *cmd, uint16_t port)
{
  /* first of all we have to sent the C+ command register */
  uint16_t ccommand = (cmd->ccommand.rxvlan << 6) |
                          (cmd->ccommand.rxchecksum << 5);

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

  cmd->ccommand.rxvlan = (ccommand >> 6) & 1;
  cmd->ccommand.rxchecksum = (ccommand >> 5) & 1;

  cmd->tx_enable = (command >> 2) & 1;
  cmd->rx_enable = (command >> 3) & 1;
  cmd->reset = (command >> 4) & 1;

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

int rtl_transmit_buff(struct net_buff *buf)
{
  return -E_NOFUNCTION;
}

int rtl_receive_buff(struct net_buff *buf)
{
  return -E_NOFUNCTION;
}

/**
 * \fn net_rx_vfio(vfile, buf, size)
 *
 * Receive a buffer from the device driver.
 */
static size_t
rtl_rx_vfio(struct vfile *file, char *buf, size_t size)
{
  struct netdev *dev = (struct netdev*)file->fs_data;

  return -E_NOFUNCTION;
}


/**
 * \fn net_tx_vfio(vfile, buf, size)
 *
 * Transmit a buffer using virtual files.
 */
static size_t
rtl_tx_vfio(struct vfile *file, char *buf, size_t size)
{
  return -E_NOFUNCTION;
}

static int
reset_rtl_device(struct rtl_cfg *cfg)
{
  cfg->command->reset = 1;
  sent_command_registers(cfg->command, cfg->portbase);

  int i = 0;
  for(; i < 0x100000; i++)
  {
    read_command_registers(cfg->command, cfg->portbase);
    if(cfg->command->reset == 0)
      return 0;
    else
      continue;
  }
  debug("RTL8168 failed");
  return -1;
}

static struct rtl_cfg* 
get_rtl_dev_list()
{
  return rtl_devs;
}

static int 
get_rtl_dev_num()
{
  int i = 0;
  struct rtl_cfg *carriage;
  for(carriage = get_rtl_dev_list(); carriage != NULL, carriage != carriage->next;
      carriage = carriage->next)
  {
    i++;
    if(carriage->next == NULL)
      break;
    else
      continue;
  }
  return i;
}

/**
 * \fn get_rtl_device(dev)
 * \brief Get a device based on the device number in the list.
 * 
 * @param dev Index in the device list.
 */
static struct rtl_cfg*
get_rtl_device(int dev)
{
  struct rtl_cfg *carriage = get_rtl_dev_list();
  int i = 0;
  for(;carriage != NULL, carriage != carriage->next; carriage = carriage->next)
  {
    if(i == dev)
      break;
    if(carriage->next == NULL)
      break;
    else
      continue;
  }
}

static int
rtl_conf_rx(struct rtl_cfg *cfg)
{
  
}

void
init_network()
{
  struct ol_pci_node *carriage = pcidevs;
  for(; carriage != NULL, carriage->next != carriage; carriage = carriage->next)
  {
    if(carriage->dev->class == NIC && carriage->dev->subclass == NIC_ETHERNET)
    {
      init_rtl_device(carriage->dev);
      return;
    }
    else
      continue;
  }
  debug("no network card found");
}
