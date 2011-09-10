/*
 *   Memory map header file.
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

#ifndef __MMAP_H
#define __MMAP_H

struct ol_mmap_entry
{
	uint64_t base;
	uint64_t len;
	uint32_t type;
	uint32_t acpi;
} __attribute__((packed));
typedef struct ol_mmap_entry *ol_mmap_entry_t;

struct ol_mmap_register 
{
	ol_mmap_entry_t entry;
	uint16_t ecount;
	uint8_t entry_size;
} __attribute__ ((packed));
typedef struct ol_mmap_register *ol_mmap_register_t;

/**
 * Updates the current memory map. It stores the result
 * in the given mmr
 */
extern void updatecmosmmap(ol_mmap_register_t mmr);

void display_mmap(ol_mmap_register_t mmr);

ol_mmap_entry_t getmmapentry(ol_mmap_register_t mmr, uint32_t idx);

/**
 * Checks the mmr. If the mmr is invalid the system will probe for ram and
 * update the mmr pointer.
 */
void init_mmap(ol_mmap_register_t mmr);
#endif
