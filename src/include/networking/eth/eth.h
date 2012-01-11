/*
 *   Andromeda Project - Ethernet frame header file
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

#ifndef __ETH_H
#define __ETH_H

#include <networking/net.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* frame_buf_t;

typedef struct ethframe
{
  uint8_t preamble[8];
  uint8_t sof;
  uint8_t destaddr[6];
  uint8_t srcaddr[6];
  uint8_t type[2];
  frame_buf_t data;
  uint8_t fcs[4];
} *ethframe_t;

void receive_ethernet_frame(struct netdev *);
static struct ethframe *alloc_eth_frame(uint16_t);
static void build_ethernet_frame(ethframe_t, struct netbuf*);

#ifdef __cplusplus
}
#endif

#endif
