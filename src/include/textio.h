/*
 *   TTY Text input/output functions.
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

#ifndef __VGA_H
#define __VGA_H

#define OL_VGAMEMORY 0xB8000
#define OL_WIDTH 80
#define OL_HEIGHT 25
#define OL_WHITE_TXT 0x07

typedef struct ol_vga_mem
{
	int line;
	uint32_t x;
	uint16_t * vidmem;
} ol_vga_mem_t;

void reloc_cursor(uint32_t x, uint32_t y);

/*
 * Initialize text i/o.
 */
void textinit();

/*
 * Clear the the screen.
 */
void clearscreen();

/*
 * Print a string on a new line.
 */
void println(uint8_t *);

/*
 * Print a string on the current line.
 */
void print(uint8_t *);

/*
 * Print a number.
 */
void printnum(int index, unsigned int base, bool sInt, bool capital);

/*
 * Put a character at the current cursor location.
 */
void putc(uint8_t c);

/* 
 * Writte a character on the current line at location x.
 */
void writeat(uint8_t c, uint32_t x);
#endif