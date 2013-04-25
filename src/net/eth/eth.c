/*
 *   Andromeda Project - Ethernet frame file
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
#include <networking/eth/eth.h>
#include <networking/net.h>
#include <andromeda/system.h>

void
init_eth()
{
  struct protocol *root = get_ptype_tree();
  struct protocol *carriage = kmalloc(sizeof(*carriage));
  carriage->type = ETHERNET;
  carriage->deliver_packet = &receive_ethernet_frame;
  carriage->notify = &eth_queue_notifier;
  add_ptype(root, carriage);
}

/**
 * \fn receive_ethernet_frame(net_buff)
 * \brief Receives a net_buff which has to be processed through the datalink
 * layer. This function will queue the buffer for processing. When it will be
 * processed is not known. process_ethernet_frame is called by the scheduler.
 *
 * \param nb The net_buff to queue.
 * \see process_ether_net_frame
 */
static enum ptype
receive_ethernet_frame(struct net_buff *nb)
{
        //nb->type = IPv4;
        return P_QUEUED;
}

/**
 * This function will check and process and incoming ethernet frame. After determining
 * the encapsulated protocol, it will call the associated protocol handler.
 *
 * @param frame The frame which has to be processed.
 */
static enum ptype
process_ethernet_frame(struct net_buff *buff, struct protocol *type)
{
  return -E_NOFUNCTION;
}

static void
eth_queue_notifier()
{
        debug("Eth frame has been queued\n");
        return;
}

#if 0
/**
 * This function will create a fake ethernet packet to test the receive functions
 */
void debug_ethernet_stack()
{
  struct netdev *dev = kmalloc(sizeof(*dev));

  int i = 0;
  for(; i < MAC_ADDR_SIZE; i++)
    dev->hwaddr[i] = 0xaa;
  dev->buf.data_len = 60;
  dev->buf.length = 86;
  dev->buf.framebuf = kmalloc(dev->buf.length);
  writew(dev->buf.framebuf+20, ARP);

  i = 0;
  for(; i < 6; i++)
    ((char*)dev->buf.framebuf+8)[i] = 0xff;

  receive_ethernet_frame(dev);

  free(dev->buf.framebuf);
  free(dev);
}
#endif
