/*
 *   Andromeda Project - Network stack - protocol independent - core driver.
 *   Copyright (C) 2011  Michel Megens - dev@michelmegens.net
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
#include <andromeda/drivers.h>
#include <networking/net.h>

char rx_buff[RX_BUFFER_SIZE];

int init_netif()
{
  struct device *dev = kalloc(sizeof(*dev));
  device_id_alloc(dev);
  dev_setup_driver(dev, &net_rx_vfio, &net_tx_vfio);
  dev->type = net_dev;
  dev->open = &device_open_driver_io;
  
  /*
   * attach the device driver to the the virtual bus.
   */
  struct device *virt_bus = dev_find_devtype(get_root_device(), virt_bus);
  device_attach(virt_bus, dev);
}

int
register_net_dev(struct device *dev, struct netdev* netdev)
{
  dev->type = net_dev;
  dev->open = &device_open_driver_io;
  dev->driver->io->fs_data = (void*)netdev;
  dev->driver->io->fs_data_size = sizeof(*netdev);
  
  /*
   * attach the device driver to the core driver.
   */
  struct device *core_dev = dev_find_devtype(dev_find_devtype(get_root_device(), 
                                                              virt_bus), net_dev);
  device_attach(core_dev, dev);
  return -E_SUCCESS;
}

int
unregister_net_dev(uint64_t id)
{
  struct device *dev = dev_find_devtype(dev_find_devtype(get_root_device(), 
                                                              virt_bus), net_dev);
  if(dev == NULL)
    return -E_NULL_PTR;
  if(device_detach(dev, device_find_id(dev, id)) == -E_NOTFOUND)
    return -E_NOTFOUND;

  else
  {
    kfree(dev->driver->io->fs_data); // free the netdev data
    kfree(dev->driver->io);
    kfree(dev->driver);
    kfree(dev);
    return -E_SUCCESS;
  }
}

struct net_buff *
alloc_buff_frame(unsigned int frame_len)
{
  struct net_buff *buff = kalloc(sizeof(*buff));
  buff->lenth = frame_len;
  buff->datalink_hdr = kalloc(frame_len);
  return buff;
}

static int
free_net_buff_list(struct net_buff* nb)
{
  free(nb->transport_hdr);
  free(nb->network_hdr);
  free(nb->datalink_hdr);
  struct net_buff *nxt = nb->next;
  free(nb);

  if(nxt)
    free_net_buff_list(nxt);
  else
    return -E_SUCCESS;
}

/**
 * \fn rx_process_net_buff(buff)
 * \brief Processes the received net_buff trough the entire network stack.
 * \warning Should only be called from net_rx_vfio(vfile, char*, size_t)
 *
 * \param buff The received net buffer.
 */
static int
rx_process_net_buff(struct net_buff *buff)
{
  return -E_NOFUNCTION;
}

/**
 * \fn net_rx_vfio(vfile, buf, size)
 *
 * Receive a buffer from the device driver.
 */
static size_t
net_rx_vfio(struct vfile *file, char *buf, size_t size)
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
net_tx_vfio(struct vfile *file, char *buf, size_t size)
{
  return -E_NOFUNCTION;
}
