#include <text.h>
#include <mm/memory.h>
#include "include/vga.h"

struct curPos cursor;
char col = 0x07;

void textInit()
{
	scroll(HEIGHT);
	cursor.x = 0;
	cursor.y = 0;
}

void scroll(int i)
{
	int p = 0;
	int q = 0;
	short *keybuf = (short *) KEYBUF;
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

void println(char *line)
{
	int i;
	for (i = 0; line[i] != '\0'; i++)
	{
		putc(line[i]);
	}
	putc('\n');
}

void putc(char i)
{
	if (i == '\n')
	{
		cursor.y++;
		cursor.x = 0;
	}
	cursor.x++;
	if (cursor.x > WIDTH)
	{
		cursor.x %= WIDTH;
		cursor.y += cursor.x/WIDTH;
	}
	if (cursor.y >= HEIGHT)
	{
		scroll(cursor.y % HEIGHT+1);
	}
	short *keybuf = (short *)KEYBUF;
	short chr = (short)(col << 8) | (short)i;
	keybuf[cursor.x-1+cursor.y*WIDTH] = chr;
}
