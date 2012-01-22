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
