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

#ifndef _ISR_H_
#define _ISR_H_

struct idtentry
{
	uint16_t base_low;
	uint16_t sel;
	uint8_t zero;
	uint8_t flags;
	uint16_t base_high;
} __attribute__((packed));
typedef struct idtentry ol_idt_entry_t;

struct idt
{
	uint16_t limit;
	uint32_t baseptr;
} __attribute__((packed));
typedef struct idt ol_idt_t;

extern void setEntry(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
extern void installIDT(ol_idt_t * idt);
void setIDT();
#endif
