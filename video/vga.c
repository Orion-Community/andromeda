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

GEBL_VGA cursor;

void textinit()
{
	cursor.line = 0;
	cursor.x = 0;
}

int print(char * txt)
{
	int line = cursor.line;
	line++;
	char *vidmem = (char *) 0xB8000;
	int i = 0;

	i = (line * 80 * 2);

	while(*txt != 0)
	{
		if(*txt == '\n') // check for a new line
		{
			line++;
			i=(line * 80 * 2);
			*txt++;
		} 
		else 
		{
			vidmem[i] = *txt;
			*txt++;
			i++;
			vidmem[i] = WHITE_TXT;
			i++;
		}
	}
	currentline.line = line;
	return(0);
}

void putc(uint8_t c)
{
	if(c == '\n')
	{
		cursor.x = 0;
		cursor.line++;
		uint8_t nop = FALSE;
	}
	cursor.vidmem[cursor.x] = GEBL_WHITE_TXT;
	cursor.vidmem[cursor.x+1] = c;
	cursor.x += 2;
}

void clearscreen() // clear the entire text screen
{
	char *vidmem = (char *) 0xb8000;
	unsigned int i = 0;
	while(i < (WIDTH * HEIGHT * 2))
	{
		vidmem[i]=' ';
		i++;
		vidmem[i] = WHITE_TXT;
		i++;
	}
}