/*
 *   System idt header.
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

#include <stdlib.h>

#ifndef __IDT_H
#define __IDT_H

struct idtentry
{
	uint16_t base_low;
	uint16_t sel;
	uint8_t zero;
	uint8_t flags;
	uint16_t base_high;
} __attribute__((packed));
typedef struct idtentry *ol_idt_entry_t;

struct idt
{
	uint16_t limit;
	ol_idt_entry_t baseptr;
} __attribute__((packed));
typedef struct idt *ol_idt_t;

static void
ol_idt_install_entry(uint16_t, uint32_t, 
        uint16_t, uint8_t, ol_idt_t);

static void
installExceptions(ol_idt_t);

static void
installInterrupts(uint16_t, uint16_t, ol_idt_t);

extern void 
installIDT(ol_idt_t);
#endif
