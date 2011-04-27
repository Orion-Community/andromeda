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
#include <io.h>
#include "include/vga.h"

struct curPos cursor = {0,0,16};
char col = 0x07;

void textInit()
{
	scroll(HEIGHT);
	//cursor.x = 0;
	//cursor.y = 0;
}

void scroll(unsigned char lines)
{
	int x, y;
	unsigned short *keybuf = (unsigned short *)KEYBUF;
	for (y = 0; y < HEIGHT-lines; y++)
	{
		for (x=0; x < WIDTH; x++)
		{
			keybuf[x+y*WIDTH] = keybuf[x+(y+lines)*WIDTH];
		}
	}
	for (; y < HEIGHT; y++)
	{
		for (x = 0; x< WIDTH; x++)
		{
			keybuf[x+y*WIDTH] = ((col << 8) | ' ');
		}
	}
	if (lines >= HEIGHT)
	{
		cursor.x=0;
		cursor.y=0;
		return;
	}
	cursor.y-=lines;
}

void println(unsigned char *line)
{
	printf(line);
	putc('\n');
}

void printf(unsigned char *line)
{
	int i;
	for (i = 0; line[i] != '\0'; i++)
	{
		putc(line[i]);
	}
}

char hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void printhex(unsigned int index)
{
	printf("0x");
	int i = 7;
	while (( (( index >> 4*i ) & 0xf) == 0x0)&(i>0)) {
		i--;
	}
	while (i>=0) {
		putc(hex[( index >> 4*i ) & 0xf]);
		i--;
	}
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

	if (i == '\t')
	{
		int i;
		if (cursor.x % cursor.tabwidth == 0)
		{
			putc(' ');
		}
		for (i = 0; i < cursor.x%cursor.tabwidth; i++)
		{
			putc(' ');
		} 
		noPrint = TRUE;
	}
	if (cursor.x+1 > WIDTH)
	{
		cursor.x %= WIDTH;
		cursor.y += 1;
	}
	if (cursor.y >= HEIGHT)
	{
		scroll(cursor.y % HEIGHT+1);
	}
	if (!noPrint)
	{
		cursor.x++;
		unsigned short *keybuf = (unsigned short *)KEYBUF;
		unsigned short chr = (unsigned short)(col << 8) | (unsigned short)(i & 0x00FF);
		keybuf[cursor.x-1+cursor.y*WIDTH] = chr;
	}
	reloc(cursor.x, cursor.y);
}

int reloc(int loc_x, int loc_y)
{
        /*
         * Set the new cursor position
         */
        unsigned short location = loc_y * WIDTH + loc_x;
        outb(0x3D4, 14); 
        outb(0x3D5, location >> 8);
        outb(0x3D4, 15);
        outb(0x3D5, location);
        return 0;
}