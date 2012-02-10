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

#include <stdlib.h>
#include <networking/net.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IP 0x800
//#define ARP 0x806

#define MAC_BROADCAST 0xffffffffffff

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

enum eth_error
{
  NO_ERROR = 0,
  BROADCAST = 1,
  WIRELESS = 2,
  MAC_MISMATCH= -1,
  UNKNOWN_PROTOCOL = -2
};

#if 0
enum eth_type
{
  IP = 0x800,
  X75,
  NBS,
  ECMA,
  CHAOSNET,
  X25,
  ARP
};
#endif

void init_eth();
static enum ptype receive_ethernet_frame(struct net_buff *buff);
static void eth_queue_notifier();

void debug_ethernet_stack();

#ifdef __cplusplus
}
#endif

#endif
