/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STDLIB_H
#define STDLIB_H

#include <text.h>
#include <types.h>
#include <error/panic.h>
#include <mm/memory.h>
#include <kern/timer.h>
#include <math/math.h>

struct registers
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t esp;
} __attribute__((packed));
typedef struct registers *and_registers_t;

struct segments
{
	uint16_t ds;
	uint16_t cs;
	uint16_t es;
	uint16_t fs;
	uint16_t gs;
	uint16_t ss;
	
} __attribute__((packed));
typedef struct segments *and_segments_t;

#endif
