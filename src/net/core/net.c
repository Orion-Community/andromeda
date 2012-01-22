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

int
register_net_dev(struct netdev* netdev)
{
  struct device *dev = kalloc(sizeof(*dev));
  dev_setup_driver(dev, &net_rx_vfio, &net_tx_vfio);
  dev->dev_id = device_id_alloc(dev);
  dev->type = net_dev;
  dev->driver->io->fs_data = (void*)netdev;
  dev->driver->io->fs_data_size = sizeof(*netdev);
  device_attach(&dev_root, dev);
}

struct net_buff *
alloc_buff_frame(unsigned int frame_len)
{
  struct net_buff *buff = kalloc(sizeof(*buff));
  buff->datalink_hdr = kalloc(frame_len);
  return buff;
}

static int 
free_net_buff_list(struct net_buff* nb)
{
  free(nb->transport_hdr);
  free(nb->network_hdr);
  free(nb->datalink_hdr);
  struct net_buff nxt = nb->next;
  free(nb);
  
  if(nxt)
    free_net_buff_list(nxt);
  else
    return 0;
}

/**
 * \fn net_rx_vfio(vfile, buf, size)
 * 
 * Receive a buffer from the device driver.
 */
static int
net_rx_vfio(struct vfile *file, char *buf, size_t size)
{
  return -E_NOFUNCTION;
}


/**
 * \fn net_tx_vfio(vfile, buf, size)
 * 
 * Transmit a buffer using virtual files.
 */
static int 
net_tx_vfio(struct vfile *file, char *buf, size_t size)
{
  return -E_NOFUNCTION;
}
