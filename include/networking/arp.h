/*
 *   Andromeda Project - Address Resolution Protocol
 *   Copyright (C) 2011  Michel Megens
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

#ifndef __ARP_H
#define __ARP_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdlib.h>
typedef struct arp
{
  uint16_t hwtype; /* hardware type */
  uint16_t ptype; /* protocol type */
  uint8_t hlength; /* hardware address length */
  uint8_t plength; /* protocol address length */
  uint16_t opcode; /* operation code */
  uint8_t * src_hw; /* source hardware address */
  uint8_t * src_pr; /* source protocol */
  uint8_t * dst_hw; /* destination hardware address */
  uint8_t * dst_pr; /* destination protocol */
} *arp_t;

struct arp_table_node
{
  struct arp_table_node *next;
  uint8_t mac[6];
  uint8_t addr[8];
};

static struct arp *alloc_arp_structure(struct arp *);
static void free_arp_structure(struct arp *);

#ifdef __cplusplus
}
#endif
#endif
