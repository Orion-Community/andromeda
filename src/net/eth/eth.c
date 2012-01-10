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
receive_ethernet_frame(frame_buf_t buffer)
{
  uint16_t size;
  struct ethframe *frame = alloc_eth_frame(size);
  
  free(buffer);
}

static struct ethframe *
alloc_eth_frame(uint16_t size)
{
  struct ethframe *frame = kalloc(sizeof(*frame));
  frame->data = kalloc(size);
  return ethframe;
}

static void 
setup_eth_frame(ethframe_t);
