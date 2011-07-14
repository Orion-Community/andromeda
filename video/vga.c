/*
 *   Print functions. These functions print messages on the screen in TTY mode.
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

#include "include/vga.h"
#include <textio.h>
#include <sys/stdlib.h>
#include <sys/io.h>

GEBL_VGA cursor;

void textinit()
{
	cursor.line = 0;
	cursor.x = 0;
	cursor.vidmem = (uint16_t *)GEBL_VGAMEMORY;
}

void scroll(uint8_t lines)
{
	uint32_t x, y;
	for(y = 0; y < GEBL_HEIGHT - lines; y++)
	{
		for(x = 0; x < GEBL_WIDTH; x++)
		{
			cursor.vidmem[x+y*GEBL_WIDTH] = cursor.vidmem[x+(y+lines)*GEBL_WIDTH];
		}
	}
	for(; y < GEBL_HEIGHT; y++)
	{
		for(x = 0; x < GEBL_WIDTH; x++)
		{
			cursor.vidmem[x+y*GEBL_WIDTH] = ((GEBL_WHITE_TXT<<8) | ' ');
		}
	}
	
	if(lines >= GEBL_HEIGHT)
	{
		cursor.line = 0;
		cursor.x = 0;
		reloc_cursor(0,0);
		return;
	}
	cursor.line -= lines;
	reloc_cursor(cursor.x, cursor.line);
}

void println(uint8_t * txt)
{
	uint32_t i = 0;
	for(; txt[i] != 0; i++) putc(txt[i]);
	putc(0xa);
}

void print(uint8_t * txt)
{
	uint32_t i = 0;
	for(; txt[i] != 0; i++) putc(txt[i]);
}

char hex[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
char HEX[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void printnum(int index, uint32_t base, bool sInt, bool capital)
{
	if(base == 16)
	{ // I like a 0x prefix on hex numbers
		putc(0x30);
		putc(0x78);
	}
	
	if(index == 0)
	{ // hack fix for not printing zero's
		putc(0x30);
		return;
	}
	char* buf[32];
	int i = 0;

	if (base > 16)
	return;

	if (index < 0 && sInt)
	{
		putc('-');
		index *= -1;
	}
	
	unsigned int uIndex = (unsigned int) index;
	for (; uIndex != 0; i++)
	{
		buf[31-i] = (capital) ? HEX[uIndex%base] : hex[uIndex%base];
		uIndex /=base;
	}
	
	for (i--; i >= 0; i--)
	{
		putc(buf[31-i]);
	}
}

void putc(uint8_t c)
{
	uint32_t i = (cursor.line * GEBL_WIDTH) + cursor.x;
	switch(c)
	{
		case '\n':
			
			cursor.x = 0;
			cursor.line++;
			break;

		case '\b':
			cursor.x -= 1;
			cursor.vidmem[i-1] = (GEBL_WHITE_TXT<<8)| ' ';
			break;

		default:
			cursor.vidmem[i] = (GEBL_WHITE_TXT<<8) | c;
			cursor.x += 1;
			break;
	}
	
	reloc_cursor(cursor.x, cursor.line);
	if(cursor.line >= GEBL_HEIGHT)
	{
		scroll(cursor.line%GEBL_HEIGHT+1);
	}
}

void writeat(uint8_t c, uint32_t x)
{
	if((x % 2) == 0)
	{
		cursor.x = x;
		putc(c);
	}
}

void clearscreen() // clear the entire text screen
{
	char *vidmem = (char *) 0xb8000;
	unsigned int i = 0;
	while(i < (GEBL_WIDTH * GEBL_HEIGHT * 2))
	{
		vidmem[i]=' ';
		i++;
		vidmem[i] = GEBL_WHITE_TXT;
		i++;
	}
}

void reloc_cursor(uint32_t x, uint32_t y)
{
	uint16_t loc = (y * GEBL_WIDTH) + x;

        outb(0x3d4, 0xf); 
        outb(0x3d5, (uint8_t)(loc&0xff));
        outb(0x3d4, 0xe);
        outb(0x3d5, (uint8_t)((loc>>8)&0xff));
}