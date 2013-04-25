/*
 *   Andromeda Project - General IP file.
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
#include <networking/net.h>
#include <networking/eth/ipv4.h>
#include <andromeda/system.h>

int
netif_init_ip()
{
  struct protocol *root = get_ptype(get_ptype_tree(), ETHERNET);
  struct protocol *item = kmalloc(sizeof(*item));
  item->type = IPv4;
  item->deliver_packet = &netif_rx_ip;
  add_ptype(root, item);
}

static enum ptype
netif_rx_ip(struct net_buff *buff)
{
        printf("Received an IP packet! %x\n", buff->type);
        return P_DELIVERED;
}
