#include <text.h>
#include <mm/memory.h>
#include "include/vga.h"

int curPos = 0;
char col = 0x70;

void vgaInit()
{
	memset(*(char*)KEYBUF, '\0', WIDTH*2*HEIGHT);
}

void scroll()
{
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
		int tmp = curPos/WIDTH;
		tmp++;
		curPos = curPos%WIDTH+tmp*WIDTH;
	}
	if (curPos/WIDTH >= HEIGHT)
	{
		scroll(HEIGHT-curPos/WIDTH);
	}
	*(char *)(KEYBUF+curPos+1) = i;
	*(char *)(KEYBUF+curPos) = col;
	curPos+=2;
}
