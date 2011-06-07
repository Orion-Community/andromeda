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

#include <sys/stdlib.h>
#ifndef __MM_H
#define __MM_H

struct GEBL_ENTRY
{
	uint64_t base;
	uint64_t len;
	uint32_t type;
	uint32_t acpi;
} __attribute__((packed));
// typedef struct GEBL_ENTRY ENTRY_T

#pragma pack(push,1)
struct GEBL_MMR 
{
	struct GEBL_ENTRY * entry;
	unsigned int count;
	unsigned int entry_size;
} __attribute__ ((packed));
#pragma pack(pop)

extern unsigned char * getmmr();
#endif

// struct mmap_register_t * getmmr();