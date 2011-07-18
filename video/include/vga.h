/*
 *   VGA header. Included by functions which use the vga memory.
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

#ifndef __VGA_H
#define __VGA_H

#define OL_VGAMEMORY 0xB8000
#define OL_WIDTH 80
#define OL_HEIGHT 25
#define OL_WHITE_TXT 0x07

typedef struct vgaProperty
{
	int line;
	uint32_t x;
	uint16_t * vidmem;
} OL_VGA;

void reloc_cursor(uint32_t x, uint32_t y);
#endif