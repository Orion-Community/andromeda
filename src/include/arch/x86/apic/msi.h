/*
 *   Andromeda Kernel - MSI
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

/*
 * MSI's are Message Signaled Interrupts; interrupts sent to the apic using
 * memory i/o.
 */

#include <stdlib.h>
#include <types.h>

#ifndef __MSI_H
#define __MSI_H

struct msi
{
  uint32_t addr_low, addr_hi;
  int vector : 8;
  int delivery_mode : 3;
  int reserved : 3;
  int trigger_level : 1;
  int trigger: 1;
  uint64_t reserved2 : 48;
} __attribute__((packed));
typedef struct msi msi_msg;

#endif
