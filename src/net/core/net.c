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

static struct net_queue net_core_que;

int init_netif()
{
  struct device *dev = kalloc(sizeof(*dev));
  device_id_alloc(dev);
  dev_setup_driver(dev, net_rx_vfio, net_tx_vfio);
  dev->type = net_core_dev;
  dev->open = &device_open_driver_io;
  
  /*
   * attach the device driver to the the virtual bus.
   */
  struct device *virtual = dev_find_devtype(get_root_device(), virtual_bus);
  device_attach(virtual, dev);
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
                                                              virtual_bus), net_core_dev);
  device_attach(core_dev, dev);
  return -E_SUCCESS;
}

int
unregister_net_dev(uint64_t id)
{
  struct device *dev = dev_find_devtype(dev_find_devtype(get_root_device(), 
                                                              virtual_bus), net_core_dev);
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

/**
 * \fn net_buff_append_list(head, x)
 * \brief The net_buff item x will be appended to the net_buff head <i>head</i>.
 * \param head The list head.
 * \param x Item to be appended.
 * \return Error code.
 */
static int 
net_buff_append_list(struct net_buff *head, struct net_buff *x)
{
  if(head == NULL || x = NULL)
    return -E_NULL_PTR;
  else
  {
    struct net_buff *carriage;
    for_each_net_buff_entry_safe(head, carriage)
    {
      if(carriage->next == NULL)
      {
        carriage->next = x;
        x->previous = x;
        x->next = NULL;
      }
      else
        continue;
    }
  }
  return -E_SUCCESS;
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
 * \fn netif_process_net_buff(buff)
 * \brief Processes the received net_buff trough the entire network stack.
 * \warning Should only be called from net_rx_vfio(vfile, char*, size_t)
 *
 * \param buff The received net buffer.
 */
static int
netif_process_net_buff(struct net_buff *buff)
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
  struct net_buff *buffer = (struct net_buff*)buf;
  debug("Packet arrived in the core driver successfully. MAC address:");
  print_mac(buffer->dev);
  return -E_SUCCESS;
}


/**
 * \fn net_tx_vfio(vfile, buf, size)
 *
 * Transmit a buffer using virtual files.
 */
static size_t
net_tx_vfio(struct vfile *file, char *buf, size_t size)
{
  struct device *core_dev = dev_find_devtype(dev_find_devtype(get_root_device(), 
                                                          virtual_bus), net_core_dev);
  struct device *dev_driver = device_find_id(core_dev, ((struct netdev*)buf)->dev_id);
  struct vfile *io = dev_driver->open(dev_driver);
  io->write(file, buf, size);
  return -E_NOFUNCTION;
}

struct device *
get_net_driver(uint64_t id)
{
  struct device *dev = dev_find_devtype(dev_find_devtype(get_root_device(), 
                                                         virtual_bus), net_core_dev);
  return device_find_id(dev, id);
}

static inline struct net_queue *
net_core_get_queue()
{
  return &net_core_que;
}

void
print_mac(struct netdev *netdev)
{
  int i;
  for(i = 0; i<5; i++)
    printf("%x:", netdev->hwaddr[i]);

  printf("%x\n", netdev->hwaddr[5]);
}
