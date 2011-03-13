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

#include <text.h>
#include <mm/memory.h>
#include <types.h>
#include "include/vga.h"

struct curPos cursor = {0,0};
char col = 0x07;

void textInit()
{
	scroll(HEIGHT);
	//cursor.x = 0;
	//cursor.y = 0;
}

void scroll(unsigned int i)
{
	unsigned int p = 0;
	unsigned int q = 0;
	unsigned short *keybuf = (unsigned short *) KEYBUF;
	for (p=0; p < i; p ++)
	{
		for (q=0; q < WIDTH; q++)
		{
			keybuf[p*WIDTH+q] = keybuf[(p+i)*WIDTH+q];
		}
	}
	for (;p < HEIGHT; p++)
	{
		for (q=0; q < WIDTH; q++)
		{
			keybuf[p*WIDTH+q] = 0x07 << 8 | ' ';
		}
	}
}

void println(unsigned char *line)
{
	int i;
	for (i = 0; line[i] != '\0'; i++)
	{
		putc(line[i]);
	}
	putc('\n');
}

void putc(unsigned char i)
{
	boolean noPrint = FALSE;
	if (i == '\n')
	{
		cursor.y++;
		cursor.x = 0;
		noPrint = TRUE;
	}

	if (cursor.x+1 > WIDTH)
	{
		cursor.x %= WIDTH;
		cursor.y += cursor.x/WIDTH;
	}
	if (cursor.y >= HEIGHT)
	{
		scroll(cursor.y % HEIGHT+1);
	}
	if (!noPrint)
	{
		cursor.x++;
		unsigned short *keybuf = (unsigned short *)KEYBUF;
		unsigned short chr = (unsigned short)(col << 8) | (unsigned short)i;
		keybuf[cursor.x-1+cursor.y*WIDTH] = chr;
	}
}
