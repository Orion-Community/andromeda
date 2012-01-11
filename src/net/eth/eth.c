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

void
receive_ethernet_frame(struct netdev *dev)
{
  struct ethframe *frame = alloc_eth_frame(dev->buf.data_len);
  build_ethernet_frame(frame, &dev->buf);
  process_ethernet_frame(frame, dev);
}

static struct ethframe *
alloc_eth_frame(uint16_t size)
{
  struct ethframe *frame = kalloc(sizeof(*frame));
  frame->data = kalloc(size);
  return frame;
}

static void 
build_ethernet_frame(ethframe_t frame, struct netbuf * buf)
{
  memcpy(frame->preamble, buf->framebuf, 7);
/*
  memcpy(frame->sof, buf->framebuf+7, 1);
*/
  frame->sof = *(uint8_t*)(buf->framebuf+7);
  memcpy(frame->destaddr, buf->framebuf+8, 6);
  memcpy(frame->srcaddr, buf->framebuf+14, 6);
  memcpy(frame->type, buf->framebuf+20, 2);
  memcpy(frame->data, buf->framebuf+22, buf->data_len);
  memcpy(frame->fcs, buf->framebuf+buf->data_len, 4);
}

/**
 * This function will check and process and incoming ethernet frame. After determining
 * the encapsulated protocol, it will call the associated protocol handler.
 * 
 * @param frame The frame which has to be processed.
 */
static enum eth_error
process_ethernet_frame(ethframe_t frame, struct netdev *dev)
{
  /* check the hw address first */
  char *broadcast = "0xffffffffffff";
  if(memcmp(frame->destaddr, dev->hwaddr, 6) || memcmp(frame->destaddr,
              broadcast, 6))
  {
    uint16_t type = (uint16_t)*((uint16_t*)frame->type);
    switch(type)
    {
      case ARP:
        printf("ARP has not been implemented (fully) yet!");
        break;

      case IP:
        printf("IP has not been implemented (fully) yet!");
        break;

      default:
        return 1;
        break;
    }
    return 0;
  }
  else
  {
    return 1;
  }
}

#ifdef ETH_DBG
void debug_ethernet_stack()
{
}
#endif
