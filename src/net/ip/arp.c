/*
 *   Andromeda Project - Address Resolution Protocol
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
#include <networking/arp.h>

/**
 * Allocate memory in the arp structure for the source & destination hardware
 * address/protocol
 */
static struct arp *
alloc_arp_structure(struct arp *arp)
{
  if(arp == NULL)
    return NULL;

  arp->src_hw = kmalloc(sizeof(uint8_t)*arp->hlength);
  arp->src_pr = kmalloc(sizeof(uint8_t)*arp->plength);
  arp->dst_hw = kmalloc(sizeof(uint8_t)*arp->hlength);
  arp->dst_pr = kmalloc(sizeof(uint8_t)*arp->plength);
  return arp;
}

/**
 * Free's all allocated memory in the given arp structure.
 *
 * @arg arp The arp structure which should be free'd
 */
static void
free_arp_structure(struct arp *arp)
{
  if(arp == NULL)
    return;

  kfree(arp->src_hw);
  kfree(arp->src_pr);
  kfree(arp->dst_hw);
  kfree(arp->dst_pr);
  kfree(arp);
}
