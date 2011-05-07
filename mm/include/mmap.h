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

#ifndef __MM_H
#define __MM_H

struct mmap_entry
{
	uint64_t addr;
	uint64_t len;
	uint32_t type
} __attribute__((packed));
typedef struct mmap_entry gebl_mmap_entry;

struct mmapregister
{
	uint32_t address; // segment:offset address
	uint16_t entrie_count;
	uint8_t entry_size;
} __attribute__((packed));
typedef struct mmr mmap_register_t;
#endif

extern uint32_t mmr;

struct mmap_register_t * getmmr();